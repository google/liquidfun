/*
 * freeglut_window.c
 *
 * Window management methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
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
#include "fg_gl2.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  fgSetupPixelFormat      -- ignores the display mode settings
 *  fgOpenWindow()          -- check the Win32 version, -iconic handling!
 *  fgCloseWindow()         -- check the Win32 version
 *  glutCreateWindow()      -- Check when default position and size is {-1,-1}
 *  glutCreateSubWindow()   -- Check when default position and size is {-1,-1}
 *  glutDestroyWindow()     -- check the Win32 version
 *  glutSetWindow()         -- check the Win32 version
 *  glutSetWindowTitle()    -- check the Win32 version
 *  glutSetIconTitle()      -- check the Win32 version
 *  glutShowWindow()        -- check the Win32 version
 *  glutHideWindow()        -- check the Win32 version
 *  glutIconifyWindow()     -- check the Win32 version
 *  glutPushWindow()        -- check the Win32 version
 *  glutPopWindow()         -- check the Win32 version
 */


extern void fgPlatformSetWindow ( SFG_Window *window );
extern void fgPlatformOpenWindow( SFG_Window* window, const char* title,
                                  GLboolean positionUse, int x, int y,
                                  GLboolean sizeUse, int w, int h,
                                  GLboolean gameMode, GLboolean isSubWindow );
extern void fgPlatformCloseWindow( SFG_Window* window );
extern void fgPlatformGlutSetWindowTitle( const char* title );
extern void fgPlatformGlutSetIconTitle( const char* title );


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

int fghIsLegacyContextRequested( void )
{
    return fgState.MajorVersion < 2 || (fgState.MajorVersion == 2 && fgState.MinorVersion <= 1);
}

int fghNumberOfAuxBuffersRequested( void )
{
  if ( fgState.DisplayMode & GLUT_AUX4 ) {
    return 4;
  }
  if ( fgState.DisplayMode & GLUT_AUX3 ) {
    return 3;
  }
  if ( fgState.DisplayMode & GLUT_AUX2 ) {
    return 2;
  }
  if ( fgState.DisplayMode & GLUT_AUX1 ) { /* NOTE: Same as GLUT_AUX! */
    return fgState.AuxiliaryBufferNumber;
  }
  return 0;
}

int fghMapBit( int mask, int from, int to )
{
  return ( mask & from ) ? to : 0;

}

void fghContextCreationError( void )
{
    fgError( "Unable to create OpenGL %d.%d context (flags %x, profile %x)",
             fgState.MajorVersion, fgState.MinorVersion, fgState.ContextFlags,
             fgState.ContextProfile );
}


/* -- SYSTEM-DEPENDENT PRIVATE FUNCTIONS ------------------------------------ */

/*
 * Sets the OpenGL context and the fgStructure "Current Window" pointer to
 * the window structure passed in.
 */
void fgSetWindow ( SFG_Window *window )
{
	fgPlatformSetWindow ( window );

    fgStructure.CurrentWindow = window;
}

/*
 * Opens a window. Requires a SFG_Window object created and attached
 * to the freeglut structure. OpenGL context is created here.
 */
void fgOpenWindow( SFG_Window* window, const char* title,
                   GLboolean positionUse, int x, int y,
                   GLboolean sizeUse, int w, int h,
                   GLboolean gameMode, GLboolean isSubWindow )
{
    fgPlatformOpenWindow( window, title,
                          positionUse, x, y,
                          sizeUse, w, h,
                          gameMode, isSubWindow );

    fgSetWindow( window );

    window->Window.DoubleBuffered =
        ( fgState.DisplayMode & GLUT_DOUBLE ) ? 1 : 0;

#ifndef EGL_VERSION_1_0  /* No glDrawBuffer/glReadBuffer in GLES */
    if ( ! window->Window.DoubleBuffered )
    {
        glDrawBuffer ( GL_FRONT );
        glReadBuffer ( GL_FRONT );
    }
#endif
    window->Window.attribute_v_coord = -1;
    window->Window.attribute_v_normal = -1;
    window->Window.attribute_v_texture = -1;

    fgInitGL2();

    window->State.WorkMask |= GLUT_INIT_WORK;
}

/*
 * Closes a window, destroying the frame and OpenGL context
 */
void fgCloseWindow( SFG_Window* window )
{
    /* if we're in gamemode and we're closing the gamemode window,
     * call glutLeaveGameMode first to make sure the gamemode is
     * properly closed before closing the window
     */
    if (fgStructure.GameModeWindow != NULL && fgStructure.GameModeWindow->ID==window->ID)
        glutLeaveGameMode();

	fgPlatformCloseWindow ( window );
}


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Creates a new top-level freeglut window
 */
int FGAPIENTRY glutCreateWindow( const char* title )
{
    /* XXX GLUT does not exit; it simply calls "glutInit" quietly if the
     * XXX application has not already done so.  The "freeglut" community
     * XXX decided not to go this route (freeglut-developer e-mail from
     * XXX Steve Baker, 12/16/04, 4:22 PM CST, "Re: [Freeglut-developer]
     * XXX Desired 'freeglut' behaviour when there is no current window"
     */
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutCreateWindow" );

    return fgCreateWindow( NULL, title, fgState.Position.Use,
                           fgState.Position.X, fgState.Position.Y,
                           fgState.Size.Use, fgState.Size.X, fgState.Size.Y,
                           GL_FALSE, GL_FALSE )->ID;
}

/*
 * This function creates a sub window.
 */
int FGAPIENTRY glutCreateSubWindow( int parentID, int x, int y, int w, int h )
{
    int ret = 0;
    SFG_Window* window = NULL;
    SFG_Window* parent = NULL;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutCreateSubWindow" );
    parent = fgWindowByID( parentID );
    freeglut_return_val_if_fail( parent != NULL, 0 );
    if ( x < 0 )
    {
        x = parent->State.Width + x ;
        if ( w >= 0 ) x -= w ;
    }

    if ( w < 0 ) w = parent->State.Width - x + w ;
    if ( w < 0 )
    {
        x += w ;
        w = -w ;
    }

    if ( y < 0 )
    {
        y = parent->State.Height + y ;
        if ( h >= 0 ) y -= h ;
    }

    if ( h < 0 ) h = parent->State.Height - y + h ;
    if ( h < 0 )
    {
        y += h ;
        h = -h ;
    }

    window = fgCreateWindow( parent, "", GL_TRUE, x, y, GL_TRUE, w, h, GL_FALSE, GL_FALSE );
    ret = window->ID;

    return ret;
}

/*
 * Destroys a window and all of its subwindows
 */
void FGAPIENTRY glutDestroyWindow( int windowID )
{
    SFG_Window* window;
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutDestroyWindow" );
    window = fgWindowByID( windowID );
    freeglut_return_if_fail( window != NULL );
    {
        fgExecutionState ExecState = fgState.ExecState;
        fgAddToWindowDestroyList( window );
        fgState.ExecState = ExecState;
    }
}

/*
 * This function selects the specified window as the current window
 */
void FGAPIENTRY glutSetWindow( int ID )
{
    SFG_Window* window = NULL;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSetWindow" );
    if( fgStructure.CurrentWindow != NULL )
        if( fgStructure.CurrentWindow->ID == ID )
            return;

    window = fgWindowByID( ID );
    if( window == NULL )
    {
        fgWarning( "glutSetWindow(): window ID %d not found!", ID );
        return;
    }

    fgSetWindow( window );
}

/*
 * This function returns the ID number of the current window, 0 if none exists
 */
int FGAPIENTRY glutGetWindow( void )
{
    SFG_Window *win = fgStructure.CurrentWindow;
    /*
     * Since GLUT did not throw an error if this function was called without a prior call to
     * "glutInit", this function shouldn't do so here.  Instead let us return a zero.
     * See Feature Request "[ 1307049 ] glutInit check".
     */
    if ( ! fgState.Initialised )
        return 0;

    while ( win && win->IsMenu )
        win = win->Parent;
    return win ? win->ID : 0;
}

/*
 * This function makes the current window visible
 */
void FGAPIENTRY glutShowWindow( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutShowWindow" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutShowWindow" );

    fgStructure.CurrentWindow->State.WorkMask |= GLUT_VISIBILITY_WORK;
    fgStructure.CurrentWindow->State.DesiredVisibility = DesireNormalState;

    fgStructure.CurrentWindow->State.WorkMask |= GLUT_DISPLAY_WORK;
}

/*
 * This function hides the current window
 */
void FGAPIENTRY glutHideWindow( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutHideWindow" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutHideWindow" );

    fgStructure.CurrentWindow->State.WorkMask |= GLUT_VISIBILITY_WORK;
    fgStructure.CurrentWindow->State.DesiredVisibility = DesireHiddenState;

    fgStructure.CurrentWindow->State.WorkMask &= ~GLUT_DISPLAY_WORK;
}

/*
 * Iconify the current window (top-level windows only)
 */
void FGAPIENTRY glutIconifyWindow( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutIconifyWindow" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutIconifyWindow" );

    fgStructure.CurrentWindow->State.WorkMask |= GLUT_VISIBILITY_WORK;
    fgStructure.CurrentWindow->State.DesiredVisibility = DesireIconicState;

    fgStructure.CurrentWindow->State.WorkMask &= ~GLUT_DISPLAY_WORK;
}

/*
 * Set the current window's title
 */
void FGAPIENTRY glutSetWindowTitle( const char* title )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSetWindowTitle" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutSetWindowTitle" );
    if( ! fgStructure.CurrentWindow->Parent )
    {
		fgPlatformGlutSetWindowTitle ( title );
    }
}

/*
 * Set the current window's iconified title
 */
void FGAPIENTRY glutSetIconTitle( const char* title )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSetIconTitle" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutSetIconTitle" );

    if( ! fgStructure.CurrentWindow->Parent )
    {
		fgPlatformGlutSetIconTitle ( title );
    }
}

/*
 * Change the current window's size
 */
void FGAPIENTRY glutReshapeWindow( int width, int height )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutReshapeWindow" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutReshapeWindow" );

    if (glutGet(GLUT_FULL_SCREEN))
    {
      /*  Leave full screen state before resizing. */
      glutLeaveFullScreen();
    }

    fgStructure.CurrentWindow->State.WorkMask |= GLUT_SIZE_WORK;
    fgStructure.CurrentWindow->State.DesiredWidth  = width ;
    fgStructure.CurrentWindow->State.DesiredHeight = height;
}

/*
 * Change the current window's position
 */
void FGAPIENTRY glutPositionWindow( int x, int y )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutPositionWindow" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutPositionWindow" );

    if (glutGet(GLUT_FULL_SCREEN))
    {
      /*  Leave full screen state before moving. */
      glutLeaveFullScreen();
    }

    fgStructure.CurrentWindow->State.WorkMask |= GLUT_POSITION_WORK;
    fgStructure.CurrentWindow->State.DesiredXpos = x;
    fgStructure.CurrentWindow->State.DesiredYpos = y;
}

/*
 * Lowers the current window (by Z order change)
 */
void FGAPIENTRY glutPushWindow( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutPushWindow" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutPushWindow" );

    fgStructure.CurrentWindow->State.WorkMask |= GLUT_ZORDER_WORK;
    fgStructure.CurrentWindow->State.DesiredZOrder = -1;
}

/*
 * Raises the current window (by Z order change)
 */
void FGAPIENTRY glutPopWindow( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutPopWindow" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutPopWindow" );

    fgStructure.CurrentWindow->State.WorkMask |= GLUT_ZORDER_WORK;
    fgStructure.CurrentWindow->State.DesiredZOrder = 1;
}

/*
 * Resize the current window so that it fits the whole screen
 */
void FGAPIENTRY glutFullScreen( void )
{
    SFG_Window *win;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutFullScreen" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutFullScreen" );

    win = fgStructure.CurrentWindow;

    if (win->Parent)
    {
        /* Child windows cannot be made fullscreen, consistent with GLUT's behavior
         * Also, what would it mean for a child window to be fullscreen, given that it
         * is confined to its parent?
         */
        fgWarning("glutFullScreen called on a child window, ignoring...");
        return;
    }
    else if (fgStructure.GameModeWindow != NULL && fgStructure.GameModeWindow->ID==win->ID && win->State.IsFullscreen)
    {
        /* Ignore fullscreen call on GameMode window, those are always fullscreen already
         * only exception is when first entering GameMode
         */
        return;
    }

    if (!win->State.IsFullscreen)
	    win->State.WorkMask |= GLUT_FULL_SCREEN_WORK;
}

/*
 * If we are fullscreen, resize the current window back to its original size
 */
void FGAPIENTRY glutLeaveFullScreen( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutFullScreen" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutFullScreen" );

    if (fgStructure.CurrentWindow->State.IsFullscreen)
        fgStructure.CurrentWindow->State.WorkMask |= GLUT_FULL_SCREEN_WORK;
}

/*
 * Toggle the window's full screen state.
 */
void FGAPIENTRY glutFullScreenToggle( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutFullScreenToggle" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutFullScreenToggle" );

    fgStructure.CurrentWindow->State.WorkMask |= GLUT_FULL_SCREEN_WORK;
}

/*
 * A.Donev: Set and retrieve the window's user data
 */
void* FGAPIENTRY glutGetWindowData( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutGetWindowData" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutGetWindowData" );
    return fgStructure.CurrentWindow->UserData;
}

void FGAPIENTRY glutSetWindowData(void* data)
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSetWindowData" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutSetWindowData" );
    fgStructure.CurrentWindow->UserData = data;
}

/*** END OF FILE ***/
