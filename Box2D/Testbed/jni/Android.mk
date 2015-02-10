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

# --- Testbed ---
include $(CLEAR_VARS)
namespace:=$(if $(NDK_PROJECT_PATH),,_liquidfun)
LOCAL_MODULE:=Testbed
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:=\
	$(subst $(LOCAL_PATH)/,,\
	  $(wildcard $(LOCAL_PATH)/Framework/*.cpp) \
	  $(wildcard $(LOCAL_PATH)/Tests/*.cpp) \
	  $(wildcard $(LOCAL_PATH)/../GLEmu/*cpp))
LOCAL_CFLAGS:=-DFREEGLUT_GLES1
LOCAL_STATIC_LIBRARIES:=liquidfun_static libandroidutil_static \
                        freeglut-gles1_static
LOCAL_LDLIBS:=-llog -landroid -lEGL -lGLESv1_CM
LOCAL_ARM_MODE:=arm
include $(BUILD_SHARED_LIBRARY)
# Import liquidfun libraries and freeglut.
$(call import-module,Box2D/jni)
$(call import-module,Box2D/AndroidUtil/jni)
$(call import-module,freeglut/jni)
