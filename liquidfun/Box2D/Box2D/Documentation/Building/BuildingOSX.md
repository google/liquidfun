# Building for OS X

A Xcode project is distributed with liquidfun to build the library and examples on OSX.  The Xcode project has been generated using cmake, and modified to remove any host-specific dependencies. You must have installed [Xquartz][] in order to run liquidfun sample applications. After installing Xquartz, you must reboot your machine to set the `DISPLAY` environment variable correctly.

To build using Xcode:

-   Double-click on liquidfun/Box2D/Box2D.xcodeproj to open the project in Xcode. 

-   Select "Product-\>Build" from the menu.

To execute a sample:

-   Select a sample Scheme, for example "Testbed -\> My Mac 64-bit" from the combo box to the right of the "Run" button.

-   Click the "Run" button.

When working with the source distribution, you can generate the Xcode project using [cmake][].  cmake version 2.8.12.1 or above is required to build this project on OSX 10.9 (Mavericks). For example, the following generates the Xcode project in the liquidfun/Box2D directory.

    cd liquidfun/Box2D
    cmake -G"Xcode"

Use the run\_tests.sh script to execute unit tests:

    ./liquidfun/Box2D/Unittests/run_tests.sh

  [Xquartz]: http://xquartz.macosforge.org/
  [cmake]: see http://www.cmake.org
