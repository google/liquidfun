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

### Debugging

The default output of `make` and `uglify.sh` is whitespace-optimized
JavaScript that is difficult to debug. To create an un-optimized version
of the testbed, please follow the steps below.

+ In `lfjs/Makefile` change -O2 to -O0, to disable Emscripten optimizations.
+ In `lfjs/index.html` replace `<script src="liquidfun.js"></script>` with
  a similar line for every .js file in `lfjs/uglify.sh`. It should look
  something like,

        <script src="lf_core.js"></script>
        <script src="jsBindings/offsets.js"></script>
        <script src="jsBindings/Common/b2Math.js"></script>
        <script src="jsBindings/Collision/b2Collision.js"></script>
        <script src="jsBindings/Collision/Shapes/b2EdgeShape.js"></script>
        <script src="jsBindings/Collision/Shapes/b2PolygonShape.js"></script>
        <script src="jsBindings/Collision/Shapes/b2Shape.js"></script>
        <script src="jsBindings/Collision/Shapes/b2ChainShape.js"></script>
        <script src="jsBindings/Collision/Shapes/b2CircleShape.js"></script>
        <script src="jsBindings/Dynamics/b2Body.js"></script>
        <script src="jsBindings/Dynamics/b2World.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2WheelJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2WeldJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2GearJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2Joint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2FrictionJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2RevoluteJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2MotorJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2PulleyJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2DistanceJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2PrismaticJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2RopeJoint.js"></script>
        <script src="jsBindings/Dynamics/Joints/b2MouseJoint.js"></script>
        <script src="jsBindings/Dynamics/Contacts/b2Contact.js"></script>
        <script src="jsBindings/Dynamics/b2Fixture.js"></script>
        <script src="jsBindings/Dynamics/b2WorldCallbacks.js"></script>
        <script src="jsBindings/Particle/b2ParticleSystem.js"></script>
        <script src="jsBindings/Particle/b2ParticleGroup.js"></script>
        <script src="jsBindings/Particle/b2Particle.js"></script>

