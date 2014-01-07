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

# NOTE: This does not use 'readlink -f' as it's not available on OSX.
declare -r script_directory="$(cd "$(dirname "$0")"; pwd)"
cd "${script_directory}"
declare lcovInstalled=0
which lcov 2>&1 && lcovInstalled=1

declare islinux=0
[[ "$(uname -s)" == "Linux" ]] && islinux=1
if [[ $((islinux)) -eq 1 ]]; then
  declare -r test_executable_directories=.
  declare -r codeCoverage_script_directories="${script_directory}/CMakeFiles"
else
  declare -r test_executable_directories="Debug Release"
  declare -r codeCoverage_script_directories="${script_directory}/Box2D.build"
fi

# Reset all execution counters to zero and cature initial coverage
if [[ -d "${codeCoverage_script_directories}" &&  $((lcovInstalled)) == 1 ]]; then
  lcov --zerocounters --directory "${codeCoverage_script_directories}"
  lcov --capture --initial --directory "${codeCoverage_script_directories}" \
    --output-file codeCoverage.info
fi

usage() {
  echo "
Execute unit tests on Linux or OSX.

Usage: $(basename $0) [-h] [-n]

-h: Display this help message.
-n: Disable the unit test build step.
" >&2
  exit 1
}

# Parse arguments.
build=1
while getopts 'hn' option; do
  case ${option} in
    h) usage ;;
    n) build=0 ;;
    *) usage ;;
  esac
done

# Build the tests if they're not built already and the build step isn't
# disabled.
if [[ $((build)) -ne 0 ]]; then
  declare build_configs="debug release"
  # cmake makefiles build all binaries to the same location irrespective of
  # build configuration so only build and test "debug" binaries on Linux.
  [[ $((islinux)) -eq 1 ]] && build_configs="debug"
  for build_config in ${build_configs}; do
    ${script_directory}/../AutoBuild/build.sh -n -b ${build_config} || exit $?
  done
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

# Capture the gcov data after the tests are run
if [[ $((lcovInstalled)) == 1 ]]; then
  mkdir -p "${script_directory}/CodeCoverage"
  lcov --no-checksum --directory "${script_directory}" --capture \
    --output-file codeCoverage.info
  genhtml -o "${script_directory}/CodeCoverage" codeCoverage.info
  coverageLocation="file://${script_directory}/CodeCoverage/index.html"
  echo "Check the code coverage data: ${coverageLocation}"
else
  echo "Warning: You need to have LCOV installed in order to generate" \
       "the test coverage report!" >&2
fi

