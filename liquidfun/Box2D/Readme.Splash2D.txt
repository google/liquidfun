Splash2D is an extension of Box2D, and will build on any system that can build Box2D (Windows, OS X, Linux), see Building.txt

Splash2D specific instructions:

All:
- install cmake

Windows:
- run Build\generate_vs2010_project_with_cmake.bat
- open Box2D.sln
- set Testbed as startup project
- compile & run

OS X:
- install and run http://xquartz.macosforge.org/
- run Build/generate_xcode_project_with_cmake.sh
- open Build/Box2D.xcodeproj
- change scheme to Testbed
- in scheme, set environment variable DISPLAY to :0.0
- compile & run

Linux:
- cmake -G "Unix Makefiles"
- make

Android:
- run Build/
