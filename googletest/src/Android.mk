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

ifeq ($(HOST_OS),linux)

LOCAL_PATH := $(call my-dir)

#######################################################################
# gtest lib host

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cc
# TODO: may need to drag these in a shared variable when we start to
# support target builds.
LOCAL_SRC_FILES := \
    gtest.cc \
    gtest-death-test.cc \
    gtest-filepath.cc \
    src/gtest-internal-inl.h \
    gtest-port.cc \
    gtest-test-part.cc \
    gtest-typed-test.cc


LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/../include


LOCAL_CFLAGS += -O0

LOCAL_MODULE := libgtest
LOCAL_MODULE_TAGS := tests

include $(BUILD_HOST_STATIC_LIBRARY)

#######################################################################
# gtest_main lib host

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cc
# TODO: may need to drag these in a shared variable when we start to
# support target builds.
LOCAL_SRC_FILES := \
    gtest_main.cc

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/../include

LOCAL_CFLAGS += -O0

LOCAL_STATIC_LIBRARIES := libgtest

LOCAL_MODULE := libgtest_main
LOCAL_MODULE_TAGS := eng

include $(BUILD_HOST_STATIC_LIBRARY)

endif # HOST_OS == linux
