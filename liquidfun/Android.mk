LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

# Module name and dependencies.
LOCAL_MODULE:=libBox2D
LOCAL_MODULE_TAGS:=optional
LOCAL_SHARED_LIBRARIES:=libstlport

# Whether to build using Box2D's cmake project or use Android's build system
# directly.
BOX2D_BUILD_USING_CMAKE?=0

# === Build using the standard Android build process. ===
ifeq ($(BOX2D_BUILD_USING_CMAKE),0)

# Execute a shell command relative to this module's directory.
define execute-local
$(patsubst ./%,%,$(shell cd $(LOCAL_PATH) ; eval "$(1)"))
endef

# Build list of source files and headers for distribution.
LOCAL_SRC_FILES:=\
	$(call execute-local, "find Box2D/Box2D -name '*.cpp'")
LOCAL_COPY_HEADERS:=\
	$(call execute-local, "find Box2D/Box2D -name '*.h'")
LOCAL_C_INCLUDES:=$(LOCAL_PATH)/Box2D

#include stlport headers
include external/stlport/libstlport.mk
# include rules to build this as a shared library.
include $(BUILD_SHARED_LIBRARY)

else  # BOX2D_BUILD_USING_CMAKE

# === Build using cmake. ===

# Configure box2d.
LOCAL_CMAKE_OPTIONS:=\
	-DBOX2D_INSTALL=ON \
    -DBOX2D_INSTALL_DOC=OFF \
    -DBOX2D_BUILD_EXAMPLES=OFF \
    -DBOX2D_BUILD_STATIC=OFF \
    -DBOX2D_BUILD_SHARED=ON \
    -DLIB_INSTALL_DIR="."
# Path of the source project relative to LOCAL_PATH.
LOCAL_CMAKE_PROJECT_PATH:=Box2D
# Path containing headers to be redistributed with the library.
LOCAL_CMAKE_HEADER_INSTALL_PATH:=include

# include stlport headers
include external/stlport/libstlport.mk
# Generate build rules for the module.
include external/box2d/cmake.mk

endif  # BOX2D_BUILD_USING_CMAKE
