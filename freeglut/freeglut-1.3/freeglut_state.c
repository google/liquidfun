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
#include "../include/GL/freeglut_internal.h"

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

/*
 * Those are definitions introduced to make the glutGet() more easy,
 * however they might introduce errors if someone ports GLX to Win32 :)
 *
 * Btw. this is not that a bad idea (wrapping WGL around GLX)...
 */
#if TARGET_HOST_WIN32
#	define GLX_RGBA                 0x01
#	define GLX_DOUBLEBUFFER         0x02
#	define GLX_BUFFER_SIZE          0x03
#	define GLX_STENCIL_SIZE         0x04
#	define GLX_DEPTH_SIZE           0x05
#	define GLX_RED_SIZE             0x06
#	define GLX_GREEN_SIZE           0x07
#	define GLX_BLUE_SIZE            0x08
#	define GLX_ALPHA_SIZE           0x09
#	define GLX_ACCUM_RED_SIZE       0x0A
#	define GLX_ACCUM_GREEN_SIZE     0x0B
#	define GLX_ACCUM_BLUE_SIZE      0x0C
#	define GLX_ACCUM_ALPHA_SIZE     0x0D
#	define GLX_STEREO               0x0E
#endif


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

/*
 * Queries the GL context about some attributes
 */
static gint fghGetConfig( gint attribute )
{
    gint returnValue;

    /*
     * Return nothing if there is no current window set
     */
    if( fgStructure.Window == NULL )
        return( 0 );

#if TARGET_HOST_UNIX_X11
    /*
     * glXGetConfig should work fine
     */
    glXGetConfig( fgDisplay.Display, fgStructure.Window->Window.VisualInfo, attribute, &returnValue );

#elif TARGET_HOST_WIN32
    /*
     * This is going to be a bit harder than the X11 version...
     */
#   pragma message( "freeglut_state.c::fghGetConfig() says hello world my name is not implemented!" )
    switch( attribute )
    {
    case GLX_RGBA:
    case GLX_DOUBLEBUFFER:
    case GLX_BUFFER_SIZE:
    case GLX_RED_SIZE:
    case GLX_GREEN_SIZE:
    case GLX_BLUE_SIZE:
    case GLX_DEPTH_SIZE:
        return( 1 );

    case GLX_STENCIL_SIZE:
    case GLX_ALPHA_SIZE:
    case GLX_ACCUM_RED_SIZE:
    case GLX_ACCUM_GREEN_SIZE:
    case GLX_ACCUM_BLUE_SIZE:
    case GLX_ACCUM_ALPHA_SIZE:
    case GLX_STEREO:
    default:
        /*
         * Well, this looks like not implemented to me :)
         */
        return( 0 );
    }

#endif

    /*
     * Have the query results returned
     */
    return( returnValue );
}

/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * General settings query method
 */
int FGAPIENTRY glutGet( GLenum eWhat )
{
    gint returnValue;

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
        return( (int) (g_timer_elapsed( fgState.Timer, NULL ) * 1000.0) );

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

    /*
     * The rest of GLX queries is general enough to use a macro to check them
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

#if TARGET_HOST_UNIX_X11
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
    {
        XWindowAttributes winAttributes;
        Window another, window;
        gint x, y;

        /*
         * Return zero if there is no current window set
         */
        if( fgStructure.Window == NULL )
            return( 0 );

        /*
         * So, grab the current window's position
         */
        window = fgStructure.Window->Window.Handle;

        /*
         * Grab the current window's attributes now
         */
        XGetWindowAttributes(
            fgDisplay.Display,
            window,
            &winAttributes
        );

        /*
         * Correct the results for the parental relation and border size
         */
        XTranslateCoordinates(
            fgDisplay.Display,
            window,
            winAttributes.root,
            -winAttributes.border_width,
            -winAttributes.border_width,
            &x, &y,
            &another
        );

        /*
         * See if we have to return the X or Y coordinate
         */
        return( eWhat == GLUT_WINDOW_X ? x : y );
    }

    case GLUT_WINDOW_WIDTH:
    case GLUT_WINDOW_HEIGHT:
    {
        XWindowAttributes winAttributes;

        /*
         * Return zero if there is no current window set
         */
        if( fgStructure.Window == NULL )
            return( 0 );

        /*
         * Checking for window's size is much easier:
         */
        XGetWindowAttributes(
            fgDisplay.Display,
            fgStructure.Window->Window.Handle,
            &winAttributes
        );

        /*
         * See if to return the window's width or height
         */
        return( eWhat == GLUT_WINDOW_WIDTH ? winAttributes.width : winAttributes.height );
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

    case GLUT_WINDOW_X:
    case GLUT_WINDOW_Y:
    case GLUT_WINDOW_WIDTH:
    case GLUT_WINDOW_HEIGHT:
    {
        RECT winRect;

        /*
         * Check if there is a window to be queried for dimensions:
         */
        freeglut_return_val_if_fail( fgStructure.Window != NULL, 0 );

        /*
         * We need to call GetWindowRect() first...
         */
        GetWindowRect( fgStructure.Window->Window.Handle, &winRect );

        /*
         * ...then we've got to correct the results we've just received...
         */
        winRect.left   += GetSystemMetrics( SM_CXSIZEFRAME ) - 1;
        winRect.right  -= GetSystemMetrics( SM_CXSIZEFRAME ) - 1;
        winRect.top    += GetSystemMetrics( SM_CYSIZEFRAME ) - 1 + GetSystemMetrics( SM_CYCAPTION );
        winRect.bottom -= GetSystemMetrics( SM_CYSIZEFRAME ) + 1;

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

    case GLUT_DISPLAY_MODE_POSSIBLE:
        /*
         * Check if the current display mode is possible
         */
        return( fgSetupPixelFormat( fgStructure.Window, TRUE ) );

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

        return( g_list_length( fgStructure.Window->Children ) );

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

        return( g_list_length( fgStructure.Menu->Entries ) );

    default:
        /*
         * Just have it reported, so that we can see what needs to be implemented
         */
        g_warning( "glutGet(): missing enum handle %i\n", eWhat );
        break;
    }

    /*
     * If nothing happens, then we are in deep trouble...
     */
    g_assert_not_reached();
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
        g_warning( "glutDeviceGet(): missing enum handle %i\n", eWhat );
        break;
    }

    /*
     * And now -- the failure.
     */
    g_assert_not_reached();
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
        g_warning( "glutGetModifiers() called outside an input callback" );
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

    default:
        /*
         * Complain to the user about the obvious bug
         */
        g_warning( "glutLayerGet(): missing enum handle %i\n", eWhat );
        break;
    }

    /*
     * And fail. That's good. Programs do love failing.
     */
    g_assert_not_reached();
}

/*** END OF FILE ***/
