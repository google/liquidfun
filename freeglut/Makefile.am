## Process this file with automake to produce Makefile.in

SUBDIRS = src include progs doc
AUTOMAKE_OPTIONS = gnu

EXTRA_DIST = \
	README.win32 LISEZ_MOI \
	freeglut.lsm autogen.sh \
	freeglut.kdevprj freeglut.kdevelop \
	freeglut.dsp freeglut.dsw \
	freeglut_static.dsp \
	freeglut_evc4.vcp freeglut_evc4.vcw

# Thanks to WinDoze, some files in CVS have the "execute" bit set when they
# shoudn't. We could fix this if we had direct access to the CVS repository,
# but we are not that lucky. Instead we fix this at distribution creation time.
dist-hook:
	find $(distdir) -type f \( \
	    -name "*.c" -o \
	    -name "*.dsp" -o \
	    -name "*.dsw" -o \
	    -name "*.h" -o \
	    -name "*.html" -o \
	    -name "*.png" -o \
	    -name "LISEZ_MOI" -o \
	    -name "README*" \
	    \) -exec chmod -x {} \;

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
