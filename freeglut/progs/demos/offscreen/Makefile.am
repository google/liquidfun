EXTRA_DIST = unmanaged.c unmanaged.dsp
noinst_PROGRAMS = offscreen
offscreen_SOURCES = offscreen.c
offscreen_LDFLAGS = -export-dynamic ../../../src/lib@LIBRARY@.la
offscreen_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
