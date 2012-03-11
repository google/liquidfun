/*
 * freeglut_state_x11.c
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
#include "../fg_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  fgPlatformChooseFBConfig()      -- OK, but what about glutInitDisplayString()?
 */

/* A helper function to check if a display mode is possible to use */
GLXFBConfig* fgPlatformChooseFBConfig( int* numcfgs );

/*
 * Queries the GL context about some attributes
 */
int fgPlatformGetConfig( int attribute )
{
  int returnValue = 0;
  int result;  /*  Not checked  */

  if( fgStructure.CurrentWindow )
      result = glXGetFBConfigAttrib( fgDisplay.pDisplay.Display,
                                     *(fgStructure.CurrentWindow->Window.pContext.FBConfig),
                                     attribute,
                                     &returnValue );

  return returnValue;
}

int fgPlatformGlutGet ( GLenum eWhat )
{
    int nsamples = 0;

    switch( eWhat )
    {
    /*
     * The window/context specific queries are handled mostly by
     * fgPlatformGetConfig().
     */
    case GLUT_WINDOW_NUM_SAMPLES:
#ifdef GLX_VERSION_1_3
        glGetIntegerv(GL_SAMPLES, &nsamples);
#endif
        return nsamples;

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

    /* Colormap size is handled in a bit different way than all the rest */
    case GLUT_WINDOW_COLORMAP_SIZE:
        if( (fgPlatformGetConfig( GLX_RGBA )) || (fgStructure.CurrentWindow == NULL) )
        {
            /*
             * We've got a RGBA visual, so there is no colormap at all.
             * The other possibility is that we have no current window set.
             */
            return 0;
        }
        else
        {
          const GLXFBConfig * fbconfig =
                fgStructure.CurrentWindow->Window.pContext.FBConfig;

          XVisualInfo * visualInfo =
                glXGetVisualFromFBConfig( fgDisplay.pDisplay.Display, *fbconfig );

          const int result = visualInfo->visual->map_entries;

          XFree(visualInfo);

          return result;
        }

    /*
     * Those calls are somewhat similiar, as they use XGetWindowAttributes()
     * function
     */
    case GLUT_WINDOW_X:
    case GLUT_WINDOW_Y:
    case GLUT_WINDOW_BORDER_WIDTH:
    case GLUT_WINDOW_HEADER_HEIGHT:
    {
        int x, y;
        Window w;

        if( fgStructure.CurrentWindow == NULL )
            return 0;

        XTranslateCoordinates(
            fgDisplay.pDisplay.Display,
            fgStructure.CurrentWindow->Window.Handle,
            fgDisplay.pDisplay.RootWindow,
            0, 0, &x, &y, &w);

        switch ( eWhat )
        {
        case GLUT_WINDOW_X: return x;
        case GLUT_WINDOW_Y: return y;
        }

        if ( w == 0 )
            return 0;
        XTranslateCoordinates(
            fgDisplay.pDisplay.Display,
            fgStructure.CurrentWindow->Window.Handle,
            w, 0, 0, &x, &y, &w);

        switch ( eWhat )
        {
        case GLUT_WINDOW_BORDER_WIDTH:  return x;
        case GLUT_WINDOW_HEADER_HEIGHT: return y;
        }
    }

    case GLUT_WINDOW_WIDTH:
    case GLUT_WINDOW_HEIGHT:
    {
        XWindowAttributes winAttributes;

        if( fgStructure.CurrentWindow == NULL )
            return 0;
        XGetWindowAttributes(
            fgDisplay.pDisplay.Display,
            fgStructure.CurrentWindow->Window.Handle,
            &winAttributes
        );
        switch ( eWhat )
        {
        case GLUT_WINDOW_WIDTH:            return winAttributes.width ;
        case GLUT_WINDOW_HEIGHT:           return winAttributes.height ;
        }
    }

    /* I do not know yet if there will be a fgChooseVisual() function for Win32 */
    case GLUT_DISPLAY_MODE_POSSIBLE:
    {
        /*  We should not have to call fgPlatformChooseFBConfig again here.  */
        GLXFBConfig * fbconfig;
        int isPossible;

        fbconfig = fgPlatformChooseFBConfig(NULL);

        if (fbconfig == NULL)
        {
            isPossible = 0;
        }
        else
        {
            isPossible = 1;
            XFree(fbconfig);
        }

        return isPossible;
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


int fgPlatformGlutDeviceGet ( GLenum eWhat )
{
    switch( eWhat )
    {
    case GLUT_HAS_KEYBOARD:
        /*
         * X11 has a core keyboard by definition, although it can
         * be present as a virtual/dummy keyboard. For now, there
         * is no reliable way to tell if a real keyboard is present.
         */
        return 1;

    /* X11 has a mouse by definition */
    case GLUT_HAS_MOUSE:
        return 1 ;

    case GLUT_NUM_MOUSE_BUTTONS:
        /* We should be able to pass NULL when the last argument is zero,
         * but at least one X server has a bug where this causes a segfault.
         *
         * In XFree86/Xorg servers, a mouse wheel is seen as two buttons
         * rather than an Axis; "freeglut_main.c" expects this when
         * checking for a wheel event.
         */
        {
            unsigned char map;
            int nbuttons = XGetPointerMapping(fgDisplay.pDisplay.Display, &map,0);
            return nbuttons;
        }

    default:
        fgWarning( "glutDeviceGet(): missing enum handle %d", eWhat );
        break;
    }

    /* And now -- the failure. */
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
          int result;   /*  Returned by glXGetFBConfigAttrib. Not checked.  */
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

          array = malloc(sizeof(int) * (*size));
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


