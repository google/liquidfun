/*
 * fg_joystick_android.c
 *
 * Joystick handling code
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Steve Baker, <sjbaker1@airmail.net>
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
#include "fg_internal.h"

/**
 * TODO: Android has no joysticks at the moment (only touchscreens/touchpads),
 * but we could expose the accelerometer as a 3-axis joystick.
 */

void fgPlatformJoystickRawRead( SFG_Joystick* joy, int* buttons, float* axes )
{
    fgWarning("fgPlatformJoystickRawRead: STUB\n");
}

void fgPlatformJoystickOpen( SFG_Joystick* joy )
{
    fgWarning("fgPlatformJoystickOpen: STUB\n");
}

void fgPlatformJoystickInit( SFG_Joystick *fgJoystick[], int ident )
{
    fgWarning("fgJoystick: STUB\n");
}

void fgPlatformJoystickClose ( int ident )
{
    fgWarning("fgPlatformJoystickClose: STUB\n");
}
