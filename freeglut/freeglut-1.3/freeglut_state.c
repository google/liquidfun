/*
 * freeglut_state.c
 *
 * Freeglut state query methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Thu Dec 16 1999
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define  G_LOG_DOMAIN  "freeglut-state"

#include "../include/GL/freeglut.h"
#include "freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  glutGet()               -- X11 tests passed, but check if all enums handled (what about Win32?)
 *  glutDeviceGet()         -- X11 tests passed, but check if all enums handled (what about Win32?)
 *  glutGetModifiers()      -- OK, but could also remove the limitation
 *  glutLayerGet()          -- what about GLUT_NORMAL_DAMAGED?
 *
 * The fail-on-call policy will help adding the most needed things imho.
 */

/* -- LOCAL DEFINITIONS ---------------------------------------------------- */

/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

#if TARGET_HOST_UNIX_X11
/*
 * Queries the GL context about some attributes
 */
static int fghGetConfig( int attribute )
{
  int returnValue ;

  /*
   * Return nothing if there is no current window set
   */
  if( fgStructure.Window == NULL )
    return( 0 );

  /*
   * glXGetConfig should work fine
   */
  glXGetConfig( fgDisplay.Display, fgStructure.Window->Window.VisualInfo, attribute, &returnValue );


  /*
   * Have the query results returned
   */
  return ( returnValue ) ;
}
#endif

/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * General settings assignment method
 */
void FGAPIENTRY glutSetOption( GLenum eWhat, int value )
{
  freeglut_assert_ready;

  /*
   * Check what is the caller querying for. In chronological code add order.
   */
  switch( eWhat )
  {
  case GLUT_INIT_WINDOW_X:          fgState.Position.X          = (GLint)value ;
                                    break ;
  case GLUT_INIT_WINDOW_Y:          fgState.Position.Y          = (GLint)value ;
                                    break ;
  case GLUT_INIT_WINDOW_WIDTH:      fgState.Size.X              = (GLint)value ;
                                    break ;
  case GLUT_INIT_WINDOW_HEIGHT:     fgState.Size.Y              = (GLint)value ;
                                    break ;
  case GLUT_INIT_DISPLAY_MODE:      fgState.DisplayMode         = (unsigned int)value ;
                                    break ;

  case GLUT_ACTION_ON_WINDOW_CLOSE: fgState.ActionOnWindowClose = value ;
                                    break ;

  case GLUT_WINDOW_CURSOR:
      if( fgStructure.Window != NULL ) fgStructure.Window->State.Cursor = value ;
      break ;

  default:
      /*
       * Just have it reported, so that we can see what needs to be implemented
       */
      fgWarning( "glutSetOption(): missing enum handle %i\n", eWhat );
      break;
  }
}

/*
 * General settings query method
 */
int FGAPIENTRY glutGet( GLenum eWhat )
{
    int returnValue ;
    GLboolean boolValue ;

    if ( eWhat == GLUT_INIT_STATE )
	return ( fgState.Time.Set ) ;

    freeglut_assert_ready;

    /*
     * Check what is the caller querying for. In chronological code add order.
     */
    switch( eWhat )
    {
    case GLUT_ELAPSED_TIME:
        /*
         * This is easy and nicely portable, as we are using GLib...
         */
        return( fgElapsedTime() );

    /*
     * Following values are stored in fgState and fgDisplay global structures
     */
    case GLUT_SCREEN_WIDTH:         return( fgDisplay.ScreenWidth    );
    case GLUT_SCREEN_HEIGHT:        return( fgDisplay.ScreenHeight   );
    case GLUT_SCREEN_WIDTH_MM:      return( fgDisplay.ScreenWidthMM  );
    case GLUT_SCREEN_HEIGHT_MM:     return( fgDisplay.ScreenHeightMM );
    case GLUT_INIT_WINDOW_X:        return( fgState.Position.X       );
    case GLUT_INIT_WINDOW_Y:        return( fgState.Position.Y       );
    case GLUT_INIT_WINDOW_WIDTH:    return( fgState.Size.X           );
    case GLUT_INIT_WINDOW_HEIGHT:   return( fgState.Size.Y           );
    case GLUT_INIT_DISPLAY_MODE:    return( fgState.DisplayMode      );

    /*
     * The window/context specific queries are handled mostly by fghGetConfig().
     */
    case GLUT_WINDOW_NUM_SAMPLES:
        /*
         * Multisampling. Return what I know about multisampling.
         */
        return( 0 );

#if TARGET_HOST_UNIX_X11
    /*
     * The rest of GLX queries under X are general enough to use a macro to check them
     */
#   define GLX_QUERY(a,b) case a: return( fghGetConfig( b ) );

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

    /*
     * Colormap size is handled in a bit different way than all the rest
     */
    case GLUT_WINDOW_COLORMAP_SIZE:
        /*
         * Check for the visual type
         */
        if( (fghGetConfig( GLX_RGBA )) || (fgStructure.Window == NULL) )
        {
            /*
             * We've got a RGBA visual, so there is no colormap at all.
             * The other possibility is that we have no current window set.
             */
            return( 0 );
        }

        /*
         * Otherwise return the number of entries in the colormap
         */
        return( fgStructure.Window->Window.VisualInfo->visual->map_entries );

    /*
     * Those calls are somewhat similiar, as they use XGetWindowAttributes() function
     */
    case GLUT_WINDOW_X:
    case GLUT_WINDOW_Y:
    case GLUT_WINDOW_WIDTH:
    case GLUT_WINDOW_HEIGHT:
    case GLUT_WINDOW_BORDER_WIDTH :
    case GLUT_WINDOW_HEADER_HEIGHT :
    {
        XWindowAttributes winAttributes;

        /*
         * Return zero if there is no current window set
         */
        if( fgStructure.Window == NULL )
            return( 0 );

        /*
         * Grab the current window's attributes now
         */
        XGetWindowAttributes(
            fgDisplay.Display,
            fgStructure.Window->Window.Handle,
            &winAttributes
        );

        /*
         * See which window attribute to return
         */
        switch ( eWhat )
        {
        case GLUT_WINDOW_X:                return winAttributes.x ;
        case GLUT_WINDOW_Y:                return winAttributes.y ;
        case GLUT_WINDOW_WIDTH:            return winAttributes.width ;
        case GLUT_WINDOW_HEIGHT:           return winAttributes.height ;
        case GLUT_WINDOW_BORDER_WIDTH :    return winAttributes.border_width ;
        case GLUT_WINDOW_HEADER_HEIGHT :   return winAttributes.border_width * 3 ;  /* a kludge for now */
        }
    }

    /*
     * I do not know yet if there will be a fgChooseVisual() function for Win32
     */
    case GLUT_DISPLAY_MODE_POSSIBLE:
        /*
         * Check if the current display mode is possible
         */
        return( fgChooseVisual() == NULL ? 0 : 1 );

    /*
     * This is system-dependant
     */
    case GLUT_WINDOW_FORMAT_ID:
        /*
         * Return the visual ID, if there is a current window naturally:
         */
        if( fgStructure.Window == NULL )
            return( 0 );

        return( fgStructure.Window->Window.VisualInfo->visualid );

#elif TARGET_HOST_WIN32

    /*
     * Handle the OpenGL inquiries
     */
    case GLUT_WINDOW_RGBA:
      glGetBooleanv ( GL_RGBA_MODE, &boolValue ) ;         /* True if color buffers store RGBA */
      returnValue = boolValue ? 1 : 0 ;
      return ( returnValue ) ;
    case GLUT_WINDOW_DOUBLEBUFFER:
      glGetBooleanv ( GL_DOUBLEBUFFER, &boolValue ) ;      /* True if front and back buffers exist */
      returnValue = boolValue ? 1 : 0 ;
      return ( returnValue ) ;
    case GLUT_WINDOW_STEREO:
      glGetBooleanv ( GL_STEREO, &boolValue ) ;            /* True if left and right buffers exist */
      returnValue = boolValue ? 1 : 0 ;
      return ( returnValue ) ;

    case GLUT_WINDOW_RED_SIZE:
      glGetIntegerv ( GL_RED_BITS, &returnValue ) ;          /* Number of bits per red component in color buffers */
      return ( returnValue ) ;
    case GLUT_WINDOW_GREEN_SIZE:
      glGetIntegerv ( GL_GREEN_BITS, &returnValue ) ;        /* Number of bits per green component in color buffers */
      return ( returnValue ) ;
    case GLUT_WINDOW_BLUE_SIZE:
      glGetIntegerv ( GL_BLUE_BITS, &returnValue ) ;         /* Number of bits per blue component in color buffers */
      return ( returnValue ) ;
    case GLUT_WINDOW_ALPHA_SIZE:
      glGetIntegerv ( GL_ALPHA_BITS, &returnValue ) ;        /* Number of bits per alpha component in color buffers */
      return ( returnValue ) ;
    case GLUT_WINDOW_ACCUM_RED_SIZE:
      glGetIntegerv ( GL_ACCUM_RED_BITS, &returnValue ) ;    /* Number of bits per red component in the accumulation buffer */
      return ( returnValue ) ;
    case GLUT_WINDOW_ACCUM_GREEN_SIZE:
      glGetIntegerv ( GL_ACCUM_GREEN_BITS, &returnValue ) ;  /* Number of bits per green component in the accumulation buffer */
      return ( returnValue ) ;
    case GLUT_WINDOW_ACCUM_BLUE_SIZE:
      glGetIntegerv ( GL_ACCUM_BLUE_BITS, &returnValue ) ;   /* Number of bits per blue component in the accumulation buffer */
      return ( returnValue ) ;
    case GLUT_WINDOW_ACCUM_ALPHA_SIZE:
      glGetIntegerv ( GL_ACCUM_ALPHA_BITS, &returnValue ) ;  /* Number of bits per alpha component in the accumulation buffer */
      return ( returnValue ) ;
    case GLUT_WINDOW_DEPTH_SIZE:
      glGetIntegerv ( GL_DEPTH_BITS, &returnValue ) ;        /* Number of depth-buffer bitplanes */
      return ( returnValue ) ;

    case GLUT_WINDOW_BUFFER_SIZE:
      returnValue = 1 ;                                      /* ????? */
      return ( returnValue ) ;
    case GLUT_WINDOW_STENCIL_SIZE:
      returnValue = 0 ;                                      /* ????? */
      return ( returnValue ) ;

    /*
     * Window position and size
     */
    case GLUT_WINDOW_X:
    case GLUT_WINDOW_Y:
    case GLUT_WINDOW_WIDTH:
    case GLUT_WINDOW_HEIGHT:
    {
        /*
         *  There is considerable confusion about the "right thing to do" concerning window
         * size and position.  GLUT itself is not consistent between Windows and Linux; since
         * platform independence is a virtue for "freeglut", we decided to break with GLUT's
         * behaviour.
         *  Under Linux, it is apparently not possible to get the window border sizes in order
         * to subtract them off the window's initial position until some time after the window
         * has been created.  Therefore we decided on the following behaviour, both under
         * Windows and under Linux:
         *  - When you create a window with position (x,y) and size (w,h), the upper left hand
         *    corner of the outside of the window is at (x,y) and the size of the drawable area
         *    is (w,h).
         *  - When you query the size and position of the window--as is happening here for
         *    Windows--"freeglut" will return the size of the drawable area--the (w,h) that you
         *    specified when you created the window--and the coordinates of the upper left hand
         *    corner of the drawable area--which is NOT the (x,y) you specified.
         */

        RECT winRect;

        /*
         * Check if there is a window to be queried for dimensions:
         */
        freeglut_return_val_if_fail( fgStructure.Window != NULL, 0 );

        /*
         * We need to call GetWindowRect() first...
         *  (this returns the pixel coordinates of the outside of the window)
         */
        GetWindowRect( fgStructure.Window->Window.Handle, &winRect );

        /*
         * ...then we've got to correct the results we've just received...
         */
        if ( fgStructure.Window->Parent == NULL )
        {
          winRect.left   += GetSystemMetrics( SM_CXSIZEFRAME );
          winRect.right  -= GetSystemMetrics( SM_CXSIZEFRAME );
          winRect.top    += GetSystemMetrics( SM_CYSIZEFRAME ) + GetSystemMetrics( SM_CYCAPTION );
          winRect.bottom -= GetSystemMetrics( SM_CYSIZEFRAME );
        }

        /*
         * ...and finally return the caller the desired value:
         */
        switch( eWhat )
        {
        case GLUT_WINDOW_X:      return( winRect.left                 );
        case GLUT_WINDOW_Y:      return( winRect.top                  );
        case GLUT_WINDOW_WIDTH:  return( winRect.right - winRect.left );
        case GLUT_WINDOW_HEIGHT: return( winRect.bottom - winRect.top );
        }
    }
    break;

    case GLUT_WINDOW_BORDER_WIDTH :
        return ( GetSystemMetrics( SM_CXSIZEFRAME ) ) ;

    case GLUT_WINDOW_HEADER_HEIGHT :
        return ( GetSystemMetrics( SM_CYCAPTION ) ) ;

    case GLUT_DISPLAY_MODE_POSSIBLE:
        /*
         * Check if the current display mode is possible
         */
        return( fgSetupPixelFormat( fgStructure.Window, TRUE, PFD_MAIN_PLANE ) );

    case GLUT_WINDOW_FORMAT_ID:
        /*
         * Return the pixel format of the current window
         */
        if( fgStructure.Window != NULL )
            return( GetPixelFormat( fgStructure.Window->Window.Device ) );

        /*
         * If the current window does not exist, fail:
         */
        return( 0 );

#endif

    /*
     * The window structure queries
     */
    case GLUT_WINDOW_PARENT:
        /*
         * Return the ID number of current window's parent, if any
         */
        if( fgStructure.Window         == NULL ) return( 0 );
        if( fgStructure.Window->Parent == NULL ) return( 0 );

        return( fgStructure.Window->Parent->ID );

    case GLUT_WINDOW_NUM_CHILDREN:
        /*
         * Return the number of children attached to the current window
         */
        if( fgStructure.Window == NULL )
            return( 0 );

        return( fgListLength( &fgStructure.Window->Children ) );

    case GLUT_WINDOW_CURSOR:
        /*
         * Return the currently selected window cursor
         */
        if( fgStructure.Window == NULL )
            return( 0 );

        return( fgStructure.Window->State.Cursor );

    case GLUT_MENU_NUM_ITEMS:
        /*
         * Return the number of menu entries in the current menu
         */
        if( fgStructure.Menu == NULL )
            return( 0 );

        return( fgListLength( &fgStructure.Menu->Entries ) );

    case GLUT_ACTION_ON_WINDOW_CLOSE:
	return fgState.ActionOnWindowClose;

    case GLUT_VERSION:
	return VERSION_MAJOR * 10000 + VERSION_MINOR * 100 + VERSION_PATCH;

    default:
        /*
         * Just have it reported, so that we can see what needs to be implemented
         */
        fgWarning( "glutGet(): missing enum handle %i\n", eWhat );
        break;
    }

    /*
     * If nothing happens, then we are in deep trouble...
     */
    return( -1 );
}

/*
 * Returns various device information.
 */
int FGAPIENTRY glutDeviceGet( GLenum eWhat )
{
    freeglut_assert_ready;

    /*
     * See why are we bothered...
     *
     * WARNING: we are mostly lying in this function.
     */
    switch( eWhat )
    {
    case GLUT_HAS_KEYBOARD:
        /*
         * We always have a keyboard present on PC machines...
         */
        return( TRUE );

#if TARGET_HOST_UNIX_X11

    case GLUT_HAS_MOUSE:
        /*
         * Hey, my Atari 65XE hasn't had a mouse!
         */
        return( TRUE );

    case GLUT_NUM_MOUSE_BUTTONS:
        /*
         * Return the number of mouse buttons available. This is a big guess.
         */
        return( 3 );

#elif TARGET_HOST_WIN32

    case GLUT_HAS_MOUSE:
        /*
         * The Windows can be booted without a mouse. 
         * It would be nice to have this reported.
         */
        return( GetSystemMetrics( SM_MOUSEPRESENT ) );

    case GLUT_NUM_MOUSE_BUTTONS:
        /*
         * We are much more fortunate under Win32 about this...
         */
        return( GetSystemMetrics( SM_CMOUSEBUTTONS ) );

#endif

    case GLUT_JOYSTICK_POLL_RATE:
    case GLUT_HAS_JOYSTICK:
    case GLUT_JOYSTICK_BUTTONS:
    case GLUT_JOYSTICK_AXES:
        /*
         * WARNING: THIS IS A BIG LIE!
         */
        return( 0 );

    case GLUT_HAS_SPACEBALL:
    case GLUT_HAS_DIAL_AND_BUTTON_BOX:
    case GLUT_HAS_TABLET:
        /*
         * Sounds cool. And unuseful.
         */
        return( FALSE );

    case GLUT_NUM_SPACEBALL_BUTTONS:
    case GLUT_NUM_BUTTON_BOX_BUTTONS:
    case GLUT_NUM_DIALS:
    case GLUT_NUM_TABLET_BUTTONS:
        /*
         * Zero is not the answer. Zero is the question. Continuum is the answer.
         */
        return( 0 );

    case GLUT_DEVICE_IGNORE_KEY_REPEAT:
        /*
         * Return what we think about the key auto repeat settings
         */
        return( fgState.IgnoreKeyRepeat );

    case GLUT_DEVICE_KEY_REPEAT:
        /*
         * WARNING: THIS IS A BIG LIE!
         */
        return( GLUT_KEY_REPEAT_DEFAULT );

    default:
        /*
         * Complain.
         */
        fgWarning( "glutDeviceGet(): missing enum handle %i\n", eWhat );
        break;
    }

    /*
     * And now -- the failure.
     */
    return( -1 );
}

/*
 * This should return the current state of ALT, SHIFT and CTRL keys.
 */
int FGAPIENTRY glutGetModifiers( void )
{
    /*
     * Fail if there is no current window or called outside an input callback
     */
    if( fgStructure.Window == NULL )
        return( 0 );

    if( fgStructure.Window->State.Modifiers == 0xffffffff )
    {
        fgWarning( "glutGetModifiers() called outside an input callback" );
        return( 0 );
    }

    /*
     * Return the current modifiers state otherwise
     */
    return( fgStructure.Window->State.Modifiers );
}

/*
 * Return the state of the GLUT API overlay subsystem. A misery ;-)
 */
int FGAPIENTRY glutLayerGet( GLenum eWhat )
{
    freeglut_assert_ready;

    /*
     * This is easy as layers are not implemented ;-)
     */
    switch( eWhat )
    {

#if TARGET_HOST_UNIX_X11

    case GLUT_OVERLAY_POSSIBLE:
        /*
         * Nope, overlays are not possible.
         */
        return( FALSE );

    case GLUT_LAYER_IN_USE:
        /*
         * The normal plane is always in use
         */
        return( GLUT_NORMAL );

    case GLUT_HAS_OVERLAY:
        /*
         * No window is allowed to have an overlay
         */
        return( FALSE );

    case GLUT_TRANSPARENT_INDEX:
        /*
         * Return just anything, which is always defined as zero
         */
        return( 0 );

    case GLUT_NORMAL_DAMAGED:
        /*
         * Actually I do not know. Maybe.
         */
        return( FALSE );

    case GLUT_OVERLAY_DAMAGED:
        /*
         * Return minus one to mark that no layer is in use
         */
        return( -1 );

#elif TARGET_HOST_WIN32

    case GLUT_OVERLAY_POSSIBLE:
        /*
         * Check if an overlay display mode is possible
         */
        return( fgSetupPixelFormat( fgStructure.Window, TRUE, PFD_OVERLAY_PLANE ) );

    case GLUT_LAYER_IN_USE:
        /*
         * The normal plane is always in use
         */
        return( GLUT_NORMAL );

    case GLUT_HAS_OVERLAY:
        /*
         * No window is allowed to have an overlay
         */
        return( FALSE );

    case GLUT_TRANSPARENT_INDEX:
        /*
         * Return just anything, which is always defined as zero
         */
        return( 0 );

    case GLUT_NORMAL_DAMAGED:
        /*
         * Actually I do not know. Maybe.
         */
        return( FALSE );

    case GLUT_OVERLAY_DAMAGED:
        /*
         * Return minus one to mark that no layer is in use
         */
        return( -1 );
#endif

    default:
        /*
         * Complain to the user about the obvious bug
         */
        fgWarning( "glutLayerGet(): missing enum handle %i\n", eWhat );
        break;
    }

    /*
     * And fail. That's good. Programs do love failing.
     */
    return( -1 );
}

/*** END OF FILE ***/
