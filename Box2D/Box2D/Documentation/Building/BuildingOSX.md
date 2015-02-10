# Building for OS X

You can use [cmake][] to generate an [Xcode][] project for LiquidFun's
Testbed and Unittests on [OS X][].

Alternatively, you can download a pre-generated Xcode
project from the [release page][]. The [Xcode][] project is free of
host-specific dependencies.

### Version Requirements

These are the minimum required versions for building LiquidFun on OS X:

-   OS X: Mavericks 10.9.1.
-   Xcode: 5.0.1
-   Xquartz: 2.7.5 (xorg-server 1.14.4)
-   cmake (when building from source) 2.8.12.1

### Before Building

-   Install [Xquartz][] to run LiquidFun sample applications.
    The Testbed uses [freeglut][], which requires [Xquartz][].
-   Reboot your machine.  Rebooting sets the `DISPLAY` environment variable for
    [Xquartz][], which enables sample applications to run correctly.

### Creating the Xcode project using [cmake][]

When working directly with the source, you can generate the [Xcode][]
project using [cmake][].  [cmake][] version 2.8.12.1 or above is required to
build this project on [OS X][] 10.9 (Mavericks).  For example, the following
generates the Xcode project in the liquidfun/Box2D directory.

    cd liquidfun/Box2D
    cmake -G "Xcode"

### Building with [Xcode][]

-   Double-click on `liquidfun/Box2D/Box2D.xcodeproj` to open the project in
    [Xcode][].
-   Select "Product-->Build" from the menu.

### Executing a Sample

-   Select a sample `Scheme`, for example "Testbed-->My Mac 64-bit", from the
    combo box to the right of the "Run" button.
-   Click the "Run" button.

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
  [release page]: http://github.com/google/liquidfun/releases
