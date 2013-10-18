cmake_minimum_required(VERSION 2.6)

project(Box2D)

if(UNIX)
	set(BOX2D_INSTALL_BY_DEFAULT ON)
else(UNIX)
	set(BOX2D_INSTALL_BY_DEFAULT OFF)
endif(UNIX)

# Splash2d build configuration.
# Force gtest to compile against the shared CRT.
if(MSVC)
  set(gtest_force_shared_crt ON CACHE INTERNAL "")
endif()
# Disable build of freeglut demos.
set(FREEGLUT_BUILD_DEMOS OFF CACHE INTERNAL "")
# Disable install of Box2D by default on all platforms.
set(BOX2D_INSTALL_BY_DEFAULT OFF)

option(BOX2D_INSTALL "Install Box2D libs, includes, and CMake scripts" ${BOX2D_INSTALL_BY_DEFAULT})
option(BOX2D_INSTALL_DOC "Install Box2D documentation" OFF)
option(BOX2D_BUILD_SHARED "Build Box2D shared libraries" OFF)
option(BOX2D_BUILD_STATIC "Build Box2D static libraries" ON)
option(BOX2D_BUILD_EXAMPLES "Build Box2D examples" ON)
option(BOX2D_BUILD_UNITTESTS "Build Box2D Unit Tests" ON)

set(BOX2D_VERSION 2.3.0)

# The Box2D library.
add_subdirectory(Box2D)

if(BOX2D_BUILD_EXAMPLES)
  # HelloWorld console example.
  add_subdirectory(HelloWorld)

  # Testbed and dependencies.
  find_package(OpenGL REQUIRED)
  add_subdirectory(../../freeglut freeglut)
  add_subdirectory(glui)
  add_subdirectory(Testbed)
endif(BOX2D_BUILD_EXAMPLES)

if(BOX2D_BUILD_UNITTESTS)
  add_subdirectory(../../googletest googletest)
  add_subdirectory(Unittests)
endif(BOX2D_BUILD_UNITTESTS)

if(BOX2D_INSTALL_DOC)
  install(DIRECTORY Documentation DESTINATION share/doc/Box2D PATTERN ".svn" EXCLUDE)
endif(BOX2D_INSTALL_DOC)
