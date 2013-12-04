# Building for Windows

LiquidFun releases include a [Visual Studio][] solution
(`liquidfun/Box2D/Box2D.sln`) and project files to build the library and
examples on [Windows][].  The Visual Studio solution has been generated using
[cmake][], and modified to remove any host-specific dependencies.  It has been
tested with Microsoft [Visual Studio][] 2012.

### Building with [Visual Studio][]

-   Double-click on `liquidfun/Box2D/Box2D.sln` to open the solution.
-   Select "Build-->Build Solution" from the menu.

### Executing a Sample

-   Right-click on an example project (e.g Testbed) in the Solution Explorer
    pane and select "Set as StartUp Project".
-   Select "Debug-->Start Debugging" from the menu.

### Building using [cmake][]

When working directly with the source, generate the [Visual Studio][] solution
and project files using [cmake][].  For example, the following generates the
[Visual Studio][] solution in the `liquidfun/Box2D` directory.

    cd liquidfun/Box2D
    cmake -G"Visual Studio 11"

### Running Unit Tests

Developers modifying the LiquidFun library can verify that it still working
correctly by executing the provided unit tests.

Use the run\_tests.bat batch file to execute unit tests:

    liquidfun\Box2D\Unittests\run_tests.bat

  [cmake]: http://www.cmake.org
  [Visual Studio]: http://www.visualstudio.com/
  [Windows]: http://windows.microsoft.com/
