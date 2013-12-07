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
# Build, deploy, debug / execute a native Android package based upon
# NativeActivity.

declare -r script_directory=$(dirname $0)
declare -r android_root=${script_directory}/../../../../../../

# Minimum Android target version supported by this project.
: ${BUILDAPK_ANDROID_TARGET_MINVERSION:=10}
# Directory containing the Android SDK
# (http://developer.android.com/sdk/index.html).
: ${ANDROID_SDK_HOME:=}
# Directory containing the Android NDK
# (http://developer.android.com/tools/sdk/ndk/index.html).
: ${NDK_HOME:=}

# Get the number of CPU cores present on the host.
get_number_of_cores() {
  case $(uname -s) in
    Darwin)
      sysctl hw.ncpu | awk '{ print $2 }'
      ;;
    CYGWIN*|Linux)
      awk '/^processor/ { n=$3 } END { print n + 1 }' /proc/cpuinfo
      ;;
    *)
      echo 1
      ;;
  esac
}

# Get the package name from an AndroidManifest.xml file.
get_package_name_from_manifest() {
  xmllint --xpath 'string(/manifest/@package)' "${1}"
}

# Get the library name from an AndroidManifest.xml file.
get_library_name_from_manifest() {
  echo "\
setns android=http://schemas.android.com/apk/res/android
xpath string(/manifest/application/activity\
[@android:name=\"android.app.NativeActivity\"]/meta-data\
[@android:name=\"android.app.lib_name\"]/@android:value)" |
  xmllint --shell "${1}" | awk '/Object is a string/ { print $NF }'
}

# Get the number of Android devices connected to the system.
get_number_of_devices_connected() {
  adb devices -l | \
    awk '/^..*$/ { if (p) { print $0 } }
         /List of devices attached/ { p = 1 }' | \
    wc -l
  return ${PIPESTATUS[0]}
}

# Kill a process and its' children.  This is provided for cygwin which
# doesn't ship with pkill.
kill_process_group() {
  local parent_pid="${1}"
  local child_pid=
  for child_pid in $(ps -f | \
                     awk '{ if ($3 == '"${parent_pid}"') { print $2 } }'); do
    kill_process_group "${child_pid}"
  done
  kill "${parent_pid}"
}

# Find and run "adb".
adb() {
  local adb_path=
  for path in "$(which adb 2>/dev/null)" \
              "${ANDROID_SDK_HOME}/sdk/platform-tools/adb" \
              "${android_root}/prebuilts/sdk/platform-tools/adb"; do
    if [[ -e "${path}" ]]; then
      adb_path="${path}"
      break
    fi
  done
  if [[ "${adb_path}" == "" ]]; then
    echo -e "Unable to find adb." \
           "\nAdd the Android ADT sdk/platform-tools directory to the" \
           "PATH." >&2
    exit 1
  fi
  "${adb_path}" "$@"
}

# Find and run "android".
android() {
  local android_executable=android
  if echo "$(uname -s)" | grep -q CYGWIN; then
    android_executable=android.bat
  fi
  local android_path=
  for path in "$(which ${android_executable})" \
              "${ANDROID_SDK_HOME}/sdk/tools/${android_executable}" \
              "${android_root}/prebuilts/sdk/tools/${android_executable}"; do
    if [[ -e "${path}" ]]; then
      android_path="${path}"
      break
    fi
  done
  if [[ "${android_path}" == "" ]]; then
    echo -e "Unable to find android tool." \
           "\nAdd the Android ADT sdk/tools directory to the PATH." >&2
    exit 1
  fi
  # Make sure ant is installed.
  if [[ "$(which ant)" == "" ]]; then
    echo -e "Unable to find ant." \
            "\nPlease install ant and add to the PATH." >&2
    exit 1
  fi

  "${android_path}" "$@"
}

# Find and run "ndk-build"
ndkbuild() {
  local ndkbuild_path=
  for path in "$(which ndk-build 2>/dev/null)" \
              "${NDK_HOME}/ndk-build" \
              "${android_root}/prebuilts/ndk/current/ndk-build"; do
    if [[ -e "${path}" ]]; then
      ndkbuild_path="${path}"
      break
    fi
  done
  if [[ "${ndkbuild_path}" == "" ]]; then
    echo -e "Unable to find ndk-build." \
            "\nAdd the Android NDK directory to the PATH." >&2
    exit 1
  fi
  "${ndkbuild_path}" "$@"
}

# Parse arguments for this script.
adb_device=
ant_target=release
disable_deploy=0
disable_build=0
run_debugger=0
launch=1
for opt; do
  case ${opt} in
    NDK_DEBUG=1)
      ant_target=debug
      ;;
    ADB_DEVICE*)
      adb_device=${opt/ADB_DEVICE=/}
      if [[ "${adb_device}" != "" ]]; then
        adb_device="-s ${adb_device}"
      fi
      ;;
    BUILD=0)
      disable_build=1
      ;;
    DEPLOY=0)
      disable_deploy=1
      ;;
    RUN_DEBUGGER=1)
      run_debugger=1
      ;;
    LAUNCH=0)
      launch=0
      ;;
    -h|--help|help)
      echo "
Build the Android package in the current directory and deploy it to a
connected device.

Usage: $(basename $0) \\
         [ADB_DEVICE=serial_number] [BUILD=0] [DEPLOY=0] [RUN_DEBUGGER=1] \
         [LAUNCH=0] [ndk-build arguments ...]

ADB_DEVICE=serial_number:
  serial_number specifies the device to deploy the built apk to if multiple
  Android devices are connected to the host.
BUILD=0:
  Disables the build of the package.
DEPLOY=0:
  Disables the deployment of the built apk to the Android device.
RUN_DEBUGGER=1:
  Launches the application in gdb after it has been deployed.  To debug in
  gdb, NDK_DEBUG=1 must also be specified on the command line to build a
  debug apk.
LAUNCH=0:
  Disable the launch of the apk on the Android device.
ndk-build arguments...:
  Additional arguments for ndk-build.  See ndk-build -h for more information.
" >&2
      exit 1
      ;;
  esac
done

# If a target device hasn't been specified and multiple devices are connected
# to the host machine, display an error.
devices_connected=$(get_number_of_devices_connected)
if [[ "${adb_device}" == "" && $((devices_connected)) -gt 1 && \
      ($((disable_deploy)) -eq 0 || $((launch)) -ne 0 || \
       $((run_debugger)) -ne 0) ]]; then
  if [[ $((disable_deploy)) -ne 0 ]]; then
    echo "Deployment enabled, disable using DEPLOY=0" >&2
  fi
  if [[ $((launch)) -ne 0 ]]; then
   echo "Launch enabled." >&2
  fi
  if [[ $((disable_deploy)) -eq 0 ]]; then
    echo "Deployment enabled." >&2
  fi
  if [[ $((run_debugger)) -ne 0 ]]; then
    echo "Debugger launch enabled." >&2
  fi
  echo "
Multiple Android devices are connected to this host.  Either disable deployment
and execution of the built .apk using \"$(basename $0) DEPLOY=0 LAUNCH=0\" or
specify a device to deploy to using \"$(basename $0)
ADB_DEVICE=\${device_serial}\".

The Android devices connected to this machine are:
$(adb devices -l)
" >&2
  exit 1
fi

if [[ $((disable_build)) -eq 0 ]]; then
  # Build native code.
  ndkbuild -j$(get_number_of_cores) "$@"
fi

# Get the package name from the manifest.
declare -r android_manifest=AndroidManifest.xml
package_name=$(get_package_name_from_manifest ${android_manifest})
if [[ "${package_name}" == "" ]]; then
  echo -e "No package name specified in ${android_manifest},"\
          "skipping apk build, deploy\nand launch steps." >&2
  exit 0
fi
package_basename=${package_name/*./}
package_filename=$(get_library_name_from_manifest ${android_manifest})
[[ "${package_filename}" == "" ]] && package_filename="${package_basename}"

# Output apk name.
declare -r built_apk=bin/${package_filename}-${ant_target}.apk

if [[ $((disable_build)) -eq 0 ]]; then
  # Get the list of installed android targets and select the oldest target
  # that is at least as new as BUILDAPK_ANDROID_TARGET_MINVERSION.
  declare -r android_targets_installed=$( \
    android list targets | \
    awk -F'"' '/^id:.*android/ { print $2 }')
  android_build_target=
  for android_target in $(echo "${android_targets_installed}" | \
                          awk -F- '{ print $2 }' | sort -n); do
    if [[ $((android_target)) -ge \
          $((BUILDAPK_ANDROID_TARGET_MINVERSION)) ]]; then
      android_build_target="android-${android_target}"
      break
    fi
  done
  if [[ "${android_build_target}" == "" ]]; then
    echo -e \
      "Found installed Android targets:" \
      "$(echo ${android_targets_installed} | sed 's/ /\n  /g;s/^/\n  /;')" \
      "\nAndroid SDK platform" \
      "android-$((BUILDAPK_ANDROID_TARGET_MINVERSION))" \
      "must be installed to build this project." \
      "\nUse the \"android\" application to install API" \
      "$((BUILDAPK_ANDROID_TARGET_MINVERSION)) or newer." >&2
    exit 1
  fi
  echo "Building for android target ${android_build_target}" >&2

  # Create build.xml and local.properties files.
  android update project --target "${android_build_target}" \
                         -n ${package_filename} --path .

  # Build the apk.
  ant ${ant_target}

  # Sign release apks with a temporary key as these packages will not be
  # redistributed.
  if [[ "${ant_target}" == "release" ]]; then
    keystore=${package_filename}.keystore
    key_alias=${package_basename}
    key_password=${key_alias}
    if [[ ! -e ${keystore} ]]; then
      keytool -genkey -v -dname "cn=, ou=${key_alias}, o=fpl" \
        -storepass ${key_password} \
        -keypass ${key_password} -keystore ${package_filename}.keystore \
        -alias ${key_alias} -keyalg RSA -keysize 2048 -validity 60
    fi
    cp bin/${package_filename}-${ant_target}-unsigned.apk ${built_apk}
    jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 \
      -keystore ${package_filename}.keystore -storepass ${key_password} \
      -keypass ${key_password} ${built_apk} ${key_alias}
  fi
fi

if [[ $((disable_deploy)) -eq 0 ]]; then
  # Uninstall the package if it's already installed.
  adb ${adb_device} uninstall ${package_name} 1>&2 > /dev/null || \
    true # no error check

  # Install the apk.
  # NOTE: The following works around adb not returning an error code when
  # it fails to install an apk.
  echo "Install ${built_apk}" >&2
  declare -r adb_install_result=$(adb ${adb_device} install ${built_apk})
  echo "${adb_install_result}"
  if echo "${adb_install_result}" | grep -qF 'Failure ['; then
    exit 1
  fi
fi

if [[ "${ant_target}" == "debug" && $((run_debugger)) -eq 1 ]]; then
  # Start debugging.
  ndk-gdb ${adb_device} --start
elif [[ $((launch)) -eq 1 ]]; then
  (
    # Clear logs from previous runs.
    # Note that logcat does not just 'tail' the logs, it dumps the entire log
    # history.
    adb ${adb_device} logcat -c

    # Display logcat in the background.
    adb ${adb_device} logcat &
    logcat_pid=$!
    # Kill adb logcat if this shell exits.
    trap "kill_process_group ${logcat_pid}" SIGINT SIGTERM EXIT

    # Determine the SDK version of Android on the device.
    declare -r android_sdk_version=$(
      adb ${adb_device} shell cat system/build.prop | \
      awk -F= '/ro.build.version.sdk/ {
                 v=$2; sub(/[ \r\n]/, "", v); print v
               }')
    # If the SDK is newer than 10, "am" supports stopping an activity.
    adb_stop_activity=
    if [[ $((android_sdk_version)) -gt 10 ]]; then
      adb_stop_activity=-S
    fi

    # Launch the activity and wait for it to complete.
    adb ${adb_device} shell am start ${adb_stop_activity} -W -n \
      ${package_name}/android.app.NativeActivity
  )
fi

