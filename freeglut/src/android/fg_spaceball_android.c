/*
 * fg_spaceball_android.c
 *
 * Spaceball support for Windows
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by Evan Felix <karcaw at gmail.com>
 * Creation date: Sat Feb 4, 2012
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
/*
 * This code is a very complicated way of doing nothing.  
 * But is needed for Android platform builds.
 */

#include <GL/freeglut.h>
#include "fg_internal.h"

void fgPlatformInitializeSpaceball(void)
{
	return;
}

void fgPlatformSpaceballClose(void)
{
	return;
}

int fgPlatformHasSpaceball(void)
{
	return 0;
}

int fgPlatformSpaceballNumButtons(void)
{
	return 0;
}

void fgPlatformSpaceballSetWindow(SFG_Window *window)
{
	return;
}
