# Building for Linux

### Before Building

Prior to building, install the following components using the [Linux][]
distribution's package manager:
-    Install [cmake][] using the package manager or download and manually
     install from [cmake.org](http://cmake.org).
-    Install OpenGL (`libglapi-mesa`).
-    Install GLU (`libglu1-mesa-dev`).

For example on Ubuntu:

    sudo apt-get install cmake
    sudo apt-get install libglapi-mesa
    sudo apt-get install libglu1-mesa-dev

OpenGL and GLU are required by the sample applications.

### Building

-   Generate makefiles from the [cmake][] project in `liquidfun/Box2D`.
-   Execute `make` to build the library and sample applications.

For example:

    cd liquidfun/Box2D
    cmake -G'Unix Makefiles'
    make

Build targets can be configured using options exposed in
`liquidfun/Box2D/CMakeLists.txt` by using cmake's `-D` option.

### Executing a Sample

After building the project, you can execute the samples from the command line.
For example:

    ./liquidfun/Box2D/Testbed/Testbed

### Running Unit Tests

Developers modifying the LiquidFun library can verify that it still working
correctly by executing the provided unit tests.

Use the run\_tests.sh script to execute unit tests:

    ./liquidfun/Box2D/Unittests/run_tests.sh

  [cmake]: http://www.cmake.org
  [Linux]: http://en.wikipedia.org/wiki/Linux
  [Ubuntu]: http://www.ubuntu.com
