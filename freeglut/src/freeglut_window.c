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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define  G_LOG_DOMAIN  "freeglut-window"

#include "../include/GL/freeglut.h"
#include "freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  fgChooseVisual()        -- OK, but what about glutInitDisplayString()?
 *  fgSetupPixelFormat      -- ignores the display mode settings
 *  fgOpenWindow()          -- check the Win32 version, -iconic handling!
 *  fgCloseWindow()         -- check the Win32 version
 *  glutCreateWindow()      -- see what happens when default position and size is {-1,-1}
 *  glutCreateSubWindow()   -- see what happens when default position and size is {-1,-1}
 *  glutDestroyWindow()     -- check the Win32 version
 *  glutSetWindow()         -- check the Win32 version
 *  glutGetWindow()         -- OK
 *  glutSetWindowTitle()    -- check the Win32 version
 *  glutSetIconTitle()      -- check the Win32 version
 *  glutShowWindow()        -- check the Win32 version
 *  glutHideWindow()        -- check the Win32 version
 *  glutIconifyWindow()     -- check the Win32 version
 *  glutReshapeWindow()     -- check the Win32 version
 *  glutPositionWindow()    -- check the Win32 version
 *  glutPushWindow()        -- check the Win32 version
 *  glutPopWindow()         -- check the Win32 version
 */

/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

/*
 * Chooses a visual basing on the current display mode settings
 */
#if TARGET_HOST_UNIX_X11

XVisualInfo* fgChooseVisual( void )
{
    int bufferSize[] = { 16, 12, 8, 4, 2, 1 };
    GLboolean wantIndexedMode = FALSE;
    int attributes[ 32 ];
    int where = 0;

    /*
     * First we have to process the display mode settings...
     */
#   define ATTRIB(a) attributes[where++]=a;

    /*
     * Decide if we want a true or indexed color visual:
     */
    if( !(fgState.DisplayMode & GLUT_INDEX) )
    {
        /*
         * We are sure that there will be R, B and B components requested:
         */
        ATTRIB( GLX_RGBA       );
        ATTRIB( GLX_RED_SIZE   ); ATTRIB( 1 );
        ATTRIB( GLX_GREEN_SIZE ); ATTRIB( 1 );
        ATTRIB( GLX_BLUE_SIZE  ); ATTRIB( 1 );

        /*
         * Check if the A component is required, too:
         */
        if( fgState.DisplayMode & GLUT_ALPHA )
        {
            ATTRIB( GLX_ALPHA_SIZE ); ATTRIB( 1 );
        }
    }
    else
    {
        /*
         * We've got an indexed color request
         */
        ATTRIB( GLX_BUFFER_SIZE ); ATTRIB( 8 );

        /*
         * Set the 'I want indexed mode' switch
         */
        wantIndexedMode = TRUE;
    }

    /*
     * We can have double or single buffered contexts created
     */
    if( fgState.DisplayMode & GLUT_DOUBLE )
    {
        ATTRIB( GLX_DOUBLEBUFFER );
    }

    /*
     * Stereoscopy seems a nice thing to have
     */
    if( fgState.DisplayMode & GLUT_STEREO )
    {
        ATTRIB( GLX_STEREO );
    }

    /*
     * Depth buffer is almost always required
     */
    if( fgState.DisplayMode & GLUT_DEPTH )
    {
        ATTRIB( GLX_DEPTH_SIZE ); ATTRIB( 1 );
    }

    /*
     * Stenciling support
     */
    if( fgState.DisplayMode & GLUT_STENCIL )
    {
        ATTRIB( GLX_STENCIL_SIZE ); ATTRIB( 1 );
    }

    /*
     * And finally the accumulation buffers
     */
    if( fgState.DisplayMode & GLUT_ACCUM )
    {
        ATTRIB( GLX_ACCUM_RED_SIZE );   ATTRIB( 1 );
        ATTRIB( GLX_ACCUM_GREEN_SIZE ); ATTRIB( 1 );
        ATTRIB( GLX_ACCUM_BLUE_SIZE );  ATTRIB( 1 );

        /*
         * Check if the A component is required, too:
         */
        if( fgState.DisplayMode & GLUT_ALPHA )
        {
            ATTRIB( GLX_ACCUM_ALPHA_SIZE ); ATTRIB( 1 );
        }
    }

    /*
     * Push a null at the end of the list
     */
    ATTRIB( None );

    /*
     * OKi now, we've got two cases -- RGB(A) and index mode visuals
     */
    if( wantIndexedMode == FALSE )
    {
        /*
         * The easier one. And more common, too.
         */
        return( glXChooseVisual( fgDisplay.Display, fgDisplay.Screen, attributes ) );
    }
    else
    {
        XVisualInfo* visualInfo;
        int i;

        /*
         * In indexed mode, we need to check how many bits of depth can we achieve
         */
        for( i=0; i<6; i++ )
        {

            /*
             * The GLX_BUFFER_SIZE value comes always first, so:
             */
            attributes[ 1 ] = bufferSize[ i ];

            /*
             * Check if such visual is possible
             */
            visualInfo = glXChooseVisual( fgDisplay.Display, fgDisplay.Screen, attributes );

            /*
             * The buffer size are sorted in descendant order, so choose the first:
             */
            if( visualInfo != NULL )
                return( visualInfo );
        }

        /*
         * If we are still here, it means that the visual info was not found
         */
        return( NULL );
    }
}
#endif

/*
 * Setup the pixel format for a Win32 window
 */
#if TARGET_HOST_WIN32
GLboolean fgSetupPixelFormat( SFG_Window* window, GLboolean checkOnly, unsigned char layer_type )
{
	PIXELFORMATDESCRIPTOR* ppfd, pfd;
	int flags, pixelformat;

	/*
	 * Check if the window seems valid
	 */
	freeglut_return_val_if_fail( window != NULL, 0 );

	/*
	 * The pixel format should allow us to draw to the window using OpenGL
	 */
	flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	
	/*
	 * It might be the case for us to use double buffering
	 */
  if( fgState.DisplayMode & GLUT_DOUBLE )
    flags |= PFD_DOUBLEBUFFER;

  /*
   * Specify which pixel format do we opt for...
   */
#	pragma message( "fgSetupPixelFormat(): there is still some work to do here!" )

  pfd.nSize				= sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion			= 1;
  pfd.dwFlags				= flags;
  pfd.iPixelType			= PFD_TYPE_RGBA;
  pfd.cColorBits			= 24;
  pfd.cRedBits			= 0;
  pfd.cRedShift			= 0;
  pfd.cGreenBits			= 0;
  pfd.cGreenShift			= 0;
  pfd.cBlueBits			= 0;
  pfd.cBlueShift			= 0;
  pfd.cAlphaBits			= 0;
  pfd.cAlphaShift			= 0;
  pfd.cAccumBits			= 0;
  pfd.cAccumRedBits		= 0;
  pfd.cAccumGreenBits		= 0;
  pfd.cAccumBlueBits		= 0;
  pfd.cAccumAlphaBits		= 0;
#if 0
  pfd.cDepthBits			= 32;
  pfd.cStencilBits		= 0;
#else
  pfd.cDepthBits			= 24;
  pfd.cStencilBits		= 8;
#endif
  pfd.cAuxBuffers			= 0;
  pfd.iLayerType			= layer_type;
  pfd.bReserved			= 0;
  pfd.dwLayerMask			= 0;
  pfd.dwVisibleMask		= 0;
  pfd.dwDamageMask		= 0;

  /*
   * Fill in the color bits...
   */
  pfd.cColorBits = (BYTE) GetDeviceCaps( window->Window.Device, BITSPIXEL );
  ppfd = &pfd;

	/*
	 * Choose the pixel format that matches our demand
	 */
  pixelformat = ChoosePixelFormat( window->Window.Device, ppfd );
	if( pixelformat == 0 )
		return( FALSE );

	/*
	 * We might have been called to check if the pixel format exists only
	 */
	if( checkOnly )
		return( TRUE );

	/*
	 * Finally, set the window's pixel format
	 */
	return ( SetPixelFormat( window->Window.Device, pixelformat, ppfd ) ) ;
}
#endif

/*
 * Sets the OpenGL context and the fgStructure "Current Window" pointer to the window
 * structure passed in.
 */
void fgSetWindow ( SFG_Window *window )
{
#if TARGET_HOST_UNIX_X11
  if ( window )
  {
    /*
	 * Make the selected window's GLX context the current one
     */
    glXMakeCurrent(
        fgDisplay.Display,
        window->Window.Handle,
        window->Window.Context
    );
  }
#elif TARGET_HOST_WIN32
	/*
	 * Release the previous' context's device context
	 */
	if( fgStructure.Window != NULL )
		ReleaseDC( fgStructure.Window->Window.Handle, fgStructure.Window->Window.Device );

  if ( window )
  {
  	/*
	   * We will care about releasing the device context later
	   */
  	window->Window.Device = GetDC( window->Window.Handle );

  	/*
	   * Set the new current context:
	   */
  	wglMakeCurrent( 
	  	window->Window.Device, 
		  window->Window.Context 
  	);
  }
#endif

    /*
     * Remember that we have changed the current window state
     */
    fgStructure.Window = window;
}


/*
 * Opens a window. Requires a SFG_Window object created and attached
 * to the freeglut structure. OpenGL context is created here.
 */
void fgOpenWindow( SFG_Window* window, const char* title, int x, int y, int w, int h, GLboolean gameMode, int isSubWindow )
{
#if TARGET_HOST_UNIX_X11
    XSetWindowAttributes winAttr;
    XTextProperty textProperty;
    XSizeHints sizeHints;
    XWMHints wmHints;
    unsigned long mask;

    freeglut_assert_ready;

    /*
     * Here we are upon the stage. Have the visual selected.
     */
    if ( fgState.BuildingAMenu )
    {
      /*
       * If there isn't already an OpenGL rendering context for menu windows, make one
       */
      if ( !fgStructure.MenuContext )
      {
        unsigned int current_DisplayMode = fgState.DisplayMode ;
        fgState.DisplayMode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH ;
        window->Window.VisualInfo = fgChooseVisual();
        fgState.DisplayMode = current_DisplayMode ;
      }
      else
        window->Window.VisualInfo = fgChooseVisual();
    }
    else
      window->Window.VisualInfo = fgChooseVisual();

    if ( ! window->Window.VisualInfo )
    {
      /*
       * The "fgChooseVisual" returned a null meaning that the visual context is not available.
       * Try a couple of variations to see if they will work.
       */
      if ( ! ( fgState.DisplayMode & GLUT_DOUBLE ) )
      {
        /*
         * Single buffering--try it doubled
         */
        fgState.DisplayMode |= GLUT_DOUBLE ;
        window->Window.VisualInfo = fgChooseVisual();
	/* OK, we got a double-buffered window, but we only wanted
	 * single-buffered.  Clear the double-buffer flag now.
	 */
	fgState.DisplayMode &= ~GLUT_DOUBLE ;
      }

      /*
       * GLUT also checks for multi-sampling, but I don't see that anywhere else in FREEGLUT
       * so I won't bother with it for the moment.
       */
    }

    assert( window->Window.VisualInfo != NULL );

    /*
     * Have the windows attributes set
     *
     * HINT: the masks should be updated when adding/removing callbacks.
     *       This might speed up message processing. Is that true?
     */
    winAttr.event_mask        = StructureNotifyMask | SubstructureNotifyMask | ExposureMask |
                                ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyRelease |
                                VisibilityChangeMask | EnterWindowMask | LeaveWindowMask |
                                PointerMotionMask | ButtonMotionMask;
    winAttr.background_pixmap = None;
    winAttr.background_pixel  = 0;
    winAttr.border_pixel      = 0;

    /*
     * The color map is required, too
     */
    winAttr.colormap = XCreateColormap(
        fgDisplay.Display, fgDisplay.RootWindow,
        window->Window.VisualInfo->visual, AllocNone
    );

    /*
     * This tells the XCreateWindow() what attributes are we supplying it with
     */
    mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;

    /*
     * If this is a menu window we want the window manager to ignore it.
     */
    if ( fgState.BuildingAMenu )
    {
        winAttr.override_redirect = True;
	mask |= CWOverrideRedirect;
    }

    /*
     * Have the window created now
     */
    window->Window.Handle = XCreateWindow(
        fgDisplay.Display,
        window->Parent == NULL ? fgDisplay.RootWindow : window->Parent->Window.Handle,
        x, y, w, h, 0,
        window->Window.VisualInfo->depth, InputOutput,
        window->Window.VisualInfo->visual, mask,
        &winAttr
    );

    /*
     * The GLX context creation, possibly trying the direct context rendering
     *  or else use the current context if the user has so specified
     */
    if ( fgState.BuildingAMenu )
    {
      /*
       * If there isn't already an OpenGL rendering context for menu windows, make one
       */
      if ( !fgStructure.MenuContext )
      {
        fgStructure.MenuContext = (SFG_MenuContext *)malloc ( sizeof(SFG_MenuContext) ) ;
        fgStructure.MenuContext->VisualInfo = window->Window.VisualInfo ;
        fgStructure.MenuContext->Context = glXCreateContext(
            fgDisplay.Display, fgStructure.MenuContext->VisualInfo,
            NULL, fgState.ForceDirectContext | fgState.TryDirectContext
        );
      }

/*      window->Window.Context = fgStructure.MenuContext->Context ; */
      window->Window.Context = glXCreateContext(
            fgDisplay.Display, window->Window.VisualInfo,
            NULL, fgState.ForceDirectContext | fgState.TryDirectContext
        );
    }
    else if ( fgState.UseCurrentContext == TRUE )
    {
      window->Window.Context = glXGetCurrentContext();

      if ( ! window->Window.Context )
        window->Window.Context = glXCreateContext(
            fgDisplay.Display, window->Window.VisualInfo,
            NULL, fgState.ForceDirectContext | fgState.TryDirectContext
        );
    }
    else
      window->Window.Context = glXCreateContext(
          fgDisplay.Display, window->Window.VisualInfo,
          NULL, fgState.ForceDirectContext | fgState.TryDirectContext
      );

    /*
     * Make sure the context is direct when the user wants it forced
     */
    if( fgState.ForceDirectContext && !glXIsDirect( fgDisplay.Display, window->Window.Context ) )
        fgError( "unable to force direct context rendering for window '%s'", title );

    /*
     * Set the new context as the current one. That's all about the window creation.
     */
    glXMakeCurrent(
        fgDisplay.Display,
        window->Window.Handle,
        window->Window.Context
    );

    /*
     * Assume the new window is visible by default
     */
    window->State.Visible = TRUE;

    /*
     * For the position and size hints -- make sure we are passing valid values
     */
    sizeHints.flags = 0;

    if (fgState.Position.Use == TRUE) sizeHints.flags |= USPosition;
    if (fgState.Size.Use     == TRUE) sizeHints.flags |= USSize;

    /*
     * Fill in the size hints values now (the x, y, width and height
     * settings are obsolote, are there any more WMs that support them?)
     */
    sizeHints.x      = x; sizeHints.y      = y;
    sizeHints.width  = w; sizeHints.height = h;

    /*
     * We can have forced all new windows start in iconified state:
     */
    wmHints.flags = StateHint;
    wmHints.initial_state = (fgState.ForceIconic == FALSE) ? NormalState : IconicState;

    /*
     * Prepare the window and iconified window names...
     */
    XStringListToTextProperty( (char **) &title, 1, &textProperty );

    /*
     * Set the window's properties now
     */
    XSetWMProperties(
        fgDisplay.Display,
        window->Window.Handle,
        &textProperty,
        &textProperty,
        0,
        0,
        &sizeHints,
        &wmHints,
        NULL
    );

    /*
     * Make sure we are informed about the window deletion commands
     */
    XSetWMProtocols( fgDisplay.Display, window->Window.Handle, &fgDisplay.DeleteWindow, 1 );

    /*
     * Finally, have the window mapped to our display
     */
    XMapWindow( fgDisplay.Display, window->Window.Handle );

#elif TARGET_HOST_WIN32

	WNDCLASS wc;
	int flags;
	ATOM atom;

    freeglut_assert_ready;

	/*
	 * Grab the window class we have registered on glutInit():
	 */
	atom = GetClassInfo( fgDisplay.Instance, "FREEGLUT", &wc );
	assert( atom != 0 );

    if( gameMode == FALSE )
    {
      if ( ( !isSubWindow ) && ( ! window->IsMenu ) )
      {
        /*
         * Update the window dimensions, taking account of window decorations.
         * "freeglut" is to create the window with the outside of its border at (x,y)
         * and with dimensions (w,h).
         */
	    	w += (GetSystemMetrics( SM_CXSIZEFRAME ) )*2;
	    	h += (GetSystemMetrics( SM_CYSIZEFRAME ) )*2 + GetSystemMetrics( SM_CYCAPTION );
      }

      /*
	     * Check if the user wants us to use the default position/size
	     */
	    if( fgState.Position.Use == FALSE ) { x = CW_USEDEFAULT; y = CW_USEDEFAULT; }
	    if( fgState.Size    .Use == FALSE ) { w = CW_USEDEFAULT; h = CW_USEDEFAULT; }

	    /*
	     * There's a small difference between creating the top, child and game mode windows
	     */
	    flags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;

      /*
       * If we're a menu, set our flags to include WS_POPUP to remove decorations
       */
      if ( window->IsMenu )
        flags |= WS_POPUP ;
	    else if( window->Parent == NULL )
		    flags |= WS_OVERLAPPEDWINDOW;
	    else
		    flags |= WS_CHILD;
    }
    else
    {
        /*
         * In game mode, the story is a little bit different...
         */
        assert( window->Parent == NULL );

        /*
         * Set the window creation flags appropriately to make the window entirely visible:
         */
        flags = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
    }

    /*
     * Create the window now, passing the freeglut window structure as the parameter
     */
	window->Window.Handle = CreateWindow( 
		"FREEGLUT",
        title,
		flags,
        x, y, w, h,
		(HWND) window->Parent == NULL ? NULL : window->Parent->Window.Handle,
		(HMENU) NULL,
		fgDisplay.Instance,
		(LPVOID) window
	);

	/*
     * Make sure window was created
     */
	assert( window->Window.Handle != NULL );

    /*
     * Show and update the main window. Hide the mouse cursor.
     */
    ShowWindow( window->Window.Handle, fgState.ForceIconic ? SW_SHOWMINIMIZED : SW_SHOW );
    UpdateWindow( window->Window.Handle );
    ShowCursor( TRUE );

#endif

    /*
     * Save the window's single- or double-buffering state
     */
    window->Window.DoubleBuffered = ( fgState.DisplayMode & GLUT_DOUBLE ) ? 1 : 0 ;

    /*
     * If it's not double-buffered, make sure the rendering is done to the front buffer.
     */
    if ( ! window->Window.DoubleBuffered )
    {
      glDrawBuffer ( GL_FRONT ) ;
      glReadBuffer ( GL_FRONT ) ;
    }

    /*
     * Set the newly created window as the current one
     */
    fgSetWindow( window );
}

/*
 * Closes a window, destroying the frame and OpenGL context
 */
void fgCloseWindow( SFG_Window* window )
{
    freeglut_assert_ready;

#if TARGET_HOST_UNIX_X11
    /*
     * As easy as kill bunnies with axes. Destroy the context first:
     */
    glXDestroyContext( fgDisplay.Display, window->Window.Context );

    /*
     * Then have the window killed:
     */
    XDestroyWindow( fgDisplay.Display, window->Window.Handle );

    /*
     * Finally, flush the rests down the stream
     */
    XFlush( fgDisplay.Display );

#elif TARGET_HOST_WIN32
	/*
	 * Send the WM_CLOSE message to the window now
	 */
	SendMessage( 
		window->Window.Handle,
		WM_CLOSE,
		0,
		0
	);

#endif
}


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Creates a new top-level freeglut window
 */
int FGAPIENTRY glutCreateWindow( const char* title )
{
    /*
     * Create a new window and return its unique ID number
     */
    return( fgCreateWindow( NULL, title, fgState.Position.X, fgState.Position.Y,
                            fgState.Size.X, fgState.Size.Y, FALSE )->ID );
}

/*
 * This function creates a sub window.
 */
int FGAPIENTRY glutCreateSubWindow( int parentID, int x, int y, int w, int h )
{
    SFG_Window* window = NULL;
    SFG_Window* parent = NULL;

    freeglut_assert_ready;

    /*
     * Find a parent to the newly created window...
     */
    parent = fgWindowByID( parentID );

    /*
     * Fail if the parent has not been found
     */
    freeglut_return_val_if_fail( parent != NULL, 0 );

    /*
     * Create the new window
     */
    window = fgCreateWindow( parent, "", x, y, w, h, FALSE );

    /*
     * Return the new window's ID
     */
    return( window->ID );
}

/*
 * Destroys a window and all of its subwindows
 */
void FGAPIENTRY glutDestroyWindow( int windowID )
{
  fgExecutionState ExecState = fgState.ExecState ;

  /*
   * Grab the freeglut window pointer from the structure
   */
  SFG_Window* window = fgWindowByID( windowID );
  freeglut_return_if_fail( window != NULL );

  /*
   * There is a function that performs all needed steps
   * defined in freeglut_structure.c. Let's use it:
   */
  fgAddToWindowDestroyList( window, TRUE );

  /*
   * Since the "fgAddToWindowDestroyList" function could easily have set the "ExecState"
   * to stop, let's set it back to what it was.
   */
  fgState.ExecState = ExecState ;
}

/*
 * This function selects the current window
 */
void FGAPIENTRY glutSetWindow( int ID )
{
    SFG_Window* window = NULL;

    /*
     * Make sure we don't get called too early
     */
    freeglut_assert_ready;

    /*
     * Be wise. Be wise. Be wise. Be quick.
     */
    if( fgStructure.Window != NULL )
        if( fgStructure.Window->ID == ID )
            return;

    /*
     * Now we are sure there is sense in looking for the window
     */
    window = fgWindowByID( ID );

    /*
     * In the case of an utter failure...
     */
    if( window == NULL )
    {
        /*
         * ...issue a warning message and keep rolling on
         */
        fgWarning( "glutSetWindow(): window ID %i not found!", ID );
        return;
    }

    fgSetWindow ( window ) ;
}

/*
 * This function returns the ID number of the current window, 0 if none exists
 */
int FGAPIENTRY glutGetWindow( void )
{
    freeglut_assert_ready;

    /*
     * Do we have a current window selected?
     */
    if( fgStructure.Window == NULL )
    {
        /*
         * Nope. Return zero to mark the state.
         */
        return( 0 );
    }

    /*
     * Otherwise, return the ID of the current window
     */
    return( fgStructure.Window->ID );
}

/*
 * This function makes the current window visible
 */
void FGAPIENTRY glutShowWindow( void )
{
    freeglut_assert_ready; freeglut_assert_window;

#if TARGET_HOST_UNIX_X11
    /*
     * Showing the window is done via mapping under X
     */
    XMapWindow( fgDisplay.Display, fgStructure.Window->Window.Handle );
    XFlush( fgDisplay.Display );

#elif TARGET_HOST_WIN32
	/*
	 * Restore the window's originial position and size
	 */
	ShowWindow( fgStructure.Window->Window.Handle, SW_SHOW );

#endif

  /*
   * Since the window is visible, we need to redisplay it ...
   */
  fgStructure.Window->State.Redisplay = TRUE;

}

/*
 * This function hides the current window
 */
void FGAPIENTRY glutHideWindow( void )
{
    freeglut_assert_ready; freeglut_assert_window;

#if TARGET_HOST_UNIX_X11
    /*
     * The way we hide a window depends on if we're dealing
     * with a top-level or children one...
     */
    if( fgStructure.Window->Parent == NULL )
    {
        /*
         * This is a top-level window
         */
        XWithdrawWindow( fgDisplay.Display, fgStructure.Window->Window.Handle, fgDisplay.Screen );
    }
    else
    {
        /*
         * Nope, it's a child window
         */
        XUnmapWindow( fgDisplay.Display, fgStructure.Window->Window.Handle );
    }

    /*
     * Flush the X state now
     */
    XFlush( fgDisplay.Display );

#elif TARGET_HOST_WIN32
	/*
	 * Hide the window
	 */
	ShowWindow( fgStructure.Window->Window.Handle, SW_HIDE );

#endif

  /*
   * Since the window is hidden, we don't need to redisplay it ...
   */
  fgStructure.Window->State.Redisplay = FALSE;
}

/*
 * Iconify the current window (top-level windows only)
 */
void FGAPIENTRY glutIconifyWindow( void )
{
    freeglut_assert_ready; freeglut_assert_window;

#if TARGET_HOST_UNIX_X11
    /*
     * Iconify the window and flush the X state
     */
    XIconifyWindow( fgDisplay.Display, fgStructure.Window->Window.Handle, fgDisplay.Screen );
    XFlush( fgDisplay.Display );

#elif TARGET_HOST_WIN32
	/*
	 * Minimize the current window (this should be the same as X window iconifying)
	 */
	ShowWindow( fgStructure.Window->Window.Handle, SW_MINIMIZE );

#endif

  /*
   * Since the window is just an icon, we don't need to redisplay it ...
   */
  fgStructure.Window->State.Redisplay = FALSE;

}

/*
 * Set the current window's title
 */
void FGAPIENTRY glutSetWindowTitle( const char* title )
{
	freeglut_assert_ready; freeglut_assert_window;

    /*
     * Works only for top-level windows
     */
    if( fgStructure.Window->Parent != NULL )
        return;

#if TARGET_HOST_UNIX_X11
	{
		XTextProperty text;

		/*
		 * Prepare the text properties
		 */
		text.value = (unsigned char *) title;
		text.encoding = XA_STRING;
		text.format = 8;
		text.nitems = strlen( title );

		/*
		 * Set the title now
		 */
		XSetWMName(
			fgDisplay.Display,
			fgStructure.Window->Window.Handle,
			&text
		);

		/*
		 * Have the X display state flushed
		 */
		XFlush( fgDisplay.Display );
	}

#elif TARGET_HOST_WIN32
	/*
	 * This seems to be a bit easier under Win32
	 */
	SetWindowText( fgStructure.Window->Window.Handle, title );

#endif
}

/*
 * Set the current window's iconified title
 */
void FGAPIENTRY glutSetIconTitle( const char* title )
{
    freeglut_assert_ready; freeglut_assert_window;

    /*
     * Works only for top-level windows
     */
    if( fgStructure.Window->Parent != NULL )
        return;

#if TARGET_HOST_UNIX_X11
	{
		XTextProperty text;

		/*
		 * Prepare the text properties
		 */
		text.value = (unsigned char *) title;
		text.encoding = XA_STRING;
		text.format = 8;
		text.nitems = strlen( title );

		/*
		 * Set the title now
		 */
		XSetWMIconName(
			fgDisplay.Display,
			fgStructure.Window->Window.Handle,
			&text
		);

		/*
		 * Have the X display state flushed
		 */
		XFlush( fgDisplay.Display );
	}

#elif TARGET_HOST_WIN32
	/*
	 * This seems to be a bit easier under Win32
	 */
	SetWindowText( fgStructure.Window->Window.Handle, title );

#endif
}

/*
 * Change the current window's size
 */
void FGAPIENTRY glutReshapeWindow( int width, int height )
{
    freeglut_assert_ready; freeglut_assert_window;

#if TARGET_HOST_UNIX_X11
    /*
     * Resize the window and flush the X state
     */
    XResizeWindow( fgDisplay.Display, fgStructure.Window->Window.Handle, width, height );
    XFlush( fgDisplay.Display );

#elif TARGET_HOST_WIN32
	{
		RECT winRect;
    int x, y ;

		/*
		 * First off, grab the current window's position
		 */
		GetWindowRect( fgStructure.Window->Window.Handle, &winRect );
    x = winRect.left ;
    y = winRect.top ;

    if ( fgStructure.Window->Parent == NULL )  /* If this is not a subwindow ... */
    {
      /*
       * Adjust the size of the window to allow for the size of the frame, if we are not a menu
       */
      if ( ! fgStructure.Window->IsMenu )
      {
    		width += GetSystemMetrics( SM_CXSIZEFRAME ) * 2;
	    	height += GetSystemMetrics( SM_CYSIZEFRAME ) * 2 + GetSystemMetrics( SM_CYCAPTION );
      }
    }
    else  /* This is a subwindow, get the parent window's position and subtract it off */
    {
      GetWindowRect ( fgStructure.Window->Parent->Window.Handle, &winRect ) ;
      x -= winRect.left + GetSystemMetrics( SM_CXSIZEFRAME ) ;
      y -= winRect.top + GetSystemMetrics( SM_CYSIZEFRAME ) + GetSystemMetrics( SM_CYCAPTION ) ;
    }

		/*
		 * Resize the window, forcing a redraw to happen
		 */
		MoveWindow(
			fgStructure.Window->Window.Handle,
			x,
			y,
			width,
			height,
			TRUE
		);
	}
#endif
}

/*
 * Change the current window's position
 */
void FGAPIENTRY glutPositionWindow( int x, int y )
{
    freeglut_assert_ready; freeglut_assert_window;

#if TARGET_HOST_UNIX_X11
    /*
     * Reposition the window and flush the X state
     */
    XMoveWindow( fgDisplay.Display, fgStructure.Window->Window.Handle, x, y );
    XFlush( fgDisplay.Display );

#elif TARGET_HOST_WIN32
	{
		RECT winRect;

		/*
		 * First off, grab the current window's position
		 */
		GetWindowRect( fgStructure.Window->Window.Handle, &winRect );

    /*
		 * Reposition the window, forcing a redraw to happen
		 */
		MoveWindow(
			fgStructure.Window->Window.Handle,
			x,
			y,
			winRect.right - winRect.left,
			winRect.bottom - winRect.top,
			TRUE
		);
	}

#endif
}

/*
 * Lowers the current window (by Z order change)
 */
void FGAPIENTRY glutPushWindow( void )
{
    freeglut_assert_ready; freeglut_assert_window;

#if TARGET_HOST_UNIX_X11
    /*
     * Lower the current window
     */
    XLowerWindow( fgDisplay.Display, fgStructure.Window->Window.Handle );

#elif TARGET_HOST_WIN32
	/*
	 * Set the new window's Z position, not affecting the rest of the settings:
	 */
	SetWindowPos(
		fgStructure.Window->Window.Handle,
		HWND_BOTTOM,
		0, 0, 0, 0,
		SWP_NOSIZE | SWP_NOMOVE
	);

#endif
}

/*
 * Raises the current window (by Z order change)
 */
void FGAPIENTRY glutPopWindow( void )
{
    freeglut_assert_ready; freeglut_assert_window;

#if TARGET_HOST_UNIX_X11
    /*
     * Raise the current window
     */
    XRaiseWindow( fgDisplay.Display, fgStructure.Window->Window.Handle );

#elif TARGET_HOST_WIN32
	/*
	 * Set the new window's Z position, not affecting the rest of the settings:
	 */
	SetWindowPos(
		fgStructure.Window->Window.Handle,
		HWND_TOP,
		0, 0, 0, 0,
		SWP_NOSIZE | SWP_NOMOVE
	);

#endif
}

/*
 * Resize the current window so that it fits the whole screen
 */
void FGAPIENTRY glutFullScreen( void )
{
    freeglut_assert_ready; freeglut_assert_window;

#if TARGET_HOST_UNIX_X11
    {
        int x, y;
        Window w;

        XMoveResizeWindow(
            fgDisplay.Display,
            fgStructure.Window->Window.Handle,
            0, 0,
            fgDisplay.ScreenWidth,
            fgDisplay.ScreenHeight
        );
        XFlush( fgDisplay.Display );

        XTranslateCoordinates(
            fgDisplay.Display,
            fgStructure.Window->Window.Handle,
            fgDisplay.RootWindow,
            0, 0, &x, &y, &w
        );

        if (x || y)
        {
            XMoveWindow(
                fgDisplay.Display,
                fgStructure.Window->Window.Handle,
                -x, -y
            );
            XFlush( fgDisplay.Display );
        }
    }
#elif TARGET_HOST_WIN32
    MoveWindow(
        fgStructure.Window->Window.Handle,
        0, 0,
        fgDisplay.ScreenWidth,
        fgDisplay.ScreenHeight,
        TRUE
    );
#endif
}

/*
 * A.Donev: Set and retrieve the window's user data
 */
void* FGAPIENTRY glutGetWindowData( void )
{
   return(fgStructure.Window->UserData);
}

void FGAPIENTRY glutSetWindowData(void* data)
{
  fgStructure.Window->UserData=data;
}

/*** END OF FILE ***/
