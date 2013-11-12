# Copyright 2013 Google. All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd
APP_PLATFORM := android-10
APP_ABI:=armeabi-v7a
APP_STL:=stlport_static
NDK_MODULE_PATH+=$(subst $(space),,\
  $(abspath $(NDK_PROJECT_PATH)/../../)$(HOST_DIRSEP) \
  $(abspath $(NDK_PROJECT_PATH)/../../../))
APP_MODULES:=Testbed
