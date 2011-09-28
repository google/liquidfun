EXTRA_DIST = spaceball.c vmath.c vmath.h vmath.inl
noinst_PROGRAMS = spaceball
spaceball_SOURCES = spaceball.c vmath.c
spaceball_LDFLAGS = -export-dynamic
spaceball_LDADD = ../../../src/lib@LIBRARY@.la $(GL_LIBS) -lm
spaceball_CFLAGS = -I$(top_srcdir)/include $(X_CFLAGS)
