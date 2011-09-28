EXTRA_DIST = smooth_opengl3.c smooth_opengl3.dsp
noinst_PROGRAMS = smooth_opengl3
smooth_opengl3_SOURCES = smooth_opengl3.c
smooth_opengl3_LDFLAGS = -export-dynamic
smooth_opengl3_LDADD = ../../../src/lib@LIBRARY@.la $(GL_LIBS)
smooth_opengl3_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
