EXTRA_DIST = shapes.c shapes.dsp
noinst_PROGRAMS = shapes
shapes_SOURCES = shapes.c
shapes_LDFLAGS = -export-dynamic ../../../src/lib@LIBRARY@.la
shapes_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
