# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#

# Gtest builds 2 libraries: libgtest and libgtest_main. libgtest
# contains most of the code (assertions...) and libgtest_main just
# provide a common main to run the test (ie if you link against
# libgtest_main you won't/should not provide a main() entry point.
#
# We build these 2 libraries for the target device and for the host if
# it is running linux. The linux build and tests are run under
# valgrind by 'runtest'.
#
# Includes:
# * For a host build we need to specify bionic/libstdc++/include
#   otherwise gcc will pick the system's STL. For targets build this
#   is automatically done by the toolchain but we add it for the
#   simulator builds..
# * libjingle's include directives start at the 'talk' level which is
#   2 directories up. We use $(LOCAL_PATH) to get an absolute include
#   path just in case talk has been symlinked.

LOCAL_PATH := $(call my-dir)

libgtest_includes:= \
    bionic/libstdc++/include \
    external/astl/include \
    $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/../include

ifeq ($(HOST_OS),linux)

#######################################################################
# gtest lib host

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := gtest-all.cc

LOCAL_C_INCLUDES := $(libgtest_includes)

LOCAL_CFLAGS += -O0

LOCAL_MODULE := libgtest
LOCAL_MODULE_TAGS := eng

include $(BUILD_HOST_STATIC_LIBRARY)

#######################################################################
# gtest_main lib host

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := gtest_main.cc

LOCAL_C_INCLUDES := $(libgtest_includes)

LOCAL_CFLAGS += -O0

LOCAL_STATIC_LIBRARIES := libgtest

LOCAL_MODULE := libgtest_main
LOCAL_MODULE_TAGS := eng

include $(BUILD_HOST_STATIC_LIBRARY)

endif # HOST_OS == linux

#######################################################################
# gtest lib target

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := gtest-all.cc

LOCAL_C_INCLUDES := $(libgtest_includes)

LOCAL_MODULE := libgtest
LOCAL_MODULE_TAGS := eng

include $(BUILD_STATIC_LIBRARY)

#######################################################################
# gtest_main lib target

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := gtest_main.cc

LOCAL_C_INCLUDES := $(libgtest_includes)

LOCAL_STATIC_LIBRARIES := libgtest

LOCAL_MODULE := libgtest_main
LOCAL_MODULE_TAGS := eng

include $(BUILD_STATIC_LIBRARY)
