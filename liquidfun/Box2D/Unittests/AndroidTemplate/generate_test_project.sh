#!/bin/bash -eu
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
# Genrate an Android project from the template files in this script's
# directory.

declare -r script_name="$(basename $0)"
declare -r script_dir="$(dirname $0)"

# Display the script usage and exit.
usage() {
  echo "\
Usage:
${script_name} test_name output_directory
${script_name} all

For example, to generate an Android NDK project for the HelloWorldTests unit
tests in the Box2D/Unittests/HelloWorld directory:
  ${script_name} HelloWorldTests Box2D/Unittests/HelloWorld

To generate Android NDK projects for all unit tests do:
  ${script_name} all
" >&2
  exit 1
}

# Generate a unit test project from the template files in this script's
# directory.
generate_project() {
  local -r test_name="${1}"
  local -r output_dir="${2}"
  echo "Generating project ${test_name} in ${output_dir}" >&2
  (
    IFS=$'\n'
    for f in $(find "${script_dir}" -type f | grep -vF "${script_name}"); do
      output_file=$( \
        echo "${f}" | \
        sed -E 's@'"${script_dir}"'@'"${output_dir}"'@;
                s@(/[^/]+)Ignore(.*)@\1\2@g')
    file_output_dir=$(dirname "${output_file}")
    mkdir -p "${file_output_dir}"
    cat "${f}" | \
      sed "s@TEST_NAME_TO_REPLACE_LOWER@$(echo ${test_name} | \
                                            tr 'A-Z' 'a-z')@g;
             s@TEST_NAME_TO_REPLACE@${test_name}@g;" > \
        "${output_file}"
    done
  )
}

# Generate projects for all unit tests in the parent directory of this
# script's directory.
generate_all_projects() {
  local test_dir
  cd "${script_dir}/.."
  (
    IFS=$'\n'
    for test_dir in $(find . -mindepth 1 -maxdepth 1 -type d | \
                      grep -vE '(baselines|AndroidTemplate)' | \
                      sed 's@^\./@@'); do
      generate_project ${test_dir}Tests ${test_dir}
    done
  )
}

# Validate arguments.
if [[ $# -eq 1 ]]; then
  if [[ "${1}" != "all" ]]; then
    usage
  fi
elif [[ $# -ne 2 ]]; then
  usage
fi

# Generate project(s).
if [[ "${1}" == "all" ]]; then
  generate_all_projects
else
  generate_project "${1}" "${2}"
fi
