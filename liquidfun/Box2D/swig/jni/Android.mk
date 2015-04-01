# Copyright (c) 2014 Google, Inc.
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

ifneq ($(NDK_PROJECT_PATH),)

LOCAL_PATH:=$(call my-dir)/..

include $(CLEAR_VARS)

SWIG_BIN ?= swig
swig_java_package:=com.google.fpl.liquidfun
swig_java_out_dir:=$(LOCAL_PATH)/gen/com/google/fpl/liquidfun
swig_cpp_out_dir:=gen/cpp/$(TARGET_ARCH_ABI)
swig_cpp_out_file:=liquidfun_wrap.cpp
swig_cpp_out_dir_full:=$(LOCAL_PATH)/$(swig_cpp_out_dir)
swig_cpp_out_buildsrc:=$(swig_cpp_out_dir)/$(swig_cpp_out_file)
swig_cpp_out:=$(LOCAL_PATH)/$(swig_cpp_out_buildsrc)
swig_in_file:=$(LOCAL_PATH)/java/liquidfun.swig

# Check swig version
swig_minversion_minmaj:=2.0
swig_minversion_patch:=11
swig_version:=$(word 3, $(shell $(SWIG_BIN) -version))
swig_version_space:=$(subst ., ,$(swig_version))
swig_version_minmaj:=\
$(word 1,$(swig_version_space)).$(word 2,$(swig_version_space))
swig_version_patch:=$(word 3,$(swig_version_space))

ifneq (,$(findstring cmd,$(SHELL)))
  swig_minmaj_too_old:=$(shell \
	if ${swig_version_minmaj} lss ${swig_minversion_minmaj} echo 1)
  swig_patch_too_old:=
  ifeq (${swig_version_minmaj},${swig_minversion_minmaj})
    swig_version_patch_too_old:=$(shell \
      if ${swig_version_patch} lss ${swig_minversion_patch} echo 1)
  endif
  swig_version_too_old:=$(swig_minmaj_too_old)$(swig_patch_too_old)
else
  swig_version_too_old:=$(shell \
    if [[ "${swig_version_minmaj}" < "${swig_minversion_minmaj}" ]]; then \
      echo 1; \
    elif [[ "${swig_version_minmaj}" == "${swig_minversion_minmaj}" && \
            "${swig_version_patch}" -lt "${swig_minversion_patch}" ]]; then \
      echo 1; \
	fi)
endif

ifeq ($(swig_version_too_old),1)
  $(error Found SWIG $(swig_version), requires\
$(swig_minversion_minmaj).$(swig_minversion_patch) or above)
endif

# All SWIG interface files
swig_src_files:=$(swig_in_file)
swig_src_dirs:=\
  Collision/Shapes \
  Common \
  Dynamics \
  Particle
swig_src_files+=\
  $(foreach dir,$(swig_src_dirs),\
    $(wildcard $(LOCAL_PATH)/java/$(dir)/*.swig))

namespace:=$(if $(NDK_PROJECT_PATH),,_liquidfun)
LOCAL_MODULE:=libliquidfun_jni${namespace}
LOCAL_C_INCLUDES:=$(LOCAL_PATH)/../
LOCAL_CFLAGS:=-frtti

LOCAL_SRC_FILES+=$(swig_cpp_out_buildsrc)
LOCAL_STATIC_LIBRARIES+=libliquidfun
ANDROIDUTIL_ADDITIONAL_CFLAGS:=-DANDROID_LOG_PRINT_TAG="$(LOCAL_MODULE)"
LOCAL_ARM_MODE:=arm

$(swig_cpp_out): PRIVATE_SWIG_JAVA_OUT_DIR:=$(swig_java_out_dir)
$(swig_cpp_out): PRIVATE_SWIG_CPP_OUT_DIR_FULL:=$(swig_cpp_out_dir_full)
$(swig_cpp_out): PRIVATE_SWIG_JAVA_PACKAGE:=$(swig_java_package)
$(swig_cpp_out): PRIVATE_SWIG_CPP_OUT:=$(swig_cpp_out)
$(swig_cpp_out): PRIVATE_SWIG_IN_FILE:=$(swig_in_file)

# Define SWIG target and call SWIG when input files change
$(swig_cpp_out): $(swig_src_files)
	$(call host-mkdir,$(PRIVATE_SWIG_JAVA_OUT_DIR))
	$(call host-mkdir,$(PRIVATE_SWIG_CPP_OUT_DIR_FULL))
	$(SWIG_BIN) -c++ -java -v -package $(PRIVATE_SWIG_JAVA_PACKAGE) \
			-outdir $(PRIVATE_SWIG_JAVA_OUT_DIR) \
			-o $(PRIVATE_SWIG_CPP_OUT) \
			$(PRIVATE_SWIG_IN_FILE)

clean_swig-$(TARGET_ARCH_ABI): PRIVATE_SWIG_JAVA_OUT_DIR:=$(swig_java_out_dir)
clean_swig-$(TARGET_ARCH_ABI): \
	PRIVATE_SWIG_CPP_OUT_DIR_FULL:=$(swig_cpp_out_dir_full)

# Remove all SWIG generated files during clean.
clean_swig-$(TARGET_ARCH_ABI):
	$(call host-rmdir,$(PRIVATE_SWIG_JAVA_OUT_DIR))
	$(call host-rmdir,$(PRIVATE_SWIG_CPP_OUT_DIR_FULL))

# Add SWIG clean rule to the list of clean rules.
clean: clean_swig-$(TARGET_ARCH_ABI)

include $(BUILD_SHARED_LIBRARY)

# Import Box2D libraries
$(call import-module,Box2D/jni)

endif #ifneq ($(NDK_PROJECT_PATH),)
