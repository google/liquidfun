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

# This is the directory into which the executables are built.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

include_directories(${GUNIT_INCDIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
link_directories(${GTEST_LIBDIR} ${CMAKE_CURRENT_SOURCE_DIR}/../Box2D)
add_definitions(-DGUNIT_NO_GOOGLE3)

# These are support sources that are used for unit tests.
set(SUPPORT_SRCS BodyTracker.cpp)

# PUT ADDITIONAL UNIT TEST BINARIES BELOW!
# The commands should be of the form:
#
# add_executable(<binary-name> <cpp-files> SUPPORT_SRCS)
# target_link_libraries(<binary-name> gtest Box2D)
#
# Where:
#    <binary-name> is the name of the binary you wish to create
#    <cpp-files> is the list of cpp files in support of that binary.

# HelloWorldTests
add_executable(HelloWorldTests HelloWorldTests.cpp ${SUPPORT_SRCS})
target_link_libraries(HelloWorldTests gtest Box2D pthread)

# ConfinementTests
add_executable(ConfinementTests ConfinementTests.cpp ${SUPPORT_SRCS})
target_link_libraries(ConfinementTests gtest Box2D pthread)

# ConservationTests
add_executable(ConservationTests ConservationTests.cpp ${SUPPORT_SRCS})
target_link_libraries(ConservationTests gtest Box2D pthread)

# FunctionTests
add_executable(FunctionTests FunctionTests.cpp ${SUPPORT_SRCS})
target_link_libraries(FunctionTests gtest Box2D pthread)

# CallbackTests
add_executable(CallbackTests CallbackTests.cpp ${SUPPORT_SRCS})
target_link_libraries(CallbackTests gtest Box2D pthread)
