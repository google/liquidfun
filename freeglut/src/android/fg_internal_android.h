/*
 * fg_internal_android.h
 *
 * The freeglut library private include file.
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

#ifndef  FREEGLUT_INTERNAL_ANDROID_H
#define  FREEGLUT_INTERNAL_ANDROID_H


/* -- PLATFORM-SPECIFIC INCLUDES ------------------------------------------- */
/* Android OpenGL ES is accessed through EGL */
#include "egl/fg_internal_egl.h"

/* -- GLOBAL TYPE DEFINITIONS ---------------------------------------------- */
/* The structure used by display initialization in freeglut_init.c */
typedef struct tagSFG_PlatformDisplay SFG_PlatformDisplay;
struct android_app;
struct tagSFG_PlatformDisplay
{
  struct tagSFG_PlatformDisplayEGL egl;
  EGLNativeWindowType *single_native_window;
  struct android_app* app;
};

typedef struct tagSFG_PlatformContext SFG_PlatformContext;
/* SFG_PlatformContext is used for SFG_Window.Window */
struct tagSFG_PlatformContext
{
  struct tagSFG_PlatformContextEGL egl;
};


typedef int fg_bool;
static const fg_bool fg_false = 0;
static const fg_bool fg_true = 1;

/**
 * Virtual PAD (spots on touchscreen that simulate keys)
 */
struct vpad_state {
    fg_bool on;
    fg_bool left;
    fg_bool right;
    fg_bool up;
    fg_bool down;
};
struct touchscreen {
    struct vpad_state vpad;
    fg_bool in_mmotion;
};


/* -- JOYSTICK-SPECIFIC STRUCTURES AND TYPES ------------------------------- */
/*
 * Initial defines from "js.h" starting around line 33 with the existing "freeglut_joystick.c"
 * interspersed
 */

  /*
   * We'll put these values in and that should
   * allow the code to at least compile when there is
   * no support. The JS open routine should error out
   * and shut off all the code downstream anyway and if
   * the application doesn't use a joystick we'll be fine.
   */

  struct JS_DATA_TYPE
  {
    int buttons;
    int x;
    int y;
  };

#            define JS_RETURN (sizeof(struct JS_DATA_TYPE))

/* XXX It might be better to poll the operating system for the numbers of buttons and
 * XXX axes and then dynamically allocate the arrays.
 */
#    define _JS_MAX_AXES 16
typedef struct tagSFG_PlatformJoystick SFG_PlatformJoystick;
struct tagSFG_PlatformJoystick
{
       struct JS_DATA_TYPE js;

    char         fname [ 128 ];
    int          fd;
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

#endif  /* FREEGLUT_INTERNAL_ANDROID_H */
