## Process this file with automake to produce Makefile.im

#
# The library we want to build
#
lib_LTLIBRARIES = lib@LIBRARY@.la

noinst_HEADERS = freeglut_internal.h
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
lib@LIBRARY@_la_LIBADD = $(LIBM) $(X_LIBS) -lGL -lGLU -lXext -lX11 $(LIBXXF86VM)
lib@LIBRARY@_la_LDFLAGS = -version-info 11:0:8
lib@LIBRARY@_la_CFLAGS = $(X_CFLAGS)

#
# End of file
#
