EXTRA_DIST = CallbackMaker.c CallbackMaker.dsp
noinst_PROGRAMS = CallbackMaker
CallbackMaker_SOURCES = CallbackMaker.c
CallbackMaker_LDFLAGS = -export-dynamic
CallbackMaker_LDADD = ../../../src/lib@LIBRARY@.la $(GL_LIBS)
CallbackMaker_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
