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
declare islinux=0
[[ "$(uname -s)" == "Linux" ]] && islinux=1

declare lcov_installed=0
declare code_coverage_stream=/dev/null

usage() {
  echo "
Execute unit tests on Linux or OSX.

Usage: $(basename $0) [-h] [-n] [-v] [-C]

-h: Display this help message.
-n: Disable the unit test build step.
-v: Display verbose code coverage output.
-C: Disable code coverage.
" >&2
  exit 1
}

# Parse arguments.
build=1
coverage_enabled=1
while getopts 'hnvC' option; do
  case ${option} in
    h) usage ;;
    n) build=0 ;;
    v) code_coverage_stream=/dev/stderr ;;
    C) coverage_enabled=0 ;;
    *) usage ;;
  esac
done

# Build the tests if they're not built already and the build step isn't
# disabled.
if [[ $((build)) -ne 0 ]]; then
  ${script_directory}/../AutoBuild/build.sh -n || exit $?
fi

# Determine whether lcov is installed.
which lcov &>/dev/null && lcov_installed=1

if [[ $((lcov_installed)) -eq 0 && $((coverage_enabled)) -eq 1 ]]; then
  echo "\
Warning: LCOV not installed.  LCOV is required to generate the code coverage
report." >&2
fi

# Objects on Linux and OSX are placed underneath this script's directory
# so strip the directory components up to the object file directory
# (script directory components + 1) to make it possible to redirect
# the profile output to object directories that are renamed by build.sh on
# Linux.
export GCOV_PREFIX_STRIP=$(($(echo "${script_directory}" | sed 's@[^/]@@g' | \
                              wc -c)))

# Execute execute tests for each config.
declare failed=
declare -r test_executable_directories="Debug Release"
for test_executable_directory in ${test_executable_directories}; do
  if [[ -d "${test_executable_directory}" ]]; then
    # Directory for code coverage results.
    code_coverage_report_dir="${test_executable_directory}/CodeCoverage"
    # File which contains the raw code coverage report.
    code_coverage_info="${test_executable_directory}/CodeCoverage.info"

    # Directory containing object files required for code coverage.
    if [[ $((islinux)) -eq 1 ]]; then
      object_dir="${script_directory}/CMakeFiles_${test_executable_directory}"
    else
      object_dir="${script_directory}/Box2D.build"
    fi

    # Reset all code coverage execution counters to zero and initialize raw
    # report data.
    if [[ $((coverage_enabled)) -eq 1 && $((lcov_installed)) -eq 1 && \
          -d "${object_dir}" ]]; then
      lcov --zerocounters --directory "${object_dir}" \
        1> ${code_coverage_stream}
      lcov --capture --initial --directory "${object_dir}" \
         --output-file "${code_coverage_info}" 1>${code_coverage_stream}
    fi

    if [[ ! -e "${test_executable_directory}/baselines" ]]; then
      # Create a symlink to the baselines directory so that it's possible
      # for HelloWorldTests to find golden files.
      ln -sf ../baselines "${test_executable_directory}/baselines"
    fi

    # Change the default output location for the code coverage data.
    export GCOV_PREFIX="${object_dir}"
    # Execute each test.
    for test_executable in $(find "${test_executable_directory}" \
                               -maxdepth 1 -type f -perm +111 \
                               -name '*Tests'); do
      echo ${test_executable}
      ${test_executable}
      if [[ $? -ne 0 ]]; then
        failed="${failed}${test_executable} "
      fi
    done

    # Generate code coverage report.
    if [[ $((coverage_enabled)) -eq 1 && $((lcov_installed)) -eq 1 ]]; then
      lcov --no-checksum --directory "${object_dir}" --capture \
        --output-file "${code_coverage_info}" 1>${code_coverage_stream}
      genhtml -o "${code_coverage_report_dir}" "${code_coverage_info}" 1> \
        ${code_coverage_stream}
      echo "Code coverage report in \
file://${script_directory}/${code_coverage_report_dir}/index.html"
    fi
  fi
done

if [[ "${failed}" != "" ]]; then
  echo -e "\nERROR: The following tests failed:"
  for test_executable in ${failed}; do
    echo "  ${test_executable}"
  done
fi

