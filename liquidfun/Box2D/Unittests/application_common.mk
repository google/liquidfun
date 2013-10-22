APP_PLATFORM:=android-10
APP_ABI:=armeabi-v7a
APP_STL:=stlport_static
SPLASH2D_PATH:=$(NDK_PROJECT_PATH)/../../../
NDK_MODULE_PATH+=$(abspath $(SPLASH2D_PATH)):$(abspath $(SPLASH2D_PATH)/../)
