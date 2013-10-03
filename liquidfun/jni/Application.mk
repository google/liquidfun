LOCAL_PATH:=$(call my-dir)

# This library uses stl.
APP_STL:=stlport_static

# The root of the project.
APP_PROJECT_PATH := $(abspath $(NDK_PROJECT_PATH))
APP_BUILD_SCRIPT := $(APP_PROJECT_PATH)/Android.mk

# Build targets.
APP_MODULES:=\
	libsplash2d \
	libsplash2d_static

