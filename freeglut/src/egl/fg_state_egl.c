/*
 * fg_state_egl.c
 *
 * EGL-specific freeglut state query methods.
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
#include "egl/fg_window_egl.h"

/*
 * Queries the GL context about some attributes
 */
static int fgPlatformGetConfig( int attribute )
{
  int returnValue = 0;
  int result __fg_unused;  /*  Not checked  */

  if( fgStructure.CurrentWindow )
      result = eglGetConfigAttrib( fgDisplay.pDisplay.egl.Display,
				   fgStructure.CurrentWindow->Window.pContext.egl.Config,
				   attribute,
				   &returnValue );

  return returnValue;
}

int fghPlatformGlutGetEGL ( GLenum eWhat )
{
    int nsamples = 0;

    switch( eWhat )
    {
    /*
     * The window/context specific queries are handled mostly by
     * fgPlatformGetConfig().
     */
    case GLUT_WINDOW_NUM_SAMPLES:
        glGetIntegerv(GL_SAMPLES, &nsamples);
        return nsamples;

    /*
     * The rest of GLX queries under X are general enough to use a macro to
     * check them
     */
#   define EGL_QUERY(a,b) case a: return fgPlatformGetConfig( b );

    EGL_QUERY( GLUT_WINDOW_BUFFER_SIZE,         EGL_BUFFER_SIZE         );
    EGL_QUERY( GLUT_WINDOW_STENCIL_SIZE,        EGL_STENCIL_SIZE        );
    EGL_QUERY( GLUT_WINDOW_DEPTH_SIZE,          EGL_DEPTH_SIZE          );
    EGL_QUERY( GLUT_WINDOW_RED_SIZE,            EGL_RED_SIZE            );
    EGL_QUERY( GLUT_WINDOW_GREEN_SIZE,          EGL_GREEN_SIZE          );
    EGL_QUERY( GLUT_WINDOW_BLUE_SIZE,           EGL_BLUE_SIZE           );
    EGL_QUERY( GLUT_WINDOW_ALPHA_SIZE,          EGL_ALPHA_SIZE          );

#   undef EGL_QUERY

    /* I do not know yet if there will be a fgChooseVisual() function for Win32 */
    case GLUT_DISPLAY_MODE_POSSIBLE:
    {
        /*  We should not have to call fghChooseConfig again here.  */
        EGLConfig config;
        return fghChooseConfig(&config);
    }

    /* This is system-dependant */
    case GLUT_WINDOW_FORMAT_ID:
        if( fgStructure.CurrentWindow == NULL )
            return 0;
	return fgPlatformGetConfig( EGL_NATIVE_VISUAL_ID );

    default:
        fgWarning( "glutGet(): missing enum handle %d", eWhat );
        break;
    }

    return -1;
}

int* fgPlatformGlutGetModeValues(GLenum eWhat, int *size)
{
  int *array;

  int attributes[9];
  int attribute_name = 0;

  array = NULL;
  *size = 0;
  
  switch (eWhat)
    {
    case GLUT_AUX:
      break;

    case GLUT_MULTISAMPLE:
      attributes[0] = EGL_BUFFER_SIZE;
      attributes[1] = EGL_DONT_CARE;
      attributes[2] = EGL_SAMPLE_BUFFERS;
      attributes[3] = 1;
      attributes[4] = EGL_SAMPLES;
      attributes[5] = 1;
      attributes[6] = EGL_NONE;
      
      attribute_name = EGL_SAMPLES;

      EGLConfig* configArray;
      EGLint configArraySize = 0;
 
      /* Get number of available configs */
      if (!eglChooseConfig(fgDisplay.pDisplay.egl.Display,
			   attributes, NULL, 0,
			   &configArraySize))
	break;
      configArray = calloc(configArraySize, sizeof(EGLConfig));
      
      if (!eglChooseConfig(fgDisplay.pDisplay.egl.Display,
			   attributes, configArray, configArraySize,
			   &configArraySize))
	break;
      
      /* We get results in ascending order */
      {
	int previous_value = 0;
	int i;
	
	array = malloc(sizeof(int) * configArraySize);

	for (i = 0; i < configArraySize; i++) {
	  int value = 0;
	  eglGetConfigAttrib(fgDisplay.pDisplay.egl.Display,
			     configArray[i], attribute_name, &value);
	  if (value > previous_value)
	    {
	      previous_value = value;
	      array[*size] = value;
	      (*size)++;
	    }
	}
	
	array = realloc(array, sizeof(int) * (*size));
      }
      free(configArray);
      break;

    default:
      break;
    }

  return array;
}
