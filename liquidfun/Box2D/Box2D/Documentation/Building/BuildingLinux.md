# Building for Linux

### Version Requirements

Following are the minimum required versions for the tools and libraries you
need for building LiquidFun for Linux:

-   OpenGL: libglapi-mesa 8.0.4 (tested with libglapi-mesa 8.0.4-0ubuntu0)
-   GLU: libglu1-mesa-dev 8.0.4 (tested with libglu1-mesa-dev 8.0.4.0ubuntu0)
-   cmake (when building from source): 2.8.12.1

### Before Building

Prior to building, install the following components using the [Linux][]
distribution's package manager:
-    [cmake][]. You can also manually install from [cmake.org]
     (http://cmake.org).
-    OpenGL (`libglapi-mesa`).
-    GLU (`libglu1-mesa-dev`).

For example, on Ubuntu:

    sudo apt-get install cmake
    sudo apt-get install libglapi-mesa
    sudo apt-get install libglu1-mesa-dev

The sample applications require OpenGL and GLU.

### Building

-   Generate makefiles from the [cmake][] project in `liquidfun/Box2D`.
-   Execute `make` to build the library and sample applications.

For example:

    cd liquidfun/Box2D
    cmake -G'Unix Makefiles'
    make

To perform a debug build:

    cd liquidfun/Box2D
    cmake -G'Unix Makefiles' -DCMAKE_BUILD_TYPE=Debug
    make

Build targets can be configured using options exposed in
`liquidfun/Box2D/CMakeLists.txt` by using cmake's `-D` option.
Build configuration set using the `-D` option is sticky across subsequent
builds.

For example, if a build is performed using:

    cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
    make

to switch to a release build CMAKE_BUILD_TYPE must be explicitly specified:

    cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    make

### Executing a Sample

After building the project, you can execute the samples from the command line.
For example:

    ./liquidfun/Box2D/Testbed/Release/Testbed

### Running Unit Tests

Developers modifying the LiquidFun library can verify that it still working
correctly by executing the provided unit tests.

Use the run\_tests.sh script to execute unit tests:

    ./liquidfun/Box2D/Unittests/run_tests.sh

  [cmake]: http://www.cmake.org
  [Linux]: http://en.wikipedia.org/wiki/Linux
  [Ubuntu]: http://www.ubuntu.com
