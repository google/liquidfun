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
# Build a LiquidFun distribution, see usage() for help.
#
# NOTE: This script requires
# * bash 3 or above
# * sed 4.2 or above
# * gawk 3.1.8 or above

: ${SKIP_CLIENT_CHECK:=0}

# Determine the canonical path of $1 using readlink.
realpath_readlink() {
  readlink -f "$@"
}

# Determine the canonical path of $1 using python.
# OSX's readlink doesn't determine canonical filenames, so emulate it using
# python.
realpath_python() {
  python -c "import os; import sys; print os.path.realpath(sys.argv[1]);" "$@"
}

# Determine the canonical path of $1.
canonicalpath() {
  if [[ "$(uname -s)" == "Darwin" ]]; then
    realpath_python "$@"
  else
    realpath_readlink "$@"
  fi
}

declare -r this="$(canonicalpath ${0})"
declare -r this_dir="${PWD}"
declare -r script_name="$(basename ${this})"
declare -r project_dir="$(canonicalpath $(dirname ${0})/../)"
declare -r project_name="$(basename ${project_dir})"
# Space separated paths to directories required by the project relative to
# project_dir/..
declare -r out_of_tree_dependencies="freeglut googletest"
declare -r tempdir_template="${project_name}tmpXXXXXX"
declare -r host_types="windows osx"
# Bash 3 (default on OSX) doesn't have associative arrays so do this manually.
declare -r generators_windows="Visual Studio 11"
declare -r generators_osx="Xcode"

declare -r archivers="\
local archiver=\"tar -czf\"; local extension=tar.gz
local archiver=\"zip -q -r\"; local extension=zip"

# Root directory of the cmake project relative to ${project_dir}.
declare -r project_cmake_dir="${project_name}/Box2D"

# Generate a case insensitive sed expression from $1.
case_insensitive_sed_expression() {
  local exp=
  for c in $(echo "${1}" | sed 's/./& /g'); do
    if echo "${c}" | grep -q '[A-Za-z]'; then
      exp="${exp}[$(echo ${c} | tr 'a-z' 'A-Z')$(echo ${c} | tr 'A-Z' 'a-z')]"
    else
      exp="${exp}${c}"
    fi
  done
  echo "${exp}"
}

# Convert a path $1 from a/b/c to ../../../ and convert separators from /
# to $2.
relative_path() {
  local -r separator="${2}"
  echo "${1}" | sed -r 's@^./@@;
                        s@[^/]*$@@;
                        s@([^/]+)@..@g;
                        s@/@'"${separator//\\/\\\\}"'@g;'
}

# Convert absolute paths to paths relative to $1 using the path separator $2
# and the path separator regular expression $3.  Enable / disable relative
# path replacement using $4.  When relative path replacement is disabled the
# string $1 is replaced with the current directory i.e ".$2".
absolute_to_relative_paths() {
  local -r replace_regexp="${1}"
  local separator="${2}"
  local separator_regexp="${3}"
  local use_relative_replacement="${4}"
  (
    IFS=$'\n'
    for f in $(find . -type f); do
      local relative=".${separator}"
      if [[ $((use_relative_replacement)) -ne 0 ]]; then
        relative=$(relative_path "${f}" "${separator}")
        # Special case replacement for .pbxproj files which reference files
        # relative to the parent directory.
        if echo "${f}" | grep -q '.pbxproj$'; then
          relative=$(echo "${relative}" | sed 's@^\.\./@@;t;s@^\.\.@@;')
        fi
        if [[ "${relative}" == "" ]]; then
          relative=".${separator}"
        fi
      fi

      sed -i -r '
s@'"${replace_regexp}${separator_regexp}"'@'"${relative}"'@g;
s@'"${replace_regexp}"'@'"${relative}"'@g;' "${f}" 2>/dev/null || true
    done
  )
}

# Make a cmake generated Xcode project redistributable.
# $1 is the name of the host used to generate the Xcode project.
# $2 is the name of the temporary directory that was used on the host to
# generate the Xcode project.
# $3 is the subdirectory within the temporary directory that contains the
# cmake project.
make_redistributable_osx() {
  local -r host="${1}"
  local -r temp_dir="${2}"
  local -r cmake_project="${3}"
  local cmake_path="$(dirname "$(ssh ${host} which cmake)")"
  cmake_path="$(echo "${cmake_path}" | sed -r 's@/[^/]+$@@')"
  local temp_dir_cmake="${temp_dir}/${cmake_project}"
  absolute_to_relative_paths "${cmake_path}" / / 0
  pushd "${cmake_project}" >/dev/null
  absolute_to_relative_paths "${temp_dir_cmake}" / / 1
  popd >/dev/null
  absolute_to_relative_paths "${temp_dir}" / / 1
  # Fix up mangled header search paths.
  for f in $(find . -type f -name '*.pbxproj'); do
    sed -i -r 's@shellScript[^"]+"@&#@;' "${f}"
  done
  # Remove cmake files that aren't required.
  find -name CMakeCache.txt -o \
       -name CMakeFiles -o \
       -name CMakeScripts -o \
       -name cmake_install.cmake | xargs rm -rf
  # Testbed and HelloWorld targets can't find Box2D headers with the default
  # search paths, so make all targets recursively search for headers.
  for f in $(find . -type f -name '*.pbxproj'); do
    awk '
      {
        if ($0 ~ /HEADER_SEARCH_PATHS =/) {
          print gensub(/\.\/,/, "\"./Box2D/..\",", "g", $0)
        } else {
          print $0
        }
      }' "${f}" > "${f}.new" && \
      mv "${f}.new" "${f}"
  done
}

# Make a cmake generated Visual Studio project redistributable.
# $1 is the name of the host used to generate the Visual Studio project.
# $2 is the name of the temporary directory that was used on the host to
# generate the Visual Studio project.
# $3 is the subdirectory within the temporary directory that contains the
# cmake project.
make_redistributable_windows() {
  local -r host="${1}"
  local -r temp_dir="${2}"
  local -r cmake_project="${3}"
  local cmake_path="$(ssh ${host} cygpath -w \
                       \$\(readlink -f \$\(dirname \$\(which cmake\)\)/..\))\\"
  local temp_dir_native="$(ssh ${host} cygpath -w \
                             \$\(readlink -f ${temp_dir}\))"
  if [[ "${temp_dir_native}" == "" ]]; then
    echo "\
Unable to determine ${host} path for ${temp_dir}, can't convert project files
to relative paths." >&2
    return 1
  fi
  local temp_dir_native_cmake="${temp_dir_native}\\${cmake_project//\//\\}"
  temp_dir_native_cmake="$(case_insensitive_sed_expression \
                             "${temp_dir_native_cmake}")"
  cmake_path="$(case_insensitive_sed_expression "${cmake_path}")"
  temp_dir_native="$(case_insensitive_sed_expression "${temp_dir_native}")"
  absolute_to_relative_paths "${cmake_path//\\/[/\\\\]}" '\\' '[/\\\\]' 0
  pushd "${cmake_project}" >/dev/null
  absolute_to_relative_paths "${temp_dir_native_cmake//\\/[/\\\\]}" '\\' \
    '[/\\\\]' 1
  popd >/dev/null
  absolute_to_relative_paths "${temp_dir_native//\\/[/\\\\]}" '\\' '[/\\\\]' 1
  # Remove machine specific cmake files that aren't required.
  find -name CMakeCache.txt -o \
       -name CMakeFiles -o \
       -name CMakeScripts -o \
       -name cmake_install.cmake -o \
       -name 'ALL_BUILD.*' -o \
       -name 'INSTALL.*' -o \
       -name 'ZERO_CHECK.*' | \
    xargs rm -rf
  pushd "${cmake_project}" >/dev/null
  # Remove the ALL_BUILD, ZERO_CHECK and INSTALL projects from the solution
  # file.
  cat Box2D.sln | \
    tr -d '\r' | \
    awk '
      BEGIN {
        p = 1;
        removed_project_guids_regexp = "";
      }

      /Project.*(ALL_BUILD|ZERO_CHECK|INSTALL)/ {
        p = 0;
        if (removed_project_guids_regexp != "") {
          removed_project_guids_regexp = substr( \
            removed_project_guids_regexp, 1, \
            length(removed_project_guids_regexp) - 1) "|";
        }
        removed_project_guids_regexp = \
          removed_project_guids_regexp "" \
          gensub(/.*, "{([^}]*)}"/, "\\1", "", $0);
      }

      {
        if (p) {
          print $0;
        }
      }

      /^EndProject/ {
        p = 1;
      }

      END {
        print removed_project_guids_regexp > "/dev/stderr"
      }' Box2D.sln 1>Box2D.sln.pass1 2>Box2D.sln.remove && \
    grep -vE "($(cat Box2D.sln.remove | tr -d '\r'))" Box2D.sln.pass1 | \
      sed 's@\r@@g;s@$@\r@g' > Box2D.sln && \
    rm Box2D.sln.pass1
    rm Box2D.sln.remove
  popd >/dev/null
}

# Clean the source tree of any files that don't need to be redistributed.
clean_tree() {
  local -r files_to_remove="\
# .git directories.
$(for d in ${project_name} ${out_of_tree_dependencies}; do \
  echo ${d}/.git; done)
# .git files
$(find ${project_name} -name '.git*' -type f)
# Autobuild consists of scripts that are used by the automated build system.
${project_name}/Box2D/AutoBuild
# Not required: this is generated by Cmake for LiquidFun.
${project_name}/Box2D/Build
# This is the old Box2D Building.txt, LiquidFun version is in
# Box2D/Documentation/Building.txt
${project_name}/Box2D/Building.txt
# Not tested.
${project_name}/Contributions
# Only contains the build distribution script.
${project_name}/tools
# License tracking in the Android source tree.
${project_name}/MODULE_LICENSE_BSD_LIKE
"
  (
    IFS=$'\n'
    for f in $(echo "${files_to_remove}" | grep -v '^#'); do
      rm -rf "${f}"
    done
  )
  find "${project_name}" -type d -empty | xargs -I@ rm -rf "@"
}

# Archive the current directory in a couple of formats.  ${1} specifies the
# prefix for the output archive names.
archive_tree() {
  (
    local archiver=
    local -r name="${1}"
    IFS=$'\n'
    for expression in ${archivers[@]}; do
      eval "${expression}"
      local archive_name="${name}.${extension}"
      eval "${archiver} ${archive_name} *"
      echo "Created ${archive_name}" >&2
    done
  )
}

# Display the help for this script and exit.
usage() {
  echo "\
Build a liquidfun distribution archive.

A host of each target OS is required to generate target specific IDE files.
Each host requires an installation of cmake and the cmake binary to be in the
host's PATH.  On Windows, in order to generate the Visual Studio projects,
Visual Studio 2012 needs to be installed.

If the script is successful, liquidfun-\${version}.\* archive files are created
in the current working directory.

Usage: ${script_name} -r release_version [-w windows_host] [-o osx_host] [-l]

-r release_version_extension:
  String added to the end of the version number parsed from the top level
  readme which is ultimately added to the output archive filename.
-w windows_host:
  Name of the Windows host to use to generate the Visual Studio project.
-o osx_host:
  Name of the Mac OSX host to use to generate the Xcode project.
-l:
  If specified the temporary local directories copied from remote hosts are
  not deleted.
" >&2
  exit 1
}

main() {
  local release_version_ext=""
  local release_version=
  local windows_host=""
  local osx_host=""
  local leave_host_dirs=0
  while getopts "r:w:o:l" options; do
    case ${options} in
      r) release_version_ext="${OPTARG}" ;;
      w) windows_host="${OPTARG}" ;;
      o) osx_host="${OPTARG}" ;;
      l) leave_host_dirs=1 ;;
      *) echo "Unknown option ${options}" >&2 && usage ;;
    esac
  done

  release_version=$(head -n1 "${project_dir}/Readme.md" | \
                    sed -r 's/.* Version \[([^]]*)\].*/\1/')
  if [[ "${release_version}" == "" ]]; then
    echo "Unable to parse version number from Readme.md" >&2
    exit 1
  fi
  release_version="${release_version}${release_version_ext}"

  if [[ -d .git ]]; then
    local -r dirty_files=$(git status -s --ignored | grep ${script_name})
    if [[ "${dirty_files}" != "" && $((SKIP_CLIENT_CHECK)) -ne 0 ]]; then
      echo "\
Files not checked in found in the git client ${project_dir}
${dirty_files}" >&2
      exit 1
    fi
  fi

  # Copy the project directory and clean it.
  local -r project_dir_clean="$(mktemp -d ${tempdir_template})"
  cp -a "${project_dir}" "${project_dir_clean}"
  for d in ${out_of_tree_dependencies}; do
    cp -a "${project_dir}/../${d}" "${project_dir_clean}"
  done
  pushd "${project_dir_clean}" >/dev/null
  clean_tree
  popd >/dev/null

  local -r hosts_windows=${windows_host}
  local -r hosts_osx=${osx_host}
  for host_type in ${host_types}; do
    eval "local host=\"\${hosts_${host_type}}\""
    if [[ "${host}" != "" ]]; then
      eval "local generator=\"\${generators_${host_type}}\""
      local target_dir=${project_dir}_${host_type}
      local temp_dir=$(ssh "${host}" \
                           "cd \$(mktemp -d ${tempdir_template}); pwd")
      echo "Generating ${generator} project files for ${host_type} on"\
           "${host}" >&2
      rsync -aru ${project_dir_clean}/* ${host}:${temp_dir}
      # Regeneration of the project is suppressed as cmake will attempt to
      # recreate the project after an archive has been unpacked if the
      # generate.stamp files may have timestamps older than the project.
      ssh "${host}" \
        "cd ${temp_dir}/${project_cmake_dir} &&" \
        "cmake -G \"${generator}\" \
               -DLIB_INSTALL_DIR=. \
               -DCMAKE_SUPPRESS_REGENERATION=TRUE \
               -DCMAKE_USE_RELATIVE_PATHS=TRUE"
      rsync -aru ${host}:${temp_dir}/* ${target_dir}
      pushd "${target_dir}" >/dev/null
      eval "make_redistributable_${host_type} ${host} ${temp_dir} \
              ${project_cmake_dir}"
      popd >/dev/null
      ssh "${host}" "rm -rf ${temp_dir}"
      cp -a ${target_dir}/* "${project_dir_clean}"
      if [[ $((leave_host_dirs)) -eq 0 ]]; then
        rm -rf "${target_dir}"
      fi
    fi
  done

  pushd "${project_dir_clean}" 2>/dev/null
  archive_tree "${this_dir}/${project_name}-${release_version}"
  popd >/dev/null

  rm -rf "${project_dir_clean}"
}

main "$@"

