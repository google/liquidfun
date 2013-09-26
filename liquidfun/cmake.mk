###########################################################
## Rules to build a cmake project.
##
## The following variables configure the behavior of this
## script...
##
## LOCAL_CMAKE_OPTIONS:
##   Adds additional options to cmake's command line when
##   configuring the project's makefiles.
##
## LOCAL_CMAKE_PROJECT_PATH:
##   Path of the cmake project relative to the LOCAL_PATH
##   directory.
##
## LOCAL_CMAKE_HEADER_INSTALL_PATH:
##   Optional location of headers to redistribute with a
##   library relative to the project's install path.
##
## NOTE:
## * This currently only supports building shared libraries.
###########################################################

LOCAL_ADDITIONAL_DEPENDENCIES+=$(LOCAL_MODULE)-cmake

# include rules to build this as a shared library.
include $(BUILD_SHARED_LIBRARY)

# Select the cmake binary for the host OS.
CMAKE_BASE_DIR:=$(TOP)/prebuilts/cmake
CMAKE:=
ifeq ($(HOST_OS),darwin)
  ifneq (,$(findstring x86_64,$(UNAME)))
    CMAKE:=$(CMAKE_BASE_DIR)/$(HOST_OS)-x86_64/current/CMake\ *.app/\
Contents/bin/cmake
  else
    CMAKE:=$(CMAKE_BASE_DIR)/$(HOST_OS)-x86/current/CMake\ *.app\
Contents/bin/cmake
  endif
endif
ifeq ($(HOST_OS),linux)
  CMAKE:=$(CMAKE_BASE_DIR)/$(HOST_OS)-x86/current/bin/cmake
endif
ifeq ($(HOST_OS),windows)
  CMAKE:=$(CMAKE_BASE_DIR)/$(HOST_OS)/current/bin/cmake.exe
endif
ifeq (,$(CMAKE))
  $(error Platform $(UNAME) not supported.)
endif
CMAKE:=$(abspath $(CMAKE))

# Where source_file is relative to source_directory and
# target_directory is a directory to copy into copy
# source_directory/source_file to target_directory/source_file.
#
# $1: Source file
# $2: Directory source file is relative to.
# $3: Directory to link source file from.
define mirror-to-dir
$(3)/$(1): $(abspath $(2)/$(1))
	$(hide) mkdir -p $(3)/$(dir $(1))
	$(hide) $(ACP) -fp $$(<) $$(@)
endef

# Set CMAKE_$(1) to $(2) if it isn't set and add CMAKE_$(1) to the
# CMAKE_VARIABLES list.
define cmake_variable
$(eval \
  ifeq ($(CMAKE_$(1)),)
    CMAKE_$(1):=$(2)
  endif
  CMAKE_VARIABLES:=$(CMAKE_VARIABLES) CMAKE_$(1))
endef

# Set CMAKE_$(1) to TARGET_$(2) if it's set, otherwise set it to
# $(TARGET_TOOLS_PREFIX)$(3)$(HOST_EXECUTABLE_SUFFIX).
define cmake_tool
$(eval \
  ifeq ($(TARGET_$(2)),)
    CMAKE_TOOL_$(1):=$(TARGET_TOOLS_PREFIX)$(3)$(HOST_EXECUTABLE_SUFFIX)
  else
    CMAKE_TOOL_$(1):=$(TARGET_$(2))
  endif)
$(call cmake_variable,$(1),$$(abspath $$(CMAKE_TOOL_$(1))))
endef

# Replace each string that reference a filename in $(1) with the absolute
# path of the file.
define abspath-filenames
$(foreach variable,$(1), \
  $(if $(variable:-%=), \
    $(if $(wildcard $(variable)),$(abspath $(variable)),$(variable)), \
    $(variable)))
endef

# Extract CFLAGS from transform-c-or-s-to-o-no-deps.
#
# TODO(smiles): Since this is super brittle so the flags should probably be
# factored out of transform-c-or-s-to-o-no-deps in build/core/definitions.mk.
define transform-c-or-s-to-o-no-deps-cflags
$(call abspath-filenames, \
  $(shell echo $(call transform-c-or-s-to-o-no-deps) | \
               sed -r 's@.*$(PRIVATE_CC)[ \t]*@@; \
                       s@[ \t]*-c[ \t]+@ @g; \
                       s@[ \t]*-MD[ \t]+.*@@g;'))
endef

# Extract LDFLAGS from transform-o-to-shared-lib.
#
# TODO(smiles): Since this is super brittle so the flags should probably be
# factored out of transform-o-to-shared-lib in build/core/definitions.mk.
define transform-o-to-shared-lib-ldflags
$(call abspath-filenames, \
  $(shell echo '$(call transform-o-to-shared-lib)' | \
                  sed -r 's@.*$(subst +,\+,$(PRIVATE_CXX))[ \t]*@@; \
                          s@([ \t]*-L)([^ \t])@\1 \2@g; \
                          s@[ \t]*-o[ \t]*[^-][^ \t]+@ @g;'))
endef

# Extract LDFLAGS from transform-o-to-executable
#
# TODO(smiles): Since this is super brittle so the flags should probably be
# factored out of transform-o-to-executable in build/core/definitions.mk.
define transform-o-to-executable-ldflags
$(call abspath-filenames, \
  $(shell echo '$(call transform-o-to-executable)' | \
                  sed -r 's@.*$(subst +,\+,$(PRIVATE_CXX))[ \t]*@@; \
                          s@([ \t]*-L)([^ \t])@\1 \2@g; \
                          s@[ \t]*-o[ \t]*[^-][^ \t]+@ @g;'))
endef

# Set variables for cmake within the context of a rule.
define set-cmake-variables
$(eval \
  # Configure cmake's environment variables.)
$(call cmake_tool,C_COMPILER,CC,gcc)
$(call cmake_tool,CXX_COMPILER,CC,g++)
$(eval \
  # Don't check the compiler as cmake doesn't correctly add
  # CMAKE_*_LINKER_FLAGS to the command line during the link step causing
  # the check to fail.)
$(call cmake_variable,C_COMPILER_FORCED,TRUE)
$(call cmake_variable,CXX_COMPILER_FORCED,TRUE)
$(eval \
  # CMAKE_ASM_COMPILER defaults to gcc rather than as so this works with
  # command line flags derived from transform-c-or-s-to-o-no-deps.)
$(call cmake_tool,ASM_COMPILER,AS,gcc)
$(call cmake_tool,STRIP,STRIP,strip)

$(call cmake_tool,AR,AR,ar)
$(call cmake_tool,LINKER,LD,ld)
$(call cmake_tool,NM,NM,nm)
$(call cmake_tool,OBJCOPY,OBJCOPY,objcopy)
$(call cmake_tool,OBJDUMP,OBJDUMP,objdump)
$(call cmake_tool,RANLIB,RANLIB,ranlib)
$(call cmake_variable,SYSTEM_NAME,Linux)
$(call cmake_variable,SYSTEM_VERSION,1)
$(call cmake_variable,SYSTEM_PROCESSOR,$(TARGET_ARCH_VARIANT))
$(call cmake_variable,FIND_ROOT_PATH, \
  $(abspath $(TARGET_TOOLCHAIN_ROOT)/bin $(TARGET_TOOLCHAIN_ROOT)))
$(call cmake_variable,SHARED_LINKER_FLAGS, \
  $(call transform-o-to-shared-lib-ldflags))
$(call cmake_variable,MODULE_LINKER_FLAGS,)
$(call cmake_variable,EXE_LINKER_FLAGS, \
  $(call transform-o-to-executable-ldflags))
$(call cmake_variable,C_FLAGS, \
  $(call transform-c-or-s-to-o-no-deps-cflags))
$(call cmake_variable,CXX_FLAGS,$(CMAKE_C_FLAGS))
$(eval \
    # Finally, build the list of arguments for cmake.
	CMAKE_ARGUMENTS:= \
		$(foreach variable,$(CMAKE_VARIABLES),\
		    -D$(variable)="$($(variable))") \
		-DTARGET_SUPPORTS_SHARED_LIBS=TRUE \
		-DCMAKE_INSTALL_PREFIX:PATH=$(install_path) \
		-G 'Unix Makefiles' \
	    --no-warn-unused-cli )
endef

# Generate rules to mirror source_project_files to intermediates_directory.
source_project_files:=$(patsubst ./%,%,\
  $(shell cd $(LOCAL_PATH); \
    find -L $(LOCAL_CMAKE_PROJECT_PATH) -type f))
intermediates_directory:=$(call local-intermediates-dir)
$(foreach file,$(source_project_files),$(eval \
  $(call mirror-to-dir,$(file),$(LOCAL_PATH),$(intermediates_directory))))
# List of mirrored project files in the intermediates_directory.
intermediate_project_files:=$(addprefix $(intermediates_directory)/, \
                                        $(source_project_files))

# install path for cmake's build artifacts.
install_path:=$(abspath $(intermediates_directory)/$(LOCAL_MODULE))

# Make LOCAL_* variables at this point in the script visible to the commands
# in the $(LOCAL_MODULE)-cmake build rule.
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_MODULE:=$(LOCAL_MODULE)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_MODULE_SUFFIX:=$(LOCAL_MODULE_SUFFIX)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_CMAKE_OPTIONS:=$(LOCAL_CMAKE_OPTIONS)
$(LOCAL_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_PROJECT_PATH:=$(LOCAL_CMAKE_PROJECT_PATH)
$(LOCAL_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_HEADER_INSTALL_PATH:=$(LOCAL_CMAKE_HEADER_INSTALL_PATH)
$(LOCAL_INTERMEDIATE_TARGETS): \
	PRIVATE_INTERMEDIATES_DIR:=$(intermediates_directory)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_INSTALL_PATH:=$(install_path)
$(LOCAL_INTERMEDIATE_TARGETS): PRIVATE_SYMBOLIC_INPUT:=$(symbolic_input)

# Create the cmake makefiles in the intermediates directory.
#
# TODO(smiles): The Cmake command line is really big.  This needs to be moved
# into an intermediate script.
$(LOCAL_MODULE)-cmake: $(intermediate_project_files)
	$(call set-cmake-variables)
	$(hide) ( \
		cd $(PRIVATE_INTERMEDIATES_DIR)/$(PRIVATE_CMAKE_PROJECT_PATH) && \
		$(CMAKE) $(PRIVATE_CMAKE_OPTIONS) $(CMAKE_ARGUMENTS) && \
		$(MAKE) $(if $(hide),,VERBOSE=1) all install )
	$(hide) mkdir -p $(dir $(PRIVATE_SYMBOLIC_INPUT))
	$(hide) $(ACP) -fp \
	    $(PRIVATE_INSTALL_PATH)/$(PRIVATE_MODULE)$(PRIVATE_MODULE_SUFFIX) \
	    $(PRIVATE_SYMBOLIC_INPUT)
	$(hide) ( \
	    cd $(PRIVATE_INSTALL_PATH)/$(PRIVATE_CMAKE_HEADER_INSTALL_PATH) && \
	    mkdir -p $(abspath $(TARGET_OUT_HEADERS)/$(PRIVATE_MODULE)) && \
	    $(abspath $(ACP)) -fprt * \
	        $(abspath $(TARGET_OUT_HEADERS)/$(PRIVATE_MODULE))/ )

# This overrides the recipe to build a shared library in
# build/core/shared_library.mk to prevent the library built by the cmake
# generate makefiles from being overwritten by the link step.
#
# TODO(smiles): Modify shared_library.mk so that it's possible to override
# the default behavior of this recipe.
$(linked_module): $(all_objects) $(all_libraries) \
                  $(LOCAL_ADDITIONAL_DEPENDENCIES) \
                  $(my_target_crtbegin_so_o) $(my_target_crtend_so_o)
	$(hide) echo Empty recipe for $(@)


# Clear local variables.
LOCAL_CMAKE_OPTIONS:=
LOCAL_CMAKE_PROJECT_PATH:=
LOCAL_CMAKE_HEADER_INSTALL_PATH:=
