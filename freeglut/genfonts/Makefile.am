## Process this file with automake to produce Makefile.im

bin_PROGRAMS      = genfonts genstroke
genfonts_SOURCES  = genfonts.c
genstroke_SOURCES = genstroke.c
genfonts_LDADD    = -L/usr/X11R6/lib -lglib -lX11 -lXext
genstroke_LDADD   = -lglib
EXTRA_DIST        = genfonts.c genstroke.c
