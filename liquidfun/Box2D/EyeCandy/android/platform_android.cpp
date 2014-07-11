/*
 * Copyright (c) 2013 Google, Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "platform.h"
#include <android_native_app_glue.h>
#include <android/log.h>
#include <sys/system_properties.h>

#define TAG "EyeCandy"
#define PRINT_LOG(prio, format) ({\
  va_list ap;\
  va_start(ap, format);\
  __android_log_vprint(prio, TAG, format, ap);\
  va_end(ap);\
})

void LOGI(const char *format, ...)
{
  PRINT_LOG(ANDROID_LOG_INFO, format);
}

void LOGE(const char *format, ...)
{
  PRINT_LOG(ANDROID_LOG_ERROR, format);
}

void LOGW(const char *format, ...)
{
  PRINT_LOG(ANDROID_LOG_WARN, format);
}

double System_getTime(SystemRef system) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

FileRef System_openFile(SystemRef system, const char *path) {
  AAssetManager *assetManager = ((android_app*)system)->activity->assetManager;
  return (FileRef)AAssetManager_open(assetManager, path, AASSET_MODE_BUFFER);
}

size_t File_getLength(FileRef file) {
  return AAsset_getLength((AAsset*)file);
}

const void *File_getBuffer(FileRef file) {
  return AAsset_getBuffer((AAsset*)file);
}

void File_close(FileRef file) {
  AAsset_close((AAsset*)file);
}
