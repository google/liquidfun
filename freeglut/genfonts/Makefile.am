## Process this file with automake to produce Makefile.im

bin_PROGRAMS      = genfonts genstroke
genfonts_SOURCES  = genfonts.c
genstroke_SOURCES = genstroke.c
genfonts_LDADD    = $(X_LIBS) -lglib -lX11 -lXext
genstroke_LDADD   = 
EXTRA_DIST        = genfonts.c genstroke.c
