EXTRA_DIST = shapes.c shapes.dsp
noinst_PROGRAMS = shapes
shapes_SOURCES = shapes.c
shapes_LDFLAGS = -export-dynamic
shapes_LDADD = ../../../src/lib@LIBRARY@.la $(GL_LIBS)
shapes_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
