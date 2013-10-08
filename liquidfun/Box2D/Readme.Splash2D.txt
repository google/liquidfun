Splash2D is an extension of Box2D, a 2D physics engine for games
(see http://www.box2d.org).

API documentation is located in Documentation/.

Building Splash2D and Running Examples
--------------------------------------

=== Android ===

The following assumes the Android NDK
(http://developer.android.com/tools/sdk/ndk/index.html) is installed on the
machine performing the build.

Each Splash2D project for Android has an associated AndroidManifest.xml
file and jni subdirectory.  In the case of samples, AndroidManifest.xml
contain details of how to build an Android package (apk).  For libraries like
Splash2D and freeglut, the AndroidManifest.xml file is used to inform the
ndk-build tool that the directory contains NDK makefiles under the jni
subdirectory.

The following Android NDK projects are present in the splash2d directory tree:
* splash2d/Box2D             - splash2d dynamic and static libraries.
* splash2d/Box2D/HelloWorld  - Non-interactive Helloworld sample application.
* splash2d/Box2D/Testbed     - Interactive Testbed sample application.

To build an Android NDK project:
* Open a command line window.
* Change the working directory to the project to build.
* Execute ndk-build.

For example, to build the splash2d dynamic and static libraries:

$ cd splash2d/Box2D
$ ndk-build

Running an example, requires the Android Development Toolkit (ADT) and the NDK
Eclipse plugin (see http://developer.android.com/sdk/index.html).

* Build a project using "ndk-build" see above.
* Open ADT Eclipse.
* Select "File->Import..." from the menu.
* Select "Android > Existing Android Code Into Workspace", click "Next".
* Click the "Browse..." button next to "Root Directory:" and select the project
  folder (e.g splash2d/Box2D/Testbed).
* Optionally rename "NativeActivity" to a meaningful project name.
* Click "Finish" and the project should be imported into Eclipse and visible
  in the Package Explorer pane.
* Select the project in the Package Explorer pane and select
  "Run->Run As->Android Application" from the menu.
* Choose a device to execute the application on and click OK.
* If the target is a physical device, unlock the device and observe the
  application executing.


=== Linux ===

The Linux build system requires "cmake" and has been tested with cmake 2.8.7.
cmake can either be installed using the Linux distribution's package manager
(e.g Ubuntu - "sudo apt-get install cmake") or by downloading the package
from cmake.org (http://www.cmake.org/cmake/resources/software.html).
In addition, to build sample applications the OpenGL and GLU
(e.g libglapi-mesa & libglu1-mesa-dev) development libraries need to be
installed on the development system.

Building with cmake requires the generation of makefiles from the cmake
project and then executing "make" to build the library and sample applications.

The cmake project directory for splash2d is located in splash2d/Box2D.
It's possible to build all build targets using the following:

$ cd splash2d/Box2D
$ cmake -G'Unix Makefiles' -DBOX2D_INSTALL=OFF
$ make

It's possible to configure the build targets using options in exposed in
splash2d/Box2D/CMakeLists.txt by using cmake's -D option.

After building the project it's possible to execute the samples from the
command line.  For example...

$ ./splash2d/Box2D/Testbed/Testbed


=== Windows ===

A Visual Studio solution (splash2d/Box2D/Box2D.sln) and project files
are distributed with splash2d to build the library and examples on Windows.
The Visual Studio solution is generated using cmake and modified to remove any
host specific dependencies.  This has been tested with Microsoft Visual Studio
2012.

To build with Visual Studio:
* Double click on splash2d/Box2D/Box2D.sln to open the solution.
* Select "Build->Build Solution" from the menu.

To execute a sample:
* Right click on an example project (e.g Testbed) in the Solution Explorer
  pane and select "Set as StartUp Project".
* Select "Debug->Start Debugging" from the menu.

When working with the source distribution, it's possible to generate the
Visual Studio solution and project file using cmake (see http://www.cmake.org).
For example...

> cd splash2d/Box2D
> cmake -G"Visual Studio 11" -DBOX2D_INSTALL=OFF

will generate the Visual Studio solution in the splash2d/Box2D directory.


=== OSX ===

A Xcode project is distributed with splash2d to build the library and examples
on OSX.  The Xcode project is generated using cmake and modified to remove any
host specific dependencies.  Xquartz (http://xquartz.macosforge.org/) is
required to run splash2d sample applications.  After installing Xquartz the
machine must be rebooted to set the DISPLAY environment variable correctly.

To build using Xcode:
* Double click on splash2d/Box2D/Box2D.xcodeproj to open the project in Xcode.
* Select "Product->Build" from the menu.

To execute a sample:
* Select a sample Scheme, for example "Testbed -> My Mac 64-bit" from the combo
  box to the right of the "Run" button.
* Click the "Run" button.

When working with the source distribution, it's possible to generate the
Xcode project using cmake (see http://www.cmake.org).  For example...

$ cd splash2d/Box2D
$ cmake -G"Xcode" -DBOX2D_INSTALL=OFF

will generate the Xcode project in the splash2d/Box2D directory.
