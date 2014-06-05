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

source_directories:=\
	Collision \
	Collision/Shapes \
	Common \
	Dynamics \
	Dynamics/Contacts \
	Dynamics/Joints \
	Particle \
	Rope

include $(LOCAL_PATH)/b2_android_common.mk

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
  LOCAL_MODULE:=libliquidfun$(1)
  LOCAL_MODULE_TAGS:=optional
  LOCAL_COPY_HEADERS_TO:=liquidfun$(1))
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
        $(foreach extension,$(b2_extensions),\
          $(wildcard $(LOCAL_PATH)/Box2D/$(source_dir)/*.$(extension)))))
  LOCAL_COPY_HEADERS:=\
    Box2D/Box2D.h \
    $(subst $(LOCAL_PATH)/,,\
      $(foreach source_dir,$(source_directories),\
        $(wildcard $(LOCAL_PATH)/Box2D/$(source_dir)/*.h)))
  LOCAL_CFLAGS:=$(if $(APP_DEBUG),-DDEBUG=1,-DDEBUG=0) $(b2_cflags)
  LOCAL_EXPORT_C_INCLUDES:=$(LOCAL_PATH)
  LOCAL_ARM_MODE:=arm
  $$(call add-stlport-includes))
endef

# --- libliquidfun ---
# Build shared library.
include $(CLEAR_VARS)
$(call box2d-build,)
include $(BUILD_SHARED_LIBRARY)

# --- libliquidfun_static ---
# Build static library.
include $(CLEAR_VARS)
$(call box2d-build,_static)
include $(BUILD_STATIC_LIBRARY)
