EXTRA_DIST = Fractals.dsp fractals.c fractals.dat
noinst_PROGRAMS = fractals
fractals_SOURCES = fractals.c
fractals_LDFLAGS = -export-dynamic
fractals_LDADD = ../../../src/lib@LIBRARY@.la $(GL_LIBS)
fractals_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
