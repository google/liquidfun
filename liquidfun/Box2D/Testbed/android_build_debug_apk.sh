#!/bin/bash

# build native code
ndk-build NDK_DEBUG=1 || exit 1

# create build.xml local.properties file
# use `android list targets` to figure out what your options are for --target
android update project --target android-10 --path . || exit 1

# build an apk
ant debug || exit 1

# uninstall it if it's already there
adb uninstall com.google.fpl.splash2d.testbed 1>&2 > /dev/null # no error check

# install the apk
adb install bin/NativeActivity-debug.apk || exit 1

# start debugging
# ndk-gdb --start

