EXTRA_DIST = Fractals.dsp fractals.c fractals.dat
noinst_PROGRAMS = fractals
fractals_SOURCES = fractals.c
fractals_LDFLAGS = -export-dynamic ../../../src/lib@LIBRARY@.la
fractals_CFLAGS = -I../../../include $(X_CFLAGS)
