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
         -r 'url0|remote0|remote_branch0 ... url0|remoteN|remote_branchN'

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
-r url|remote|remote_branch
  Space separated list of tuples that specify a list of git project url,
  remote name and remote branch names to merge into a local branch.
  History is rewritten locally for each \"remote_branch\" such that it's moved
  into a directory called \"remote\" under the root of the local branch.
  Finally, a local branch is created which is a merge of each project's history
  into a single branch.
-u upstream_url|branch
  Upstream URL and branch to push the resultant merged branch to.
  If this isn't specified it defaults to
  \"${UPSTREAM_DEFAULT}\".
" >&2
  exit 1
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

  local first_branch=
  local merge_branches=
  for remote_description in ${remotes}; do
    # Add the remote.
    local url=
    local remote=
    local branch=
    eval $(echo "${remote_description}" | \
           sed -r 's/([^|]*)\|([^|]*)\|([^|]*)/url=\1; remote=\2; branch=\3;/')
    if [[ "${url}" == "" || "${remote}" == "" || "${branch}" == "" ]]; then
      echo "Invalid remote format: ${remote}" >&2
      usage
    fi
    git remote add -f ${remote} ${url}

    # Move the remote into subdirectory called "remote" and filter
    # author / committer names and e-mail addresses from commit messages.
    local current_branch=${remote}_${branch}
    echo "--- ${current_branch} ---" >&2
    git checkout -b ${current_branch} ${remote}/${branch}
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
            xargs -I@ sed -r -i '${sed_file_filter_exp}' '@'" \
      ${current_branch}
    if [[ "${first_branch}" == "" ]]; then
      first_branch="${current_branch}"
    else
      merge_branches="${merge_branches} ${current_branch}"
    fi
  done

  # Merge all branches together.
  git checkout "${first_branch}"
  for merge_branch in ${merge_branches}; do
    git merge -q ${merge_branch}
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
    git merge ${first_branch}
    echo "
Run the following command to push the current branch to
${upstream}:
  git push upstream HEAD:${branch}" >&2
  fi
}

main "${@}"
