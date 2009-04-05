## Process this file with automake to produce Makefile.in

SUBDIRS = src include progs doc
AUTOMAKE_OPTIONS = gnu

EXTRA_DIST = \
	FrequentlyAskedQuestions \
	LISEZ_MOI \
	README.win32 \
	autogen.sh \
	freeglut.dep \
	freeglut.dsp \
	freeglut.dsw \
	freeglut.kdevelop \
	freeglut.kdevprj \
	freeglut.lsm \
	freeglut.mak \
	freeglut.rc \
	freeglut.spec \
	freeglut.tgt \
	freeglut.wpj \
	freeglut_evc4.vcp \
	freeglut_evc4.vcw \
	freeglut_static.dep \
	freeglut_static.dsp \
	freeglut_static.mak \
	freeglut_static.tgt \
	freeglut_static_vs2008.vcproj \
	freeglut_vs2008.sln \
	freeglut_vs2008.vcproj \
	progs/demos/CallbackMaker/CallbackMakerStatic.dsp \
	progs/demos/CallbackMaker/CallbackMakerStatic_vs2008.vcproj \
	progs/demos/CallbackMaker/CallbackMaker_vs2008.vcproj \
	progs/demos/Fractals/FractalsStatic.dsp \
	progs/demos/Fractals/FractalsStatic_vs2008.vcproj \
	progs/demos/Fractals/Fractals_vs2008.vcproj \
	progs/demos/Fractals_random/Fractals_randomStatic.dsp \
	progs/demos/Fractals_random/Fractals_randomStatic_vs2008.vcproj \
	progs/demos/Fractals_random/Fractals_random_vs2008.vcproj \
	progs/demos/Lorenz/lorenzStatic.dsp \
	progs/demos/Lorenz/lorenzStatic_vs2008.vcproj \
	progs/demos/Lorenz/lorenz_vs2008.vcproj \
	progs/demos/One/oneStatic.dsp \
	progs/demos/One/oneStatic_vs2008.vcproj \
	progs/demos/One/one_vs2008.vcproj \
	progs/demos/demos_vs2008.sln \
	progs/demos/shapes/shapesStatic.dsp \
	progs/demos/shapes/shapesStatic_vs2008.vcproj \
	progs/demos/shapes/shapes_vs2008.vcproj \
	progs/demos/smooth_opengl3/smooth_opengl3Static.dsp \
	progs/demos/smooth_opengl3/smooth_opengl3Static_vs2008.vcproj \
	progs/demos/smooth_opengl3/smooth_opengl3_vs2008.vcproj

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
