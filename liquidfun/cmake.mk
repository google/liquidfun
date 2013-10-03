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
## LOCAL_CMAKE_INCLUDE_RULES:
##   Path of rules file to include.  For example:
##   $(BUILD_SHARED_LIBRARY).
##
## LOCAL_CMAKE_BUILD_TARGETS:
##   Targets to build from the cmake generated makefiles.
##   Defaults to "all install"
##
## LOCAL_CMAKE_MODULE_OVERRIDE:
##   Specify to optionally override the name of the target
##   built by cmake.  This is used when LOCAL_MODULE name
##   differs from the name of the build artifact from cmake.
##
## LOCAL_CMAKE_GENERATOR:
##   cmake generator used to generate project files.  If
##   this variable isn't set, it defaults to "Unix Makefiles".
##   If "Unix Makefiles" isn't selected as the generator
##   this makefile will not generate rules to build the
##   the cmake project.
##
## NOTE:
## * This currently only supports building shared libraries.
###########################################################

ifeq ($(LOCAL_CMAKE_GENERATOR),)
LOCAL_CMAKE_GENERATOR:='Unix Makefiles'
endif
ifeq ($(LOCAL_CMAKE_BUILD_TARGETS),)
LOCAL_CMAKE_BUILD_TARGETS:=all install
endif

# If this isn't a NDK build, add the $(LOCAL_MODULE)-cmake to the list of
# dependencies for the binary build target.
ifeq ($(NDK_PROJECT_PATH),)
LOCAL_ADDITIONAL_DEPENDENCIES+=$(LOCAL_MODULE)-cmake
endif

# include rules to build this as a shared library.
ifeq ($(LOCAL_CMAKE_INCLUDE_RULES),)
$(error LOCAL_CMAKE_INCLUDE_RULES not defined)
endif
include $(LOCAL_CMAKE_INCLUDE_RULES)

# Set CMAKE to the location of the prebuilt cmake executable based upon the
# host OS.
define cmake-select-prebuilt-executable
$(eval \
  # Select the cmake binary for the host OS.
  CMAKE_BASE_DIR:=$(TOP)/prebuilts/cmake
  # Search if CMAKE doesn't point to a file.
  CMAKE:=$(abspath $(wildcard $(CMAKE)))
  ifeq ($$(CMAKE),)
    ifeq ($(HOST_OS),darwin)
      ifneq (,$(findstring x86_64,$(UNAME)))
        CMAKE:=$$(CMAKE_BASE_DIR)/$(HOST_OS)-x86_64/current/CMake\ *.app/\
Contents/bin/cmake
      else
        CMAKE:=$$(CMAKE_BASE_DIR)/$(HOST_OS)-x86/current/CMake\ *.app\
Contents/bin/cmake
      endif
    endif
    ifeq ($(HOST_OS),linux)
      CMAKE:=$$(CMAKE_BASE_DIR)/$(HOST_OS)-x86/current/bin/cmake
    endif
    ifeq ($(HOST_OS),windows)
      CMAKE:=$$(CMAKE_BASE_DIR)/$(HOST_OS)/current/bin/cmake.exe
    endif
    ifeq ($$(CMAKE),)
      $$(error Unable to find cmake for platform $(UNAME) not supported.)
    endif
    CMAKE:=$$(abspath $$(CMAKE))
  endif)
endef

# Find the cmake exectuable and set the CMAKE variable to its' path.
# If the makefile is being used by the NDK, the PATH is searched for the
# cmake executable.
define cmake-find-executable
$(eval \
  ifeq ($(NDK_PROJECT_PATH),)
	$(call cmake-select-prebuilt-executable)
  else
    # Search for cmake in the PATH.
    CMAKE?=$(wildcard $(shell which cmake))
    ifeq ($$(CMAKE),)
      $$(error Unable to find cmake executable.)
    endif
  endif)
endef

# Recursively copy $(1) to $(2) and preserve file attributes.
define cmake-copy-file
$(if $(NDK_PROJECT_PATH),\
  $(hide) cp -frL $(1) $(2),\
  $(hide) $(abspath $(ACP)) -fprt $(1) $(2))
endef

# Where source_file is relative to source_directory and
# target_directory is a directory to copy into copy
# source_directory/source_file to target_directory/source_file.
#
# $1: Source file
# $2: Directory source file is relative to.
# $3: Directory to link source file from.
define cmake-mirror-to-dir
$(3)/$(1): $(abspath $(2)/$(1))
	$(hide) mkdir -p $(3)/$(dir $(1))
	$(call cmake-copy-file,$$(<),$$(@))
endef

# Clear the cmake variable list.
define cmake-clear-variables
$(eval PRIVATE_CMAKE_VARIABLES:=)
endef

# Set CMAKE_$(1) to $(2) if it isn't set and add CMAKE_$(1) to the
# PRIVATE_CMAKE_VARIABLES list.
define cmake-add-variable
$(eval \
  ifeq ($(CMAKE_$(1)),)
    CMAKE_$(1):=$(2)
  endif
  PRIVATE_CMAKE_VARIABLES:=$(PRIVATE_CMAKE_VARIABLES) CMAKE_$(1))
endef

# Set CMAKE_$(1) to TARGET_$(2) if it's set, otherwise set it to
# $(PRIVATE_CMAKE_TOOLS_PREFIX)$(3)$(PRIVATE_EXECUTABLE_SUFFIX).
define cmake-add-tool
$(eval \
  ifeq ($(PRIVATE_$(2)),)
    CMAKE_TOOL_$(1):=$(PRIVATE_CMAKE_TOOLS_PREFIX)$(3)\
$(PRIVATE_EXECUTABLE_SUFFIX)
  else
    CMAKE_TOOL_$(1):=$(PRIVATE_$(2))
  endif)
$(call cmake-add-variable,$(1),$$(abspath $$(CMAKE_TOOL_$(1))))
endef

# Replace each string that references a filename in $(1) with the absolute
# path of the file.
define cmake-abspath-filenames
$(foreach variable,$(1), \
  $(if $(variable:-%=), \
    $(if $(wildcard $(variable)),$(abspath $(variable)),$(variable)), \
    $(variable)))
endef

# Extract CFLAGS from transform-c-or-s-to-o-no-deps.
#
# TODO(smiles): Since this is super brittle so the flags should probably be
# factored out of transform-c-or-s-to-o-no-deps in build/core/definitions.mk.
define cmake-transform-c-or-s-to-o-no-deps-cflags
$(call cmake-abspath-filenames, \
  $(shell echo $(call transform-c-or-s-to-o-no-deps) | \
               sed -r 's@.*$(PRIVATE_CC)[ \t]*@@; \
                       s@[ \t]*-c[ \t]+@ @g; \
                       s@[ \t]*-MD[ \t]+.*@@g;'))
endef

# Extract LDFLAGS from the linker command line $(1).
define cmake-get-ldflags-from-linker-command-line
$(call cmake-abspath-filenames, \
  $(shell echo '$(1)' | sed -r 's@.*$(subst +,\+,$(PRIVATE_CXX))[ \t]*@@; \
                                s@([ \t]*-L)([^ \t])@\1 \2@g; \
                                s@[ \t]*-o[ \t]*[^-][^ \t]+@ @g;'))
endef

# Extract LDFLAGS from transform-o-to-shared-lib.
#
# TODO(smiles): Since this is super brittle so the flags should probably be
# factored out of transform-o-to-shared-lib in build/core/definitions.mk.
define cmake-transform-o-to-shared-lib-ldflags
$(call cmake-get-ldflags-from-linker-command-line,\
$(call transform-o-to-shared-lib))
endef

# Extract LDFLAGS from transform-o-to-executable.
#
# TODO(smiles): Since this is super brittle so the flags should probably be
# factored out of transform-o-to-executable in build/core/definitions.mk.
define cmake-transform-o-to-executable-ldflags
$(call cmake-get-ldflags-from-linker-command-line,\
$(call transform-o-to-executable))
endef

# Extract LDFLAGS from cmd-build-shared-library-ldflags.
#
# TODO(smiles): Extract from default-built-commands.mk
# cmd-build-shared-library.
define cmake-cmd-build-shared-library-ldflags
$(call cmake-get-ldflags-from-linker-command-line,\
$(call cmd-build-shared-library))
endef

# Extract LDFLAGS from cmd-build-executable-ldflags.
#
# TODO(smiles): Extract from default-built-commands.mk cmd-build-excectuable.
define cmake-cmd-build-executable-ldflags
$(call cmake-get-ldflags-from-linker-command-line,\
$(call cmd-build-executable))
endef

# Get CFLAGS.
# NOTE: may need --sysroot=$(call host-path,$(PRIVATE_SYSROOT_LINK)) in here.
define cmake-get-cflags
$(if $(NDK_PROJECT_PATH),\
  $(PRIVATE_CFLAGS),\
  $(call cmake-transform-c-or-s-to-o-no-deps-cflags))
endef

# Get LDFLAGS for the link of a shared library.
define cmake-get-shared-lib-ldflags
$(if $(NDK_PROJECT_PATH),\
  $(call cmake-cmd-build-shared-library-ldflags), \
  $(call cmake-transform-o-to-shared-lib-ldflags))
endef

# Get LDFLAGS for the link of an executable.
define cmake-get-exectuable-ldflags
$(if $(NDK_PROJECT_PATH),\
  $(call cmake-cmd-build-executable-ldflags), \
  $(call cmake-transform-o-to-executable-ldflags))
endef

# Search path for sysroot.
define cmake-get-sysroot
$(if $(NDK_PROJECT_PATH),\
  $(abspath $(PRIVATE_SYSROOT_LINK)),\
  $(abspath $(PRIVATE_CMAKE_TOOLCHAIN_ROOT)/bin \
    $(PRIVATE_CMAKE_TOOLCHAIN_ROOT)))
endef

# Set variables for cmake within the context of a rule.
define cmake-set-variables
$(eval \
$(call cmake-clear-variables)
  # Configure cmake's environment variables.)
$(call cmake-add-tool,C_COMPILER,CC,gcc)
$(call cmake-add-tool,CXX_COMPILER,CXX,g++)
$(eval \
  # Don't check the compiler as cmake doesn't correctly add
  # CMAKE_*_LINKER_FLAGS to the command line during the link step causing
  # the check to fail.)
$(call cmake-add-variable,C_COMPILER_FORCED,TRUE)
$(call cmake-add-variable,CXX_COMPILER_FORCED,TRUE)
$(eval \
  # CMAKE_ASM_COMPILER defaults to gcc rather than as so this works with
  # command line flags derived from transform-c-or-s-to-o-no-deps.)
$(call cmake-add-tool,ASM_COMPILER,AS,gcc)
$(call cmake-add-tool,STRIP,STRIP,strip)
$(call cmake-add-tool,AR,AR,ar)
$(call cmake-add-tool,LINKER,LD,ld)
$(call cmake-add-tool,NM,NM,nm)
$(call cmake-add-tool,OBJCOPY,OBJCOPY,objcopy)
$(call cmake-add-tool,OBJDUMP,OBJDUMP,objdump)
$(call cmake-add-tool,RANLIB,RANLIB,ranlib)
$(call cmake-add-variable,SYSTEM_NAME,Linux)
$(call cmake-add-variable,SYSTEM_VERSION,1)
$(call cmake-add-variable,SYSTEM_PROCESSOR,$(PRIVATE_CMAKE_ARCH_VARIANT))
$(call cmake-add-variable,FIND_ROOT_PATH,$(call cmake-get-sysroot))
$(call cmake-add-variable,SHARED_LINKER_FLAGS, \
  $(call cmake-get-shared-lib-ldflags))
$(call cmake-add-variable,MODULE_LINKER_FLAGS,)
$(call cmake-add-variable,EXE_LINKER_FLAGS, \
  $(call cmake-get-exectuable-ldflags))
$(call cmake-add-variable,C_FLAGS, \
  $(call cmake-get-cflags))
$(call cmake-add-variable,CXX_FLAGS,$(CMAKE_C_FLAGS))
$(eval \
  # Finally, build the list of arguments for cmake.
  PRIVATE_CMAKE_ARGUMENTS:= \
    $(foreach variable,$(PRIVATE_CMAKE_VARIABLES),\
      -D$(variable)="$($(variable))") \
      -DTARGET_SUPPORTS_SHARED_LIBS=TRUE \
      -DCMAKE_INSTALL_PREFIX:PATH=$(PRIVATE_CMAKE_INSTALL_PATH) \
      -G$(PRIVATE_CMAKE_GENERATOR) \
      --no-warn-unused-cli)
endef

ifeq ($(NDK_PROJECT_PATH),)
# Install a cmake built module in $(PRIVATE_CMAKE_SYMBOLIC_INPUT) and copy
# its' headers to $(PRIVATE_CMAKE_OUT_HEADERS)/$(PRIVATE_CMAKE_MODULE).
define cmake-install
( mkdir -p $(dir $(PRIVATE_CMAKE_SYMBOLIC_INPUT)) && \
  $(call cmake-copy-file,\
$(PRIVATE_CMAKE_INSTALL_PATH)/$(PRIVATE_CMAKE_MODULE)$(PRIVATE_CMAKE_MODULE_SUFFIX), \
$(PRIVATE_CMAKE_SYMBOLIC_INPUT)) && \
  ( cd $(PRIVATE_CMAKE_INSTALL_PATH)/$(PRIVATE_CMAKE_HEADER_INSTALL_PATH) && \
    mkdir -p $(abspath \
$(PRIVATE_CMAKE_OUT_HEADERS)/$(PRIVATE_CMAKE_MODULE)) && \
    $(call cmake-copy-file,*, \
      $(abspath $(PRIVATE_CMAKE_OUT_HEADERS)/$(PRIVATE_CMAKE_MODULE))/) ) )
endef
else
# Install a cmake built module in
# $(PRIVATE_CMAKE_TARGET_OUT)/$(PRIVATE_CMAKE_MODULE_FILENAME).
define cmake-install
$(call cmake-copy-file,\
$(PRIVATE_CMAKE_INSTALL_PATH)/$(PRIVATE_CMAKE_MODULE)$(PRIVATE_CMAKE_MODULE_SUFFIX),\
  $(PRIVATE_CMAKE_TARGET_OUT)/$(PRIVATE_CMAKE_MODULE_FILENAME))
endef
endif

# Generate rules to mirror CMAKE_SOURCE_PROJECT_FILES to
# CMAKE_INTERMEDIATES_DIRECTORY.
CMAKE_SOURCE_PROJECT_FILES:=$(patsubst ./%,%,\
  $(shell cd $(LOCAL_PATH); find -L $(LOCAL_CMAKE_PROJECT_PATH) -type f))
ifeq ($(NDK_PROJECT_PATH),)
CMAKE_INTERMEDIATES_DIRECTORY:=$(call local-intermediates-dir)
else
CMAKE_INTERMEDIATES_DIRECTORY:=$(abspath $(LOCAL_OBJS_DIR))
endif
$(foreach file,$(CMAKE_SOURCE_PROJECT_FILES),$(eval \
  $(call cmake-mirror-to-dir,$(file),$(LOCAL_PATH),\
    $(CMAKE_INTERMEDIATES_DIRECTORY))))
# List of mirrored project files in the CMAKE_INTERMEDIATES_DIRECTORY.
CMAKE_INTERMEDIATE_PROJECT_FILES:=\
$(addprefix $(CMAKE_INTERMEDIATES_DIRECTORY)/, \
  $(CMAKE_SOURCE_PROJECT_FILES))

# install path for cmake's build artifacts.
CMAKE_INSTALL_PATH:=$(abspath $(CMAKE_INTERMEDIATES_DIRECTORY)/$(LOCAL_MODULE))

ifeq ($(NDK_PROJECT_PATH),)
CMAKE_INTERMEDIATE_TARGETS:=$(LOCAL_INTERMEDIATE_TARGETS)
else
CMAKE_INTERMEDIATE_TARGETS:=$(LOCAL_BUILT_MODULE)
$(CMAKE_INTERMEDIATE_TARGETS): $(LOCAL_MODULE)-cmake
endif

# Make LOCAL_* variables at this point in the script visible to the commands
# in the $(LOCAL_MODULE)-cmake build rule.
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_OPTIONS:=$(LOCAL_CMAKE_OPTIONS)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_PROJECT_PATH:=$(LOCAL_CMAKE_PROJECT_PATH)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_INTERMEDIATES_DIR:=$(CMAKE_INTERMEDIATES_DIRECTORY)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_INSTALL_PATH:=$(CMAKE_INSTALL_PATH)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_EXECUTABLE_SUFFIX:=$(HOST_EXECUTABLE_SUFFIX)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_GENERATOR:=$(LOCAL_CMAKE_GENERATOR)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_BUILD_TARGETS:=$(LOCAL_CMAKE_BUILD_TARGETS)

ifeq ($(NDK_PROJECT_PATH),)
# Since non-NDK builds use a different install step (see cmake-install),
# the following PRIVATE_ variables are not defined:
# * PRIVATE_CMAKE_TARGET_OUT
# * PRIVATE_CMAKE_TARGET_FILENAME

# Configure private variables for non-NDK builds.
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_ARCH_VARIANT:=$(TARGET_ARCH_VARIANT)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_TOOLCHAIN_ROOT:=$(TARGET_TOOLCHAIN_ROOT)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_TOOLS_PREFIX:=$(TARGET_TOOLS_PREFIX)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_MODULE:=$(strip $(if $(LOCAL_CMAKE_MODULE_OVERRIDE),\
      $(LOCAL_CMAKE_MODULE_OVERRIDE),$(LOCAL_MODULE)))
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_MODULE_SUFFIX:=$(LOCAL_MODULE_SUFFIX)
# Variables required by cmake-install in non-NDK builds.
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_HEADER_INSTALL_PATH:=$(LOCAL_CMAKE_HEADER_INSTALL_PATH)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_OUT_HEADERS:=$(TARGET_OUT_HEADERS)
ifeq ($(LOCAL_MODULE_CLASS),SHARED_LIBRARIES)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_SYMBOLIC_INPUT:=$(symbolic_input)
else
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_SYMBOLIC_INPUT:=$(LOCAL_BUILT_MODULE)
endif
else
# Configure private variables for NDK builds.
# Since NDK builds use a different install step (see cmake-install),
# the following PRIVATE_ variables are not defined:
# * PRIVATE_CMAKE_HEADER_INSTALL_PATH
# * PRIVATE_CMAKE_OUT_HEADERS
# * PRIVATE_CMAKE_SYMBOLIC_INPUT

# TODO(smiles): Figure out how to extract this from ev-compile-c-source in
# definitions.mk.
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CFLAGS:=\
		$($(my)CXXFLAGS) \
		$(call get-src-file-target-cflags,$(1)) \
		$(call host-c-includes, $(LOCAL_C_INCLUDES) $(LOCAL_PATH)) \
		$(LOCAL_CFLAGS) \
		$(LOCAL_CPPFLAGS) \
		$(LOCAL_CXXFLAGS) \
		$(NDK_APP_CFLAGS) \
		$(NDK_APP_CPPFLAGS) \
		$(NDK_APP_CXXFLAGS) \
		$(call host-c-includes,$($(my)C_INCLUDES)) \
		$(if $(APP_STL),-I$(NDK_ROOT)/sources/cxx-stl/stlport/stlport,)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_ARCH_VARIANT:=$(TARGET_ARCH)
$(CMAKE_INTERMEDIATE_TARGETS): \
	PRIVATE_CMAKE_TOOLS_PREFIX:=$(TOOLCHAIN_PREFIX)
CMAKE_MODULE_SUFFIX:=$(shell echo $(LOCAL_MODULE_FILENAME) | \
                         sed -r 's@.*(\.[^.]+)$$@\1@')
# NOTE: The following adds PRIVATE_ variables for the
# $(LOCAL_MODULE)-cmake-install so that it's possible to clobber the output
# of the link step.
$(LOCAL_MODULE)-cmake-install: \
	PRIVATE_CMAKE_INSTALL_PATH:=$(CMAKE_INSTALL_PATH)
$(LOCAL_MODULE)-cmake-install: \
	PRIVATE_CMAKE_MODULE_SUFFIX:=$(CMAKE_MODULE_SUFFIX)
$(LOCAL_MODULE)-cmake-install: \
	PRIVATE_CMAKE_MODULE:=$(strip $(if $(LOCAL_CMAKE_MODULE_OVERRIDE),\
      $(LOCAL_CMAKE_MODULE_OVERRIDE),\
	  $(LOCAL_MODULE_FILENAME:$(CMAKE_MODULE_SUFFIX)=)))
# Variables required by cmake-install in NDK builds.
$(LOCAL_MODULE)-cmake-install: \
	PRIVATE_CMAKE_TARGET_OUT:=$(TARGET_OUT)
$(LOCAL_MODULE)-cmake-install: \
	PRIVATE_CMAKE_MODULE_FILENAME:=$(LOCAL_MODULE_FILENAME)
endif

# Find the cmake executable.
$(call cmake-find-executable)

# Create the cmake makefiles in the intermediates directory.
#
# TODO(smiles): The Cmake command line is really big.  This needs to be moved
# into an intermediate script.
$(LOCAL_MODULE)-cmake: $(CMAKE_INTERMEDIATE_PROJECT_FILES)
	$(call cmake-set-variables)
	$(hide) ( \
	  cd $(PRIVATE_CMAKE_INTERMEDIATES_DIR)/$(PRIVATE_CMAKE_PROJECT_PATH) && \
	  $(CMAKE) $(PRIVATE_CMAKE_OPTIONS) $(PRIVATE_CMAKE_ARGUMENTS) && \
	  $(MAKE) $(if $(hide),,VERBOSE=1) $(PRIVATE_CMAKE_BUILD_TARGETS) )
	$(hide) $(cmake-install)

ifeq ($(NDK_PROJECT_PATH),)
ifeq ($(LOCAL_MODULE_CLASS),SHARED_LIBRARIES)
# This overrides the recipe to build a shared library in
# build/core/shared_library.mk to prevent the library built by the cmake
# generated makefiles from being overwritten by the link step.
#
# TODO(smiles): Modify shared_library.mk so that it's possible to override
# the default behavior of this recipe.
$(linked_module): $(all_objects) $(all_libraries) \
                  $(LOCAL_ADDITIONAL_DEPENDENCIES) \
                  $(my_target_crtbegin_so_o) $(my_target_crtend_so_o)
	$(hide) echo Empty recipe for $(@)
endif
else
# Clobber the output of the link step in build/core/build-binary.mk byb
# installing the cmake built module over the top of the empty linked module.
$(LOCAL_MODULE): $(LOCAL_MODULE)-cmake-install
$(LOCAL_MODULE)-cmake-install: $(LOCAL_BUILT_MODULE)
	$(call cmake-install)
endif

# Clear local variables.
LOCAL_CMAKE_OPTIONS:=
LOCAL_CMAKE_PROJECT_PATH:=
LOCAL_CMAKE_HEADER_INSTALL_PATH:=
LOCAL_CMAKE_INCLUDE_RULES:=
LOCAL_CMAKE_GENERATOR:=
LOCAL_CMAKE_MODULE_OVERRIDE:=
