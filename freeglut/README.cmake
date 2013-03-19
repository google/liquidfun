How to build freeglut with CMake on Windows (MS Visual Studio)
--------------------------------------------------------------
1.  Download CMake (http://www.cmake.org/cmake/resources/software.html).
    Get one of the releases from the binary distribution section.
2.  Run the CMake installer, install wherever you like. 
3.  Launch CMake via Start > Program Files > CMake 2.8 > CMake (GUI)
    (note that the shortcut put by the installer on your desktop does NOT
    point to the CMake GUI program!)
4.  In the "Where is the source code" box, type or browse to the root
    directory of your freeglut source (so that's /freeglut, not
    /freeglut/src).
5.  In the "Where to build the binaries" box, type or browse to any
    folder you like - this will be where the Visual Studio solution will be
    generated. This folder does not have to exist yet.
6.  Hit the Configure button near the bottom of the window. 
7.  Pick your target compiler, make sure that its installed on your
    system of course!
8.  Answer Ok when asked if you want to create the build directory. 
9.  Wait for the configure process to finish. 
10. The screen will now have some configuration options on it, for
    instance specifying whether you want to build static and/or shared
    libraries (see below for a complete list). When you've selected your
    options, click the Configure button again.
11. The Generate button at the bottom will now be enabled. Click Generate.
12. The build files will now be generated in the location you picked. 

You can now navigate to the build directory you specified in step 5.
Open the freeglut.sln file that was generated in your build directory,
and compile as usual


How to build freeglut on UNIX
-----------------------------
- Make sure you have cmake installed. Examples:
  - Debian/Ubuntu: apt-get install cmake
  - Fedora: yum install cmake
  - FreeBSD: cd /usr/ports/devel/cmake && make install
  Or directly from their website:
  http://www.cmake.org/cmake/resources/software.html
- Make sure you have the basics for compiling code, such as C compiler
  (e.g., GCC) and the make package.
- Also make sure you have packages installed that provide the relevant
  header files for x11 (including xrandr) and opengl (e.g.,
  libgl1-mesa-dev, libx11-dev and libxrandr-dev on Debian/Ubuntu).
- Install XInput: libxi-dev / libXi-devel
- Run 'cmake .' in the freeglut directory to generate the makefile.
- Run 'make' to build, and 'make install' to install freeglut.
- If you wish to change any build options run 'ccmake .'


Breakdown of CMake configuration options
----------------------------------------
BUILD_SHARED_LIBS       [ON, OFF] Build freeglut as a shared library
BUILD_STATIC_LIBS       [ON, OFF] Build freeglut as a static library
CMAKE_BUILD_TYPE        [Empty, Debug, Release] Can be overriden by passing it
                        as a make variable during build.
CMAKE_INSTALL_PREFIX	Installation prefix (e.g. /usr/local on UNIX)
FREEGLUT_BUILD_DEMOS	[ON, OFF] Controls whether the demos are built or not.
FREEGLUT_GLES1          [ON, OFF] TODO
FREEGLUT_GLES2          [ON, OFF] TODO
