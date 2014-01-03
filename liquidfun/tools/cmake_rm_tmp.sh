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

usage() {
  echo "\
If you run cmake on a network mount, and then re-run cmake from a different 
computer on the same network mount, cmake will fail if the mount paths are 
different.  e.g. OSX vs Windows

This script removes all temporary cmake files that interfere with re-running 
cmake from a different computer.  It does *not* remove the cmake generated 
build files.  

It operates recursively from the current directory.  Run this script from 
the same directory that you run cmake in.

Usage: $(basename $0) [-p] [-h]
  -p preview mode - show commands but don't execute them
  -h help - display this message
" >&2
  exit 1
}

declare preview=0

# echo rm command line
# if we're *not* in preview mode, execute the command
process() {
  echo "$@"
  if [[ $((preview)) -eq 0 ]]; then
    $("$@")
  fi
}

main() {
  # Parse arguments
  while getopts "hp" options; do
    case ${options} in
    h) usage;;
    p) preview=1;;
    esac
  done

  declare -r files=$(find . -type f -a \( -name CMakeCache.txt -o -name \
    cmake_install.cmake -o -name flags.cmake \))
  for f in $files; do
    process rm ${f}
  done

  # -depth option is necessary to ensure that 
  # .../CMakeFiles/CMakeTmp/CMakeFiles
  # is output before
  # .../CMakeFiles
  # so that all rm operations succeed
  local -r dirs=$(find . -depth -type d -a \( -name CMakeFiles -o -name \
    CMakeScripts \))
  for d in $dirs; do
    process rm -r ${d}
  done
}

main "$@"
