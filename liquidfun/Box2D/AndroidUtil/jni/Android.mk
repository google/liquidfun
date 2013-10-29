LOCAL_PATH:=$(call my-dir)/..

# Additional CFLAGS can be added to the library by setting ANDROIDUTIL_CFLAGS
# before importing this package.
include $(CLEAR_VARS)
namespace:=$(if $(NDK_PROJECT_PATH),,_splash2d)
LOCAL_MODULE:=libandroidutil$(namespace)_static
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:=\
	AndroidLogPrint.c \
	AndroidMainWrapper.c
LOCAL_C_INCLUDES:=\
	$(LOCAL_PATH)/.. \
	$(NDK_ROOT)/sources/android/native_app_glue
LOCAL_CFLAGS:=\
	-DANDROID_MAIN_WRAPPER_DISABLE_ANDROID_MAIN \
	$(ANDROIDUTIL_ADDITIONAL_CFLAGS)
LOCAL_EXPORT_C_INCLUDES:=$(LOCAL_C_INCLUDES)
LOCAL_ARM_MODE:=arm
include $(BUILD_STATIC_LIBRARY)
ANDROIDUTIL_CFLAGS:=
