#!/bin/bash -eu
#
# Copyright 2013 Google. All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

# Upstream git repo to compare against.
: ${GIT_UPSTREAM:=../../../../external/box2d}
# Name of the git branch from the upstream repo to compare against.
: ${GIT_BRANCH:=master}

declare -r this_dir=$(dirname $0)
declare -r project_root=$(cd "${this_dir}/.."; pwd)
declare -r project_name=$(basename "${project_root}")
declare -r year=$(date +%Y)

declare -r copyright="\
Copyright ${year} Google. All rights reserved.

Use of this source code is governed by a BSD-style
license that can be found in the LICENSE file or at
https://developers.google.com/open-source/licenses/bsd"

# Files to add the copyright header to (re - regular expression), the
# comment style to use (cs / ce - line comment start / end or pcs / pce -
# paragraph comment start / end) and the number of lines to skip before
# adding the comment (skip).
declare -r files_comment_regexp='
re="\.cpp$"; cs="// "; ce=""; pcs=""; pce=""; skip=0;
re="\.c$"; cs="// "; ce=""; pcs=""; pce=""; skip=0;
re="\.h$"; cs="// "; ce=""; pcs=""; pce=""; skip=0;
re="\.mk$"; cs="# "; ce=""; pcs=""; pce=""; skip=0;
re="\.sh$"; cs="# "; ce=""; pcs=""; pce=""; skip=1;
re="\.bat$"; cs="@rem "; ce=""; pcs=""; pce=""; skip=0;
re="CMakeLists\.txt$"; cs="# "; ce=""; pcs=""; pce=""; skip=0;
re="/\.project$"; cs="     "; ce="";  pcs="<!-- "; pce=" -->"; skip=1;
re="\.xml$"; cs="     "; ce=""; pcs="<!-- "; pce=" -->"; skip=1;
'

# Display usage of this script.
usage() {
  echo "\
Add a copyright header to all Google authored source files.

Usage: $(basename $0) [upstream_git_repo] [upstream_git_branch]
" >&2
  exit 1
}

main() {
  # Parse arguments.
  while getopts "hg:b:" options; do
    case ${options} in
      g ) GIT_UPSTREAM="${1}";;
      b ) GIT_BRANCH="${2}";;
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
    # For all added files...
    for filename in $(git diff --summary ${remote_branch}..HEAD | \
                        awk '/create/ {
                               line=$0;
                               sub(/ *create  *mode  *[^ ][^ ]* */, "", line);
                               print line }'); do
      local check_file=0
      for comment_data in ${files_comment_regexp}; do
        eval "${comment_data}"
        # If the file should contain a copyright header.
        if echo ${filename} | grep -qE "${re}"; then
          check_file=1
          # If the copyright header isn't present.
          if ! head -n10 ${filename} | \
               grep -qE 'Copyright [^ ][^ ]* Google'; then
            # Add the copyright header to the file.
            echo "${filename}" >&2
            local new_filename="${filename}.new"
            head -n${skip} "${filename}" > "${new_filename}"
            echo -n "${pcs}" >> "${new_filename}"
            echo -n "${copyright}" | \
              sed 's~^~'"${cs}"'~;s~$~'"${ce}"'~;s~ *$~~' >> "${new_filename}"
            echo "${pce}" >> "${new_filename}"
            tail -n+$((skip + 1)) "${filename}" >> "${new_filename}"
            cat "${new_filename}" | sed 's@^\(<!-- \) *@\1@' > "${filename}"
            rm "${new_filename}"
          fi
        fi
      done
      if [[ $((check_file)) -eq 0 ]]; then
        echo "Skipping ${filename}" >&2
      fi
    done
  )
}

main "$@"
