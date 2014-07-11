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
# Build the project for Android on Linux or OSX.

declare -r project_name=LiquidFun
declare -r script_name=$(basename $0)
declare -r script_directory="$(cd "$(dirname "$0")"; pwd)"
declare -r project_root="$(cd "${script_directory}/../.."; pwd)"
declare -r android_root=${script_directory}/../../../../../../

declare -r arch=$(uname -m)
declare -r os_name=$(uname -s)
declare -r os_name_lower=$(echo ${os_name} | tr 'A-Z' 'a-z')
declare -r swig_root=${android_root}/prebuilts/misc

# Set this to "echo" to only display the build commands.
declare dryrun=

usage() {
  echo "\
Build all ${project_name} libraries and applications for Android.

Usage: ${script_name} [-h] [-n] [-a abi] [-o output_directory] [-z archive] \
[-t tests_archive] [-v] [-p projects] [-b build_configuration] \
[build_configuration]

-h:
  Display this help message.
-n:
  Do not clean before building the project.
-a abi:
  ABIs to build.  Defaults to \"${build_abi}\".
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
-t tests_archive:
  Create a zip archive for unittest apks.
-p projects:
  Whitespace separated list of projects to build.
-v:
  Display verbose output.

build_configuration:
  Legacy form of '-b build_configuration'.

e.g. ${script_name} -a \"x86 mips\" -b \"debug\" -z my_archive.zip -p \
\"Box2D/Testbed Box2D/EyeCandy\"
" >&2
  exit 1
}

# Outputs the swig binary and include dirs with a separator.
find_swig() {
  local swig_bin=$(which swig)
  local swig_lib=""
  if [[ "${swig_bin}" == "" ]]; then
    if [[ "${os_name_lower}" == "darwin" ]]; then
      swig_bin=${swig_root}/darwin-${arch}/swig/swig
    else
      swig_bin=${swig_root}/${os_name_lower}-x86_64/swig/swig
    fi
    swig_lib=${swig_root}/common/swig/include/
    swig_lib+=$(${swig_bin} -version | awk '/SWIG Version/ { print $3 }')
  fi
  if [[ ! -e "${swig_bin}" ]]; then
    echo "swig not found ${swig_bin}." >&2
    exit 1
  fi
  # If no libs are set, we set to the default
  if [[ "${swig_lib}" == "" ]]; then
    swig_lib=$("${swig_bin}" -swiglib)
  fi
  echo "${swig_bin}:${swig_lib}"
}

# Build the project in the current working directory.
build_apk() {
  # Find the swig binary.
  local swig
  swig=$(find_swig)
  local -r swig_bin=${swig/:*/}
  local -r swig_lib=${swig/*:/}
  ${dryrun} ${project_root}/Box2D/AndroidUtil/build_apk.sh DEPLOY=0 LAUNCH=0 \
    "$@" "SWIG_BIN=${swig_bin}" "SWIG_LIB=${swig_lib}"
}

# Find all projects to build relative to ${project_root}.
find_projects() {
  (
    pushd "${project_root}" >/dev/null
    IFS=$'\n'
    for manifest in $(find Box2D -type f -name AndroidManifest.xml | sort); do
      target_dir=$(dirname "${manifest}")
      if [[ ! -d "${target_dir}/jni" ]]; then
        continue
      fi
      echo "${target_dir}"
    done
    popd >/dev/null
  )
}


main() {
  # Parse arguments.
  local build_configs="debug release"
  local build_abi="all"
  local clean=1
  local output_dir=
  local archive=
  local tests_archive=
  local delete_dirs=
  local verbose=0
  local projects_to_build=

  while getopts 'hnda:b:o:z:t:vp:' option; do
    case ${option} in
      h) usage ;;
      n) clean=0 ;;
      a) build_abi="${OPTARG}";;
      b) build_configs="${OPTARG}";;
      d) dryrun=echo ;;
      o) mkdir -p "${OPTARG}"
         output_dir="$(cd ${OPTARG} && pwd)";;
      z) archive="${OPTARG}";;
      t) tests_archive="${OPTARG}";;
      v) verbose=1;;
      p) projects_to_build=$(echo ${OPTARG} | tr ' ' '\n');;
      *) usage ;;
    esac
  done
  # Manually parse legacy build config option.
  shift $((OPTIND-1))
  if [[ $# -gt 0 ]]; then
    build_configs="${1}"
  fi
  # Validate the build configuration argument.
  if [[ "$(echo "${build_configs}" | tr ' ' '\n' | \
           grep -vE '(debug|release)')" != "" ]]; then
    echo "Invalid build config ${build_configs}" >&2
    exit 1
  fi

  # If an archive was specified with no output directory, create a temporary
  # output directory.
  if [[ ("${archive}" != "" || "${tests_archive}" != "") && \
       "${output_dir}" == "" ]]; then
    if [[ "${archive}" != "" ]]; then
      archive="$(cd "$(dirname "${archive}")" ; \
        echo $(pwd)/$(basename "${archive}"))"
    fi
    if [[ "${tests_archive}" != "" ]]; then
      tests_archive="$(cd "$(dirname "${tests_archive}")" ; \
        echo $(pwd)/$(basename "${tests_archive}"))"
    fi
    output_dir=$(mktemp -d XXXXXX)
    output_dir="$(cd "${output_dir}" && pwd)"
    delete_dirs="${delete_dirs} ${output_dir}"
  fi

  if [[ "${projects_to_build}" == "" ]]; then
    projects_to_build="$(find_projects)"
  fi

  local any_builds_failed=0
  for build_config in ${build_configs}; do
    # Enable / disable NDK_DEBUG.
    local ndk_debug=
    if [[ "${build_config}" == "debug" ]]; then
      ndk_debug="NDK_DEBUG=1"
    fi
    local ndk_verbose=
    if [[ $((verbose)) -eq 1 ]]; then
      ndk_verbose="V=1"
    fi

    pushd "${project_root}" >/dev/null
    (
      IFS=$'\n'
      # Find each NDK project in the tree and build it.
      local failed_builds=
      local build_failed=0

      if [[ $((clean)) -eq 1 ]]; then
        # Parallel clean is flakey on OSX, so limit the number of tasks "make"
        # can execute to 1.
        for target_dir in ${projects_to_build}; do
          pushd "${target_dir}" >/dev/null
          rm -r bin >/dev/null
          rm -r obj >/dev/null
          rm -r libs >/dev/null
          popd >/dev/null
        done
      fi

      # Build all projects.
      for target_dir in ${projects_to_build}; do
        pushd "${target_dir}" >/dev/null
        echo "Building ${target_dir}..." >&2
        if ! build_apk ${ndk_debug} ${ndk_verbose} \
                                APP_ABI=${build_abi}; then
          failed_builds="${failed_builds} ${target_dir}"
          build_failed=1
        fi
        popd >/dev/null
      done

      # Display a list of projects that failed to build.
      if [[ "${failed_builds}" != "" ]]; then
        echo "ERROR: One or more projects failed to build:" >&2
        IFS=' '
        for failed in ${failed_builds}; do
          echo "  ${failed}" >&2
        done
      fi

      # Copy build artifacts from each project to the output directory.
      if [[ "${output_dir}" != "" ]]; then
        pushd Box2D >/dev/null
        for project_dir in $(echo "${projects_to_build}" | \
                               sed 's@^Box2D@.@'); do
          artifact_output="${output_dir}/${project_dir}/${build_config}"
          mkdir -p "${artifact_output}"
          # Copy bin and libs directories if they exist.
          for sub_dir in bin libs; do
            artifact_dir=${project_dir}/${sub_dir}
            if [[ -d ${artifact_dir} ]]; then
              cp -a "${artifact_dir}" "${artifact_output}"
            fi
          done
          # Copy static libraries if they are present.
          for static_lib in $(cd "${project_dir}" && \
                              find obj/local/* -maxdepth 1 \
                                -type f -name '*.a'); do
            artifact_dir="$(dirname ${artifact_output}/${static_lib})"
            mkdir -p "${artifact_dir}"
            cp -a "${project_dir}/${static_lib}" "${artifact_dir}"
          done
        done
        popd >/dev/null
      fi
      popd >/dev/null

      exit ${build_failed}
    ) || any_builds_failed=1
  done

  # Create the archive of build artifacts.
  if [[ "${archive}" != ""  ||  "${tests_archive}" != "" ]]; then
    pushd "${output_dir}" >/dev/null
    rm -f "${archive}"
    rm -f "${tests_archive}"
    if [[ "${archive}" != "" ]]; then
      zip -r "${archive}" .
    fi
    if [[ "${tests_archive}" != "" ]]; then
      find "${output_dir}" -name "*Tests-release.apk" | \
        xargs zip -j "${tests_archive}"
    fi
    popd >/dev/null
  fi

  # Clean up temporary directories.
  for delete_dir in ${delete_dirs}; do
    rm -rf "${delete_dir}"
  done

  exit ${any_builds_failed}
}

main "$@"

