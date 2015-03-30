/*
 * fg_gamemode_x11.c
 *
 * The game mode handling code.
 *
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
#include "fg_internal.h"

/*
 * Changes the current display mode to match user's settings
 */
GLboolean fgPlatformChangeDisplayMode( GLboolean haveToTest )
{
  fprintf(stderr, "fgPlatformChangeDisplayMode: STUB\n");
  return GL_FALSE;
}

void fgPlatformEnterGameMode( void )
{
  fprintf(stderr, "fgPlatformEnterGameMode: STUB\n");
}

void fgPlatformRememberState( void )
{
  fprintf(stderr, "fgPlatformRememberState: STUB\n");
}

void fgPlatformRestoreState( void )
{
  fprintf(stderr, "fgPlatformRestoreState: STUB\n");
}

void fgPlatformLeaveGameMode( void ) 
{
  fprintf(stderr, "fgPlatformLeaveGameMode: STUB\n");
}

