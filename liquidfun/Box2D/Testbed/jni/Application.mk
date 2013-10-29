APP_PLATFORM := android-10
APP_ABI:=armeabi-v7a
APP_STL:=stlport_static
NDK_MODULE_PATH+=$(subst $(space),,\
  $(abspath $(NDK_PROJECT_PATH)/../../)$(HOST_DIRSEP) \
  $(abspath $(NDK_PROJECT_PATH)/../../../))
APP_MODULES:=Testbed
