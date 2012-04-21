/*
 * fg_window_x11_glx.c
 *
 * Window management methods for X11 with GLX
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Copied for Platform code by Evan Felix <karcaw at gmail.com>
 * Creation date: Thur Feb 2 2012
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

/* pushing attribute/value pairs into an array */
#define ATTRIB(a) attributes[where++]=(a)
#define ATTRIB_VAL(a,v) {ATTRIB(a); ATTRIB(v);}

/*
 * Chooses a visual basing on the current display mode settings
 */

int fghChooseConfig(GLXFBConfig* fbconfig)
{
  GLboolean wantIndexedMode = GL_FALSE;
  int attributes[ 100 ];
  int where = 0, numAuxBuffers;

  /* First we have to process the display mode settings... */
  if( fgState.DisplayMode & GLUT_INDEX ) {
    ATTRIB_VAL( GLX_BUFFER_SIZE, 8 );
    /*  Buffer size is selected later.  */

    ATTRIB_VAL( GLX_RENDER_TYPE, GLX_COLOR_INDEX_BIT );
    wantIndexedMode = GL_TRUE;
  } else {
    ATTRIB_VAL( GLX_RED_SIZE,   1 );
    ATTRIB_VAL( GLX_GREEN_SIZE, 1 );
    ATTRIB_VAL( GLX_BLUE_SIZE,  1 );
    if( fgState.DisplayMode & GLUT_ALPHA ) {
      ATTRIB_VAL( GLX_ALPHA_SIZE, 1 );
    }
  }

  if( fgState.DisplayMode & GLUT_DOUBLE ) {
    ATTRIB_VAL( GLX_DOUBLEBUFFER, True );
  }

  if( fgState.DisplayMode & GLUT_STEREO ) {
    ATTRIB_VAL( GLX_STEREO, True );
  }

  if( fgState.DisplayMode & GLUT_DEPTH ) {
    ATTRIB_VAL( GLX_DEPTH_SIZE, 1 );
  }

  if( fgState.DisplayMode & GLUT_STENCIL ) {
    ATTRIB_VAL( GLX_STENCIL_SIZE, 1 );
  }

  if( fgState.DisplayMode & GLUT_ACCUM ) {
    ATTRIB_VAL( GLX_ACCUM_RED_SIZE, 1 );
    ATTRIB_VAL( GLX_ACCUM_GREEN_SIZE, 1 );
    ATTRIB_VAL( GLX_ACCUM_BLUE_SIZE, 1 );
    if( fgState.DisplayMode & GLUT_ALPHA ) {
      ATTRIB_VAL( GLX_ACCUM_ALPHA_SIZE, 1 );
    }
  }

  numAuxBuffers = fghNumberOfAuxBuffersRequested();
  if ( numAuxBuffers > 0 ) {
    ATTRIB_VAL( GLX_AUX_BUFFERS, numAuxBuffers );
  }

  if( fgState.DisplayMode & GLUT_SRGB ) {
    ATTRIB_VAL( GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, True );
  }

  if (fgState.DisplayMode & GLUT_MULTISAMPLE) {
    ATTRIB_VAL(GLX_SAMPLE_BUFFERS, 1);
    ATTRIB_VAL(GLX_SAMPLES, fgState.SampleNumber);
  }

  /* Push a terminator at the end of the list */
  ATTRIB( None );

    {
        GLXFBConfig * fbconfigArray;  /*  Array of FBConfigs  */
        int fbconfigArraySize;        /*  Number of FBConfigs in the array  */


        /*  Get all FBConfigs that match "attributes".  */
        fbconfigArray = glXChooseFBConfig( fgDisplay.pDisplay.Display,
                                           fgDisplay.pDisplay.Screen,
                                           attributes,
                                           &fbconfigArraySize );

        if (fbconfigArray != NULL)
        {
            int result __fg_unused;  /* Returned by glXGetFBConfigAttrib, not checked. */


            if( wantIndexedMode )
            {
                /*
                 * In index mode, we want the largest buffer size, i.e. visual
                 * depth.  Here, FBConfigs are sorted by increasing buffer size
                 * first, so FBConfigs with the largest size come last.
                 */

                int bufferSizeMin, bufferSizeMax;

                /*  Get bufferSizeMin.  */
                result =
                  glXGetFBConfigAttrib( fgDisplay.pDisplay.Display,
                                        fbconfigArray[0],
                                        GLX_BUFFER_SIZE,
                                        &bufferSizeMin );
                /*  Get bufferSizeMax.  */
                result =
                  glXGetFBConfigAttrib( fgDisplay.pDisplay.Display,
                                        fbconfigArray[fbconfigArraySize - 1],
                                        GLX_BUFFER_SIZE,
                                        &bufferSizeMax );

                if (bufferSizeMax > bufferSizeMin)
                {
                    /* 
                     * Free and reallocate fbconfigArray, keeping only FBConfigs
                     * with the largest buffer size.
                     */
                    XFree(fbconfigArray);

                    /*  Add buffer size token at the end of the list.  */
                    where--;
                    ATTRIB_VAL( GLX_BUFFER_SIZE, bufferSizeMax );
                    ATTRIB( None );

                    fbconfigArray = glXChooseFBConfig( fgDisplay.pDisplay.Display,
                                                       fgDisplay.pDisplay.Screen,
                                                       attributes,
                                                       &fbconfigArraySize );
                }
            }

            *fbconfig = fbconfigArray[0];
        }
        else
        {
           *fbconfig = NULL;
	   return 0;
        }
	XFree(fbconfigArray);

        return 1;
    }
}

static void fghFillContextAttributes( int *attributes ) {
  int where = 0, contextFlags, contextProfile;

  ATTRIB_VAL( GLX_CONTEXT_MAJOR_VERSION_ARB, fgState.MajorVersion );
  ATTRIB_VAL( GLX_CONTEXT_MINOR_VERSION_ARB, fgState.MinorVersion );

  contextFlags =
    fghMapBit( fgState.ContextFlags, GLUT_DEBUG, GLX_CONTEXT_DEBUG_BIT_ARB ) |
    fghMapBit( fgState.ContextFlags, GLUT_FORWARD_COMPATIBLE, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB );
  if ( contextFlags != 0 ) {
    ATTRIB_VAL( GLX_CONTEXT_FLAGS_ARB, contextFlags );
  }

  contextProfile =
    fghMapBit( fgState.ContextProfile, GLUT_CORE_PROFILE, GLX_CONTEXT_CORE_PROFILE_BIT_ARB ) |
    fghMapBit( fgState.ContextProfile, GLUT_COMPATIBILITY_PROFILE, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB );
  if ( contextProfile != 0 ) {
    ATTRIB_VAL( GLX_CONTEXT_PROFILE_MASK_ARB, contextProfile );
  }

  ATTRIB( 0 );
}

typedef GLXContext (*CreateContextAttribsProc)(Display *dpy, GLXFBConfig config,
					       GLXContext share_list, Bool direct,
					       const int *attrib_list);

GLXContext fghCreateNewContext( SFG_Window* window )
{
  /* for color model calculation */
  int menu = ( window->IsMenu && !fgStructure.MenuContext );
  int index_mode = ( fgState.DisplayMode & GLUT_INDEX );

  /* "classic" context creation */
  Display *dpy = fgDisplay.pDisplay.Display;
  GLXFBConfig config = window->Window.pContext.FBConfig;
  int render_type = ( !menu && index_mode ) ? GLX_COLOR_INDEX_TYPE : GLX_RGBA_TYPE;
  GLXContext share_list = NULL;
  Bool direct = ( fgState.DirectContext != GLUT_FORCE_INDIRECT_CONTEXT );
  GLXContext context;

  /* new context creation */
  int attributes[9];
  CreateContextAttribsProc createContextAttribs = (CreateContextAttribsProc) fgPlatformGetProcAddress( "glXCreateContextAttribsARB" );
 
  /* glXCreateContextAttribsARB not found, yet the user has requested the new context creation */
  if ( !createContextAttribs && !fghIsLegacyContextRequested() ) {
    fgWarning( "OpenGL >2.1 context requested but glXCreateContextAttribsARB is not available! Falling back to legacy context creation" );
	fgState.MajorVersion = 2;
	fgState.MinorVersion = 1;
  }

  /* If nothing fancy has been required, simply use the old context creation GLX API entry */
  if ( fghIsLegacyContextRequested() || !createContextAttribs )
  {
    context = glXCreateNewContext( dpy, config, render_type, share_list, direct );
    if ( context == NULL ) {
      fghContextCreationError();
    }
    return context;
  }

  /* color index mode is not available anymore with OpenGL 3.0 */
  if ( render_type == GLX_COLOR_INDEX_TYPE ) {
    fgWarning( "color index mode is deprecated, using RGBA mode" );
  }

  fghFillContextAttributes( attributes );

  context = createContextAttribs( dpy, config, share_list, direct, attributes );
  if ( context == NULL ) {
    fghContextCreationError();
  }
  return context;
}

void fgPlatformSetWindow ( SFG_Window *window )
{
    if ( window )
    {
        glXMakeContextCurrent(
            fgDisplay.pDisplay.Display,
            window->Window.Handle,
            window->Window.Handle,
            window->Window.Context
        );
    }
}
