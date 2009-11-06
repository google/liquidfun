## Process this file with automake to produce Makefile.im

#
# The library we want to build
#
lib_LTLIBRARIES = lib@LIBRARY@.la

#
# Headers and extra stuff we want to distribute
#
noinst_HEADERS = freeglut_internal.h freeglut_teapot_data.h

EXTRA_DIST = freeglutdll.def

#
# Those source files build the freeglut library
#
lib@LIBRARY@_la_SOURCES = freeglut_callbacks.c \
			 freeglut_cursor.c \
			 freeglut_display.c \
			 freeglut_ext.c \
			 freeglut_font.c \
			 freeglut_glutfont_definitions.c \
			 freeglut_font_data.c \
			 freeglut_stroke_roman.c \
			 freeglut_stroke_mono_roman.c \
			 freeglut_gamemode.c \
			 freeglut_geometry.c \
			 freeglut_init.c \
			 freeglut_input_devices.c \
			 freeglut_spaceball.c \
			 freeglut_joystick.c \
			 freeglut_main.c \
			 freeglut_menu.c \
			 freeglut_misc.c \
			 freeglut_overlay.c \
			 freeglut_state.c \
			 freeglut_structure.c \
			 freeglut_teapot.c \
			 freeglut_videoresize.c \
			 freeglut_window.c

#
# Additional linker flags
#	 
lib@LIBRARY@_la_LIBADD  = $(LIBM) $(X_LIBS) $(GL_LIBS) $(LIBXXF86VM) $(LIBXI)
lib@LIBRARY@_la_LDFLAGS = $(VERSION_INFO) -no-undefined
lib@LIBRARY@_la_CFLAGS  = $(X_CFLAGS) $(EXPORT_FLAGS)
lib@LIBRARY@_la_CPPFLAGS= -I$(top_srcdir)/include

#
# End of file
#
