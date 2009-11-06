# Process this file with autoconf to produce a configure script.

AC_INIT([freeglut library], [2.6.0], [freeglut-bugs@lists.sourceforge.net], [freeglut])
AM_INIT_AUTOMAKE
AC_PREREQ([2.56])
AC_CONFIG_SRCDIR([AUTHORS])
AC_CONFIG_HEADERS([config.h])

# Compatibility hack for older autoconf versions
m4_ifdef([AS_HELP_STRING], [], [AC_DEFUN([AS_HELP_STRING], [AC_HELP_STRING($][@)])])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_C_CONST
AC_EXEEXT
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

# Checks for libraries.
AC_PATH_XTRA

if test x"$no_x" = xyes; then
  GL_LIBS="-lopengl32 -lgdi32 -lwinmm"
  LIBXXF86VM=
  LIBXI=
  VERSION_INFO=
  EXPORT_FLAGS="-DFREEGLUT_EXPORTS"
else
  GL_LIBS="-lGL -lXext -lX11"
  AC_CHECK_LIB([Xxf86vm], [XF86VidModeSwitchToMode],
               [LIBXXF86VM=-lXxf86vm], [LIBXXF86VM=],
               [$X_LIBS -lXext -lX11])
  LIBXI=-lXi
  VERSION_INFO="-version-info 12:0:9"
  EXPORT_FLAGS=
fi
AC_SUBST([GL_LIBS])
AC_SUBST([LIBXXF86VM])
AC_SUBST([LIBXI])
AC_SUBST([VERSION_INFO])
AC_SUBST([EXPORT_FLAGS])

# Checks for header files.
AC_HEADER_STDC
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_CHECK_HEADERS([GL/gl.h GL/glu.h GL/glx.h fcntl.h limits.h sys/ioctl.h sys/param.h sys/time.h])
AC_HEADER_TIME
AC_CHECK_HEADERS([X11/extensions/xf86vmode.h], [], [], [#include <X11/Xlib.h>])
AC_CHECK_HEADERS([X11/extensions/XI.h X11/extensions/XInput.h])
CPPFLAGS="$save_CPPFLAGS"

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_VPRINTF
AC_CHECK_LIBM
AC_SUBST([LIBM])
AC_CHECK_FUNCS([gettimeofday])

# Build time configuration.
AC_ARG_ENABLE([replace-glut],
[AS_HELP_STRING([--enable-replace-glut],
                [be a replacement for GLUT @<:@default=yes@:>@])])
if test "x$enable_replace_glut" != xno; then
  LIBRARY=glut
  HEADER=glut.h
else
  LIBRARY=freeglut
  HEADER=
fi
AC_SUBST([LIBRARY])
AC_SUBST([HEADER])

AC_ARG_ENABLE([warnings],
[AS_HELP_STRING([--enable-warnings],
                [use all gcc compiler warnings @<:@default=yes@:>@])])
if test "x$enable_warnings" != xno -a "x$GCC" = xyes; then
  CFLAGS="$CFLAGS -Wall -pedantic"
fi

AC_ARG_ENABLE([warnings-as-errors],
[AS_HELP_STRING([--enable-warnings-as-errors],
                [make all warnings into errors @<:@default=no@:>@])])
if test "x$enable_warnings_as_errors" = xyes -a "x$GCC" = xyes; then
  CFLAGS="$CFLAGS -Werror"
fi

AC_ARG_ENABLE([debug],
[AS_HELP_STRING([--enable-debug],
                [enable debugging code (for developers) @<:@default=no@:>@])])
if test "x$enable_debug" = xyes; then
  AC_DEFINE([_DEBUG], [1], [Define to 1 if you want to include debugging code.])
fi

# Generate output.
AC_CONFIG_FILES([Makefile doc/Makefile include/GL/Makefile include/Makefile progs/Makefile progs/demos/CallbackMaker/Makefile progs/demos/Fractals/Makefile progs/demos/Fractals_random/Makefile progs/demos/Lorenz/Makefile progs/demos/Makefile progs/demos/One/Makefile progs/demos/shapes/Makefile progs/demos/smooth_opengl3/Makefile progs/demos/spaceball/Makefile src/Makefile])
AC_OUTPUT
