/*
 * fg_ext_android.c
 *
 * Functions related to OpenGL extensions.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Copied for Platform code by Evan Felix <karcaw at gmail.com>
 * Copyright (C) 2012  Sylvain Beucler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <GL/freeglut.h>
#include "../fg_internal.h"

GLUTproc fgPlatformGetGLUTProcAddress( const char* procName )
{
    /* optimization: quick initial check */
    if( strncmp( procName, "glut", 4 ) != 0 )
        return NULL;

#define CHECK_NAME(x) if( strcmp( procName, #x ) == 0) return (GLUTproc)x;
    CHECK_NAME(glutJoystickFunc);
    CHECK_NAME(glutForceJoystickFunc);
    CHECK_NAME(glutGameModeString);
    CHECK_NAME(glutEnterGameMode);
    CHECK_NAME(glutLeaveGameMode);
    CHECK_NAME(glutGameModeGet);
#undef CHECK_NAME

    return NULL;
}
