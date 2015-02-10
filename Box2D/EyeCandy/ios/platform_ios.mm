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
#include <Foundation/Foundation.h>

#define PRINT_LOG(format) ({\
  va_list ap;\
  va_start(ap, format);\
  NSLogv(@(format), ap);\
  va_end(ap);\
})

void LOGI(const char *format, ...)
{
#ifdef DEBUG
  PRINT_LOG(format);
#endif
}

void LOGE(const char *format, ...)
{
  PRINT_LOG(format);
}

void LOGW(const char *format, ...)
{
  PRINT_LOG(format);
}

double System_getTime(SystemRef system) {
  return [NSDate timeIntervalSinceReferenceDate];
}

struct FileIOS {
  NSData *data;
};

FileRef System_openFile(SystemRef system, const char *path) {
  NSString *fullPath =
                     [[NSBundle mainBundle] pathForResource:@(path) ofType:nil];
  NSData *data = [NSData dataWithContentsOfFile:fullPath];
  FileIOS *file = (FileIOS*)calloc(1, sizeof(FileIOS));
  file->data = data;
  return file;
}

size_t File_getLength(FileRef file) {
  return [((FileIOS*)file)->data length];
}

const void *File_getBuffer(FileRef file) {
  return [((FileIOS*)file)->data bytes];
}

void File_close(FileRef file) {
  ((FileIOS*)file)->data = nil;
  free(file);
}



