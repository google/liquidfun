EXTRA_DIST = lorenz.c lorenz.dsp

noinst_PROGRAMS = lorenz
lorenz_SOURCES = lorenz.c
lorenz_LDFLAGS = -export-dynamic ../../../src/lib@LIBRARY@.la
lorenz_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
