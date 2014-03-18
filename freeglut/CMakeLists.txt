CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
PROJECT(freeglut)

# NOTE: On Windows and Cygwin, the dll's are placed in the
# CMAKE_RUNTIME_OUTPUT_DIRECTORY, while their corresponding import
# libraries end up in CMAKE_ARCHIVE_OUTPUT_DIRECTORY. On other
# platforms, such as Linux, the shared libraries are put in
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY instead.
# Static libraries end up in CMAKE_ARCHIVE_OUTPUT_DIRECTORY on all
# platforms.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# setup version numbers
FILE(STRINGS "version.mk" MAJOR_MINOR_PATCH_MK)
SET(VERSION_FILE "version.cmake_${CMAKE_BUILD_TYPE}")
FILE(WRITE "${VERSION_FILE}" "")
FOREACH(LINE ${MAJOR_MINOR_PATCH_MK})
    STRING(REGEX REPLACE "(.*):=(.*)" "set(\\1 \\2)"
           LINE_CMAKE "${LINE}")
    FILE(APPEND "${VERSION_FILE}" "${LINE_CMAKE}\n")
ENDFOREACH(LINE)
INCLUDE("${VERSION_FILE}")

# FREEGLUT_BUILD_SHARED_LIBS is already a standard CMake variable, but we need to
# re-declare it here so it will show up in the GUI.
# by default, we want to build both
OPTION(FREEGLUT_BUILD_SHARED_LIBS "Build FreeGLUT shared library." ON)
OPTION(FREEGLUT_BUILD_STATIC_LIBS "Build FreeGLUT static library." ON)

# option for whether warnings and errors should be printed
OPTION(FREEGLUT_PRINT_ERRORS "Lib prints errors to stderr" ON)
#MARK_AS_ADVANCED(FREEGLUT_PRINT_ERRORS)
OPTION(FREEGLUT_PRINT_WARNINGS "Lib prints warnings to stderr" ON)
#MARK_AS_ADVANCED(FREEGLUT_PRINT_WARNINGS)

# option to also copy .pdb files to install directory when executing
# INSTALL target
IF(MSVC)
    OPTION(INSTALL_PDB "Also install .pdb files" ON)
ELSE()
    SET(INSTALL_PDB OFF)
ENDIF()

# OpenGL ES support
OPTION(FREEGLUT_GLES1 "Use OpenGL ES 1.x (requires EGL)" OFF)
OPTION(FREEGLUT_GLES2 "Use OpenGL ES 2.x (requires EGL) (overrides BUILD_GLES1)" OFF)



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
    src/fg_font.c
    src/fg_font_data.c
    src/fg_gamemode.c
    src/fg_geometry.c
    src/fg_gl2.c
    src/fg_gl2.h
    src/fg_init.c
    src/fg_init.h
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
    src/fg_teapot.c
    src/fg_teapot_data.h
    src/fg_videoresize.c
    src/fg_window.c
)
# TODO: OpenGL ES requires a compatible version of these files:
IF(NOT FREEGLUT_GLES2 AND NOT FREEGLUT_GLES1)
    LIST(APPEND FREEGLUT_SRCS
        src/fg_menu.c
    )
ELSE()
    LIST(APPEND FREEGLUT_SRCS
        src/gles_stubs.c
    )
ENDIF()

IF(WIN32)
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
        ${CMAKE_BINARY_DIR}/freeglut.rc # generated below from freeglut.rc.in
    )
    IF (MSVC AND NOT CMAKE_CL_64)
        # .def file only for 32bit Windows builds (TODO: MSVC only right
        # now, needed for any other Windows platform?)
        LIST(APPEND FREEGLUT_SRCS
            ${CMAKE_BINARY_DIR}/freeglutdll.def # generated below from src/freeglutdll.def.in
        )
    ENDIF()

ELSEIF(ANDROID)
    LIST(APPEND FREEGLUT_SRCS
        src/android/native_app_glue/android_native_app_glue.c
        src/android/native_app_glue/android_native_app_glue.h
        src/android/fg_internal_android.h
        src/android/fg_cursor_android.c
        src/android/fg_ext_android.c
        src/android/fg_gamemode_android.c
        src/android/fg_init_android.c
        src/android/fg_input_devices_android.c
        src/android/fg_joystick_android.c
        src/android/fg_main_android.c
        src/android/fg_main_android.h
        src/android/fg_runtime_android.c
        src/android/fg_spaceball_android.c
        src/android/fg_state_android.c
        src/android/fg_structure_android.c
        src/android/fg_window_android.c
    )
ELSE()
    LIST(APPEND FREEGLUT_SRCS
        src/x11/fg_cursor_x11.c
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
    IF(NOT(FREEGLUT_GLES2 OR FREEGLUT_GLES1))
        LIST(APPEND FREEGLUT_SRCS
            src/x11/fg_internal_x11_glx.h
            src/x11/fg_display_x11_glx.c
            src/x11/fg_state_x11_glx.c
            src/x11/fg_state_x11_glx.h
            src/x11/fg_window_x11_glx.c
            src/x11/fg_window_x11_glx.h
        )
    ENDIF()
ENDIF()
IF(FREEGLUT_GLES2 OR FREEGLUT_GLES1)
    LIST(APPEND FREEGLUT_SRCS
        src/egl/fg_internal_egl.h
        src/egl/fg_display_egl.c
        src/egl/fg_ext_egl.c
        src/egl/fg_init_egl.c
        src/egl/fg_init_egl.h
        src/egl/fg_state_egl.c
        src/egl/fg_state_egl.h
        src/egl/fg_structure_egl.c
        src/egl/fg_structure_egl.h
        src/egl/fg_window_egl.c
        src/egl/fg_window_egl.h
    )
ENDIF()

# For OpenGL ES (GLES):
# - compile with -DFREEGLUT_GLES1 and -DFREEGLUT_GLES2 to cleanly
#   bootstrap headers inclusion in freeglut_std.h; these constants
#   also need to be defined in client applications (e.g. through
#   pkg-config), but use GLES constants directly for all other needs
# - define GLES version-specific library
IF(FREEGLUT_GLES2)
  ADD_DEFINITIONS(-DFREEGLUT_GLES2)
  LIST(APPEND LIBS GLESv2 EGL)
ELSEIF(FREEGLUT_GLES1)
  ADD_DEFINITIONS(-DFREEGLUT_GLES1)
  LIST(APPEND LIBS GLESv1_CM EGL)
ELSE()
  FIND_PACKAGE(OpenGL REQUIRED)
  LIST(APPEND LIBS ${OPENGL_gl_LIBRARY})
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
ENDIF()

# lib m for math, not needed on windows
IF (NOT WIN32)
    # For compilation:
    LIST(APPEND LIBS m)
    # For CHECK_FUNCTION_EXISTS:
    LIST(APPEND CMAKE_REQUIRED_LIBRARIES m)
ENDIF()

IF(WIN32)
    # hide insecure CRT warnings, common practice
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    IF(MSVC)
        SET( CMAKE_DEBUG_POSTFIX "d" )
    ENDIF(MSVC)
    
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
  IF(!ANDROID)
    # not setting -ansi as EGL/KHR headers doesn't support it
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
  ENDIF()
ENDIF(CMAKE_COMPILER_IS_GNUCC)

INCLUDE(CheckIncludeFiles)
IF(UNIX AND NOT ANDROID)
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
            FIND_LIBRARY(X11_Xxf86vm_LIB NAMES Xxf86vm HINTS /usr/X11/lib)
        ENDIF()
        LIST(APPEND LIBS ${X11_Xxf86vm_LIB})
    ENDIF()
    IF(X11_Xinput_FOUND)
        # Needed for multi-touch:
        CHECK_INCLUDE_FILES("${X11_Xinput_INCLUDE_PATH}/X11/extensions/XInput2.h" HAVE_X11_EXTENSIONS_XINPUT2_H)
        LIST(APPEND LIBS ${X11_Xinput_LIB})
    ENDIF()
    # Add XQuartz GL to the linker command line on OSX and /usr/X11/lib to the
    # search path so it's possible to find Xxf86vm.
    IF(APPLE)
      LIST(APPEND LIBS GL)
      LINK_DIRECTORIES(/usr/X11/lib)
    ENDIF()
ENDIF()
IF(ANDROID)
    # -landroid for ANativeWindow
    # -llog for native Android logging
    LIST(APPEND LIBS android log)
ENDIF()

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
        # Some old Microsoft VC releases don't support unsigned long
        # long, but all we care about is support for unsigned __int64 on
        # MSVC, so test for presence of that type
        CHECK_TYPE_SIZE("unsigned __int64" U__INT64 BUILTIN_TYPES_ONLY)
    ELSEIF()
        CHECK_TYPE_SIZE("unsigned long long" ULONG_LONG BUILTIN_TYPES_ONLY)
    ENDIF()
ENDIF()

# The generated config.h is placed in the project's build directory, just to
# ensure that all CMake-generated files are kept away from the main source tree.
# As a result, the build directory must to be added to the include path list.
#
IF(${CMAKE_SYSTEM_NAME} MATCHES "Android" OR
   ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR
   ${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR
   ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
STRING(TOLOWER -${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWERCASE)
ELSE()
SET(SYSTEM_NAME_LOWERCASE "-other")
ENDIF()
FILE(WRITE ${CMAKE_BINARY_DIR}/config.h "
#if defined(_MSC_VER) || defined(__WATCOMC__) || defined(__MINGW32__) \\
    || defined(_WIN32) || defined(_WIN32_WCE) || defined(__CYGWIN__)
#    include \"config-windows.h\"
#elif defined(__ANDROID__)
#    include \"config-android.h\"
#elif defined(__APPLE__)
#    include \"config-darwin.h\"
#elif defined(__linux__)
#    include \"config-linux.h\"
#else
#    include \"config-other.h\"
#endif
")
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_BINARY_DIR}/config${SYSTEM_NAME_LOWERCASE}.h)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
# Add X11 include path on OSX so that XQuartz headers are found.
IF(APPLE)
    INCLUDE_DIRECTORIES(${X11_INC_SEARCH_PATH})
ENDIF()
ADD_DEFINITIONS(-DHAVE_CONFIG_H)
IF(WIN32)
    # we also have to generate freeglut.rc, which contains the version
    # number
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/freeglut.rc.in ${CMAKE_BINARY_DIR}/freeglut.rc)
    IF (MSVC AND NOT CMAKE_CL_64)
        # .def file only for 32bit Windows builds with Visual Studio
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/freeglutdll.def.in ${CMAKE_BINARY_DIR}/freeglutdll.def)
    ENDIF()
ENDIF()

IF(FREEGLUT_BUILD_SHARED_LIBS)
    ADD_LIBRARY(freeglut SHARED ${FREEGLUT_SRCS})
ENDIF()
IF(FREEGLUT_BUILD_STATIC_LIBS)
    ADD_LIBRARY(freeglut_static STATIC ${FREEGLUT_SRCS})
ENDIF()


IF(WIN32)
    LIST(APPEND LIBS winmm)
    IF(FREEGLUT_BUILD_SHARED_LIBS)
        SET_TARGET_PROPERTIES(freeglut PROPERTIES COMPILE_FLAGS -DFREEGLUT_EXPORTS)
    ENDIF()
    IF(FREEGLUT_BUILD_STATIC_LIBS)
        SET_TARGET_PROPERTIES(freeglut_static PROPERTIES COMPILE_FLAGS -DFREEGLUT_STATIC)
        # need to set machine:x64 for linker, at least for VC10, and
        # doesn't hurt for older compilers:
        # http://public.kitware.com/Bug/view.php?id=11240#c22768
        IF (CMAKE_CL_64)
            SET_TARGET_PROPERTIES(freeglut_static PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
        ENDIF()
    ENDIF()
ELSE()
    # on UNIX we need to make sure:
    # - all shared libraries must have a soname/version, see :
    #   http://sourceware.org/autobook/autobook/autobook_91.html#SEC91
    #   http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
    #   Current: -version-info 12:0:9 -> 3.9.0
    #   Note: most platforms now prefer the latter major.minor.revision form
    #   (e.g. FreeBSD, cf. http://debbugs.gnu.org/cgi/bugreport.cgi?bug=8765),
    #   or special-cased FreeGLUT long ago (e.g. .so.4 on OpenBSD), so
    #   the lack of support for current:revision:age in CMake should
    #   not be a problem.
    # - the output library should be named glut so it'll be linkable with -lglut
    # - the shared library should link to the dependency libraries so that the user
    #   won't have to link them explicitly (they shouldn't have to know that we depend
    #   on Xrandr or Xxf86vm)
    IF(FREEGLUT_GLES2)
      SET(LIBNAME freeglut-gles2)
    ELSEIF(FREEGLUT_GLES1)
      SET(LIBNAME freeglut-gles1)
    ELSE()
      SET(LIBNAME glut)
    ENDIF()

    SET_TARGET_PROPERTIES(freeglut PROPERTIES VERSION 3.9.0 SOVERSION 3 OUTPUT_NAME ${LIBNAME})
    SET_TARGET_PROPERTIES(freeglut_static PROPERTIES OUTPUT_NAME ${LIBNAME})
    IF(ANDROID)
        # Not in CMake toolchain file, because the toolchain
        # file is called several times and generally doesn't
        # seem to be meant for modifying CFLAGS:
        # '-mandroid' is not mandatory but doesn't hurt
        # '-O0 -gstabs+' helps the currently buggy GDB port
        # Too late to manipulate ENV: SET(ENV{CFLAGS} "$ENV{CFLAGS} -mandroid")
        # Not using _INIT variables, they seem to be used internally only
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mandroid")
        SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -gstabs+")
    ENDIF()
ENDIF()
IF(FREEGLUT_BUILD_SHARED_LIBS)
    TARGET_LINK_LIBRARIES(freeglut ${LIBS})
ENDIF()
IF(FREEGLUT_BUILD_STATIC_LIBS)
    TARGET_LINK_LIBRARIES(freeglut_static ${LIBS})
ENDIF()

IF(FREEGLUT_BUILD_SHARED_LIBS)
    INSTALL(TARGETS freeglut
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
    )
    IF(INSTALL_PDB)
        INSTALL(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/freeglut${CMAKE_DEBUG_POSTFIX}.pdb
            DESTINATION bin
			CONFIGURATIONS Debug)
    ENDIF()
ENDIF()
IF(FREEGLUT_BUILD_STATIC_LIBS)
    INSTALL(TARGETS freeglut_static
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
    )
    IF(INSTALL_PDB)
        INSTALL(FILES ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug/freeglut_static${CMAKE_DEBUG_POSTFIX}.pdb
            DESTINATION lib
			CONFIGURATIONS Debug)
    ENDIF()
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
        IF(FREEGLUT_BUILD_SHARED_LIBS)
            ADD_EXECUTABLE(${name} ${ARGN})
            TARGET_LINK_LIBRARIES(${name} ${DEMO_LIBS} freeglut)
            IF(WIN32 AND MSVC)
                SET_TARGET_PROPERTIES(${name} PROPERTIES DEBUG_POSTFIX d)
            ENDIF()
        ENDIF()
        IF(FREEGLUT_BUILD_STATIC_LIBS)
            ADD_EXECUTABLE(${name}_static ${ARGN})
            TARGET_LINK_LIBRARIES(${name}_static ${DEMO_LIBS} freeglut_static)
            SET_TARGET_PROPERTIES(${name}_static PROPERTIES COMPILE_FLAGS -DFREEGLUT_STATIC)
            IF(WIN32 AND MSVC)
                SET_TARGET_PROPERTIES(${name}_static PROPERTIES DEBUG_POSTFIX d)
            ENDIF()
        ENDIF()
    ENDIF()
ENDMACRO()

ADD_DEMO(CallbackMaker   progs/demos/CallbackMaker/CallbackMaker.c)
ADD_DEMO(Fractals        progs/demos/Fractals/fractals.c)
ADD_DEMO(Fractals_random progs/demos/Fractals_random/fractals_random.c)
ADD_DEMO(Lorenz          progs/demos/Lorenz/lorenz.c)
IF (NOT WIN32)
    ADD_DEMO(One             progs/demos/One/one.c)
ELSE()
    ADD_DEMO(One             progs/demos/One/one.c
                             progs/demos/One/one.rc)
ENDIF()
ADD_DEMO(Resizer         progs/demos/Resizer/Resizer.cpp)
ADD_DEMO(multi-touch     progs/demos/multi-touch/multi-touch.c)
ADD_DEMO(shapes          progs/demos/shapes/shapes.c
                         progs/demos/shapes/glmatrix.h
                         progs/demos/shapes/glmatrix.c)
ADD_DEMO(smooth_opengl3  progs/demos/smooth_opengl3/smooth_opengl3.c)
IF(UNIX)
    ADD_DEMO(spaceball       progs/demos/spaceball/spaceball.c
                             progs/demos/spaceball/vmath.c
                             progs/demos/spaceball/vmath.h)
ENDIF()
ADD_DEMO(subwin          progs/demos/subwin/subwin.c)
ADD_DEMO(timer           progs/demos/timer/timer.c)



# pkg-config support, to install at $(libdir)/pkgconfig
# Define static build dependencies
IF(WIN32)
  SET(PC_LIBS_PRIVATE "-lopengl32 -lwinmm -lgdi32 -lm")
ELSEIF(FREEGLUT_GLES2)
  IF(ANDROID)
    SET(PC_LIBS_PRIVATE "-llog -landroid -lGLESv2 -lEGL -lm")
  ELSE()
    SET(PC_LIBS_PRIVATE "-lX11 -lXxf86vm -lXrandr -lGLESv2 -lEGL -lm")
  ENDIF()
ELSEIF(FREEGLUT_GLES1)
  IF(ANDROID)
    SET(PC_LIBS_PRIVATE "-llog -landroid -lGLESv1_CM -lEGL -lm")
  ELSE()
    SET(PC_LIBS_PRIVATE "-lX11 -lXxf86vm -lXrandr -lGLESv1_CM -lEGL -lm")
  ENDIF()
ELSE()
  SET(PC_LIBS_PRIVATE "-lX11 -lXxf86vm -lXrandr -lGL -lm")
ENDIF()
# Client applications need to define FreeGLUT GLES version to
# bootstrap headers inclusion in freeglut_std.h:
SET(PC_LIBNAME "glut")
SET(PC_FILENAME "freeglut.pc")
IF(FREEGLUT_GLES2)
  SET(PC_CFLAGS "-DFREEGLUT_GLES2")
  SET(PC_LIBNAME "freeglut-gles2")
  SET(PC_FILENAME "freeglut-gles2.pc")
ELSEIF(FREEGLUT_GLES1)
  SET(PC_CFLAGS "-DFREEGLUT_GLES1")
  SET(PC_LIBNAME "freeglut-gles1")
  SET(PC_FILENAME "freeglut-gles1.pc")
ENDIF()
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/freeglut.pc.in ${CMAKE_BINARY_DIR}/freeglut.pc @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/freeglut.pc DESTINATION share/pkgconfig/ RENAME ${PC_FILENAME})
# TODO: change the library and .pc name when building for GLES,
# e.g. -lglut-GLES2
