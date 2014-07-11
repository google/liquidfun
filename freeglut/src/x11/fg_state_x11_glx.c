/*
 * fg_state_x11_glx.c
 *
 * X11-specific freeglut state query methods.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by John F. Fay, <fayjf@sourceforge.net>
 * Creation date: Sat Feb 4 2012
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
#include "x11/fg_window_x11_glx.h"

/*
 * Queries the GL context about some attributes
 */
int fgPlatformGetConfig( int attribute )
{
  int returnValue = 0;
  int result __fg_unused;  /*  Not checked  */

  if( fgStructure.CurrentWindow )
      result = glXGetFBConfigAttrib( fgDisplay.pDisplay.Display,
                                     fgStructure.CurrentWindow->Window.pContext.FBConfig,
                                     attribute,
                                     &returnValue );

  return returnValue;
}

int fghPlatformGlutGetGLX ( GLenum eWhat )
{
    switch( eWhat )
    {
    /*
     * The window/context specific queries are handled mostly by
     * fgPlatformGetConfig().
     */
    case GLUT_WINDOW_NUM_SAMPLES:
      {
	int nsamples = 0;
#ifdef GLX_VERSION_1_3
        glGetIntegerv(GL_SAMPLES, &nsamples);
#endif
        return nsamples;
      }

    /*
     * The rest of GLX queries under X are general enough to use a macro to
     * check them
     */
#   define GLX_QUERY(a,b) case a: return fgPlatformGetConfig( b );

    GLX_QUERY( GLUT_WINDOW_RGBA,                GLX_RGBA                );
    GLX_QUERY( GLUT_WINDOW_DOUBLEBUFFER,        GLX_DOUBLEBUFFER        );
    GLX_QUERY( GLUT_WINDOW_BUFFER_SIZE,         GLX_BUFFER_SIZE         );
    GLX_QUERY( GLUT_WINDOW_STENCIL_SIZE,        GLX_STENCIL_SIZE        );
    GLX_QUERY( GLUT_WINDOW_DEPTH_SIZE,          GLX_DEPTH_SIZE          );
    GLX_QUERY( GLUT_WINDOW_RED_SIZE,            GLX_RED_SIZE            );
    GLX_QUERY( GLUT_WINDOW_GREEN_SIZE,          GLX_GREEN_SIZE          );
    GLX_QUERY( GLUT_WINDOW_BLUE_SIZE,           GLX_BLUE_SIZE           );
    GLX_QUERY( GLUT_WINDOW_ALPHA_SIZE,          GLX_ALPHA_SIZE          );
    GLX_QUERY( GLUT_WINDOW_ACCUM_RED_SIZE,      GLX_ACCUM_RED_SIZE      );
    GLX_QUERY( GLUT_WINDOW_ACCUM_GREEN_SIZE,    GLX_ACCUM_GREEN_SIZE    );
    GLX_QUERY( GLUT_WINDOW_ACCUM_BLUE_SIZE,     GLX_ACCUM_BLUE_SIZE     );
    GLX_QUERY( GLUT_WINDOW_ACCUM_ALPHA_SIZE,    GLX_ACCUM_ALPHA_SIZE    );
    GLX_QUERY( GLUT_WINDOW_STEREO,              GLX_STEREO              );

#   undef GLX_QUERY

    /* I do not know yet if there will be a fgChooseVisual() function for Win32 */
    case GLUT_DISPLAY_MODE_POSSIBLE:
    {
        /*  We should not have to call fghChooseConfig again here.  */
        GLXFBConfig config;
        return fghChooseConfig(&config);
    }

    /* This is system-dependant */
    case GLUT_WINDOW_FORMAT_ID:
        if( fgStructure.CurrentWindow == NULL )
            return 0;

        return fgPlatformGetConfig( GLX_VISUAL_ID );

    default:
        fgWarning( "glutGet(): missing enum handle %d", eWhat );
        break;
    }

	return -1;
}

int *fgPlatformGlutGetModeValues(GLenum eWhat, int *size)
{
  int *array;

  int attributes[9];
  GLXFBConfig * fbconfigArray;  /*  Array of FBConfigs  */
  int fbconfigArraySize;        /*  Number of FBConfigs in the array  */
  int attribute_name = 0;

  array = NULL;
  *size = 0;

  switch (eWhat)
    {
    case GLUT_AUX:
    case GLUT_MULTISAMPLE:

      attributes[0] = GLX_BUFFER_SIZE;
      attributes[1] = GLX_DONT_CARE;

      switch (eWhat)
        {
        case GLUT_AUX:
          /*
            FBConfigs are now sorted by increasing number of auxiliary
            buffers.  We want at least one buffer.
          */
          attributes[2] = GLX_AUX_BUFFERS;
          attributes[3] = 1;
          attributes[4] = None;

          attribute_name = GLX_AUX_BUFFERS;

          break;


        case GLUT_MULTISAMPLE:
          attributes[2] = GLX_AUX_BUFFERS;
          attributes[3] = GLX_DONT_CARE;
          attributes[4] = GLX_SAMPLE_BUFFERS;
          attributes[5] = 1;
          /*
            FBConfigs are now sorted by increasing number of samples per
            pixel.  We want at least one sample.
          */
          attributes[6] = GLX_SAMPLES;
          attributes[7] = 1;
          attributes[8] = None;

          attribute_name = GLX_SAMPLES;

          break;
        }

      fbconfigArray = glXChooseFBConfig(fgDisplay.pDisplay.Display,
                                        fgDisplay.pDisplay.Screen,
                                        attributes,
                                        &fbconfigArraySize);

      if (fbconfigArray != NULL)
        {
          int * temp_array;
          int result __fg_unused;   /*  Returned by glXGetFBConfigAttrib. Not checked.  */
          int previous_value;
          int i;

          temp_array = malloc(sizeof(int) * fbconfigArraySize);
          previous_value = 0;

          for (i = 0; i < fbconfigArraySize; i++)
            {
              int value;

              result = glXGetFBConfigAttrib(fgDisplay.pDisplay.Display,
                                            fbconfigArray[i],
                                            attribute_name,
                                            &value);
              if (value > previous_value)
                {
                  temp_array[*size] = value;
                  previous_value = value;
                  (*size)++;
                }
            }

          if (*size) array = malloc(sizeof(int) * (*size));
          for (i = 0; i < *size; i++)
            {
              array[i] = temp_array[i];
            }

          free(temp_array);
          XFree(fbconfigArray);
        }

      break;

    default:
      break;
    }

  return array;
}
