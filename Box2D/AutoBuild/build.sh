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

Usage: ${script_name} [-h] [-n] [-o output_directory] [-z archive] [-v]
         [-c] [-b build_configuration] [build_configuration]

-h:
  Display this help message.
-n:
  Do not clean before building the project.
-d:
  Display the build commands this script would run without building.
-b build_configuration:
  build_configuration specifies the project configuration to build using a
  whitespace separated list of the following:
    * \"debug\"
    * \"release\"
  If not specified, defaults to \"${build_configs}\".
-o output_directory:
  Directory to copy build artifacts to.
-z archive:
  Create a zip archive of build artifacts.
-c:
  Enable code coverage build.  WARNING it's not possible to build code
  coverage builds and non-code coverage builds side by side in the same
  tree.  All build targets should be cleaned before switching between these
  build configurations.
-v:
  Display verbose output.

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
  local -r cmake_version_minmaj=$("${cmake}" --version | \
                                    awk -F. '{ print $1 "." $2; }')
  if [[ ${cmake_version_minmaj} < ${cmake_minversion_minmaj} ]]; then
    echo "Found cmake ${cmake_version_minmaj}, ${project_name} requires" \
         "${cmake_minversion_minmaj} or above." >&2
    exit 1
  fi
  echo "${cmake}"
}

# Capitalize the string ${1}.
capitalize() {
  local -r string="${1}"
  # Capitalize the first letter (${string^} doesn't work with bash that
  # ships with OSX).
  echo $(echo ${string:0:1} | tr '[a-z]' '[A-Z]')${string:1}
}

# Filter xcodebuild output (stdin) to make it less verbose.
xcodebuild_verbose_filter() {
  grep -Ev '(cd |setenv |/bin/clang|Check dependencies|^$)'
}

# Build the project for OSX.
# $1: build configuration (title case).
# $2: cmake binary path.
# $3: whether to clean the project (1 = clean).
# $4: whether to print verbose output (1 = enable).
# $5: Addition flags for cmake.
build_osx() {
  local -r title_case_build_config="${1}"
  local -r cmake="${2}"
  local -r clean="${3}"
  local -r verbose="${4}"
  local -r cmake_flags="${5}"
  local status=
  local verbose_filter=xcodebuild_verbose_filter
  if [[ $((verbose)) -eq 1 ]]; then
    verbose_filter=cat
  fi
  # Only rebuild the xcode project if the CMakeLists.txt file is newer.
  # Xcode will rebuild a big chunk of a project if the project file is
  # modified by cmake.
  # NOTE: This will *not* result in the xcode project being
  # rebuilt if any files CMakeLists.txt depends upon are modified.
  [[ $((clean)) -eq 1 || \
     ! -e Box2D.xcodeproj || \
     $(stat -f%m CMakeLists.txt) -gt $(stat -f%m Box2D.xcodeproj) ]] && \
    "${cmake}" -G'Xcode' ${cmake_flags}
  if [[ $((clean)) -ne 0 ]]; then
    ${dryrun} xcodebuild -configuration ${title_case_build_config} clean | \
      ${verbose_filter}
    status=${PIPESTATUS[0]} && [[ $((status)) -ne 0 ]] && return $((status))
  fi
  ${dryrun} xcodebuild -configuration ${title_case_build_config} | \
      ${verbose_filter}
  status=${PIPESTATUS[0]} && [[ $((status)) -ne 0 ]] && return $((status))
  return 0
}

# Get a list of OSX build artifact base directories.
get_osx_build_artifact_dirs() {
  find . -type d -name Box2D.build | sed 's@/[^/]*$@@'
}

# Build the project for Linux.
# $1: build configuration (title case).
# $2: cmake binary path.
# $3: whether to clean the project (1 = clean).
# $4: whether to print verbose output (1 = enable).
# $5: Additional flags for cmake.
build_linux() {
  local -r title_case_build_config="${1}"
  local -r cmake="${2}"
  local -r clean="${3}"
  local -r verbose="${4}"
  local -r cmake_flags="${5}"
  local -r cmake_config="-DCMAKE_BUILD_TYPE=${title_case_build_config} \
                         ${cmake_flags}"
  local verbose_arg=

  if [[ $((verbose)) -eq 1 ]]; then
    verbose_arg="VERBOSE=1"
  fi

  # The cmake makefile generator places intermediate files for different build
  # configurations in the same output directories which prevents the user from
  # building multiple configurations in the same directory.
  # To work around this issue the following renames the intermediate files
  # to a name based upon the config after the build.  Then in
  # subsequent builds the names of the intermediate files are restored prior
  # to building the project.
  local -r intermediate_files="\
    cmake_install.cmake CMakeCache.txt CMakeFiles Makefile \
    config-linux.h config.h flags.cmake freeglut.pc"

  # Restore intermediate files for this build configuration if they're present.
  find -depth '(' $(echo "${intermediate_files}" | \
            sed -E 's@([^ ]+)@-name \1'"_${title_case_build_config}"' -o @g;
                    s@ -o $@@') ')' \
       -exec ${SHELL} -c \
         'target="$(echo "{}" | \
                    sed -E '\''s@(.*)_'"${title_case_build_config}"'@\1@'\'')";
          rm -rf "${target}"; \
          mv "{}" "${target}"' ';'

  # Only rebuild the makefiles if the CMakeLists.txt file is newer than the
  # top level makefile.
  # NOTE: This will result in the xcode project not being rebuilt if any
  # files CMakeLists.txt depends upon are modified.
  [[ $((clean)) -eq 1 || \
     ! -e Makefile || \
     $(stat -c%Y CMakeLists.txt) -gt $(stat -c%Y Makefile) ]] && \
    "${cmake}" -G'Unix Makefiles' ${cmake_config}

  [[ $((clean)) -ne 0 ]] && ${dryrun} make clean ${verbose_arg}
  ${dryrun} make -j$(awk '/^processor/ { n=$3 } END { print n + 1 }' \
                       /proc/cpuinfo) ${verbose_arg}

  # Rename intermediate files for this build configuration.
  find -depth '(' $(echo "${intermediate_files}" | \
                   sed -E 's@([^ ]+)@-name \1 -o @g;s@ -o $@@') ')' \
       -exec ${SHELL} -c 'mv "{}" "{}_'"${title_case_build_config}"'"' ';'
}

# Get a list of Linux build artifact base directories.
get_linux_build_artifact_dirs() {
  find . -type d -path '*/CMakeFiles_*/*.dir' | \
    sed 's@/CMakeFiles_.*@@' | \
    sort | \
    uniq
}

# Copy build artifacts to an output directory.
# $1: build configuration (title case).
# $2: Newline separated list of build artifact base directories.
# $3: Output directory.
copy_to_output() {
  local -r title_case_build_config="${1}"
  local -r base_artifact_dirs="${2}"
  local -r output_dir="${3}"
  mkdir -p "${output_dir}"
  (
    IFS=$'\n'
    for base_dir in ${base_artifact_dirs}; do
      local artifact_dir="${base_dir}/${title_case_build_config}"
      if [[ -d "${artifact_dir}" ]]; then
        # NOTE: This requires that ${artifact_dir} is a relative
        # path underneath the current directory.
        artifact_output="${output_dir}/${artifact_dir}"
        artifact_output_dir="$(dirname "${artifact_output}")"
        mkdir -p "${artifact_output_dir}"
        cp -a "${artifact_dir}" "${artifact_output_dir}"
      fi
    done
  )
}

main() {
  # Parse arguments.
  local build_configs="debug release"
  local clean=1
  local output_dir=
  local delete_dirs=
  local archive=
  local verbose=0
  local cmake_flags=

  while getopts 'hndb:o:z:cv' option; do
    case ${option} in
      h) usage ;;
      n) clean=0 ;;
      b) build_configs="${OPTARG}";;
      d) dryrun=echo ;;
      o) mkdir -p "${OPTARG}"
         output_dir="$(cd ${OPTARG} && pwd)";;
      z) archive="${OPTARG}";;
      v) verbose=1;;
	  c) cmake_flags="${cmake_flags} -DBOX2D_CODE_COVERAGE=ON";;
      *) usage ;;
    esac
  done
  # Manually parse legacy build config option.
  shift $((OPTIND-1))
  if [[ $# -gt 0 ]]; then
    build_configs="${1}"
  fi

  # If an archive was specified with no output directory, create a temporary
  # output directory.
  if [[ "${archive}" != "" && "${output_dir}" == "" ]]; then
    archive="$(cd "$(dirname "${archive}")" ; \
               echo $(pwd)/$(basename "${archive}"))"
    output_dir=$(mktemp -d XXXXXX)
    output_dir="$(cd "${output_dir}" && pwd)"
    delete_dirs="${delete_dirs} ${output_dir}"
  fi

  # Validate the build configuration argument.
  if [[ "$(echo "${build_configs}" | tr ' ' '\n' | \
           grep -vE '(debug|release)')" != "" ]]; then
    echo "Invalid build config ${build_configs}" >&2
    exit 1
  fi

  # Find the cmake binary.
  local cmake
  cmake=$(find_cmake)

  # Change into the project's root directory for the cmake build.
  cd "${script_directory}/.."

  for build_config in ${build_configs}; do
    local title_case_build_config=$(capitalize "${build_config}")

    # Build the project.
    if [[ "${os_name}" == "Darwin" ]]; then
      build_osx "${title_case_build_config}" "${cmake}" $((clean)) \
                $((verbose)) "${cmake_flags}"
      base_artifact_dirs="$(get_osx_build_artifact_dirs)"
    else
      build_linux "${title_case_build_config}" "${cmake}" $((clean)) \
                  $((verbose)) "${cmake_flags}"
      base_artifact_dirs="$(get_linux_build_artifact_dirs)"
    fi

    # If an output directory was specified, copy the build artifacts there.
    if [[ "${output_dir}" != "" ]]; then
      copy_to_output "${title_case_build_config}" "${base_artifact_dirs}" \
                     "${output_dir}"
    fi
  done

  # Create the archive of build artifacts.
  if [[ "${archive}" != "" ]]; then
    pushd "${output_dir}" >/dev/null
    rm -f "${archive}"
    zip -r "${archive}" .
    popd >/dev/null
  fi

  # Clean up temporary directories.
  for delete_dir in ${delete_dirs}; do
    rm -rf "${delete_dir}"
  done
}

main "$@"

