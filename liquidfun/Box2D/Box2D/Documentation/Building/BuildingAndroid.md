# Building for Android

This document assumes that you have installed the [Android NDK][] on the machine performing the build.

Each LiquidFun project for Android has an associated `AndroidManifest.xml` file and `jni` subdirectory.  For samples, `AndroidManifest.xml` contains details about how to build an Android package (apk). For libraries, such as LiquidFun and freeglut, the `AndroidManifest.xml` file informs the `ndk-build` tool that the directory contains NDK makefiles under the `jni` subdirectory.

The following Android NDK projects live in the liquidfun directory tree.

-   `liquidfun/Box2D` - liquidfun dynamic and static libraries.

-   `liquidfun/Box2D/HelloWorld` - Non-interactive Helloworld sample application.

-   `liquidfun/Box2D/Testbed` - Interactive Testbed sample application.

To build an Android NDK project:

-   Open a command line window.

-   Go to the working directory containing the project to build.

-   Execute ndk-build.

For example, to build the liquidfun dynamic and static libraries:

    cd liquidfun/Box2D
    ndk-build

Running an example requires the Android Development Toolkit ([ADT][]) and the [NDK Eclipse plugin][].

### Running an application using Eclipse:

-   Build a project using `ndk-build` (see above).

-   Open [ADT][] Eclipse.

-   Select `File->Import...` from the menu.

-   Select `Android > Existing Android Code Into Workspace`, click `Next`.

-   Click the `Browse...` button next to `Root Directory:` and select the project folder (e.g. `liquidfun/Box2D/Testbed`).

-   Click `Finish` and the project should be imported into Eclipse and visible in the Package Explorer pane.

-   Right-click the project in the Package Explorer pane and select `Run->Run As->Android Application` from the menu.

-   If you do not have a physical device, you must define a virtual one. For details about how to define a virtual device, see [here][]. We don’t recommend a virtual device for development.

-   If the target is a physical device, unlock the device and observe the application executing.

### Build & Run from the command line:

It's also possible to build, deploy and execute an application using `build_apk.sh`.  `build_apk.sh` requires the installation of the [Android NDK][] and [Android SDK][NDK Eclipse plugin] along the Android SDK's prerequisites.  On Windows the script requires a [Cygwin installation][] as it makes use of bash and common unix utilities. In addition, `build_apk.sh` requires the addition of the Android SDK sdk/tools, Android SDK sdk/platform-tools and NDK root directories to the PATH environment variable.

Building and running an application using `build_apk.sh`:

-   Change into the directory containing the project to build.

-   Run build\_apk.sh.

For example, the following will build the Testbed application, deploy it to the connected device and start it...

    cd liquidfun/Box2D/Testbed
    ../AndroidUtil/build_apk.sh

NOTE: The Testbed application currently only runs on a physical target.

By default code is generated for devices that support the `armeabi-v7a` ABI. To generate code for all ABIs to include in a fat `.apk` override APP\_ABI on ndk-build's command line...

    ndk-build APP_ABI=all

 Use the run\_tests\_android.sh script to execute unit tests:

    ./liquidfun/Box2D/Unittests/run_tests_android.sh

`run_tests_android.sh` will build, deploy and execute each unit test on the connected device using `build_apk.sh`. `run_tests_android.sh` makes use of `build_apk.sh` which has setup requirements as described above.

  [Android NDK]: http://developer.android.com/tools/sdk/ndk/index.html
  [NDK Eclipse plugin]: http://developer.android.com/sdk/index.html
  [here]: http://developer.android.com/tools/devices/managing-avds.html
  [Cygwin installation]: http://www.cygwin.com/
  [ADT]: http://developer.android.com/tools/sdk/eclipse-adt.html

