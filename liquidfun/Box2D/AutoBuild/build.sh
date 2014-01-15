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

# Build this project on Linux or OSX.

declare -r project_name=LiquidFun
declare -r script_name=$(basename $0)
declare -r script_directory="$(cd "$(dirname "$0")"; pwd)"
declare -r android_root="${script_directory}/../../../../../.."
declare -r arch=$(uname -m)
declare -r os_name=$(uname -s)
declare -r os_name_lower=$(echo ${os_name} | tr 'A-Z' 'a-z')
declare -r cmake_minversion_minmaj=2.8
# Set this to "echo" to only display the build commands.
declare dryrun=

usage() {
  echo "\
Generate Makefiles or Xcode project for ${project_name} and build the specified
configuration.

Usage: ${script_name} [-h] [-n] [-b build_configuration] [build_configuration]

-h:
  Display this help message.
-n:
  Do not clean before building the project.
-d:
  Display the build commands this script would run without building.
-b build_configuration:
  build_configuration specifies the project configuration to build.
  Can be either \"debug\" or \"release\".  If not specified, defaults to
  \"${build_config}\".

build_configuration:
  Legacy form of '-b build_configuration'.
" >&2
  exit 1
}

# Find cmake and check the version.
find_cmake() {
  local cmake=$(which cmake)
  if [[ "${cmake}" == "" ]]; then
    if [[ "${os_name}" == "Darwin" ]]; then
      cmake=$(echo ${android_root}/prebuilts/cmake/darwin-${arch}/\
current/*.app/Contents/bin/cmake)
    else
      cmake=${android_root}/prebuilts/cmake/${os_name}-x86/current/bin/cmake
    fi
  fi
  if [[ ! -e "${cmake}" ]]; then
    echo "cmake not found ${cmake}." >&2
    exit 1
  fi
  declare -r cmake_version_minmaj=$("${cmake}" --version | \
                                    awk -F. '{ print $1 "." $2; }')
  if [[ ${cmake_version_minmaj} < 2.8 ]]; then
    echo "Found cmake ${cmake_version_minmaj}, ${project_name} requires" \
         "${cmake_minversion_minmaj} or above." >&2
    exit 1
  fi
  echo "${cmake}"
}

# Parse arguments.
build_config=release
clean=1

while getopts 'hndb:' option; do
  case ${option} in
    h) usage ;;
    n) clean=0 ;;
    b) build_config=${OPTARG};;
    d) dryrun=echo ;;
    *) usage ;;
  esac
done
# Manually parse legacy build config option.
shift $((OPTIND-1))
if [[ $# -gt 0 ]]; then
  build_config="${1}"
fi

# Validate the build configuration.
case ${build_config} in
  debug|release) ;;
  * ) echo "Invalid build config \"${build_config}\"." >&2
      exit 1 ;;
esac

declare -r cmake=$(find_cmake)

# Build the project.
cd "${script_directory}/.."
if [[ "${os_name}" == "Darwin" ]]; then
  declare xcodebuild_config=
  case ${build_config} in
    debug) xcodebuild_config="-configuration Debug" ;;
    release) xcodebuild_config="-configuration Release" ;;
  esac
  # Only rebuild the xcode project if the CMakeLists.txt file is newer.
  # Xcode will rebuild a big chunk of a project if the project file is
  # modified by cmake.
  # NOTE: This will incorrectly *not* result in the xcode project not being
  # rebuilt if any files CMakeLists.txt depends upon are modified.
  [[ $((clean)) -eq 1 || \
     ! -e Box2D.xcodeproj || \
     $(stat -f%m CMakeLists.txt) -gt $(stat -f%m Box2D.xcodeproj) ]] && \
    "${cmake}" -G'Xcode'
  eval "\
    [[ $((clean)) -ne 0 ]] && ${dryrun} xcodebuild ${xcodebuild_config} clean
    ${dryrun} xcodebuild ${xcodebuild_config}"
else
  declare makefile_config=
  declare cmake_config=
  case ${build_config} in
    debug)
      cmake_config='-DCMAKE_BUILD_TYPE=Debug'
      makefile_config="DEBUG=1"
      ;;
    release)
      cmake_config=
      makefile_config=
      ;;
  esac
  # Only rebuild the makefiles if the CMakeLists.txt file is newer than the
  # top level makefile.
  # NOTE: This will result in the xcode project not being rebuilt if any
  # files CMakeLists.txt depends upon are modified.
  [[ $((clean)) -eq 1 || \
     ! -e Makefile || \
     $(stat -c%Y CMakeLists.txt) -gt $(stat -c%Y Makefile) ]] && \
    "${cmake}" -G'Unix Makefiles' ${cmake_config}
  eval "\
    [[ $((clean)) -ne 0 ]] && ${dryrun} make ${makefile_config} clean
    ${dryrun} make -j$(awk '/^processor/ { n=$3 } END { print n + 1 }' \
                       /proc/cpuinfo) ${makefile_config}"
fi
