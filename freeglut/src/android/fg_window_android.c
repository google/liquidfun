/*
 * fg_window_android.c
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
#include <android/native_app_glue/android_native_app_glue.h>
#include <assert.h>

/*
 * Creates a Context and Surface on demand if none exist yet,
 * to be called after a valid window handle is present
 */

void fgAndroidTryCreateContextAndSurface(SFG_Window* window)
{
  if (!window->Window.Context) {
    window->State.WorkMask |= GLUT_INIT_WORK;

    /* Create context */
    fghChooseConfig(&window->Window.pContext.egl.Config);
    window->Window.Context = fghCreateNewContextEGL(window);
  }

  if (!window->Window.pContext.egl.Surface) {
    EGLDisplay display = fgDisplay.pDisplay.egl.Display;

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    EGLint vid;
    eglGetConfigAttrib(display, window->Window.pContext.egl.Config, EGL_NATIVE_VISUAL_ID, &vid);
    int32_t err = ANativeWindow_setBuffersGeometry(*window->Window.Handle, 0, 0, vid);
    assert(!err);
    (void)err;

    fghPlatformOpenWindowEGL(window);

    window->State.Visible = GL_TRUE;
  }
}

/*
 * Opens a window. Requires a SFG_Window object created and attached
 * to the freeglut structure. OpenGL context is created here.
 */
void fgPlatformOpenWindow( SFG_Window* window, const char* title,
                           GLboolean positionUse, int x, int y,
                           GLboolean sizeUse, int w, int h,
                           GLboolean gameMode, GLboolean isSubWindow )
{
  /* TODO: only one full-screen window possible? */
  if (fgDisplay.pDisplay.single_native_window && *fgDisplay.pDisplay.single_native_window != NULL) {
    fgWarning("You can't have more than one window on Android");
    return;
  }

  /* First, wait until Activity surface is available */
  /* Normally events are processed through glutMainLoop(), but the
     user didn't call it yet, and the Android may not have initialized
     the View yet.  So we need to wait for that to happen. */
  /* We can't return from this function before the OpenGL context is
     properly made current with a valid surface. So we wait for the
     surface. */
  while (fgDisplay.pDisplay.single_native_window == NULL || *fgDisplay.pDisplay.single_native_window == NULL) {
    /* APP_CMD_INIT_WINDOW will do the job */
    int ident;
    int events;
    struct android_poll_source* source;
    if ((ident=ALooper_pollOnce(0, NULL, &events, (void**)&source)) >= 0)
      if (source != NULL) source->process(source->app, source);
    /* fgPlatformProcessSingleEvent(); */
  }
  window->Window.Handle = fgDisplay.pDisplay.single_native_window;

  fgAndroidTryCreateContextAndSurface(window);
}

/*
 * Request a window resize
 */
void fgPlatformReshapeWindow ( SFG_Window *window, int width, int height )
{
  fprintf(stderr, "fgPlatformReshapeWindow: STUB\n");
}

/*
 * Closes a window, destroying the frame and OpenGL context
 */
void fgPlatformCloseWindow( SFG_Window* window )
{
  fghPlatformCloseWindowEGL(window);
  /* Window pre-created by Android, no way to delete it */
}

/*
 * This function makes the specified window visible
 */
void fgPlatformShowWindow( void )
{
  fprintf(stderr, "fgPlatformShowWindow: STUB\n");
}

/*
 * This function hides the specified window
 */
void fgPlatformHideWindow( SFG_Window *window )
{
  fprintf(stderr, "fgPlatformHideWindow: STUB\n");
}

/*
 * Iconify the specified window (top-level windows only)
 */
void fgPlatformIconifyWindow( SFG_Window *window )
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
 * Change the specified window's position
 */
void fgPlatformPositionWindow( SFG_Window *window, int x, int y )
{
  fprintf(stderr, "fgPlatformPositionWindow: STUB\n");
}

/*
 * Lowers the specified window (by Z order change)
 */
void fgPlatformPushWindow( SFG_Window *window )
{
  fprintf(stderr, "fgPlatformPushWindow: STUB\n");
}

/*
 * Raises the specified window (by Z order change)
 */
void fgPlatformPopWindow( SFG_Window *window )
{
  fprintf(stderr, "fgPlatformPopWindow: STUB\n");
}

/*
 * Toggle the window's full screen state.
 */
void fgPlatformFullScreenToggle( SFG_Window *win )
{
  fprintf(stderr, "fgPlatformFullScreenToggle: STUB\n");
}
