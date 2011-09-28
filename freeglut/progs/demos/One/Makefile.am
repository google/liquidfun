EXTRA_DIST = one.c one.dsp
noinst_PROGRAMS = one
one_SOURCES = one.c
one_LDFLAGS = -export-dynamic
one_LDADD = ../../../src/lib@LIBRARY@.la $(GL_LIBS)
one_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
