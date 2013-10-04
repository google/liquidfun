LOCAL_PATH:=$(call my-dir)

APP_PLATFORM:=android-10

# This library uses stl.
APP_STL:=stlport_static

# Build targets.
APP_MODULES:=\
	libBox2D \
	libBox2D_static

# Targets that don't build at the moment.
#	freeglut_static \
#	glui_static
