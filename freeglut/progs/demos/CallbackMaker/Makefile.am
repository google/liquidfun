EXTRA_DIST = CallbackMaker.c CallbackMaker.dsp
noinst_PROGRAMS = CallbackMaker
CallbackMaker_SOURCES = CallbackMaker.c
CallbackMaker_LDFLAGS = -export-dynamic ../../../src/lib@LIBRARY@.la
CallbackMaker_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
