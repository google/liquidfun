cmake_minimum_required(VERSION 2.8)

project(Box2D)

if(UNIX)
	set(BOX2D_INSTALL_BY_DEFAULT ON)
else(UNIX)
	set(BOX2D_INSTALL_BY_DEFAULT OFF)
endif(UNIX)

# LiquidFun build configuration.
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

# Enable / disable debug code depending upon the build configuration.
set(DEBUG_FLAGS "-DDEBUG=1")
set(RELEASE_FLAGS "-DDEBUG=0")
file(WRITE "flags.cmake" "")
foreach(FLAGS_TYPE C CXX)
  # Enable debug code when CMAKE_BUILD_TYPE=Debug.
  set(FLAGS_VARIABLE CMAKE_${FLAGS_TYPE}_FLAGS_DEBUG)
  file(APPEND "flags.cmake"
       "set(${FLAGS_VARIABLE} \"\${${FLAGS_VARIABLE}} ${DEBUG_FLAGS}\")\n")
  # Disable debug code when CMAKE_BUILD_TYPE is a release config.
  foreach(BUILD_TYPE_SUFFIX RELEASE RELWITHDEBINFO MINSIZEREL)
    set(FLAGS_VARIABLE CMAKE_${FLAGS_TYPE}_FLAGS_${BUILD_TYPE_SUFFIX})
    file(APPEND "flags.cmake"
         "set(${FLAGS_VARIABLE} \"\${${FLAGS_VARIABLE}} ${RELEASE_FLAGS}\")\n")
  endforeach(BUILD_TYPE_SUFFIX)
  # If no build configuration is specified, default to release flags to align
  # with cmake's default build configuration for makefiles.
  if(NOT CMAKE_BUILD_TYPE AND UNIX AND NOT APPLE)
    set(FLAGS_VARIABLE CMAKE_${FLAGS_TYPE}_FLAGS)
    file(APPEND "flags.cmake"
         "set(${FLAGS_VARIABLE} \"\${${FLAGS_VARIABLE}} ${RELEASE_FLAGS}\")\n")
  endif(NOT CMAKE_BUILD_TYPE AND UNIX AND NOT APPLE)
endforeach(FLAGS_TYPE)
include("flags.cmake")

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
