EXTRA_DIST = Fractals_random.dsp fractals.dat fractals_random.c
noinst_PROGRAMS = fractals_random
fractals_random_SOURCES = fractals_random.c
fractals_random_LDFLAGS = -export-dynamic
fractals_random_LDADD = ../../../src/lib@LIBRARY@.la $(GL_LIBS)
fractals_random_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
