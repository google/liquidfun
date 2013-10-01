LOCAL_PATH:=$(call my-dir)

# Whether to build using box2d's cmake project or use Android's build system
# directly.
BOX2D_BUILD_USING_CMAKE?=0

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
  LOCAL_MODULE:=libBox2D$(1)
  LOCAL_MODULE_TAGS:=optional
  LOCAL_COPY_HEADERS_TO:=Box2D$(1))
endef

# === Build using the standard Android build process. ===
ifeq ($(BOX2D_BUILD_USING_CMAKE),0)

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
    $(call execute-local, "find Box2D/Box2D -name '*.cpp'")
  LOCAL_COPY_HEADERS:=\
    $(call execute-local, "find Box2D/Box2D -name '*.h'")
  LOCAL_C_INCLUDES:=$(LOCAL_PATH)/Box2D
  $$(call add-stlport-includes))
endef

# --- libBox2D ---
# Build shared library.
include $(CLEAR_VARS)
$(call box2d-build,)
include $(BUILD_SHARED_LIBRARY)

# --- libBox2D_static ---
# Build static library.
include $(CLEAR_VARS)
$(call box2d-build,_static)
include $(BUILD_STATIC_LIBRARY)

else  # BOX2D_BUILD_USING_CMAKE

# Configure local variables to build box2d using cmake, adding $(1) to the end
# of the build target's name and $(2) to cmake's options.
define box2d-build
$(eval \
  $$(call box2d-module,$(1))
  # Configure box2d.
  LOCAL_CMAKE_OPTIONS:=\
    -DBOX2D_INSTALL=ON \
    -DBOX2D_INSTALL_DOC=OFF \
    -DBOX2D_BUILD_EXAMPLES=OFF \
    -DBOX2D_BUILD_UNITTESTS=OFF \
    -DLIB_INSTALL_DIR="." \
    $(2)
  # Path of the source project relative to LOCAL_PATH.
  LOCAL_CMAKE_PROJECT_PATH:=Box2D
  # Path containing headers to be redistributed with the library.
  LOCAL_CMAKE_HEADER_INSTALL_PATH:=include
  $$(call add-stlport-includes))
endef

# --- libBox2D ---
include $(CLEAR_VARS)
$(call box2d-build,,-DBOX2D_BUILD_STATIC=OFF -DBOX2D_BUILD_SHARED=ON)
LOCAL_CMAKE_INCLUDE_RULES:=$(BUILD_SHARED_LIBRARY)
include $(LOCAL_PATH)/cmake.mk

# --- libBox2D_static ---
include $(CLEAR_VARS)
$(call box2d-build,_static,-DBOX2D_BUILD_STATIC=ON -DBOX2D_BUILD_SHARED=OFF)
LOCAL_CMAKE_MODULE_OVERRIDE:=libBox2D
LOCAL_CMAKE_INCLUDE_RULES:=$(BUILD_STATIC_LIBRARY)
include $(LOCAL_PATH)/cmake.mk

endif  # BOX2D_BUILD_USING_CMAKE
