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

# TODO: Refactor these as 1st class build templates as suggested in
# review of the orginal import.

# $(1): source list
# $(2): "HOST_" or empty
# $(3): extra CFLAGS or empty
define _define-test
$(foreach file,$(1), \
  $(eval include $(CLEAR_VARS)) \
  $(eval LOCAL_CPP_EXTENSION := .cc) \
  $(eval LOCAL_SRC_FILES := $(file)) \
  $(eval LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include $(LOCAL_PATH)/..) \
  $(eval LOCAL_MODULE := $(notdir $(file:%.cc=%))) \
  $(eval LOCAL_CFLAGS += $(3)) \
  $(eval LOCAL_STATIC_LIBRARIES := libgtest_main libgtest) \
  $(eval LOCAL_MODULE_TAGS := tests) \
  $(eval include $(BUILD_$(2)EXECUTABLE)) \
)
endef

define host-test
$(call _define-test,$(1),HOST_,-O0)
endef

# TODO: Figure out the right CFLAGS combination needed for bionic/astl.
define target-test
$(call _define-test,$(1))
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

$(call host-test, $(sources))


# TODO: Target is not working yet.
# $(call target-test, $(sources))

endif # HOST_OS == linux
