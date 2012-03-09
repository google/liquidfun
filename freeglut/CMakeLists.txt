PROJECT(freeglut)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(FREEGLUT_HEADERS
	include/GL/freeglut.h
	include/GL/freeglut_ext.h
	include/GL/freeglut_std.h
	include/GL/glut.h
)
SET(FREEGLUT_SRCS
	${FREEGLUT_HEADERS}
	src/Common/freeglut_callbacks.c
	src/Common/freeglut_cursor.c
	src/Common/freeglut_display.c
	src/Common/freeglut_ext.c
	src/Common/freeglut_font.c
	src/Common/freeglut_font_data.c
	src/Common/freeglut_gamemode.c
	src/Common/freeglut_geometry.c
	src/Common/freeglut_init.c
	src/Common/freeglut_internal.h
	src/Common/freeglut_input_devices.c
	src/Common/freeglut_joystick.c
	src/Common/freeglut_main.c
	src/Common/freeglut_menu.c
	src/Common/freeglut_misc.c
	src/Common/freeglut_overlay.c
	src/Common/freeglut_spaceball.c
	src/Common/freeglut_state.c
	src/Common/freeglut_stroke_mono_roman.c
	src/Common/freeglut_stroke_roman.c
	src/Common/freeglut_structure.c
	src/Common/freeglut_teapot.c
	src/Common/freeglut_videoresize.c
	src/Common/freeglut_window.c
)

IF(WIN32)
	LIST(APPEND FREEGLUT_SRCS
		src/Common/xparsegeometry_repl.c
		src/Common/xparsegeometry_repl.h
		src/mswin/freeglut_cursor_mswin.c
		src/mswin/freeglut_display_mswin.c
		src/mswin/freeglut_ext_mswin.c
		src/mswin/freeglut_gamemode_mswin.c
		src/mswin/freeglut_init_mswin.c
		src/mswin/freeglut_internal_mswin.h
		src/mswin/freeglut_input_devices_mswin.c
		src/mswin/freeglut_joystick_mswin.c
		src/mswin/freeglut_main_mswin.c
		src/mswin/freeglut_menu_mswin.c
		src/mswin/freeglut_spaceball_mswin.c
		src/mswin/freeglut_state_mswin.c
		src/mswin/freeglut_structure_mswin.c
		src/mswin/freeglut_window_mswin.c
)
ELSE()
	LIST(APPEND FREEGLUT_SRCS
		src/x11/freeglut_cursor_x11.c
		src/x11/freeglut_display_x11.c
		src/x11/freeglut_ext_x11.c
		src/x11/freeglut_gamemode_x11.c
		src/x11/freeglut_glutfont_definitions_x11.c
		src/x11/freeglut_init_x11.c
		src/x11/freeglut_internal_x11.h
		src/x11/freeglut_input_devices_x11.c
		src/x11/freeglut_joystick_x11.c
		src/x11/freeglut_main_x11.c
		src/x11/freeglut_menu_x11.c
		src/x11/freeglut_spaceball_x11.c
		src/x11/freeglut_state_x11.c
		src/x11/freeglut_structure_x11.c
		src/x11/freeglut_window_x11.c
		src/x11/freeglut_xinput_x11.c
)


ENDIF()

# Neatly organize all of the output files in the build directory
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# BUILD_SHARED_LIBS is already a standard CMake variable, but we need to
# re-declare it here so it will show up in the GUI.
OPTION(BUILD_SHARED_LIBS "Build FreeGLUT shared library." ON)
OPTION(BUILD_STATIC_LIBS "Build FreeGLUT static library." ON)

FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
LIST(APPEND LIBS ${OPENGL_gl_LIBRARY})
LIST(APPEND LIBS m)

IF(WIN32)
	MESSAGE(WARNING "Insecure CRT warnings hidden (might want to fix these)")
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

if(UNIX)
	FIND_PACKAGE(X11 REQUIRED)
	LIST(APPEND LIBS ${X11_LIBRARIES})
	IF(X11_Xrandr_FOUND)
		SET(HAVE_X11_EXTENSIONS_XRANDR_H TRUE)
		LIST(APPEND LIBS ${X11_Xrandr_LIB})
	ENDIF()
	IF(X11_xf86vmode_FOUND)
		SET(HAVE_X11_EXTENSIONS_XF86VMODE_H TRUE)
		# Work-around http://www.cmake.org/Bug/bug_view_page.php?bug_id=6976
		IF(NOT "${X11_Xxf86vm_LIB}")
			SET(X11_Xxf86vm_LIB "Xxf86vm")
		ENDIF()
		LIST(APPEND LIBS ${X11_Xxf86vm_LIB})
	ENDIF()
ENDIF()

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(unistd.h 	HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(sys/time.h 	HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(stdbool.h 	HAVE_STDBOOL_H)
CHECK_INCLUDE_FILES(sys/param.h HAVE_SYS_PARAM_H)
CHECK_INCLUDE_FILES(sys/ioctl.h HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILES(fcntl.h 	HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(errno.h 	HAVE_ERRNO_H)
CHECK_INCLUDE_FILES(usbhid.h 	HAVE_USBHID_H)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS(vfprintf HAVE_VFPRINTF)
CHECK_FUNCTION_EXISTS(_doprnt HAVE_DOPRNT)

# The generated config.h is placed in the project's build directory, just to
# ensure that all CMake-generated files are kept away from the main source tree.
# As a result, the build directory must to be added to the include path list.
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# by default, at least on UNIX, we want to build both
IF(BUILD_SHARED_LIBS)
	ADD_LIBRARY(freeglut SHARED ${FREEGLUT_SRCS})
ENDIF()
IF(BUILD_STATIC_LIBS)
	ADD_LIBRARY(freeglut_static STATIC ${FREEGLUT_SRCS})
ENDIF()


IF(WIN32)
	LIST(APPEND LIBS winmm)
	IF(BUILD_SHARED_LIBS)
		LIST(APPEND FREEGLUT_SRCS src/Common/freeglutdll.def freeglut.rc)
		# XXX I assume that if we want both shared and static this one is appropriate
		ADD_DEFINITIONS(-DFREEGLUT_EXPORTS)
	ELSE()
		ADD_DEFINITIONS(-DFREEGLUT_STATIC)
	ENDIF()
ELSE()
	# on UNIX we need to make sure:
	# - all shared libraries must have a soname/version
	# - the output library should be named glut so it'll be linkable with -lglut
	# - the shared library should link to the dependency libraries so that the user
	#   won't have to link them explicitly (they shouldn't have to know that we depend
	#   on Xrandr or Xxf86vm)
	SET_TARGET_PROPERTIES(freeglut PROPERTIES VERSION 3.9.0 SOVERSION 3 OUTPUT_NAME glut)
	SET_TARGET_PROPERTIES(freeglut_static PROPERTIES OUTPUT_NAME glut)
ENDIF()
TARGET_LINK_LIBRARIES(freeglut ${LIBS})
TARGET_LINK_LIBRARIES(freeglut_static ${LIBS})

IF(BUILD_SHARED_LIBS)
	INSTALL(TARGETS freeglut DESTINATION lib)
ENDIF()
IF(BUILD_STATIC_LIBS)
	INSTALL(TARGETS freeglut_static DESTINATION lib)
ENDIF()
INSTALL(FILES ${FREEGLUT_HEADERS} DESTINATION include/GL)



# Optionally build demos, on by default.
option( FREEGLUT_BUILD_DEMOS "Build FreeGLUT demos." ON )

SET(DEMO_LIBS ${OPENGL_glu_LIBRARY} ${LIBS})
LIST(APPEND DEMO_LIBS m)

MACRO(ADD_DEMO name)
	if( FREEGLUT_BUILD_DEMOS )
		ADD_EXECUTABLE(${name} ${ARGN})
		IF(BUILD_SHARED_LIBS)
			TARGET_LINK_LIBRARIES(${name} ${DEMO_LIBS} freeglut)
		ELSE()
			TARGET_LINK_LIBRARIES(${name} ${DEMO_LIBS} freeglut_static)
		ENDIF()
	endif()
ENDMACRO()

ADD_DEMO(CallbackMaker   progs/demos/CallbackMaker/CallbackMaker.c)
ADD_DEMO(Fractals        progs/demos/Fractals/fractals.c)
ADD_DEMO(Fractals_random progs/demos/Fractals_random/fractals_random.c)
ADD_DEMO(Lorenz          progs/demos/Lorenz/lorenz.c)
ADD_DEMO(One             progs/demos/One/one.c)
ADD_DEMO(Resizer         progs/demos/Resizer/Resizer.cpp)
ADD_DEMO(shapes          progs/demos/shapes/shapes.c)
ADD_DEMO(smooth_opengl3  progs/demos/smooth_opengl3/smooth_opengl3.c)
ADD_DEMO(spaceball       progs/demos/spaceball/spaceball.c
                         progs/demos/spaceball/vmath.c
                         progs/demos/spaceball/vmath.h)
ADD_DEMO(subwin          progs/demos/subwin/subwin.c)


