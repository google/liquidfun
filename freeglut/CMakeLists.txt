PROJECT(freeglut)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(FREEGLUT_SRCS
	src/freeglut_callbacks.c
	src/freeglut_cursor.c
	src/freeglut_display.c
	src/freeglut_ext.c
	src/freeglut_font.c
	src/freeglut_font_data.c
	src/freeglut_gamemode.c
	src/freeglut_geometry.c
	src/freeglut_glutfont_definitions.c
	src/freeglut_init.c
	src/freeglut_input_devices.c
	src/freeglut_joystick.c
	src/freeglut_main.c
	src/freeglut_menu.c
	src/freeglut_misc.c
	src/freeglut_overlay.c
	src/freeglut_spaceball.c
	src/freeglut_state.c
	src/freeglut_stroke_mono_roman.c
	src/freeglut_stroke_roman.c
	src/freeglut_structure.c
	src/freeglut_teapot.c
	src/freeglut_videoresize.c
	src/freeglut_window.c
	src/freeglut_xinput.c
)

IF(WIN32)
	LIST(APPEND FREEGLUT_SRCS src/freeglut_windows.c)
ELSE()
	LIST(APPEND FREEGLUT_SRCS src/freeglut_x11.c)
ENDIF()

# Neatly organize all of the output files in the build directory
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# BUILD_SHARED_LIBS is already a standard CMake variable, but we need to
# re-delcare it here so it will show up in the GUI.
OPTION(BUILD_SHARED_LIBS "Build FreeGLUT as a shared library." ON)

FIND_PACKAGE(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIR})

IF(WIN32)
	MESSAGE(WARNING "Insecure CRT warnings hidden (might want to fix these)")
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

INCLUDE(CheckIncludeFiles)

CHECK_INCLUDE_FILES(X11/extensions/xf86vmode.h 	HAVE_X11_EXTENSIONS_XF86VMODE_H)
CHECK_INCLUDE_FILES(X11/extensions/Xrandr.h 	HAVE_X11_EXTENSIONS_XRANDR_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(unistd.h 	HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(sys/time.h 	HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(stdbool.h 	HAVE_STDBOOL_H)
CHECK_INCLUDE_FILES(sys/param.h HAVE_SYS_PARAM_H)
CHECK_INCLUDE_FILES(sys/ioctl.h HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILES(fcntl.h 	HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(errno.h 	HAVE_ERRNO_H)
CHECK_INCLUDE_FILES(usbhid.h 	HAVE_USBHID_H)

# The generated config.h is placed in the project's build directory, just to
# ensure that all CMake-generated files are kept away from the main source tree.
# As a result, the build directory must to be added to the include path list.
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

IF(BUILD_SHARED_LIBS)
	IF(WIN32)
		LIST(APPEND FREEGLUT_SRCS src/freeglutdll.def freeglut.rc)
	ENDIF()
	ADD_DEFINITIONS(-DFREEGLUT_EXPORTS)
ELSE()
	ADD_DEFINITIONS(-DFREEGLUT_STATIC)
ENDIF()

# The default library type is determined by BUILD_SHARED_LIBS
ADD_LIBRARY(freeglut ${FREEGLUT_SRCS})

IF(NOT BUILD_SHARED_LIBS)
	SET_TARGET_PROPERTIES(freeglut PROPERTIES OUTPUT_NAME freeglut_static)
ENDIF()

INSTALL(TARGETS freeglut DESTINATION lib)
INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/include/GL" DESTINATION include
		FILES_MATCHING PATTERN "*.h")

MACRO(ADD_DEMO name)
	ADD_EXECUTABLE(${name} ${ARGN})
	TARGET_LINK_LIBRARIES(${name} freeglut)
	INSTALL(TARGETS ${name} DESTINATION bin)
ENDMACRO()

ADD_DEMO(CallbackMaker   progs/demos/CallbackMaker/CallbackMaker.c)
ADD_DEMO(Fractals        progs/demos/Fractals/Fractals.c)
ADD_DEMO(Fractals_random progs/demos/Fractals_random/Fractals_random.c)
ADD_DEMO(Lorenz          progs/demos/Lorenz/Lorenz.c)
ADD_DEMO(One             progs/demos/One/One.c)
ADD_DEMO(Resizer         progs/demos/Resizer/Resizer.c)
ADD_DEMO(shapes          progs/demos/shapes/shapes.c)
ADD_DEMO(smooth_opengl3  progs/demos/smooth_opengl3/smooth_opengl3.c)
ADD_DEMO(spaceball       progs/demos/spaceball/spaceball.c)
ADD_DEMO(subwin          progs/demos/subwin/subwin.c)


