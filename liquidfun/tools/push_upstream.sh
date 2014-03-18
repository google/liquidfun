#!/bin/bash -eu
#
# Copyright (c) 2013 Google, Inc.
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
# 1. The origin of this software must not be misrepresented; you must not
# claim that you wrote the original software. If you use this software
# in a product, an acknowledgment in the product documentation would be
# appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
# misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
#

declare -r script_name="$(basename $0)"

: ${UPSTREAM_DEFAULT:="https://github.com/google/liquidfun.git|master"}

# Display a help message and exit.
usage() {
  echo "\
Merge a set of git projects into single tree preserving history and optionally
filtering author / committer names and e-mail addresses.

Usage: ${script_name} [-e email_filter_regexp] [-a name_filter_regexp] \\
         [-s sed_commit_filter_exp] [-f sed_file_filter_exp] [-h] \\
         [-u upstream_url:branch] \\
         -r 'url0|remote0|remote_branch0|start0 ... \\
             url0|remoteN|remote_branchN|startN'

-e email_filter_regexp:
  Extended regular expression used to match author / commmitter e-mail
  addresses that should be filtered from the branch history.
-a name_filter_regexp:
  Extended regular expression used to match author / commmitter names that
  should be filtered from the branch history.
-s sed_commit_filter_exp:
  Extended sed expression (see 'sed -r') that will be used to modify all commit
  messages.
-f sed_file_filter_exp:
  Extended sed expression (see 'see -r') that will be used to modify all files
  in each commit.  Since this operates over all files in each commit this can
  be a \*very\* slow operation.
-h:
  Display this help message.
-r url|remote|remote_branch|start
  Space separated list of tuples that specify a list of git project url,
  remote name and remote branch names to merge into a local branch.
  History is rewritten locally for each \"remote_branch\" such that it's moved
  into a directory called \"remote\" under the root of the local branch.
  Finally, a local branch is created which is a merge of each project's history
  into a single branch.
  = NOTE: \"start\" is an experimental feature that will only work with flat
  source branches or merges where all commits apply cleanly. =
  \"start\" is an optional argument that matches a gerrit Change-Id, git tag or
  git commit hash which indicates the change to start the history from, all
  commits prior to this point will be squashed into a single commit.
-u upstream_url|branch
  Upstream URL and branch to push the resultant merged branch to.
  If this isn't specified it defaults to
  \"${UPSTREAM_DEFAULT}\".
" >&2
  exit 1
}

# Find start commit hash, where $1 is git commit / tag or gerrit change ID to
# search for.  This function searches commits first, then tags and finally
# greps the log for the gerrit change ID.
# The resultant start commit hash is printed to stdout.
find_start() {
  # Search for a commit.
  local -r start="${1}"
  local -r start_commit=$(git log -n 1 --pretty=format:%H ${start} 2>/dev/null)
  if [[ "${start_commit}" != "" ]]; then
    echo "${start_commit}"
    return 0
  fi
  local -r start_tag_commit=$( \
    git log -n 1 --tags --pretty=format:%H ${start} 2>/dev/null)
  if [[ "${start_tag_commit}" != "" ]]; then
    echo "${start_tag_commit}"
    return 0
  fi
  local -r start_changeid_commit=$( \
    git log -n 1 --grep "Change-Id: ${start}" --pretty=format:%H 2>/dev/null)
  if [[ "${start_changeid_commit}" != "" ]]; then
    echo "${start_changeid_commit}"
    return 0
  fi
  return 1
}

main() {
  # Expressions used to filter author and committer, names and email addresses.
  local email_filter_regexp=
  local name_filter_regexp=
  # Expression used to filter all commit messages.
  local sed_commit_filter_exp=''
  # Expression used to modify all files in each commit.
  local sed_file_filter_exp=''
  # Remote branches to merge into a single tree.
  local remotes=
  # Upstream url / branch to push to.
  local upstream="${UPSTREAM_DEFAULT}"
  while getopts "e:a:hr:s:f:u:" option; do
    case ${option} in
      e ) email_filter_regexp="${OPTARG}";;
      a ) name_filter_regexp="${OPTARG}";;
      s ) sed_commit_filter_exp="${OPTARG}";;
      f ) sed_file_filter_exp="${OPTARG}";;
      h ) usage;;
      r ) remotes="${OPTARG}";;
      u ) upstream="${OPTARG}";;
      * ) echo "Unrecognized option ${option}" >&2; usage;;
    esac
  done
  [[ "${remotes}" == "" ]] && usage

  # Generate the commands to filter author and commiter names and email
  # addresses.
  if [[ "${email_filter_regexp}" != "" || \
        "${name_filter_regexp}" != "" ]]; then
    local -r git_commit_filter=$(\
      for variable in GIT_{AUTHOR,COMMITTER}_EMAIL; do
        echo "export ${variable}=\$(\
                echo \$${variable} | \
                sed -r 's%${email_filter_regexp}%anonymous@google.com%')"
      done;
      for variable in GIT_{AUTHOR,COMMITTER}_NAME; do
        echo "export ${variable}=\$(\
                echo \$${variable} | \
                sed -r 's%${name_filter_regexp}%Anonymous%')"
      done);
  else
    local -r git_commit_filter=""
  fi

  # Create a temporary output directory.
  local -r temp_dir=$(mktemp -d ${script_name}.XXXXXX)
  cd "${temp_dir}"
  git init
  # Suppress warnings about changes to line endings.
  git config --local core.safecrlf false
  git config --local core.autocrlf input

  local first_branch=
  local first_remote=
  local merge_branches=
  for remote_description in ${remotes}; do
    # Add the remote.
    local url=
    local remote=
    local branch=
    eval $(echo "${remote_description}" | \
           sed -r 's/([^|]*)\|([^|]*)\|([^ \t]*)/url=\1; remote=\2; branch=\3;/')
    if [[ "${url}" == "" || "${remote}" == "" || "${branch}" == "" ]]; then
      echo "Invalid remote format: ${remote_description}" >&2
      usage
    fi
    # Split branch and start field.
    local start=
    eval $(echo "${branch}" | \
           sed -r 's/([^|]*)\|([^|])/branch=\1; start=\2;/;t;s/.*//')
    git remote add -f ${remote} ${url}

    # Checkout the remote branch.
    local current_branch=${remote}_${branch}
    echo "--- ${current_branch} ---" >&2
    git checkout -b ${current_branch} ${remote}/${branch}

    # If a start was specified, just start from that commit.
    if [[ "${start}" != "" ]]; then
      local -r start_commit=$(find_start "${start}")
      if [[ "${start_commit}" != "" ]]; then
        git checkout -f -b ${current_branch}_start ${start_commit}
        git checkout -f -b ${current_branch}_nohistory \
          $(echo 'Squash up to ${start}.' | git commit-tree $(git write-tree))
        git cherry-pick --allow-empty \
          ${current_branch}_start..${current_branch}
        git branch -D ${current_branch}_start ${current_branch}_nohistory
      fi
    fi

    # Move the remote into subdirectory called "remote" and filter
    # author / committer names and e-mail addresses from commit messages.
    git filter-branch \
      --force \
      --commit-filter "
        ${git_commit_filter};
        sed -r '${sed_commit_filter_exp}' | git commit-tree \$@" \
      --tree-filter "
        mkdir -p '${remote}';
        find . -maxdepth 1 | sed 's@^\./@@' | grep -vE '^(${remote}|\.)\$' | \
          xargs -I@ mv '@' '${remote}';
        [[ '${sed_file_filter_exp}' != '' ]] && \
          find '${remote}' -type f | \
            xargs -I@ sed -r -i '${sed_file_filter_exp}' '@' || true" \
      ${current_branch}
    if [[ "${first_branch}" == "" ]]; then
      first_remote="${remote}"
      first_branch="${current_branch}"
    else
      merge_branches="${merge_branches} ${current_branch}|${remote}"
    fi
  done

  # Merge all branches together.
  git checkout "${first_branch}"
  for merge_branch in ${merge_branches}; do
    local branch_to_merge=${merge_branch/|*/}
    local branch_alias=${merge_branch/*|/}
    git merge -q -m "Merge ${branch_alias}" ${branch_to_merge}
  done

  # Push to upstream branch.
  if [[ "${upstream}" != "" ]]; then
    local url=
    local branch=
    eval $(echo ${upstream} | sed -r 's/([^|]*)\|([^|]*)/url=\1; branch=\2/')
    if [[ "${url}" == "" || "${branch}" == "" ]]; then
      echo "Invalid upstream format: ${upstream}" >&2
      usage
    fi
    upstream_branch=upstream_${branch}
    git remote add -f upstream "${url}"
    git checkout -b ${upstream_branch} upstream/${branch}
    # Resolve conflicts by taking the downstream version.
    # We should have already brought in all the upstream changes.
    git merge -s recursive -Xtheirs -m "Merge ${first_remote}" ${first_branch}
    echo "
The source has been staged in ${temp_dir} for inspection.

When everything looks good, run the following command from
${temp_dir} to push the current branch to
 ${upstream}:
    git push upstream HEAD:${branch}" >&2
  fi
}

main "${@}"

