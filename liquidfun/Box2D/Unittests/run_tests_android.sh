#!/bin/bash -eu

# Whether to build each unit test package before trying to run it.
#
# Use the following to disable the build of unit tests.
# BUILD_ENABLED=0 ./run_tests_android.sh
: ${BUILD_ENABLED:=1}

# NOTE: This does not use 'readlink -f' as it's not available on OSX.
declare -r script_name="$(basename $0)"
declare -r script_directory="$(cd "$(dirname "$0")"; pwd)"
# Temporary file for the test log.
declare -r test_log="$(mktemp /tmp/${script_name}.XXXXXXXX)"

# List of tests that aren't currently working on Android.
declare -r test_blacklist="HelloWorld"

# Execute the build_apk script.
build_apk() {
  "${script_directory}/../AndroidUtil/build_apk.sh" "$@"
}

# Get the test directories, assumes the current directory is script_directory.
get_test_directories() {
  local extended_re=$(( uname -s | grep -q Darwin ) && echo E || echo r)
  find . -maxdepth 2 -type f -name 'AndroidManifest.xml' | \
    sed -${extended_re} 's@^\./@@;s@/[^/]+$@@' | \
    grep -Fv "${test_blacklist}"
}

# Get the test name from a test directory's name.
get_test_name() {
  echo ${1}Tests
}

# Clean up temporary file if the script exits prematurely.
trap "rm -f ${test_log} 2>/dev/null" SIGINT SIGTERM EXIT

cd "${script_directory}"

declare -r test_dirs="$(get_test_directories)"
declare succeeded=
declare failed=
# For each test directory, build each test, push to the device, run the test
# and determine whether it passes / fails.
for test_dir in ${test_dirs}; do
  test_name=$(get_test_name ${test_dir})
  echo "${test_name} ..." >&2
  pushd "${test_dir}" >/dev/null
  # The Android framework can cause an application to restart multiple times
  # so only capture the log from the first instance.
  if [[ $((BUILD_ENABLED)) -eq 1 ]]; then
    build_apk "$@" LAUNCH=0
  fi
  build_apk "$@" BUILD=0 | \
    awk '/^I\/'"${test_name}"'/ {
           log_prefix = $0
           sub(/:.*/, "", log_prefix)
           if (!current_log_prefix) {
             current_log_prefix = log_prefix
           }
           if (current_log_prefix != log_prefix) {
             next
           }
           print $0
         }' | \
    tee "${test_log}"

  # Scrape the log to determine whether the test failed.
  if [[ "$(grep '\[ *FAILED *\]' "${test_log}")" == "" ]]; then
    succeeded="${test_name} ${succeeded}"
  else
    failed="${test_name} ${failed}"
  fi
  popd >/dev/null
done

# Display a summary of passing / failing tests.
declare -r number_of_tests=$(echo "${test_dirs}" | wc -w)
declare -r passing=$(echo "${succeeded}" | wc -w)
declare -r failing=$(echo "${failed}" | wc -w)
if [[ "${succeeded}" != "" ]]; then
  echo -e "\nThe following tests passed (${passing}/${number_of_tests}):"
  for test_name in ${succeeded}; do
    echo "  ${test_name}"
  done
fi
if [[ "${failed}" != "" ]]; then
  echo -e "\nERROR: The following tests failed"\
          "(${failing}/${number_of_tests}):"
  for test_name in ${failed}; do
    echo "  ${test_name}"
  done
fi
