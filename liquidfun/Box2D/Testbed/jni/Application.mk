APP_PLATFORM := android-10
APP_ABI:=armeabi-v7a
APP_STL:=stlport_static
NDK_MODULE_PATH+=\
$(abspath $(NDK_PROJECT_PATH)/../../):$(abspath $(NDK_PROJECT_PATH)/../../../)
APP_MODULES:=Testbed
