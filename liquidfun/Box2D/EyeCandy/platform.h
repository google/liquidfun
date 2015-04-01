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

#ifndef EyeCandy_platform_h
#define EyeCandy_platform_h

#ifdef __APPLE__
#  include <OpenGLES/ES2/gl.h>
#  include <OpenGLES/ES2/glext.h>
#else
#  include <GLES2/gl2.h>
#  include <GLES2/gl2ext.h>
#endif
#include <stddef.h>

void LOGI(const char *format, ...);
void LOGE(const char *format, ...);
void LOGW(const char *format, ...);

typedef void *SystemRef;
typedef void *FileRef;

double System_getTime(SystemRef system);
FileRef System_openFile(SystemRef system, const char *path);
size_t File_getLength(FileRef file);
const void *File_getBuffer(FileRef file);
void File_close(FileRef file);

#endif
