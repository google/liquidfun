# Building for Windows

The liquidfun distribution includes a Visual Studio solution (`liquidfun/Box2D/Box2D.sln`) and project files to build the library and examples on Windows. The Visual Studio solution has been generated using cmake, and modified to remove any host-specific dependencies. It has been tested with Microsoft Visual Studio 2012.

To build with Visual Studio:

-   Double-click on liquidfun/Box2D/Box2D.sln to open the solution.

-   Select "Build-\>Build Solution" from the menu.

To execute a sample:

-   Right-click on an example project (e.g Testbed) in the Solution Explorer pane and select "Set as StartUp Project".

-   Select "Debug-\>Start Debugging" from the menu.

When working with the source distribution, you can use [cmake][] to generate the Visual Studio solution and project file.  For example, the following generates the Visual Studio solution in the `liquidfun/Box2D` directory.

    cd liquidfun/Box2D
    cmake -G"Visual Studio 11"

Use the run\_tests.bat batch file to execute unit tests:

    liquidfun\Box2D\Unittests\run_tests.bat

  [cmake]: http://www.cmake.org
