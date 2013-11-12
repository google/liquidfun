# Copyright 2013 Google. All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd
LOCAL_PATH:=$(call my-dir)/..

source_directories:=\
	Collision \
	Collision/Shapes \
	Common \
	Dynamics \
	Dynamics/Contacts \
	Dynamics/Joints \
	Particle \
	Rope

# Conditionally include libstlport (so include path is added to CFLAGS) if
# it's not being built using the NDK build process.
define add-stlport-includes
$(eval \
  ifeq ($(NDK_PROJECT_PATH),)
  include external/stlport/libstlport.mk
  endif)
endef

# Configure common local variables to build box2d adding $(1) to the end of the
# build target's name.
define box2d-module
$(eval \
  LOCAL_MODULE:=libsplash2d$(1)
  LOCAL_MODULE_TAGS:=optional
  LOCAL_COPY_HEADERS_TO:=splash2d$(1))
endef

# Execute a shell command relative to this module's directory.
define execute-local
$(patsubst ./%,%,$(shell cd $(LOCAL_PATH) ; eval "$(1)"))
endef

# Configure local variables to build box2d adding $(1) to the end of the
# build target's name.
define box2d-build
$(eval \
  $$(call box2d-module,$(1))
  LOCAL_SRC_FILES:=\
    $(subst $(LOCAL_PATH)/,,\
      $(foreach source_dir,$(source_directories),\
        $(wildcard $(LOCAL_PATH)/Box2D/$(source_dir)/*.cpp)))
  LOCAL_COPY_HEADERS:=\
    Box2D/Box2D.h \
    $(subst $(LOCAL_PATH)/,,\
      $(foreach source_dir,$(source_directories),\
        $(wildcard $(LOCAL_PATH)/Box2D/$(source_dir)/*.h)))
  LOCAL_EXPORT_C_INCLUDES:=$(LOCAL_PATH)
  LOCAL_ARM_MODE:=arm
  $$(call add-stlport-includes))
endef

# --- libsplash2d ---
# Build shared library.
include $(CLEAR_VARS)
$(call box2d-build,)
include $(BUILD_SHARED_LIBRARY)

# --- libsplash2d_static ---
# Build static library.
include $(CLEAR_VARS)
$(call box2d-build,_static)
include $(BUILD_STATIC_LIBRARY)
