# LiquidFun JNI

LiquidFun JNI is a project that contains JNI (Java Native Interface) bindings
for Liquidfun. [SWIG][] generates these bindings, and is required for this
project.

This document describes how to setup a Java project to use LiquidFun. It
comprises the following sections:
*   "Overview": A high level description of the project.
*   "Installation": How to install [SWIG][].
*   "Build Instructions": How to include LiquidFun JNI in your project.
*   "Programmer's Guide": A discussion on how this differs from using LiquidFun
    in native C++ code.

### Overview

A subset of LiquidFun API has been exposed to [SWIG][]. The [SWIG][] interface
files are all of the file type .swig. They are located in `Box2D/swig/java`.
The file structure mirrors the file structure of Box2D for convenience. Some
LiquidFun header files are not directly pulled into SWIG so users can choose to
pull in only the functionality they need, reducing the number of JNI bindings
produced.

### Installation

Current [SWIG][] version tested: 2.0.11
To learn more about [SWIG][], and to install it, please visit their website.
In addition, you have to install PCRE as SWIG depends on it:
    apt-get install libpcre3-dev

Set up an environment variable to point to your swig installation. One way to
do it is to add to your `~/.bashrc` file:
    export SWIG_BIN=$("which" swig)

### Build Instructions

#### Using LiquidFun JNI in your Eclipse Android project

Include the library in your `jni/Android.mk` file as a shared library:

    LOCAL_SHARED_LIBRARIES := libliquidfun_jni
    include $(BUILD_SHARED_LIBRARY)
    $(call import-add-path,/path/to/liquidfun/)
    $(call import-module,Box2D/swig/jni)

This will invoke SWIG and build the C++ components of the library.
In Eclipse,

    Right-click your project > Properties.
    Click on "Java Build Path" on the left panel.
    In the Source tab, click "Link Source..."
    Browse to the Box2D/swig/gen folder, and give it a name.
    [Finish]

You should be able to build and run your application using LiquidFun. All
generated Java files reside in Liquidfun/Box2D/swig/gen, so you can use them
as references.

### Programmer's Guide

The LiquidFun SWIG interface files pulls in a subset of the LiquidFun API,
extends it, and presents it in a way consistent with typical Java style.

#### Function and Variable Renames

*   All b2 class prefixes have been removed.
*   All function names have been converted to CamelCase.
*   All m_ member variable prefixes have been removed.
*   All enum variables have been converted to upper case with underscores.

#### Memory Management

JVM (Java Virtual Machine) uses garbage collection to clean up memory, which
is very different from the user-managed memory model in C++. The following are
critical in order to facilitate efficient JNI memory management.

##### delete() method in generated Java classes

The user must use the SWIG-generated delete() method to clean up all LiquidFun
objects exposed through [SWIG][]. This is because we are allocating native
memory through every new object, and the native memory is not managed by JVM.

For example:

    BodyDef* bodyDef = new BodyDef();
    ...
    bodyDef.delete();

For a member variable that is a native object:

    private World mWorld = new World(0, 0);
    ...
    @Override
    protected void finalize() {
        mWorld.delete();
    }

##### Use primitive types whenever possible

Because the user has to be conscious of cleaning up any new objects, directly
exposing to Java a C++ function signature like

    void SetPosition(const b2Vec2& pos);

will create this method:

    void setPosition(Vec2 pos);

This method requires that the user creates Vec2 objects. Since Vec2 objects are
native objects, they have to be cleaned up with a delete() method, which can be
quite unwieldy when initializing large amounts of data.

Instead, we extend the interface using SWIG by adding a new function in the
corresponding SWIG interface file:

    %extend b2ParticleDef {
        void setPosition(float32 x, float32 y) {
            $self->position.Set(x, y);
        }
    };

which generates Java code:

    void setPosition(float x, float y);

which allows the user to call the function with primitive types (in this case,
floats) directly.

  [SWIG]: http://www.swig.org
