# Copyright 2013 Google. All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

# --- CallbackTests ---
include $(CLEAR_VARS)
namespace:=$(if $(NDK_PROJECT_PATH),,_splash2d)
LOCAL_MODULE:=$(LOCAL_TEST_NAME)${namespace}
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:=\
	$(LOCAL_PATH)/$(LOCAL_TEST_NAME).cpp \
	$(LOCAL_PATH)/../BodyTracker.cpp
LOCAL_C_INCLUDES:=\
	$(LOCAL_PATH)/ \
	$(LOCAL_PATH)/..
LOCAL_LDLIBS:=-llog -landroid
LOCAL_STATIC_LIBRARIES:=\
	android_native_app_glue libgtest libsplash2d_static \
	libandroidutil_static
# Override the default log tag in the AndroidUtil library.
ANDROIDUTIL_ADDITIONAL_CFLAGS:=-DANDROID_LOG_PRINT_TAG="$(LOCAL_MODULE)"
# Redirect gtest to AndroidUtil's buffered print functions.
GTEST_ADDITIONAL_CFLAGS:=\
    -include $(LOCAL_PATH)/../../AndroidUtil/AndroidLogPrint.h \
	-DGTEST_ANDROID_LOG_PRINT=AndroidLogPrint \
	-DGTEST_ANDROID_LOG_VPRINT=AndroidLogVPrint \
    -DANDROID_LOG_OVERRIDE_PRINTF=0
include $(BUILD_SHARED_LIBRARY)
# Import Box2D, googletest and native_app_glue libraries.
$(call import-module,Box2D/jni)
$(call import-module,Box2D/AndroidUtil/jni)
$(call import-module,googletest)
$(call import-module,android/native_app_glue)

LOCAL_TEST_NAME:=
