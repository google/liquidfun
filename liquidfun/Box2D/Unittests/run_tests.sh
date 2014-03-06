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
declare -r project_directory="$(cd "${script_directory}/.."; pwd)"
cd "${project_directory}"
declare islinux=0
[[ "$(uname -s)" == "Linux" ]] && islinux=1

declare lcov_installed=0
declare code_coverage_stream=/dev/null

usage() {
  echo "
Execute unit tests on Linux or OSX.

Usage: $(basename $0) [-h] [-n] [-v] [-C] [-m] [-x] [-b build_args] [-t regexp]

-h: Display this help message.
-n: Disable the unit test build step.
-v: Display verbose code coverage output.
-C: Disable code coverage.
-m: Memcheck tests using valgrind (slow, linux only)
-x: Enable valgrind like -m, but with xml output for tools
-b: String which specifies the set of arguments to pass to build.sh if '-n'
    isn't specified.
-t: Extended regular expression which matches tests to execute.
    Defaults to '.*.
" >&2
  exit 1
}

# Parse arguments.
build=1
build_sh_args=
coverage_enabled=1
valgrind_enabled=0
valgrind_xml=0
test_filter='.*'
while getopts 'hnvCmxb:t:' option; do
  case ${option} in
    h) usage ;;
    n) build=0 ;;
    v) code_coverage_stream=/dev/stderr ;;
    C) coverage_enabled=0 ;;
    m) valgrind_enabled=1 ;;
    x) valgrind_xml=1 ;;
    b) build_sh_args="${OPTARG}";;
    t) test_filter="${OPTARG}";;
    *) usage ;;
  esac
done

# Build the tests if they're not built already and the build step isn't
# disabled.
if [[ $((build)) -ne 0 ]]; then
  build_coverage_flag=-c
  [[ $((coverage_enabled)) -eq 0 ]] && build_coverage_flag=
  ${script_directory}/../AutoBuild/build.sh -n \
        ${build_coverage_flag} ${build_sh_args} || exit $?
fi

valgrind=
valgrind_opts="-v --track-origins=yes"

if [[ $valgrind_xml -eq 1 ]]; then
  valgrind_enabled=1
fi

if [[ $valgrind_enabled -eq 1 ]]; then
  valgrind=$(which valgrind)
  if [[ -z "$valgrind" ]]; then
     echo "Error: valgrind not installed.  valgrind is required with -m." >&2
     exit 1
  fi
  valgrind="$valgrind $valgrind_opts"
fi

# Determine whether lcov is installed.
which lcov &>/dev/null && lcov_installed=1

if [[ $((lcov_installed)) -eq 0 && $((coverage_enabled)) -eq 1 ]]; then
  echo "\
Warning: LCOV not installed.  LCOV is required to generate the code coverage
report." >&2
fi

# Execute execute tests for each config.
declare failed=
declare -r build_configs="Debug Release"
for build_config in ${build_configs}; do
  test_executable_directory="${script_directory}/${build_config}"
  # Only try to run tests that have been built.
  if [[ ! -d "${test_executable_directory}" ]]; then
    continue
  fi

  # Create a symlink to the baselines directory so that it's possible
  # for HelloWorldTests to find golden files.
  if [[ ! -e "${test_executable_directory}/baselines" ]]; then
    ln -sf "${script_directory}/baselines" \
           "${test_executable_directory}/baselines"
  fi

  # Directory for code coverage results.
  code_coverage_report_dir="${test_executable_directory}/CodeCoverage"
  # File which contains the raw code coverage report.
  code_coverage_info="${test_executable_directory}/CodeCoverage.info"

  # build.sh renames CMakeFiles directories by appending _${build_config}
  # to each directory's name allowing multiple build configs to co-exist in
  # the same tree.  However, gcov instrumentation bakes absolute file paths in
  # the executables and provides only GCOV_PREFIX and GCOV_PREFIX_STRIP to
  # control the location of output data.  The following takes the easy path
  # and restores the object file directories prior to gathering coverage data.
  if [[ $((islinux)) -eq 1 ]]; then
    object_dir=CMakeFiles_${build_config}
    find . -depth -type d -name "${object_dir}" \( ! -path '*/CMakeTmp/*' \) \
      -exec ${SHELL} -c "cd {}/.. && mv ${object_dir} CMakeFiles" ';'
  fi

  # Reset all code coverage execution counters to zero and initialize raw
  # report data.
  if [[ $((coverage_enabled)) -eq 1 && $((lcov_installed)) -eq 1 ]]; then
    lcov --zerocounters --directory . 1> ${code_coverage_stream}
    lcov --capture --initial --directory . \
      --output-file "${code_coverage_info}" 1>${code_coverage_stream}

    # Create symlinks to the build config's .gcno files in the test runtime
    # directories.
    find . -type f -name '*.gcno' -path '*/CMakeFiles/*' | \
      sed 's@^\..*/CMakeFiles/\(.*\)/\([^/]*\)@'"\
"'mkdir -p ./Unittests/\1 \&\& ln -sf & ./Unittests/\1/\2@' | /bin/bash
  fi

  # Execute each test.
  for test_executable in $(find "${test_executable_directory}" \
                             -maxdepth 1 -type f -perm +111 \
                             -name '*Tests'); do
    testname="$(basename ${test_executable})"
    testname_config=${build_config}/${testname}

    if ! echo "${testname}" | grep -q "${test_filter}"; then
      echo "Skipping ${testname_config}" >&2
      continue
    fi

    if [[ -z "$valgrind" ]]; then
      vcmd=
    else
      vcmd="$valgrind --log-file=./valgrind_${testname_config}.txt"
      if [[ $((valgrind_xml)) -eq 1 ]]; then
        vcmd+=" --xml=yes --xml-file=./valgrind_${testname_config}.xml"
      fi
    fi

    echo "${testname_config}"
    ${vcmd} ${test_executable}
    if [[ $? -ne 0 ]]; then
      failed="${failed}${test_executable} "
    fi
  done

  # Generate code coverage report.
  if [[ $((coverage_enabled)) -eq 1 && $((lcov_installed)) -eq 1 ]]; then
    echo "Generating code coverage report..." >&2
    lcov --no-checksum --directory . --capture \
      --output-file "${code_coverage_info}" 1>${code_coverage_stream}
    genhtml -o "${code_coverage_report_dir}" "${code_coverage_info}" 1> \
      ${code_coverage_stream}
    echo "Code coverage report in"\
         "file://${code_coverage_report_dir}/index.html"
  fi

  if [[ $((islinux)) -eq 1 ]]; then
    # Rename the object file directories in the same way as build.sh.
    find . -depth -type d -name "CMakeFiles" -a \
        \( ! -path '*/CMakeTmp/*' \) \
        -exec ${SHELL} -c "cd {}/.. && mv CMakeFiles ${object_dir}" ';'
  fi
done

if [[ "${failed}" != "" ]]; then
  echo -e "\nERROR: The following tests failed:"
  for test_executable in ${failed}; do
    echo "  ${test_executable}"
  done
fi

