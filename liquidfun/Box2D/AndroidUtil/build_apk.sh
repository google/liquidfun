#!/bin/bash -eu
#
# Build, deploy, debug / execute a native Android package based upon
# NativeActivity.

# Get the number of CPU cores present on the host.
get_number_of_cores() {
  case $(uname -s) in
    Darwin)
      sysctl hw.ncpu | awk '{ print $2 }'
      ;;
    CYGWIN*|Linux)
      awk '/^processor/ { n=$3 } END { print n + 1 }' /proc/cpuinfo
      ;;
    *) 1
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
  adb devices -l | tail -n+2 | wc -l
}


# Kill the entire process group if this shell exits.
trap "kill 0" SIGINT SIGTERM EXIT

# Parse arguments for this script.
adb_device=
ant_target=release
disable_deploy=0
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
         [ADB_DEVICE=serial_number] [DEPLOY=0] [RUN_DEBUGGER=1] [LAUNCH=0] \\
         [ndk-build arguments ...]

ADB_DEVICE=serial_number:
  serial_number specifies the device to deploy the built apk to if multiple
  Android devices are connected to the host.
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

# Build native code.
ndk-build -j$(get_number_of_cores) "$@"

# Get the package name from the manifest.
declare -r android_manifest=AndroidManifest.xml
package_name=$(get_package_name_from_manifest ${android_manifest})
[[ "${package_name}" == "" ]] && exit 1
package_basename=${package_name/*./}
package_filename=$(get_library_name_from_manifest ${android_manifest})
[[ "${package_filename}" == "" ]] && package_filename="${package_basename}"

# Create build.xml and local.properties files.
# Use `android list targets` to figure out what your options are for --target
android update project --target android-10 -n ${package_filename} --path .

# Build the apk.
ant ${ant_target}

declare -r built_apk=bin/${package_filename}-${ant_target}.apk

# Sign release apks with a temporary key as these packages will not be
# redistributed.
if [[ "${ant_target}" == "release" ]]; then
  keystore=${package_filename}.keystore
  key_alias=splash2d
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

if [[ $((disable_deploy)) -eq 0 ]]; then
  # Uninstall the package if it's already installed.
  adb ${adb_device} uninstall ${package_name} 1>&2 > /dev/null || \
    true # no error check

  # Install the apk.
  adb ${adb_device} install ${built_apk}
fi

if [[ "${ant_target}" == "debug" && $((run_debugger)) -eq 1 ]]; then
  # Start debugging.
  ndk-gdb ${adb_device} --start
elif [[ $((launch)) -eq 1 ]]; then
  (
    # Display logcat in the background.
    adb ${adb_device} logcat &
    logcat_pid=$!
    # Launch the activity.
    adb ${adb_device} shell am start -S -W -n \
      ${package_name}/android.app.NativeActivity
  )
fi

