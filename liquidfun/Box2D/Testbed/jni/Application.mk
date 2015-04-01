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
APP_PLATFORM := android-10
APP_ABI:=armeabi-v7a
APP_STL:=gnustl_static
NDK_MODULE_PATH+=$(subst $(space),,\
  $(abspath $(NDK_PROJECT_PATH)/../../)$(HOST_DIRSEP) \
  $(abspath $(NDK_PROJECT_PATH)/../../../))
APP_MODULES:=Testbed
# NOTE: -Warray-bounds is disabled to workaround a spurious warning with the
# use of std::sort() in Tests/CollisionProcessing.h.
APP_CFLAGS+=-Wall -Werror -Wno-long-long -Wno-variadic-macros -Wno-array-bounds
