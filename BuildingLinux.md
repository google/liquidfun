# Building for Linux

The Linux build system requires `cmake` and has been tested with cmake 2.8.7. You can install cmake using the Linux distribution's package manager (e.g Ubuntu - `sudo apt-get install cmake`) or by downloading the package from [cmake.org][]. In addition, to build sample applications, the OpenGL and GLU (e.g `libglapi-mesa` & `libglu1-mesa-dev`) development libraries need to be installed on the development system.

Building with `cmake` requires the generation of makefiles from the cmake project, and then executing `make` to build the library and sample applications.

The `cmake` project directory for liquidfun is located in liquidfun/Box2D. You can build all build targets using the following.

    cd liquidfun/Box2D
    cmake -G'Unix Makefiles'
    make

You can configure the build targets using options exposed in `liquidfun/Box2D/CMakeLists.txt` by using cmake's `-D` option.

After building the project, you can execute the samples from the command line.  For example:

    ./liquidfun/Box2D/Testbed/Testbed

Use the run\_tests.sh script to execute unit tests:

    ./liquidfun/Box2D/Unittests/run_tests.sh

  [cmake.org]: http://www.cmake.org/cmake/resources/software.html
