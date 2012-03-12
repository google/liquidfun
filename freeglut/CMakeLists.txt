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
	src/fg_callbacks.c
	src/fg_cursor.c
	src/fg_display.c
	src/fg_ext.c
	src/fg_font_data.c
	src/fg_gamemode.c
	src/fg_init.c
	src/fg_internal.h
	src/fg_input_devices.c
	src/fg_joystick.c
	src/fg_main.c
	src/fg_misc.c
	src/fg_overlay.c
	src/fg_spaceball.c
	src/fg_state.c
	src/fg_stroke_mono_roman.c
	src/fg_stroke_roman.c
	src/fg_structure.c
	src/fg_videoresize.c
	src/fg_window.c
)
# Android port requires adding a version of these compatible with
# OpenGL ES (TODO):
IF(NOT ANDROID)
       LIST(APPEND FREEGLUT_SRCS
               src/fg_font.c
               src/fg_geometry.c
               src/fg_menu.c
               src/fg_teapot.c
)
ENDIF()

IF(WIN32)   # TODO: is this true for 64 bit as well? Chaneg to WINDOWS?
	LIST(APPEND FREEGLUT_SRCS
		src/mswin/fg_cursor_mswin.c
		src/mswin/fg_display_mswin.c
		src/mswin/fg_ext_mswin.c
		src/mswin/fg_gamemode_mswin.c
		src/mswin/fg_init_mswin.c
		src/mswin/fg_internal_mswin.h
		src/mswin/fg_input_devices_mswin.c
		src/mswin/fg_joystick_mswin.c
		src/mswin/fg_main_mswin.c
		src/mswin/fg_menu_mswin.c
		src/mswin/fg_spaceball_mswin.c
		src/mswin/fg_state_mswin.c
		src/mswin/fg_structure_mswin.c
		src/mswin/fg_window_mswin.c
)
ELSEIF(ANDROID)
    LIST(APPEND FREEGLUT_SRCS
        src/egl/fg_internal_egl.h
        src/egl/fg_display_egl.c
        src/egl/fg_init_egl.c
        src/egl/fg_structure_egl.c
        src/egl/fg_window_egl.c
        src/android/native_app_glue/android_native_app_glue.c
        src/android/native_app_glue/android_native_app_glue.h
        src/android/fg_runtime_android.c
        src/android/fg_gamemode_android.c
        src/android/fg_input_devices_android.c
        src/android/fg_joystick_android.c
        src/android/fg_main_android.c
        src/android/fg_spaceball_android.c
        src/android/fg_state_android.c
        src/android/fg_window_android.c
        src/android/opengles_stubs.c
        src/android/fg_internal_android.h
)
ELSE()
	LIST(APPEND FREEGLUT_SRCS
		src/x11/fg_cursor_x11.c
		src/x11/fg_display_x11.c
		src/x11/fg_ext_x11.c
		src/x11/fg_gamemode_x11.c
		src/x11/fg_glutfont_definitions_x11.c
		src/x11/fg_init_x11.c
		src/x11/fg_internal_x11.h
		src/x11/fg_input_devices_x11.c
		src/x11/fg_joystick_x11.c
		src/x11/fg_main_x11.c
		src/x11/fg_menu_x11.c
		src/x11/fg_spaceball_x11.c
		src/x11/fg_state_x11.c
		src/x11/fg_structure_x11.c
		src/x11/fg_window_x11.c
		src/x11/fg_xinput_x11.c
)


ENDIF()

# Neatly organize all of the output files in the build directory
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# BUILD_SHARED_LIBS is already a standard CMake variable, but we need to
# re-declare it here so it will show up in the GUI.
OPTION(BUILD_SHARED_LIBS "Build FreeGLUT shared library." ON)
OPTION(BUILD_STATIC_LIBS "Build FreeGLUT static library." ON)

# OpenGL ES support
OPTION(FREEGLUT_GLES1 "Use OpenGL ES 1.x (requires EGL)" OFF)
OPTION(FREEGLUT_GLES2 "Use OpenGL ES 2.x (requires EGL) (overrides BUILD_GLES1)" OFF)

IF(FREEGLUT_GLES2)
  ADD_DEFINITIONS(-DGLESv2)
  LIST(APPEND LIBS GLESv2 EGL)
ELSEIF(FREEGLUT_GLES1)
  ADD_DEFINITIONS(-DGLESv1)
  LIST(APPEND LIBS GLESv1 EGL)
ELSE()
  FIND_PACKAGE(OpenGL REQUIRED)
  LIST(APPEND LIBS ${OPENGL_gl_LIBRARY})
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
ENDIF()

# lib m for math, not needed on windows
IF (NOT WIN32)
    LIST(APPEND LIBS m)
ENDIF()

IF(WIN32)
	MESSAGE(WARNING "Insecure CRT warnings hidden (might want to fix these)")
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

if(UNIX AND NOT ANDROID)
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
INCLUDE(CheckTypeSize)
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
CHECK_FUNCTION_EXISTS(vfprintf  HAVE_VFPRINTF)
CHECK_FUNCTION_EXISTS(_doprnt   HAVE_DOPRNT)
CHECK_FUNCTION_EXISTS(XParseGeometry   HAVE_XPARSEGEOMETRY)
IF (NOT HAVE_XPARSEGEOMETRY)
   LIST(APPEND FREEGLUT_SRCS
		src/util/xparsegeometry_repl.c
		src/util/xparsegeometry_repl.h)
   SET(NEED_XPARSEGEOMETRY_IMPL TRUE)
ENDIF()
# decide on suitable type for internal time keeping, 64-bit if possible
CHECK_INCLUDE_FILES(stdint.h    HAVE_STDINT_H)
CHECK_INCLUDE_FILES(inttypes.h  HAVE_INTTYPES_H)
IF (NOT (HAVE_STDINT_H OR HAVE_INTTYPES_H))
    IF (MSVC)
        # Some old Microsoft VC don't support unsigned long long, but all we
        # care about support unsigned __int64, so test for presence of that
        # type
        CHECK_TYPE_SIZE("unsigned __int64" U__INT64 BUILTIN_TYPES_ONLY)
    ELSEIF()
        CHECK_TYPE_SIZE("unsigned long long" ULONG_LONG BUILTIN_TYPES_ONLY)
    ENDIF()
ENDIF()


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
	# - all shared libraries must have a soname/version, see :
	#   http://sourceware.org/autobook/autobook/autobook_91.html#SEC91
	#   http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
	#   Current: -version-info 12:0:9 -> 3.9.0
	# - the output library should be named glut so it'll be linkable with -lglut
	# - the shared library should link to the dependency libraries so that the user
	#   won't have to link them explicitly (they shouldn't have to know that we depend
	#   on Xrandr or Xxf86vm)
	SET_TARGET_PROPERTIES(freeglut PROPERTIES VERSION 3.9.0 SOVERSION 3 OUTPUT_NAME glut)
	SET_TARGET_PROPERTIES(freeglut_static PROPERTIES OUTPUT_NAME glut)
	IF(ANDROID)
		# Not in CMake toolchain file, because the toolchain
		# file is called several times and generally doesn't
		# seem to be meant for it:
		# '-mandroid' is not mandatory but doesn't hurt
		# '-O0 -gstabs+' helps the currently buggy GDB port
		# '-DANDROID' is the Android build system convention
		# Too late to manipulate ENV: SET(ENV{CFLAGS} "$ENV{CFLAGS} -mandroid")
		# Not using _INIT variables, they seem to be used internally only
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mandroid")
		SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -gstabs+")
		ADD_DEFINITIONS(-DANDROID)
	ENDIF()
ENDIF()
IF(BUILD_SHARED_LIBS)
    TARGET_LINK_LIBRARIES(freeglut ${LIBS})
ENDIF()
IF(BUILD_STATIC_LIBS)
    TARGET_LINK_LIBRARIES(freeglut_static ${LIBS})
ENDIF()

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
# lib m for math, not needed on windows
IF (NOT WIN32)
    LIST(APPEND DEMO_LIBS m)
ENDIF()

MACRO(ADD_DEMO name)
    IF( FREEGLUT_BUILD_DEMOS )
        IF(BUILD_SHARED_LIBS)
            ADD_EXECUTABLE(${name} ${ARGN})
            TARGET_LINK_LIBRARIES(${name} ${DEMO_LIBS} freeglut)
        ENDIF()
        IF(BUILD_STATIC_LIBS)
            ADD_EXECUTABLE(${name}_static ${ARGN})
            TARGET_LINK_LIBRARIES(${name}_static ${DEMO_LIBS} freeglut_static)
        ENDIF()
    ENDIF()
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
ADD_DEMO(timer           progs/demos/timer/timer.c)
