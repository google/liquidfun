# Building for Windows

You can use [cmake][] to generate a [Visual Studio][] project for
LiquidFun's Testbed and Unittests on [Windows][].

Alternatively, you can download a pre-generated Visual Studio solution
from the [release page][]. The Visual Studio solution is free of
host-specific dependencies.

### Version Requirements

These are the minimum required versions for building LiquidFun for Windows:

-   Windows: 7
-   Visual Studio: 2010 or 2012
-   cmake: 2.8.12.1

### Creating the Visual Studio solution using [cmake][]

When working directly with the source, use [cmake][] to generate the
[Visual Studio][] solution and project files.  For example, the following
generates the [Visual Studio][] 2012 solution in the `liquidfun/Box2D`
directory:

    cd liquidfun\Box2D
    cmake -G "Visual Studio 11"

To generate a [Visual Studio][] 2010 solution, use this commend:

    cd liquidfun\Box2D
    cmake -G "Visual Studio 10"

Running [cmake][] under [cygwin][] requires empty TMP, TEMP, tmp and temp
variables.  To generate a [Visual Studio][] solution from a [cygwin][]
bash shell use:

    $ cd liquidfun/Box2D
    $ ( unset {temp,tmp,TEMP,TMP} ; cmake -G "Visual Studio 11" )

### Building with [Visual Studio][]

-   Double-click on `liquidfun/Box2D/Box2D.sln` to open the solution.
-   Select "Build-->Build Solution" from the menu.

### Executing a Sample

-   Right-click on an example project (e.g Testbed) in the Solution Explorer
    pane, and select "Set as StartUp Project".
-   Select "Debug-->Start Debugging" from the menu.

### Running Unit Tests

Developers modifying the LiquidFun library can verify that it is still working
correctly by executing the provided unit tests.

Use the run\_tests.bat batch file to execute unit tests:

    liquidfun\Box2D\Unittests\run_tests.bat

  [cmake]: http://www.cmake.org
  [Visual Studio]: http://www.visualstudio.com/
  [Windows]: http://windows.microsoft.com/
  [cygwin]: http://www.cygwin.com/
  [release page]: http://github.com/google/liquidfun/releases
