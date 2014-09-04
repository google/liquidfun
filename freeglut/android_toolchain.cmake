# CMake toolchain file, cf. README.android
SET(CMAKE_SYSTEM_NAME Linux)  # Tell CMake we're cross-compiling
include(CMakeForceCompiler)
# Prefix detection only works with compiler id "GNU"
CMAKE_FORCE_C_COMPILER(arm-linux-androideabi-gcc GNU)
SET(ANDROID TRUE)
