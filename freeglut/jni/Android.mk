LOCAL_PATH:= $(call my-dir)/..

define build-freeglut
${eval \
  LOCAL_MODULE:=freeglut-gles$1$2
  LOCAL_SRC_FILES:=\
    $(filter-out $(LOCAL_PATH)/src/fg_font.c \
                 $(LOCAL_PATH)/src/fg_menu.c,\
      $(wildcard $(LOCAL_PATH)/src/*.c)) \
    $(wildcard $(LOCAL_PATH)/src/android/*.c) \
    $(wildcard $(LOCAL_PATH)/src/android/native_app_glue/*.c) \
    $(wildcard $(LOCAL_PATH)/src/egl/*.c) \
    $(wildcard $(LOCAL_PATH)/src/util/*c)
  LOCAL_C_INCLUDES:=\
    $(LOCAL_PATH)/src \
    $(LOCAL_PATH)/include
  LOCAL_CFLAGS:=-DFREEGLUT_GLES$1 \
                -DNEED_XPARSEGEOMETRY_IMPL
  LOCAL_CFLAGS+=\
    ${foreach v,VERSION_MAJOR VERSION_MINOR VERSION_PATCH,\
        -D${v}=${shell awk '/${v}/ { s = $$2; sub(/\)/, "", s); print s }' \
                   $(LOCAL_PATH)/CMakeLists.txt}}
  ifeq ($1,1)
    gllibsuffix:=_CM
  else
    gllibsuffix:=
  endif
  libraries:=log android EGL GLESv$1$$(gllibsuffix)
  ifeq ($2,_static)
    LOCAL_STATIC_LIBRARIES:=$$(libraries)
  else
    LOCAL_LDLIBS:=$$(foreach l,$$(libraries),-l$$(l))
    LOCAL_EXPORT_LDLIBS:=$$(LOCAL_LDLIBS))
    # Allow runtime linking of main().
    LOCAL_ALLOW_UNDEFINED_SYMBOLS:=true
    # Export symbols.
    LOCAL_CFLAGS+=-DFREEGLUT_EXPORTS
  endif
  LOCAL_EXPORT_C_INCLUDES:=$(LOCAL_PATH)/include
  LOCAL_EXPORT_CFLAGS+=-DFREEGLUT_GLES$1}
endef

include $(CLEAR_VARS)
$(call build-freeglut,1,_static)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
$(call build-freeglut,1,)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
$(call build-freeglut,2,_static)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
$(call build-freeglut,2,)
include $(BUILD_SHARED_LIBRARY)
