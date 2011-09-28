EXTRA_DIST = lorenz.c lorenz.dsp
noinst_PROGRAMS = lorenz
lorenz_SOURCES = lorenz.c
lorenz_LDFLAGS = -export-dynamic
lorenz_LDADD = ../../../src/lib@LIBRARY@.la $(GL_LIBS) -lm
lorenz_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
