# LiquidFun JNI

LiquidFun JNI is a project that contains JNI (Java Native Interface) bindings
for LiquidFun. [SWIG](http://www.swig.org) generates these bindings, and is required for this
project.

This document describes how to set up a Java project to use LiquidFun. It
comprises the following sections:
*   Overview: A high level description of the project.
*   Installation: How to install [SWIG](http://www.swig.org).
*   Build Instructions: How to include LiquidFun JNI in your project.
*   Programmer's Guide: A discussion on how this differs from using LiquidFun
    in native C++ code.

### Overview

A subset of LiquidFun API has been exposed to [SWIG](http://www.swig.org). The [SWIG](http://www.swig.org) interface
files are all of the .swig file type. They are located in `Box2D/swig/java`.
For convenience, the file structure mirrors the file structure of Box2D. Some
LiquidFun header files are not directly pulled into SWIG, which allows users to
pull in only the functionality they need. This reducing the number of JNI
bindings produced.

### Installation

Current [SWIG](http://www.swig.org) version tested: 2.0.11
To learn more about [SWIG](http://www.swig.org), and to install it, please visit their [website]
(http://www.swig.org).
In addition, you have to install PCRE, as SWIG depends on it:
&nbsp;&nbsp;&nbsp;`apt-get install libpcre3-dev`

Set up an environment variable to point to your swig installation. One way to
do this is to add the following to your `~/.bashrc` file:
&nbsp;&nbsp;&nbsp;`export SWIG_BIN=$("which" swig)`

### Build Instructions

#### Using LiquidFun JNI in your Eclipse Android project

Include the library in your `jni/Android.mk` file as a shared library:

&nbsp;&nbsp;&nbsp;`LOCAL_SHARED_LIBRARIES := libliquidfun_jni`
&nbsp;&nbsp;&nbsp;`include $(BUILD_SHARED_LIBRARY)`
&nbsp;&nbsp;&nbsp;`$(call import-add-path,/path/to/liquidfun/)`
&nbsp;&nbsp;&nbsp;`$(call import-module,Box2D/swig/jni)`

This will invoke SWIG, and build the C++ components of the library.

Next, launch Eclipse, and perform the following steps:

1. Right-click your project > Properties.
2. Click on "Java Build Path" on the left panel.
3. In the Source tab, click "Link Source..."
4. Browse to the `Box2D/swig/gen` folder, and give it a name.

You should now be able to build and run your application using LiquidFun. All
generated Java files reside in `Liquidfun/Box2D/swig/gen`. You can refer to
these files to confirm class, method, and other names.

### Programmer's Guide

The LiquidFun SWIG interface files pulls in a subset of the LiquidFun API,
extends it, and presents it in a way consistent with typical Java style.

#### Function and Variable Renames

*   All b2 class prefixes have been removed.
*   All function names have been converted to mixedCase.
*   All m_ member variable prefixes have been removed.
*   All enum variables have been converted to uppercase with underscores.

#### Memory Management

JVM (Java Virtual Machine) uses garbage collection to clean up memory, which
is very different from the user-managed memory model in C++. The following
points are critical in order to facilitate efficient JNI memory management.

##### Use the delete() method in generated Java classes

The user must use the SWIG-generated delete() method to clean up all LiquidFun
objects exposed through [SWIG](http://www.swig.org). This is because SWIG-generated (Java) proxy
classes--not the JVM--are allocating C++ memory through every new object.

For example:

&nbsp;&nbsp;&nbsp;`BodyDef* bodyDef = new BodyDef();`
    ...
&nbsp;&nbsp;&nbsp;``bodyDef.delete();`

For a member variable that is a native object:

&nbsp;&nbsp;&nbsp;`private World mWorld = new World(0, 0);`
    ...
&nbsp;&nbsp;&nbsp;`@Override`
&nbsp;&nbsp;&nbsp;`protected void finalize() {`
    &nbsp;&nbsp;&nbsp;`mWorld.delete();`
&nbsp;&nbsp;&nbsp;`}`

##### Use primitive types whenever possible

Because the user has to be conscientious about cleaning up any new objects,
directly exposing to Java a C++ function signature like

&nbsp;&nbsp;&nbsp;`void SetPosition(const b2Vec2& pos);`

will create this method:

&nbsp;&nbsp;&nbsp;`void setPosition(Vec2 pos);`

This method requires that the user creates Vec2 objects. Since Vec2 objects are
native objects, you must clean them up using a delete() method, which can be
quite unwieldy when you are initializing large amounts of data.

Instead, use SWIG to extend the interface. Do this by adding a new function to
the corresponding SWIG interface file:

&nbsp;&nbsp;&nbsp;`%extend b2ParticleDef {`
    &nbsp;&nbsp;&nbsp;`void setPosition(float32 x, float32 y) {`
        &nbsp;&nbsp;&nbsp;`$self->position.Set(x, y);`
    &nbsp;&nbsp;&nbsp;`}`
&nbsp;&nbsp;&nbsp;`};`

This file generates the following Java code:

    void setPosition(float x, float y);

which allows the user to call the function with primitive types (in this case,
floats) directly.

  [SWIG]: http://http://www.swig.org

