# Process this file with autoconf to produce a configure script.

AC_INIT([freeglut library], [2.2.0], [freeglut-bugs@lists.sourceforge.net], [freeglut])
AM_INIT_AUTOMAKE
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([AUTHORS])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_C_CONST
AC_EXEEXT
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

# Checks for libraries.
AC_PATH_XTRA
AC_CHECK_LIB([Xxf86vm], [XF86VidModeSwitchToMode],
	     [LIBXXF86VM=-lXxf86vm], [LIBXXF86VM=],
             [$X_LIBS -lXext -lX11])
AC_SUBST([LIBXXF86VM])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([GL/gl.h GL/glu.h GL/glx.h fcntl.h limits.h sys/ioctl.h sys/param.h sys/time.h])
AC_HEADER_TIME

save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $X_CFLAGS"
AC_CHECK_HEADERS([X11/extensions/xf86vmode.h], [], [], [#include <X11/Xlib.h>])
CFLAGS="$save_CFLAGS"

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_CHECK_LIBM
AC_SUBST([LIBM])

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
  CFLAGS="$CFLAGS -Wall -pedantic -Werror"
fi

# Generate output.
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 include/GL/Makefile
                 include/Makefile
                 progs/Makefile
                 progs/demos/CallbackMaker/Makefile
                 progs/demos/Fractals/Makefile
                 progs/demos/Fractals_random/Makefile
                 progs/demos/Lorenz/Makefile
                 progs/demos/Makefile
                 progs/demos/One/Makefile
                 progs/demos/shapes/Makefile
                 src/Makefile])
AC_OUTPUT
