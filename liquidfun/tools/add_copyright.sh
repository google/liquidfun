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

# ***** For script usage run "add_copyright.sh -h". ******

# Upstream git repo to compare against.
: ${GIT_UPSTREAM:=../../../../external/box2d}
# Name of the git branch from the upstream repo to compare against.
: ${GIT_BRANCH:=master}

declare -r this_dir=$(dirname $0)
declare -r project_root=$(cd "${this_dir}/.."; pwd)
declare -r project_name=$(basename "${project_root}")
declare -r year=$(date +%Y)

declare -r existing_copyright_regexp='Copyright [^ ][^ ]*'
declare -r copyright_regexp='Copyright \(c\) [^ ][^ ]* Google'
declare -r copyright_title="\
Copyright (c) ${year} Google, Inc."
declare -r copyright="\
${copyright_title}

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
"

# Files to add the copyright header to (re - regular expression), the
# comment style to use (cs / ce - line comment start / end or pcs / pce -
# paragraph comment start / end) and the number of lines to skip before
# adding the comment (skip).
declare -r files_comment_regexp='
re="\.cpp$"; cs="* "; ce=""; pcs="/*\n"; pce="*/\n"; skip=0;
re="\.c$"; cs="* "; ce=""; pcs="/*\n"; pce="*/\n"; skip=0;
re="\.h$"; cs="* "; ce=""; pcs="/*\n"; pce="*/\n"; skip=0;
re="\.mk$"; cs="# "; ce=""; pcs=""; pce=""; skip=0;
re="\.sh$"; cs="# "; ce=""; pcs=""; pce=""; skip=1;
re="\.bat$"; cs="@rem "; ce=""; pcs=""; pce=""; skip=0;
re="CMakeLists\.txt$"; cs="# "; ce=""; pcs=""; pce=""; skip=0;
re="/\.project$"; cs="     "; ce="";  pcs="<!-- "; pce=" -->\n"; skip=1;
re="\.xml$"; cs="     "; ce=""; pcs="<!-- "; pce=" -->\n"; skip=1;
'

# Display usage of this script.
usage() {
  echo "\
Add a copyright header to *all* Google authored and modified source files.

This script adds a copyright header to all files found in...
  ${project_root}
... that have been modified in this project's git repository with respect to
the specified upstream git repository.

This script uses git to perform the diff between the upstream repository and
this project so it requires this project to be a valid git workspace.

In the case of files with no copyright header the following header is added:
\"
${copyright}
\"

Where files already have a copyright header and have been modified by Google
the following is inserted in the header:
\"
${copyright_title}
\"

Source files matching the following regular expressions are modified:
$(echo "${files_comment_regexp}" | sed 's/;.*//;s///g;s/re=/  /')

Usage: $(basename $0) [-g upstream_git_repo] [-b upstream_git_branch]

-g upstream_git_repo:
  upstream_git_repo is the location of the upstream git repository to compare
  against.  If this isn't specified it defaults to
  ${project_root}/${GIT_UPSTREAM}

-b upstream_git_branch:
  upstream_git_branch specifies the branch in the upstream_git_repo to compare
  against.  If this isn't specified it defaults to ${GIT_BRANCH}.

For example, assuming we're using the default git upstream repo and branch:
$ cd ${project_root}
$ pushd ../../../../external/box2d
$ git checkout -b master origin/master
$ popd
$ ./tools/add_copyright.sh
$ git status -s
# Displays a list of modified files.
" >&2
  exit 1
}

main() {
  # Parse arguments.
  while getopts "hg:b:" options; do
    case ${options} in
      g ) GIT_UPSTREAM="${OPTARG}";;
      b ) GIT_BRANCH="${OPTARG}";;
      h ) usage;;
    esac
  done

  # Synchronize the remote branch.
  cd ${project_root}
  local -r remote_name=${project_name}-upstream
  local -r remote_branch=${remote_name}/${GIT_BRANCH}
  if git remote | grep -qF ${remote_name}; then
    git remote rm ${remote_name}
  fi
  git remote add ${remote_name} ${GIT_UPSTREAM}
  git fetch ${remote_name}

  (
    IFS=$'\n'
	# For all modified files...
    # NOTE: -b (ignore whitespace) doesn't work with --name-only so this
    # ignores whitespace then extracts the filenames from the diff output.
	for filename in $(git diff --diff-filter=M -b ${remote_branch}..HEAD | \
                      awk '/^diff / { f=$4; sub(/b\//, "", f); print f }' ); do
      local check_file=0
      for comment_data in ${files_comment_regexp}; do
        eval "${comment_data}"
        # If the file should contain a copyright header.
        if echo "${filename}" | grep -qE "${re}"; then
          # If a license header is present but the Google copyright isn't.
          if head -n10 "${filename}" | \
             grep -qE "${existing_copyright_regexp}"; then
            if ! head -n10 "${filename}" | grep -qE "${copyright_regexp}"; then
              check_file=1
              echo "${filename} - insert copyright" >&2
              local new_filename="${filename}.new"
              awk '{ print $0 } /'"${existing_copyright_regexp}"'/ { exit }' \
                "${filename}" > "${new_filename}"
              echo -e "${cs}${copyright_title}${ce}" >> "${new_filename}"
              awk '/'"${existing_copyright_regexp}"'/ { p = 1; next; }
                   { if (p) { print $0 } }' "${filename}" >> "${new_filename}"
              cat "${new_filename}" | sed 's@^\(<!-- \) *@\1@' > "${filename}"
              rm "${new_filename}"
            fi
          fi
        fi
      done
      if [[ $((check_file)) -eq 0 ]]; then
        echo "${filename} - skipped modified file" >&2
      fi
    done

    # For all added files...
    for filename in $(git diff --name-only --diff-filter=A \
                        ${remote_branch}..HEAD); do
      local check_file=0
      for comment_data in ${files_comment_regexp}; do
        eval "${comment_data}"
        # If the file should contain a copyright header.
        if echo "${filename}" | grep -qE "${re}"; then
          check_file=1
          # If the copyright header isn't present.
          if ! head -n10 "${filename}" | \
               grep -qE 'Copyright [^ ][^ ]* [^ ][^ ]* Google'; then
            # Add the copyright header to the file.
            echo "${filename} - add copyright" >&2
            local new_filename="${filename}.new"
            head -n${skip} "${filename}" > "${new_filename}"
            echo -ne "${pcs}" >> "${new_filename}"
            echo -ne "${copyright}" | \
              sed 's~^~'"${cs}"'~;s~$~'"${ce}"'~;s~ *$~~' >> "${new_filename}"
            echo -ne "${pce}" >> "${new_filename}"
            tail -n+$((skip + 1)) "${filename}" >> "${new_filename}"
            cat "${new_filename}" | sed 's@^\(<!-- \) *@\1@' > "${filename}"
            rm "${new_filename}"
          fi
        fi
      done
      if [[ $((check_file)) -eq 0 ]]; then
        echo "${filename} - skipped added file" >&2
      fi
    done
  )
}

main "$@"
