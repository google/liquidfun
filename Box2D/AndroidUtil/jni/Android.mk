# Copyright (c) 2013 Google, Inc.
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
# 1. The origin of this software must not be misrepresented; you must not
# claim that you wrote the original software. If you use this software
# in a product, an acknowledgment in the product documentation would be
# appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
# misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
LOCAL_PATH:=$(call my-dir)/..

# Additional CFLAGS can be added to the library by setting ANDROIDUTIL_CFLAGS
# before importing this package.
include $(CLEAR_VARS)
namespace:=$(if $(NDK_PROJECT_PATH),,_liquidfun)
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
