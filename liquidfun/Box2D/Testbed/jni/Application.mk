APP_PLATFORM := android-10
APP_STL:=stlport_static
NDK_MODULE_PATH+=\
$(abspath $(NDK_PROJECT_PATH)/../../):$(abspath $(NDK_PROJECT_PATH)/../../../)
APP_MODULES:=Testbed
