LOCAL_PATH:=$(call my-dir)/..

include $(CLEAR_VARS)
namespace:=$(if $(NDK_PROJECT_PATH),,_splash2d)
LOCAL_MODULE:=HelloWorld${namespace}
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:=$(LOCAL_PATH)/HelloWorld.cpp
LOCAL_LDLIBS:=-llog -landroid
LOCAL_STATIC_LIBRARIES:=android_native_app_glue libsplash2d_static \
                        libandroidutil_static
ANDROIDUTIL_ADDITIONAL_CFLAGS:=-DANDROID_LOG_PRINT_TAG="$(LOCAL_MODULE)"
LOCAL_ARM_MODE:=arm
include $(BUILD_SHARED_LIBRARY)
# Import Box2D libraries and native_app_glue.
$(call import-module,Box2D/jni)
$(call import-module,Box2D/AndroidUtil/jni)
$(call import-module,android/native_app_glue)
