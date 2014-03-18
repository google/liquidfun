# Building for Windows

LiquidFun releases include a [Visual Studio][] solution
(`liquidfun/Box2D/Box2D.sln`) and project files to build the library and
examples on [Windows][].  The Visual Studio solution has been generated using
[cmake][], and is free of any host-specific dependencies.  It has been
tested with Microsoft [Visual Studio][] 2012.

### Version Requirements

Following are the minimum required versions for the tools and libraries you
need for building LiquidFun for Windows:

-   Windows: 7
-   Visual Studio: 2012
-   cmake: 2.8.12.1

### Building with [Visual Studio][]

-   Double-click on `liquidfun/Box2D/Box2D.sln` to open the solution.
-   Select "Build-->Build Solution" from the menu.

### Executing a Sample

-   Right-click on an example project (e.g Testbed) in the Solution Explorer
    pane, and select "Set as StartUp Project".
-   Select "Debug-->Start Debugging" from the menu.

### Building using [cmake][]

When working directly with the source, use [cmake][] to generate the
[Visual Studio][] solution and project files.  For example, the following
generates the [Visual Studio][] solution in the `liquidfun/Box2D` directory:

    cd liquidfun\Box2D
    cmake -G "Visual Studio 11"

Running [cmake][] under [cygwin][] requires empty TMP, TEMP, tmp and temp
variables.  To generate a [Visual Studio][] from a [cygwin][] bash shell use:

    $ cd liquidfun/Box2D
    $ ( unset {temp,tmp,TEMP,TMP} ; cmake -G "Visual Studio 11" )

### Running Unit Tests

Developers modifying the LiquidFun library can verify that it is still working
correctly by executing the provided unit tests.

Use the run\_tests.bat batch file to execute unit tests:

    liquidfun\Box2D\Unittests\run_tests.bat

  [cmake]: http://www.cmake.org
  [Visual Studio]: http://www.visualstudio.com/
  [Windows]: http://windows.microsoft.com/
  [cygwin]: http://www.cygwin.com/
