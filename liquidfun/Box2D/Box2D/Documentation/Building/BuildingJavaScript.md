# Building JavaScript version

### Background

LiquidFun can be translated from C++ into JavaScript by
[Emscripten](http://github.com/kripken/emscripten/wiki).
The entire JavaScript translation is in one file: liquidfun.js.

The LiquidFun source includes a pre-made version of liquidfun.js.
You can reference that liquidfun.js to start writing LiquidFun programs
in JavaScript right away.

However, if you make changes to the LiquidFun C++ code, or if you need
to expose more of the LiquidFun API, you will need to remake liquidfun.js
by following the steps on this page.

### Before Building

We use Emscripten on Linux, but you should be able to use the Emscripten SDK
on Mac or Windows too, if you prefer. Note that Mac and Windows build
environments have not been tested.

-   Get Emscripten. See [Installing from Source](http://github.com/kripken/emscripten/wiki/Emscripten-SDK#installing-from-source). 
    -   You also need [Fastcomp](http://github.com/kripken/emscripten/wiki/LLVM-Backend#getting-fastcomp)
    -   And [Node.js](http://nodejs.org/download/)
-   Setup Emscripten.
    -   As suggested in the Emscripten docs, run "emcc -v"
    -   Update your ~/.emscripten file. You'll probably want something like this:
        -   LLVM_ROOT = os.path.expanduser('~/Code/emscripten-fastcomp/build/Release/bin')
        -   NODE_JS = os.path.expanduser('~/Code/node-v0.10.29-linux-x64/bin/node')
-   Get the [Closure Compiler](http://developers.google.com/closure/compiler/).
-   Set environment variables. Depending on your setup, you may want to add
    some lines like this to your ~/.bashrc file.
    - export EMSCRIPTEN=~/Code/emscripten
    - export CLOSURE_JAR=~/Code/closure/compiler.jar

### Building

The following commands run Emscripten to translate C++ to lf_core.js,
then combine lf_core.js with the bindings in jsBindings into liquidfun.js.

    cd Box2D/lfjs
    make
    ./uglify.sh

### Executing the Testbed

For an example program, try openning `Box2D/lfjs/index.html' in your browser.
This will load a JavaScript version of the LiquidFun Testbed. It uses
the liquidfun.js file that you built above.

