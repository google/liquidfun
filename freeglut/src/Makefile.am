## Process this file with automake to produce Makefile.im

#
# The library we want to build
#
lib_LTLIBRARIES = lib@LIBRARY@.la

#
# Headers and extra stuff we want to distribute
#
noinst_HEADERS = Common/freeglut_internal.h Common/freeglut_teapot_data.h

EXTRA_DIST = Common/freeglutdll.def

#
# Those source files build the freeglut library
#
lib@LIBRARY@_la_SOURCES = Common/freeglut_callbacks.c \
			 Common/freeglut_cursor.c \
			 Common/freeglut_display.c \
			 Common/freeglut_ext.c \
			 Common/freeglut_font.c \
			 Common/freeglut_font_data.c \
			 Common/freeglut_stroke_roman.c \
			 Common/freeglut_stroke_mono_roman.c \
			 Common/freeglut_gamemode.c \
			 Common/freeglut_geometry.c \
			 Common/freeglut_init.c \
			 Common/freeglut_input_devices.c \
			 Common/freeglut_spaceball.c \
			 Common/freeglut_joystick.c \
			 Common/freeglut_main.c \
			 Common/freeglut_menu.c \
			 Common/freeglut_misc.c \
			 Common/freeglut_overlay.c \
			 Common/freeglut_state.c \
			 Common/freeglut_structure.c \
			 Common/freeglut_teapot.c \
			 Common/freeglut_videoresize.c \
			 Common/freeglut_window.c

if TARGET_HOST_MS_WINDOWS
lib@LIBRARY@_la_SOURCES += \
	Common/xparsegeometry_repl.c \
	Common/xparsegeometry_repl.h \
	mswin/freeglut_cursor_mswin.c \
	mswin/freeglut_display_mswin.c \
	mswin/freeglut_ext_mswin.c \
	mswin/freeglut_gamemode_mswin.c \
	mswin/freeglut_init_mswin.c \
	mswin/freeglut_input_devices_mswin.c \
	mswin/freeglut_internal_mswin.h \
	mswin/freeglut_joystick_mswin.c \
	mswin/freeglut_main_mswin.c \
	mswin/freeglut_menu_mswin.c \
	mswin/freeglut_spaceball_mswin.c \
	mswin/freeglut_state_mswin.c \
	mswin/freeglut_structure_mswin.c \
	mswin/freeglut_window_mswin.c
endif

if TARGET_HOST_POSIX_X11
lib@LIBRARY@_la_SOURCES += \
	x11/freeglut_cursor_x11.c \
	x11/freeglut_display_x11.c \
	x11/freeglut_ext_x11.c \
	x11/freeglut_gamemode_x11.c \
	x11/freeglut_glutfont_definitions_x11.c \
	x11/freeglut_init_x11.c \
	x11/freeglut_input_devices_x11.c \
	x11/freeglut_internal_x11.h \
	x11/freeglut_joystick_x11.c \
	x11/freeglut_main_x11.c \
	x11/freeglut_menu_x11.c \
	x11/freeglut_spaceball_x11.c \
	x11/freeglut_state_x11.c \
	x11/freeglut_structure_x11.c \
	x11/freeglut_window_x11.c \
	x11/freeglut_xinput_x11.c
endif

#
# Additional linker flags
#	 
lib@LIBRARY@_la_LIBADD  = $(GL_LIBS) $(X_LIBS) $(LIBM) $(LIBUSBHID)
lib@LIBRARY@_la_LDFLAGS = $(VERSION_INFO) -no-undefined
lib@LIBRARY@_la_CFLAGS  = $(GL_FLAGS) $(X_CFLAGS) $(EXPORT_FLAGS)
lib@LIBRARY@_la_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/src/Common

#
# End of file
#
