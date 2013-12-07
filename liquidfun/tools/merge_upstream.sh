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
# Merge upstream changes from a subversion repo into this tree.

declare -r this_dir=$(dirname $0)
declare -r project_root=$(cd "${this_dir}/.."; pwd)
declare -r project_name=$(basename "${project_root}")
declare -r git_config="${project_root}/.git/config"

# Upstream subversion repo to merge.
: ${SVN_UPSTREAM:=}
# Upstream git repo to merge.
: ${GIT_UPSTREAM:=../../../../external/box2d}
# Name of the git branch from the upstream repo to merge.
: ${GIT_BRANCH:=master}

usage() {
  echo "\
Merge upstream repository into the current git branch in this tree.

This script fetches the latest revision of the upstream repository from
subversion using git-svn to convert it into a temporary git repository.  The
temporary git repository is then merged with this project's repository located
in:
${project_root}.

Usage: $(basename $0) [-h] [-S] [-s svn_upstream_path] \\
                      [-g git_upstream_path] [-b git_upstream_branch]

-h:
  Display this help text.
-S:
  Disable fetch from subversion.  When this is specified this script will just
  add the specified git remote as an upstream repository.
-s svn_upstream_path:
  Path of the upstream subversion repository to merge.
  This defaults to ${SVN_UPSTREAM}.
-g git_upstream_path
  Path of the upstream git repository to merge if fetching from subversion is
  disabled using -S.  This defaults to \"${GIT_UPSTREAM}\".
-b git_upstream_branch
  Name of the upstream git branch to merge.  This defaults to
  \"${GIT_BRANCH}\".
" >&2
  exit 1
}

# Verify git-svn is installed, if it isn't present exit the script with an
# error.
verify_git_svn_installed() {
  if ! git svn 2>&1 | grep -q '^git-svn'; then
    echo -e \
      "git-svn is required by this script. Please install and try again." \
      "\ne.g 'sudo apt-get install git-svn'" >&2
    exit 1
  fi
}

# If the merge "ours" driver, required by .gitattributes, isn't present in
# the local .git/config, add it.
# This driver just returns 0 (success) as "our" file is already the current
# version or (%A) see
# https://www.kernel.org/pub/software/scm/git/docs/gitattributes.html
add_ours_merge_driver_to_git_config() {
  if ! grep -qF '[merge "ours"]' "${git_config}"; then
    echo '
[merge "ours"]
    name = Keep the local revision of a file dropping any upstream changes.
    driver = /bin/bash -c '"exit 0"'
    recursive = binary
' >> "${git_config}"
  fi
}

# Mirror an upstream subversion repository into a local git repository.
mirror_svn() {
  local git_dir="${1}"
  local svn_path="${2}"
  verify_git_svn_installed
  pushd "${git_dir}" >/dev/null
  git svn init "${svn_path}"
  git svn fetch
  popd >/dev/null
}

# Add and fetch a git remote.
add_fetch_git_remote() {
  local remote_name="${1}"
  local remote_path="${2}"
  git remote rm "${remote_name}" 2>/dev/null || true
  git remote add "${remote_name}" "${remote_path}"
  git fetch "${remote_name}"
}

# git currently doesn't attempt to resolve merge conflicts for files that
# have been deleted in our branch and added / copied / deleted / renamed in
# another so they're handled here.
resolve_deleted_renamed_conflicts() {
  if [[ -e .gitattributes ]]; then
    (
      # Disable filename expansion (globbing) so that it's possible to compare
      # each filename to a glob in the if statement below.
      set -f
      IFS=$'\n'
      local merge_ours=$(awk '
        /^#/ {
          next;
        }

        $2 ~ /merge=ours/ {
          filename_glob = $1;
          # Escape for bash if statement below.
          gsub(/\//, "\\/", filename_glob);
          gsub(/\*/, "*", filename_glob);
          print filename_glob;
        }' .gitattributes)
      for file in $(git status -s | \
                    awk '$1 ~ /^U[ACDR]/ || $1 ~ /^DU/ { print $2 }'); do
        for use_ours_glob in ${merge_ours}; do
          eval "
            if [[ \"\${file/${use_ours_glob}/}\" == \"\" ]]; then
              echo 'Dropping upstream changes to ${file}' >&2
              git rm ${file}
            fi
          "
        done
      done
    )
  fi
}

main() {
  # Parse arguments.
  while getopts "hSs:g:b:" options; do
    case ${options} in
      S ) SVN_UPSTREAM="";;
      s ) SVN_UPSTREAM="${OPTARG}";;
      g ) GIT_UPSTREAM="${OPTARG}";;
      b ) GIT_BRANCH="${OPTARG}";;
      h ) usage;;
    esac
  done

  cd "${project_root}"

  add_ours_merge_driver_to_git_config

  # Name of the remote that will be added to the project's git repository.
  declare -r remote_name="${project_name}-upstream"
  # Create a temporary directory to mirror the upstream subversion repository.
  declare -r temp_upstream=$(mktemp -d "${project_root}/upstream.XXXXXX")
  trap "rm -rf \"${temp_upstream}\"" SIGINT SIGTERM EXIT

  # Mirror the subversion repo into a local git repository.
  if [[ "${SVN_UPSTREAM}" != "" ]]; then
    mirror_svn "${temp_upstream}" "${SVN_UPSTREAM}"
    GIT_UPSTREAM="${temp_upstream}"
  fi

  if [[ "${GIT_UPSTREAM}" == "" ]] ;then
    echo "Subversion fetch disabled and no upstream git repository" \
         "specified." >&2
    exit 1
  fi

  # Add the git mirror of the upstream subversion repo as a remote, fetch it
  # into this project.
  add_fetch_git_remote "${remote_name}" "${GIT_UPSTREAM}"
  # Renormalize to work around differing line endings between upstream and
  # the local branch.
  if ! git merge -s recursive -Xrenormalize \
                 "${remote_name}/${GIT_BRANCH}"; then
    # Resolve files that are deleted locally which use the merge=ours driver in
    # .gitattributes.
    resolve_deleted_renamed_conflicts
    # Try to commit again.
    git commit
  fi
}

main "$@"
