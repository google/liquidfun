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

: ${UPSTREAM_DOCS_DEFAULT:="https://github.com/google/liquidfun.git|gh-pages"}
: ${UPSTREAM_MASTER_DEFAULT:="https://github.com/google/liquidfun.git|master"}
: ${DOCS_SUBDIR_DEFAULT:=liquidfun/Box2D/Documentation}

usage() {
  echo "\
Prepare a documentation branch from a LiquidFun source branch.

Usage: ${script_name} [-u upstream_url|source_branch] \\
         [-d upstream_url|docs_branch] [-s documentation_directory] [-h]

-u upstream_url|source_branch:
  Location to read LiquidFun documentation from.
  Defaults to \"${UPSTREAM_MASTER_DEFAULT}\".
-d upstream_url|docs_branch:
  Location to push documentation to.
  Defaults to \"${UPSTREAM_DOCS_DEFAULT}\".
-s documentation_directory:
  Location of the documentation subdirectory on source_branch.
  Defaults to \"${DOCS_SUBDIR_DEFAULT}\"
-h:
  Display this message.
" >&2
  exit 1
}

# Add remote described by ${1} as remote_url|branch called remote ${2},
# checkout in "branch" and return the name of the reference to the local
# branch.
add_remote_checkout_branch() {
  local url=
  local branch=
  eval $(echo "${1}" | \
    sed -r 's/([^|]*)\|([^|]*)/url=\1; branch=\2/')
  if [[ "${url}" == "" || "${branch}" == "" ]]; then
    echo "Invalid remote_url|branch format \"${1}\"" >&2
    exit 1
  fi
  git remote add -f "${2}" "${url}" 1>&2
  local local_branch="${2}/${branch}_local"
  git checkout -b "${local_branch}" "${2}/${branch}" 1>&2
  echo "${local_branch}"
}

main() {
  local upstream_docs="${UPSTREAM_DOCS_DEFAULT}"
  local upstream_master="${UPSTREAM_MASTER_DEFAULT}"
  local docs_subdir="${DOCS_SUBDIR_DEFAULT}"
  while getopts "u:d:s:h" option; do
    case ${option} in
      d ) upstream_docs="${OPTARG}";;
      u ) upstream_master="${OPTARG}";;
      s ) docs_subdir="${OPTARG}";;
      h ) usage;;
      * ) echo "Unrecognized option ${option}" >&2; usage;;
    esac
  done
  [[ "${upstream_docs}" == "" || "${upstream_master}" == "" || \
     "${docs_subdir}" == "" ]] && usage

  local -r temp_dir=$(mktemp -d ${script_name}.XXXXXX)
  cd "${temp_dir}"
  git init
  # Suppress warnings about changes to line endings.
  git config --local core.safecrlf false

  local -r master_branch=$( \
    add_remote_checkout_branch "${upstream_master}" upstream_master)
  local -r docs_branch=$( \
    add_remote_checkout_branch "${upstream_docs}" upstream_docs)
  git checkout "${master_branch}"
  git filter-branch --prune-empty --subdirectory-filter "${docs_subdir}"
  git checkout "${docs_branch}"
  git merge \
    -m "Merge branch ${upstream_master/*|/} into ${upstream_docs/*|/}" \
    "${master_branch}"
  echo "
The documentation branch has been staged in ${temp_dir}
for inspection.

When everything looks good, run the following command from
${temp_dir} to push the current branch to
${upstream_master}:

git push upstream_docs HEAD:${upstream_docs/*|/}
" >&2
}

main "${@}"
