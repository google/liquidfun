/*
 * fg_internal_ios.h
 *
 * The freeglut library private include file.
 *
 * Copyright (C) 2014 Google, Inc.
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

#ifndef  FREEGLUT_INTERNAL_IOS_H
#define  FREEGLUT_INTERNAL_IOS_H

/* -- PLATFORM-SPECIFIC INCLUDES ------------------------------------------- */
#include <OpenGLES/ES1/gl.h>

/* -- GLOBAL TYPE DEFINITIONS ---------------------------------------------- */

/*
 * Make "freeglut" window handle and context types so that we don't need so
 * much conditionally-compiled code later in the library.
 */
#ifdef __OBJC__
@class FGViewController;
typedef FGViewController *SFG_WindowHandleType;
typedef EAGLContext *SFG_WindowContextType;
#else
typedef void *SFG_WindowHandleType;
typedef void *SFG_WindowContextType;
#endif

/* The structure used by display initialization in freeglut_init.c */
typedef struct tagSFG_PlatformDisplay SFG_PlatformDisplay;
struct tagSFG_PlatformDisplay
{
};

typedef struct tagSFG_PlatformContext SFG_PlatformContext;
/* SFG_PlatformContext is used for SFG_Window.Window */
struct tagSFG_PlatformContext
{
};

typedef int fg_bool;
static const fg_bool fg_false = 0;
static const fg_bool fg_true = 1;


/* -- JOYSTICK-SPECIFIC STRUCTURES AND TYPES ------------------------------- */
#define _JS_MAX_AXES 16
typedef struct tagSFG_PlatformJoystick SFG_PlatformJoystick;
struct tagSFG_PlatformJoystick
{
};

/* Window's state description. This structure should be kept portable. */
typedef struct tagSFG_PlatformWindowState SFG_PlatformWindowState;
struct tagSFG_PlatformWindowState
{
    char unused;
};

/* Menu font and color definitions */
#define  FREEGLUT_MENU_FONT    NULL
#define  FREEGLUT_MENU_PEN_FORE_COLORS   {0.0f,  0.0f,  0.0f,  1.0f}
#define  FREEGLUT_MENU_PEN_BACK_COLORS   {0.70f, 0.70f, 0.70f, 1.0f}
#define  FREEGLUT_MENU_PEN_HFORE_COLORS  {0.0f,  0.0f,  0.0f,  1.0f}
#define  FREEGLUT_MENU_PEN_HBACK_COLORS  {1.0f,  1.0f,  1.0f,  1.0f}

#endif  /* FREEGLUT_INTERNAL_IOS_H */
