cmake_minimum_required(VERSION 2.6)
project(test-shapes-gles1)

IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
  IF(!ANDROID)
    # not setting -ansi as EGL/KHR headers doesn't support it
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
  ENDIF()
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# FreeGLUT
include(FindPkgConfig)
pkg_check_modules(freeglut REQUIRED freeglut-gles1>=3.0.0)
if(freeglut_FOUND)
  include_directories(${freeglut_STATIC_INCLUDE_DIRS})
  link_directories(${freeglut_STATIC_LIBRARY_DIRS})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${freeglut_STATIC_CFLAGS_OTHER}")
  add_definitions(${freeglut_STATIC_CFLAGS_OTHER})
endif()

if(ANDROID)
  add_library(test-shapes-gles1 SHARED test-shapes-gles1.c)
  add_custom_target(apk ALL
    DEPENDS test-shapes-gles1
    COMMAND ant clean || true
    COMMAND rm -rf libs/ src/ res/ bin/ gen/
    COMMAND mkdir -p libs/armeabi/ src/ res/values/
    COMMAND cp -a ${PROJECT_SOURCE_DIR}/AndroidManifest.xml ${PROJECT_BINARY_DIR}
    COMMAND cp -a $<TARGET_FILE:test-shapes-gles1> libs/armeabi/
    COMMAND echo '<?xml version="1.0" encoding="utf-8"?><resources><string name="app_name">FG_GLES1 test</string></resources>'
            > res/values/strings.xml
    COMMAND android update project --name cmake-apk --path . --target "android-10"
    COMMAND ant debug
    COMMAND ant installd
    COMMAND adb shell am start -a android.intenon.MAIN -n freeglut.test.gles1/android.app.NativeActivity
    )
  # Note: at least one resource and an empty src/ dir is necessary for ant...
else()
  add_executable(test-shapes-gles1 test-shapes-gles1.c)
endif()
target_link_libraries(test-shapes-gles1 ${freeglut_STATIC_LIBRARIES})
