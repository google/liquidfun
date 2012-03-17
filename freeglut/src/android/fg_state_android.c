/*
 * freeglut_state_android.c
 *
 * Android-specific freeglut state query methods.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by John F. Fay, <fayjf@sourceforge.net>
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
#include <stdio.h>
#include <android/native_window.h>
#include "fg_internal.h"

int fgPlatformGlutDeviceGet ( GLenum eWhat )
{
  fprintf(stderr, "fgPlatformGlutDeviceGet: STUB\n");
  return -1;
}

int fgPlatformGlutGet ( GLenum eWhat )
{
  fprintf(stderr, "fgPlatformGlutGet: STUB\n");

  switch (eWhat) {
  case GLUT_WINDOW_WIDTH:
  case GLUT_WINDOW_HEIGHT:
    {
      if ( fgStructure.CurrentWindow == NULL )
	return 0;
      int32_t width = ANativeWindow_getWidth(fgStructure.CurrentWindow->Window.Handle);
      int32_t height = ANativeWindow_getHeight(fgStructure.CurrentWindow->Window.Handle);
      switch ( eWhat )
	{
	case GLUT_WINDOW_WIDTH:
	  return width;
	case GLUT_WINDOW_HEIGHT:
	  return height;
	}
    }
    
  default:
    return fghPlatformGlutGetEGL(eWhat);
  }
  return -1;
}

int fgPlatformGlutLayerGet( GLenum eWhat )
{
  fprintf(stderr, "fgPlatformGlutLayerGet: STUB\n");

  /*
   * This is easy as layers are not implemented ;-)
   *
   * XXX Can we merge the UNIX/X11 and WIN32 sections?  Or
   * XXX is overlay support planned?
   */
  switch( eWhat )
    {
    case GLUT_OVERLAY_POSSIBLE:
      return 0;

    case GLUT_LAYER_IN_USE:
      return GLUT_NORMAL;

    case GLUT_HAS_OVERLAY:
      return 0;

    case GLUT_TRANSPARENT_INDEX:
      /*
       * Return just anything, which is always defined as zero
       *
       * XXX HUH?
       */
      return 0;

    case GLUT_NORMAL_DAMAGED:
      /* XXX Actually I do not know. Maybe. */
      return 0;

    case GLUT_OVERLAY_DAMAGED:
      return -1;

    default:
      fgWarning( "glutLayerGet(): missing enum handle %d", eWhat );
      break;
    }

  /* And fail. That's good. Programs do love failing. */
  return -1;
}
