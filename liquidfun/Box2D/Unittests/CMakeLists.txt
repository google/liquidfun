cmake_minimum_required(VERSION 2.8)

# Set some variables describing file locations.
#
# NOTE: This configuration assumes that gtest was built installed in the "inst"
# directory. Please see http://blah for instructions.
#
# Also keep in mind that this will need to be cleaned up were we ever to make
# this code public domain, we will wish to remove this relative path and
# replace it with something less google-specific.
set(GUNIT_INCDIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../googletest/include)
set(GTEST_LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../googletest)

# Include helper functions and macros used by Google Test.
include(${GTEST_LIBDIR}/cmake/internal_utils.cmake)
config_compiler_and_linker()
string(REPLACE "-W4" "-W3" cxx_default "${cxx_default}")
string(REPLACE "-Wshadow" "" cxx_default "${cxx_default}")
string(REPLACE "-Wextra" "" cxx_default "${cxx_default}")

# This is the directory into which the executables are built.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${GUNIT_INCDIR} ${CMAKE_CURRENT_SOURCE_DIR}
                                    ${CMAKE_CURRENT_SOURCE_DIR}/..)
link_directories(${GTEST_LIBDIR} ${CMAKE_CURRENT_SOURCE_DIR}/../Box2D)

# These are support sources that are used for unit tests.
set(SUPPORT_SRCS BodyTracker.cpp)

# Common libraries for tests.
if(NOT MSVC)
  find_package(Threads)
endif()
set(COMMON_LIBS "gtest;Box2D;${CMAKE_THREAD_LIBS_INIT}")

# PUT ADDITIONAL UNIT TEST BINARIES BELOW!
# The commands should be of the form:
#
# test_executable(<test-name>)
#
# Where <test-name> is the name of the output test executable and the basename
# of the source file for the test.  For example, test_executable(CallbackTests)
# generates an executable called CallbackTests which is the result of compiling
# CallbackTests.cpp along with any files specified by the SOURCE_SRCS variable.

function(test_executable name)
  cxx_executable_with_flags(${name}Tests "${cxx_default}" "${COMMON_LIBS}"
                            ${name}/${name}Tests.cpp ${SUPPORT_SRCS})
endfunction()

test_executable(Callback)
test_executable(Confinement)
test_executable(Conservation)
test_executable(Function)
test_executable(HelloWorld)
