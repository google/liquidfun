#!/bin/bash -eu
#
# Build this project on Linux or OSX.

declare -r project_name=Splash2D
declare -r script_name=$(basename $0)
declare -r script_directory="$(cd "$(dirname "$0")"; pwd)"
declare -r android_root="${script_directory}/../../../../../.."
declare -r arch=$(uname -m)
declare -r os_name=$(uname -s)
declare -r os_name_lower=$(echo ${os_name} | tr 'A-Z' 'a-z')
declare -r cmake_minversion_minmaj=2.8

usage() {
  echo "\
Generate Makefiles or Xcode project for ${project_name} and build the specified
configuration.

Usage: ${script_name} [build_configuration]

build_configuration: Project configuration to build.  Can be either \"debug\"
or \"release\".  If not specified, defaults to \"${build_config}\".
" >&2
  exit 1
}

# Find cmake and check the version.
find_cmake() {
  local cmake=$(which cmake)
  if [[ "${cmake}" == "" ]]; then
    if [[ "${os_name}" == "Darwin" ]]; then
      cmake=$(echo ${android_root}/prebuilts/cmake/${os_name}-${arch}/\
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
if [[ $# -gt 0 ]]; then
  case "${1}" in
    debug|release) build_config=${1} ;;
    *) usage ;;
  esac
fi

declare -r cmake=$(find_cmake)

# Build the project.
cd "${script_directory}/.."
if [[ "${os_name}" == "Darwin" ]]; then
  declare xcodebuild_config=
  case ${build_config} in
    debug) xcodebuild_config="-configuration Debug" ;;
    release) xcodebuild_config="-configuration Release" ;;
  esac
  "${cmake}" -G'Xcode'
  eval "\
    xcodebuild ${xcodebuild_config} clean
    xcodebuild ${xcodebuild_config}"
else
  declare makefile_config=
  case ${build_config} in
    debug) makefile_config="DEBUG=1" ;;
    release) makefile_config= ;;
  esac
  "${cmake}" -G'Unix Makefiles'
  eval "\
    make ${makefile_config} clean
    make -j$(awk '/^processor/ { n=$3 } END { print n + 1 }' /proc/cpuinfo) \
           ${makefile_config}"
fi
