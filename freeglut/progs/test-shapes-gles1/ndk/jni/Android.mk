LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := native-activity
LOCAL_SRC_FILES := 
LOCAL_CPPFLAGS  := -I/usr/src/glm
LOCAL_CXXFLAGS  := -gstabs+
LOCAL_LDLIBS    := -llog -landroid -lGLESv2 -lEGL
LOCAL_STATIC_LIBRARIES := freeglut

include $(BUILD_SHARED_LIBRARY)

$(call import-module,freeglut)
