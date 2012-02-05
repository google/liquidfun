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
	VisualStudio/2010/freeglut.vcxproj.filters \
	VisualStudio/2010/smooth_opengl3/smooth_opengl3.vcxproj.filters \
	VisualStudio/2010/smooth_opengl3/smooth_opengl3.vcxproj \
	VisualStudio/2010/One/One.vcxproj \
	VisualStudio/2010/One/One.vcxproj.filters \
	VisualStudio/2010/Fractals_random/Fractals_random.vcxproj \
	VisualStudio/2010/Fractals_random/Fractals_random.vcxproj.filters \
	VisualStudio/2010/Resizer/Resizer.vcxproj \
	VisualStudio/2010/Resizer/Resizer.vcxproj.filters \
	VisualStudio/2010/freeglut.sln \
	VisualStudio/2010/Lorenz/Lorenz.vcxproj.filters \
	VisualStudio/2010/Lorenz/Lorenz.vcxproj \
	VisualStudio/2010/shapes/shapes.vcxproj.filters \
	VisualStudio/2010/shapes/shapes.vcxproj \
	VisualStudio/2010/Fractals/Fractals.vcxproj.filters \
	VisualStudio/2010/Fractals/Fractals.vcxproj \
	VisualStudio/2010/freeglut.vcxproj \
	VisualStudio/2010/subwin/subwin.vcxproj.filters \
	VisualStudio/2010/subwin/subwin.vcxproj \
	VisualStudio/2010/CallbackMaker/CallbackMaker.vcxproj \
	VisualStudio/2010/CallbackMaker/CallbackMaker.vcxproj.filters \
	VisualStudio/2008/smooth_opengl3/smooth_opengl3.vcproj \
	VisualStudio/2008/One/One.vcproj \
	VisualStudio/2008/Fractals_random/Fractals_random.vcproj \
	VisualStudio/2008/Resizer/Resizer.vcproj \
	VisualStudio/2008/freeglut.sln \
	VisualStudio/2008/Lorenz/Lorenz.vcproj \
	VisualStudio/2008/shapes/shapes.vcproj \
	VisualStudio/2008/Fractals/Fractals.vcproj \
	VisualStudio/2008/subwin/subwin.vcproj \
	VisualStudio/2008/CallbackMaker/CallbackMaker.vcproj \
	VisualStudio/2008/freeglut.vcproj \
	progs/demos/CallbackMaker/CallbackMakerStatic.dsp \
	progs/demos/Fractals/FractalsStatic.dsp \
	progs/demos/Fractals_random/Fractals_randomStatic.dsp \
	progs/demos/Lorenz/lorenzStatic.dsp \
	progs/demos/One/oneStatic.dsp \
	progs/demos/shapes/shapesStatic.dsp \
	progs/demos/smooth_opengl3/smooth_opengl3Static.dsp

# Thanks to WinDoze, some files in CVS have the "execute" bit set when they
# shouldn't. We could fix this if we had direct access to the CVS repository,
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
