#!/bin/bash
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
# Execute unit tests on Linux or OSX.

# NOTE: This does not use 'readlink -f' as it's not available on OSX.
declare -r script_directory="$(cd "$(dirname "$0")"; pwd)"
cd "${script_directory}"
if [[ "$(uname -s)" == "Linux" ]]; then
  declare -r test_executable_directories=.
else
  declare -r test_executable_directories="Debug Release"
fi

declare failed=
for test_executable_directory in ${test_executable_directories}; do
  if [[ -d "${test_executable_directory}" ]]; then
    if [[ ! -e "${test_executable_directory}/baselines" ]]; then
      # Create a symlink to the baselines directory so that it's possible
      # for HelloWorldTests to find golden files.
      ln -sf ../baselines "${test_executable_directory}/baselines"
    fi
    for test_executable in $(find "${test_executable_directory}" \
                             -maxdepth 1 -type f -perm +111 \
                             -name '*Tests'); do
      echo ${test_executable}
      ${test_executable}
      if [[ $? -ne 0 ]]; then
        failed="${failed}${test_executable} "
      fi
    done
  fi
done

if [[ "${failed}" != "" ]]; then
  echo -e "\nERROR: The following tests failed:"
  for test_executable in ${failed}; do
    echo "  ${test_executable}"
  done
fi
