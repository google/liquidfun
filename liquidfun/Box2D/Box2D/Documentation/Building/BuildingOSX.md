# Building for OS X

LiquidFun releases contain an [Xcode][] project to build the library and
samples on [OS X][].  The [Xcode][] project has been generated using
[cmake][], and is free of any host-specific dependencies.

### Version Requirements

Following are the minimum required versions for the tools and libraries you
need for building LiquidFun for OS X:

-   OS X: Mavericks 10.9.1.
-   Xcode: 5.0.1
-   Xquartz: 2.7.5 (xorg-server 1.14.4)
-   cmake (when building from source) 2.8.12.1

### Before Building

-   Install [Xquartz][] to run LiquidFun sample applications.
    [freeglut][] requires [Xquartz][].
-   Reboot your machine.  Rebooting sets the `DISPLAY` environment variable for
    [Xquartz][], which enables sample applications to run correctly.

### Building with [Xcode][]

-   Double-click on `liquidfun/Box2D/Box2D.xcodeproj` to open the project in
    [Xcode][].
-   Select "Product-->Build" from the menu.

### Executing a Sample

-   Select a sample `Scheme`, for example "Testbed-->My Mac 64-bit", from the
    combo box to the right of the "Run" button.
-   Click the "Run" button.

### Building using [cmake][]

When working directly with the source, you can generate the [Xcode][]
project using [cmake][].  [cmake][] version 2.8.12.1 or above is required to
build this project on [OS X][] 10.9 (Mavericks).  For example, the following
generates the Xcode project in the liquidfun/Box2D directory.

    cd liquidfun/Box2D
    cmake -G "Xcode"

### Running Unit Tests

Developers modifying the LiquidFun library can verify that it is still working
correctly by executing the provided unit tests.

Use the run\_tests.sh script to execute unit tests:

    ./liquidfun/Box2D/Unittests/run_tests.sh

  [Xquartz]: http://xquartz.macosforge.org/
  [cmake]: http://www.cmake.org
  [Xcode]: http://developer.apple.com/xcode/
  [OS X]: http://www.apple.com/osx/
  [freeglut]: http://freeglut.sourceforge.net/
