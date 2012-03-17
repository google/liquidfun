/*
 * freeglut_window_android.c
 *
 * Window management methods for Android
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

#define FREEGLUT_BUILDING_LIB
#include <GL/freeglut.h>
#include "fg_internal.h"
#include "egl/fg_window_egl.h"

/*
 * Opens a window. Requires a SFG_Window object created and attached
 * to the freeglut structure. OpenGL context is created here.
 */
void fgPlatformOpenWindow( SFG_Window* window, const char* title,
                           GLboolean positionUse, int x, int y,
                           GLboolean sizeUse, int w, int h,
                           GLboolean gameMode, GLboolean isSubWindow )
{
  printf("fgPlatformOpenWindow %p ID=%d\n", (void*)window, window->ID);

  /* TODO: only one full-screen window possible? */
  static int nb_windows = 0;
  if (nb_windows == 0) {
    nb_windows++;
    fgDisplay.pDisplay.single_window = window;
    printf("=> %p ID=%d\n", (void*)fgDisplay.pDisplay.single_window, fgDisplay.pDisplay.single_window->ID);
  } else {
    return;
  }

  fghChooseConfigEGL(&window->Window.pContext.egl.Config);
  window->Window.Context = fghCreateNewContextEGL(window);

  /* Wait until window is available and OpenGL context is created */
  /* Normally events are processed through glutMainLoop(), but the
     user didn't call it yet, and the Android may not have initialized
     the View yet.  So we need to wait for that to happen. */
  /* We can't return from this function before the OpenGL context is
     properly made current with a valid surface. So we wait for the
     surface. */
  while (fgDisplay.pDisplay.single_window->Window.Handle == NULL) {
    /* APP_CMD_INIT_WINDOW will do the job */
    fgPlatformProcessSingleEvent();
  }

  EGLDisplay display = fgDisplay.pDisplay.egl.Display;

  /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
   * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
   * As soon as we picked a EGLConfig, we can safely reconfigure the
   * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
  EGLint vid;
  eglGetConfigAttrib(display, window->Window.pContext.egl.Config,
		     EGL_NATIVE_VISUAL_ID, &vid);

  ANativeWindow_setBuffersGeometry(window->Window.Handle, 0, 0, vid);
  fghPlatformOpenWindowEGL(window);

  window->State.Visible = GL_TRUE;
}

/*
 * Closes a window, destroying the frame and OpenGL context
 */
void fgPlatformCloseWindow( SFG_Window* window )
{
  fghPlatformCloseWindowEGL(window);
  /* Window pre-created by Android, no way to delete it */
}

void fgPlatformSetWindow ( SFG_Window *window )
{
  /* TODO: only a single window possible? */
}

/*
 * This function makes the current window visible
 */
void fgPlatformGlutShowWindow( void )
{
  fprintf(stderr, "fgPlatformGlutShowWindow: STUB\n");
}

/*
 * This function hides the current window
 */
void fgPlatformGlutHideWindow( void )
{
  fprintf(stderr, "fgPlatformGlutHideWindow: STUB\n");
}

/*
 * Iconify the current window (top-level windows only)
 */
void fgPlatformGlutIconifyWindow( void )
{
  fprintf(stderr, "fgPlatformGlutIconifyWindow: STUB\n");
}

/*
 * Set the current window's title
 */
void fgPlatformGlutSetWindowTitle( const char* title )
{
  fprintf(stderr, "fgPlatformGlutSetWindowTitle: STUB\n");
}

/*
 * Set the current window's iconified title
 */
void fgPlatformGlutSetIconTitle( const char* title )
{
  fprintf(stderr, "fgPlatformGlutSetIconTitle: STUB\n");}

/*
 * Change the current window's position
 */
void fgPlatformGlutPositionWindow( int x, int y )
{
  fprintf(stderr, "fgPlatformGlutPositionWindow: STUB\n");
}

/*
 * Lowers the current window (by Z order change)
 */
void fgPlatformGlutPushWindow( void )
{
  fprintf(stderr, "fgPlatformGlutPushWindow: STUB\n");
}

/*
 * Raises the current window (by Z order change)
 */
void fgPlatformGlutPopWindow( void )
{
  fprintf(stderr, "fgPlatformGlutPopWindow: STUB\n");
}

/*
 * Resize the current window so that it fits the whole screen
 */
void fgPlatformGlutFullScreen( SFG_Window *win )
{
  fprintf(stderr, "fgPlatformGlutFullScreen: STUB\n");
}

/*
 * If we are fullscreen, resize the current window back to its original size
 */
void fgPlatformGlutLeaveFullScreen( SFG_Window *win )
{
  fprintf(stderr, "fgPlatformGlutLeaveFullScreen: STUB\n");
}

/*
 * Toggle the window's full screen state.
 */
void fgPlatformGlutFullScreenToggle( SFG_Window *win )
{
  fprintf(stderr, "fgPlatformGlutFullScreenToggle: STUB\n");
}
