EXTRA_DIST = lorenz.c lorenz.dsp

noinst_PROGRAMS = lorenz
lorenz_SOURCES = lorenz.c
lorenz_LDFLAGS = -export-dynamic ../../../src/libglut.la
lorenz_CFLAGS = -I../../../include $(X_CFLAGS)
