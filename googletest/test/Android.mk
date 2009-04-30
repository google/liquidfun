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

LOCAL_PATH := $(call my-dir)

# used to define a simple test program and build it as a standalone
# device executable.
#
# you can use EXTRA_CFLAGS to indicate additional CFLAGS to use
# in the build. the variable will be cleaned on exit
#
define host-test
  $(foreach file,$(1), \
    $(eval include $(CLEAR_VARS)) \
    $(eval LOCAL_CPP_EXTENSION := .cc) \
    $(eval LOCAL_SRC_FILES := $(file)) \
    $(eval LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include $(LOCAL_PATH)/..) \
    $(eval LOCAL_MODULE := $(notdir $(file:%.cc=%))) \
    $(eval LOCAL_CFLAGS += $(EXTRA_CFLAGS)) \
    $(eval LOCAL_LDLIBS += $(EXTRA_LDLIBS)) \
    $(eval LOCAL_STATIC_LIBRARIES := libgtest_main libgtest) \
    $(eval LOCAL_MODULE_TAGS := eng tests) \
    $(eval include $(BUILD_HOST_EXECUTABLE)) \
  ) \
  $(eval EXTRA_CFLAGS :=) \
  $(eval EXTRA_LDLIBS :=)
endef

define target-test
  $(foreach file,$(1), \
    $(eval include $(CLEAR_VARS)) \
    $(eval LOCAL_CPP_EXTENSION := .cc) \
    $(eval LOCAL_SRC_FILES := $(file)) \
    $(eval LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include $(LOCAL_PATH)/..) \
    $(eval LOCAL_MODULE := $(notdir $(file:%.cc=%))) \
    $(eval LOCAL_CFLAGS += $(EXTRA_CFLAGS)) \
    $(eval LOCAL_LDLIBS += $(EXTRA_LDLIBS)) \
    $(eval LOCAL_STATIC_LIBRARIES := libgtest_main libgtest) \
    $(eval LOCAL_MODULE_TAGS := eng tests) \
    $(eval include $(BUILD_EXECUTABLE)) \
  ) \
  $(eval EXTRA_CFLAGS :=) \
  $(eval EXTRA_LDLIBS :=)
endef

# We use the single file option to build all the tests.
sources := \
  gtest-filepath_test.cc \
  gtest-linked_ptr_test.cc \
  gtest-message_test.cc \
  gtest-options_test.cc \
  gtest-port_test.cc \
  gtest_pred_impl_unittest.cc \
  gtest-test-part_test.cc \
  gtest-typed-test_test.cc \
  gtest-typed-test2_test.cc \
  gtest_unittest.cc \
  gtest_prod_test.cc

EXTRA_CFLAGS := -DGTEST_OS_LINUX -DGTEST_HAS_STD_STRING -O0 -g
EXTRA_LDLIBS :=
$(call host-test, $(sources))


# TODO: Target is not working yet.
# EXTRA_CFLAGS := -DGTEST_OS_LINUX -DGTEST_HAS_STD_STRING
# EXTRA_LDLIBS :=
# $(call target-test, $(sources))
