# Copyright 2013 Google. All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd
LOCAL_PATH:=$(call my-dir)/..

# --- Testbed ---
include $(CLEAR_VARS)
namespace:=$(if $(NDK_PROJECT_PATH),,_splash2d)
LOCAL_MODULE:=Testbed
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:=\
	$(subst $(LOCAL_PATH)/,,\
	  $(wildcard $(LOCAL_PATH)/Framework/*.cpp) \
	  $(wildcard $(LOCAL_PATH)/Tests/*.cpp) \
	  $(wildcard $(LOCAL_PATH)/../glui/*.cpp) \
	  $(wildcard $(LOCAL_PATH)/../GLEmu/*cpp))
LOCAL_CFLAGS:=-DFREEGLUT_GLES1
LOCAL_STATIC_LIBRARIES:=splash2d_static freeglut-gles1_static
LOCAL_LDLIBS:=-llog -landroid -lEGL -lGLESv1_CM
LOCAL_ARM_MODE:=arm
include $(BUILD_SHARED_LIBRARY)
# Import splash2d libraries and freeglut.
$(call import-module,Box2D/jni)
$(call import-module,freeglut/jni)
