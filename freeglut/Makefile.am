## Process this file with automake to produce Makefile.in

SUBDIRS = src include progs doc
AUTOMAKE_OPTIONS = gnu

EXTRA_DIST = \
	AUTHORS COPYING ChangeLog INSTALL NEWS README TODO \
	README.win32 LISEZ_MOI \
	freeglut.lsm autogen.sh \
	freeglut.kdevprj freeglut.kdevelop \
	freeglut.dsp freeglut.dsw \
	freeglut_static.dsp \
	freeglut_evc4.vcp freeglut_evc4.vcw

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
