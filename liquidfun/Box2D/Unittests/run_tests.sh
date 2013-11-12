#!/bin/bash
# Copyright 2013 Google. All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd
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
