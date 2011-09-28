EXTRA_DIST = subwin.c subwin.dsp
noinst_PROGRAMS = subwin
subwin_SOURCES = subwin.c
subwin_LDFLAGS = -export-dynamic
subwin_LDADD = ../../../src/lib@LIBRARY@.la $(GL_LIBS)
subwin_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
