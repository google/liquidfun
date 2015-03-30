#!/bin/bash -eu
# Copyright (c) 2014 Google, Inc.
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

declare -r script_directory="$(cd "$(dirname "$0")"; pwd)"

: ${PATH_ARGS:="-o -z"}
: ${EXECUTABLE:=${script_directory}/build.bat}

# Bash wrapper around build.bat to enable command line builds from Cygwin
# without having to translate from Cygwin to Windows native paths.

main() {
  # Arguments for the build.bat script which take a path.
  local arguments=
  local arg=

  while (( $# )); do
    local added_arg=0
    arg="${1}"
    shift 1
    if [[ "${arg:0:1}" == "-" ]]; then
      for path_arg in ${PATH_ARGS}; do
        if [[ "${path_arg}" == "${arg}" ]]; then
          arguments="${arguments} ${arg} \"$(cygpath -w "${1}")\""
          shift 1
          added_arg=1
          break
        fi
      done
    fi
    if [[ $((added_arg)) -eq 0 ]]; then
      arguments="${arguments} ${arg}"
    fi
  done
  eval "${EXECUTABLE} ${arguments}"
  exit $?
}

main "${@}"

