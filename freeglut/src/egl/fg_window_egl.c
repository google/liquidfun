/*
 * freeglut_display_android.c
 *
 * Window management methods for EGL
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

/**
 * Initialize an EGL context for the current display.
 */
void fghCreateNewContextEGL( SFG_Window* window ) {
  /*
   * Here specify the attributes of the desired configuration.
   * Below, we select an EGLConfig with at least 8 bits per color
   * component compatible with on-screen windows
   */
  /* Ensure OpenGLES 2.0 context */
  printf("DisplayMode: %d (DEPTH %d)\n", fgState.DisplayMode, (fgState.DisplayMode & GLUT_DEPTH));
  const EGLint attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_BLUE_SIZE, 1,
    EGL_GREEN_SIZE, 1,
    EGL_RED_SIZE, 1,
    EGL_ALPHA_SIZE, (fgState.DisplayMode & GLUT_ALPHA) ? 1 : 0,
    EGL_DEPTH_SIZE, (fgState.DisplayMode & GLUT_DEPTH) ? 1 : 0,
    EGL_STENCIL_SIZE, (fgState.DisplayMode & GLUT_STENCIL) ? 1 : 0,
    EGL_SAMPLE_BUFFERS, (fgState.DisplayMode & GLUT_MULTISAMPLE) ? 1 : 0,
    EGL_SAMPLES, (fgState.DisplayMode & GLUT_MULTISAMPLE) ? fgState.SampleNumber : 0,
    EGL_NONE
  };

  EGLint format;
  EGLint numConfigs;
  EGLConfig config;
  EGLContext context;

  EGLDisplay eglDisplay = fgDisplay.pDisplay.egl.Display;

  /* Here, the application chooses the configuration it desires. In this
   * sample, we have a very simplified selection process, where we pick
   * the first EGLConfig that matches our criteria */
  eglChooseConfig(eglDisplay, attribs, &config, 1, &numConfigs);

  /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
   * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
   * As soon as we picked a EGLConfig, we can safely reconfigure the
   * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
  eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);

  /* Default, but doesn't hurt */
  eglBindAPI(EGL_OPENGL_ES_API);

  /* Ensure OpenGLES 2.0 context */
  static const EGLint ctx_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };
  context = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, ctx_attribs);
  if (context == EGL_NO_CONTEXT) {
    fgWarning("Cannot initialize EGL context, err=%x\n", eglGetError());
    fghContextCreationError();
  }
  EGLint ver = -1;
  eglQueryContext(fgDisplay.pDisplay.egl.Display, context, EGL_CONTEXT_CLIENT_VERSION, &ver);
  if (ver != 2)
    fgError("Wrong GLES major version: %d\n", ver);

  window->Window.Context = context;
  window->Window.pContext.egl.ContextConfig = config;
  window->Window.pContext.egl.ContextFormat = format;
}

/*
 * Really opens a window when handle is available
 */
void fghPlatformOpenWindowEGL( SFG_Window* window )
{
  EGLDisplay display = fgDisplay.pDisplay.egl.Display;
  EGLConfig  config  = window->Window.pContext.egl.ContextConfig;

  EGLSurface surface = eglCreateWindowSurface(display, config, window->Window.Handle, NULL);
  if (surface == EGL_NO_SURFACE)
    fgError("Cannot create EGL window surface, err=%x\n", eglGetError());
  if (eglMakeCurrent(display, surface, surface, window->Window.Context) == EGL_FALSE)
    fgError("eglMakeCurrent: err=%x\n", eglGetError());

  //EGLint w, h;
  //eglQuerySurface(display, surface, EGL_WIDTH, &w);
  //eglQuerySurface(display, surface, EGL_HEIGHT, &h);

  window->Window.pContext.egl.Surface = surface;
}

/*
 * Closes a window, destroying the frame and OpenGL context
 */
void fghPlatformCloseWindowEGL( SFG_Window* window )
{
  eglMakeCurrent(fgDisplay.pDisplay.egl.Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  if (window->Window.Context != EGL_NO_CONTEXT) {
    eglDestroyContext(fgDisplay.pDisplay.egl.Display, window->Window.Context);
    window->Window.Context = EGL_NO_CONTEXT;
  }

  if (window->Window.pContext.egl.Surface != EGL_NO_SURFACE) {
    eglDestroySurface(fgDisplay.pDisplay.egl.Display, window->Window.pContext.egl.Surface);
    window->Window.pContext.egl.Surface = EGL_NO_SURFACE;
  }
}
