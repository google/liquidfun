/*
 * freeglut_main.c
 *
 * The windows message processing methods.
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

#define  G_LOG_DOMAIN  "freeglut-main"

#include "../include/GL/freeglut.h"
#include "../include/GL/freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 * There are some issues concerning window redrawing under X11, and maybe
 * some events are not handled. The Win32 version lacks some more features,
 * but seems acceptable for not demanding purposes.
 *
 * Need to investigate why the X11 version breaks out with an error when
 * closing a window (using the window manager, not glutDestroyWindow)...
 */

/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

/*
 * Calls a window's redraw method. This is used when
 * a redraw is forced by the incoming window messages.
 */
static void fghRedrawWindowByHandle
#if TARGET_HOST_UNIX_X11
    ( Window handle )
#elif TARGET_HOST_WIN32
    ( HWND handle )
#endif
{
    /*
     * Find the window we have to redraw...
     */
    SFG_Window* window = fgWindowByHandle( handle );
    freeglut_return_if_fail( window != NULL );

    /*
     * Check if there is a display callback hooked to it
     */
    freeglut_return_if_fail( window->Callbacks.Display != NULL );

    /*
     * Return if the window is not visible
     */
    freeglut_return_if_fail( window->State.Visible == TRUE );

    /*
     * Set the window as the current one. Calling glutSetWindow()
     * might seem slow and generally redundant, but it is portable.
     */
    glutSetWindow( window->ID );

    /*
     * Have the callback executed now. The buffers should
     * be swapped by the glutSwapBuffers() execution inside
     * the callback itself.
     */
    window->Callbacks.Display();
}

/*
 * Handle a window configuration change. When no reshape
 * callback is hooked, the viewport size is updated to
 * match the new window size.
 */
static void fghReshapeWindowByHandle
#if TARGET_HOST_UNIX_X11
    ( Window handle, int width, int height )
#elif TARGET_HOST_WIN32
    ( HWND handle, int width, int height )
#endif
{
    /*
     * Find the window that received the reshape event
     */
    SFG_Window* window = fgWindowByHandle( handle );
    freeglut_return_if_fail( window != NULL );

    /*
     * Remember about setting the current window...
     */
    glutSetWindow( window->ID );

    /*
     * Check if there is a reshape callback hooked
     */
    if( window->Callbacks.Reshape != NULL )
    {
        /*
         * OKi, have it called immediately
         */
        window->Callbacks.Reshape( width, height );
    }
    else
    {
        /*
         * Otherwise just resize the viewport
         */
        glViewport( 0, 0, width, height );
    }
}

/*
 * A static helper function to execute display callback for a window
 */
static void fghcbDisplayWindow( SFG_Window *window, SFG_Enumerator *enumerator )
{
#if TARGET_HOST_UNIX_X11
    /*
     * Check if there is an idle callback hooked
     */
    if( (window->Callbacks.Display != NULL) &&
        (window->State.Redisplay == TRUE) &&
        (window->State.Visible == TRUE) )
    {
        /*
         * OKi, this is the case: have the window set as the current one
         */
        glutSetWindow( window->ID );

        /*
         * Do not exagerate with the redisplaying
         */
        window->State.Redisplay = FALSE;

        /*
         * And execute the display callback immediately after
         */
        window->Callbacks.Display();
    }

#elif TARGET_HOST_WIN32

    /*
     * Do we need to explicitly resize the window?
     */
    if( window->State.NeedToResize )
    {
        glutSetWindow( window->ID );

        fghReshapeWindowByHandle( 
            window->Window.Handle,
            glutGet( GLUT_WINDOW_WIDTH ),
            glutGet( GLUT_WINDOW_HEIGHT )
        );

        /*
         * Never ever do that again:
         */
        window->State.NeedToResize = FALSE;
    }

    /*
     * This is done in a bit different way under Windows
     */
    RedrawWindow( 
        window->Window.Handle, NULL, NULL, 
        RDW_NOERASE | RDW_INTERNALPAINT | RDW_INVALIDATE 
    );

#endif

    /*
     * Process this window's children (if any)
     */
    fgEnumSubWindows( window, fghcbDisplayWindow, enumerator );
}

/*
 * Make all windows perform a display call
 */
static void fghDisplayAll( void )
{
    SFG_Enumerator enumerator;

    /*
     * Uses a method very similiar for fgWindowByHandle...
     */
    enumerator.found = FALSE;
    enumerator.data  =  NULL;

    /*
     * Start the enumeration now:
     */
    fgEnumWindows( fghcbDisplayWindow, &enumerator );
}

/*
 * Window enumerator callback to check for the joystick polling code
 */
static void fghcbCheckJoystickPolls( SFG_Window *window, SFG_Enumerator *enumerator )
{
    double checkTime = fgElapsedTime();

    /*
     * Check if actually need to do the poll for the currently enumerated window:
     */
    if( window->State.JoystickLastPoll + window->State.JoystickPollRate >= checkTime )
    {
        /*
         * Yeah, that's it. Poll the joystick...
         */
        fgJoystickPollWindow( window );

        /*
         * ...and reset the polling counters:
         */
        window->State.JoystickLastPoll = checkTime;
    }

    /*
     * Process this window's children (if any)
     */
    fgEnumSubWindows( window, fghcbCheckJoystickPolls, enumerator );
}

/*
 * Check all windows for joystick polling
 */
static void fghCheckJoystickPolls( void )
{
    SFG_Enumerator enumerator;

    /*
     * Uses a method very similiar for fgWindowByHandle...
     */
    enumerator.found = FALSE;
    enumerator.data  =  NULL;

    /*
     * Start the enumeration now:
     */
    fgEnumWindows( fghcbCheckJoystickPolls, &enumerator );
}

/*
 * Check the global timers
 */
static void fghCheckTimers( void )
{
    long checkTime = fgElapsedTime();
    SFG_Timer *timer, *next;
    SFG_List timedOut;

    fgListInit(&timedOut);

    /*
     * For every timer that is waiting for triggering
     */
    for( timer = fgState.Timers.First; timer; timer = next )
    {
	next = timer->Node.Next;

        /*
         * Check for the timeout:
         */
        if( timer->TriggerTime <= checkTime )
        {
            /*
             * Add the timer to the timed out timers list
             */
	    fgListRemove( &fgState.Timers, &timer->Node );
            fgListAppend( &timedOut, &timer->Node );
        }
    }

    /*
     * Now feel free to execute all the hooked and timed out timer callbacks
     * And delete the timed out timers...
     */
    while ( (timer = timedOut.First) )
    {
        if( timer->Callback != NULL )
            timer->Callback( timer->ID );
	fgListRemove( &timedOut, &timer->Node );
        free( timer );
    }
}

/*
 * Elapsed Time
 */
long fgElapsedTime( void )
{
#ifndef WIN32
	struct timeval now;
	long elapsed;

	gettimeofday( &now, NULL );

	elapsed = (now.tv_usec - fgState.Time.Value.tv_usec) / 1000;
	elapsed += (now.tv_sec - fgState.Time.Value.tv_sec) * 1000;

	return( elapsed );
#else
        return (timeGetTime() - fgState.Time.Value);
#endif
}

/*
 * Error Messages.
 */
void fgError( const char *fmt, ... )
{
    va_list ap;

    va_start( ap, fmt );

    fprintf( stderr, "freeglut: ");
    vfprintf( stderr, fmt, ap );
    fprintf( stderr, "\n" );

    va_end( ap );

    exit( 1 );
}

void fgWarning( const char *fmt, ... )
{
    va_list ap;

    va_start( ap, fmt );

    fprintf( stderr, "freeglut: ");
    vfprintf( stderr, fmt, ap );
    fprintf( stderr, "\n" );

    va_end( ap );
}

/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Enters the FreeGLUT processing loop. Never returns.
 */
void FGAPIENTRY glutMainLoop( void )
{
#if TARGET_HOST_UNIX_X11
    SFG_Window* window;
    XEvent event;
    int modifiers;

    /*
     * This code was repeated constantly, so here it goes into a definition:
     */
#   define GETWINDOW(a) window = fgWindowByHandle( event.a.window );if( window == NULL ) break;
#   define GETMOUSE(a) window->State.MouseX = event.a.x; window->State.MouseY = event.a.y;

    /*
     * Make sure the display has been created etc.
     */
    freeglut_assert_ready;

    /*
     * Enter the loop. Iterate as long as there are
     * any windows in the freeglut structure.
     */
    while( fgStructure.Windows.First != NULL )
    {
        /*
         * Do we have any event messages pending?
         */
        if( XPending( fgDisplay.Display ) )
        {
            /*
             * Grab the next event to be processed...
             */
            XNextEvent( fgDisplay.Display, &event );

            /*
             * Check the event's type
             */
            switch( event.type )
            {
            case CreateNotify:
                /*
                 * The window creation confirmation
                 */
                break;

            case DestroyNotify:
                /*
                 * This is sent to confirm the XDestroyWindow call. Ignore it.
                 */
                break;

            case ClientMessage:
                /*
                 * Destroy the window when the WM_DELETE_WINDOW message arrives
                 */
                if( (Atom) event.xclient.data.l[ 0 ] == fgDisplay.DeleteWindow )
                {
                    /*
                     * I wonder if the window still exists ;-)
                     */
                    fgDestroyWindow( fgWindowByHandle( event.xclient.window ), TRUE );
                }
                break;

            case UnmapNotify:
                /*
                 * A window of ours has been unmapped...
                 */
                break;

            case Expose:
                /*
                 * We are too dumb to process partial exposes...
                 */
                if( event.xexpose.count == 0 )
                    fghRedrawWindowByHandle( event.xexpose.window );
                break;

            case ConfigureNotify:
                /*
                 * The window gets resized
                 */
                fghReshapeWindowByHandle(
                    event.xconfigure.window,
                    event.xconfigure.width,
                    event.xconfigure.height
                );
                break;

            case MappingNotify:
                /*
                 * Have the client's keyboard knowledge updated (xlib.ps,
                 * page 206, says that's a good thing to do)
                 */
                XRefreshKeyboardMapping( (XMappingEvent *) &event );
                break;

            case VisibilityNotify:
                {
                    /*
                     * The window's visiblity might have changed
                     */
                    GETWINDOW( xvisibility );

                    /*
                     * Break now if no window status callback has been hooked to that window
                     */
                    if( window->Callbacks.WindowStatus == NULL )
                        break;

                    /*
                     * We're going to send a callback to a window. Make it current.
                     */
                    glutSetWindow( window->ID );

                    /*
                     * Sending this event, the X server can notify us that the window has just
                     * acquired one of the three possible visibility states: VisibilityUnobscured,
                     * VisibilityPartiallyObscured or VisibilityFullyObscured
                     */
                    switch( event.xvisibility.state )
                    {
                    case VisibilityUnobscured:
                        /*
                         * We are fully visible...
                         */
                        window->Callbacks.WindowStatus( GLUT_FULLY_RETAINED );
                        window->State.Visible = TRUE;
                        break;

                    case VisibilityPartiallyObscured:
                        /*
                         * The window is partially visible
                         */
                        window->Callbacks.WindowStatus( GLUT_PARTIALLY_RETAINED );
                        window->State.Visible = TRUE;
                        break;

                    case VisibilityFullyObscured:
                        /*
                         * The window is totally obscured
                         */
                        window->Callbacks.WindowStatus( GLUT_FULLY_COVERED );
                        window->State.Visible = FALSE;
                        break;
                    }
                }
                break;

            case EnterNotify:
                {
                    /*
                     * Mouse is over one of our windows
                     */
                    GETWINDOW( xcrossing ); GETMOUSE( xcrossing );

                    /*
                     * Is there an entry callback hooked to the window?
                     */
                    if( window->Callbacks.Entry != NULL )
                    {
                        /*
                         * Yeah. Notify the window about having the mouse cursor over
                         */
                        window->Callbacks.Entry( GLUT_ENTERED );
                    }
                }
                break;

            case LeaveNotify:
                {
                    /*
                     * Mouse is no longer over one of our windows
                     */
                    GETWINDOW( xcrossing ); GETMOUSE( xcrossing );

                    /*
                     * Is there an entry callback hooked to the window?
                     */
                    if( window->Callbacks.Entry != NULL )
                    {
                        /*
                         * Yeah. Notify the window about having the mouse cursor over
                         */
                        window->Callbacks.Entry( GLUT_LEFT );
                    }
                }
                break;

            case MotionNotify:
                {
                    /*
                     * The mouse cursor was moved...
                     */
                    GETWINDOW( xmotion ); GETMOUSE( xmotion );

                    /*
                     * What kind of a movement was it?
                     */
                    if( (event.xmotion.state & Button1Mask) || (event.xmotion.state & Button2Mask) ||
                        (event.xmotion.state & Button3Mask) || (event.xmotion.state & Button4Mask) ||
                        (event.xmotion.state & Button5Mask) )
                    {
                        /*
                         * A mouse button was pressed during the movement...
                         * Is there a motion callback hooked to the window?
                         */
                        if( window->Callbacks.Motion != NULL )
                        {
                            /*
                             * Yup. Have it executed immediately
                             */
                            window->Callbacks.Motion( event.xmotion.x, event.xmotion.y );
                        }
                    }
                    else
                    {
                        /*
                         * Otherwise it was a passive movement...
                         */
                        if( window->Callbacks.Passive != NULL )
                        {
                            /*
                             * That's right, and there is a passive callback, too.
                             */
                            window->Callbacks.Passive( event.xmotion.x, event.xmotion.y );
                        }
                    }
                }
                break;

            case ButtonRelease:
            case ButtonPress:
                {
                    int button;

                    /*
                     * A mouse button has been pressed or released. Traditionally,
                     * break if the window was found within the freeglut structures.
                     */
                    GETWINDOW( xbutton ); GETMOUSE( xbutton );

                    /*
                     * GLUT API assumes that you can't have more than three mouse buttons, so:
                     */
                    switch( event.xbutton.button )
                    {
                    /*
                     * WARNING: this might be wrong, if we only have two mouse buttons,
                     *          Button2 might mean the right button, isn't that right?
                     */
                    case Button1:   button = GLUT_LEFT_BUTTON;   break;
                    case Button2:   button = GLUT_MIDDLE_BUTTON; break;
                    case Button3:   button = GLUT_RIGHT_BUTTON;  break;
                    default:        button = -1;                 break;
                    }

                    /*
                     * Skip the unwanted mouse buttons...
                     */
                    if( button == -1 )
                        break;

                    /*
                     * Do not execute the callback if a menu is hooked to this key.
                     * In that case an appropriate private call should be generated
                     */
                    if( window->Menu[ button ] != NULL )
                    {
                        /*
                         * Set the current window
                         */
                        glutSetWindow( window->ID );

                        if( event.type == ButtonPress )
                        {
                            /*
                             * Activate the appropriate menu structure...
                             */
                            fgActivateMenu( button );
                        }
                        else
                        {
                            /*
                             * There are two general cases generated when a menu button
                             * is released -- it can provoke a menu call (when released
                             * over a menu area) or just deactivate the menu (when released
                             * somewhere else). Unfortunately, both cases must be checked
                             * recursively due to the submenu possibilities.
                             */
                            fgDeactivateMenu( button );
                        }
                        break;
                    }

                    /*
                     * Check if there is a mouse callback hooked to the window
                     */
                    if( window->Callbacks.Mouse == NULL )
                        break;

                    /*
                     * Set the current window
                     */
                    glutSetWindow( window->ID );

                    /*
                     * Remember the current modifiers state
                     */
                    modifiers = 0;
                    if (event.xbutton.state & (ShiftMask|LockMask))
                        modifiers |= GLUT_ACTIVE_SHIFT;
                    if (event.xbutton.state & ControlMask)
                        modifiers |= GLUT_ACTIVE_CTRL;
                    if (event.xbutton.state & Mod1Mask)
                        modifiers |= GLUT_ACTIVE_ALT;
                    window->State.Modifiers = modifiers;

                    /*
                     * Finally execute the mouse callback
                     */
                    window->Callbacks.Mouse(
                        button,
                        event.type == ButtonPress ? GLUT_DOWN : GLUT_UP,
                        event.xbutton.x,
                        event.xbutton.y
                    );

                    /*
                     * Trash the modifiers state
                     */
                    window->State.Modifiers = 0xffffffff;
                }
                break;

            case KeyPress:
                {
                    /*
                     * A key has been pressed, find the window that had the focus:
                     */
                    GETWINDOW( xkey ); GETMOUSE( xkey );

                    /*
                     * Is there a keyboard/special callback hooked for this window?
                     */
                    if( (window->Callbacks.Keyboard != NULL) || (window->Callbacks.Special != NULL) )
                    {
                        XComposeStatus composeStatus;
                        char asciiCode[ 32 ];
                        KeySym keySym;
                        int len;

                        /*
                         * Check for the ASCII/KeySym codes associated with the event:
                         */
                        len = XLookupString( &event.xkey, asciiCode, sizeof(asciiCode), &keySym, &composeStatus );

                        /*
                         * Get ready to calling the keyboard/special callbacks
                         */
                        glutSetWindow( window->ID );

                        /*
                         * GLUT API tells us to have two separate callbacks...
                         */
                        if( len > 0 )
                        {
                            /*
                             * ...one for the ASCII translateable keypresses...
                             */
                            if( window->Callbacks.Keyboard != NULL )
                            {
                                /*
                                 * Remember the current modifiers state
                                 */
                                modifiers = 0;
                                if (event.xkey.state & (ShiftMask|LockMask))
                                    modifiers |= GLUT_ACTIVE_SHIFT;
                                if (event.xkey.state & ControlMask)
                                    modifiers |= GLUT_ACTIVE_CTRL;
                                if (event.xkey.state & Mod1Mask)
                                    modifiers |= GLUT_ACTIVE_ALT;
                                window->State.Modifiers = modifiers;

                                /*
                                 * Execute the callback
                                 */
                                window->Callbacks.Keyboard( asciiCode[ 0 ], event.xkey.x, event.xkey.y );

                                /*
                                 * Trash the modifiers state
                                 */
                                window->State.Modifiers = 0xffffffff;
                            }
                        }
                        else
                        {
                            int special = -1;

                            /*
                             * ...and one for all the others, which need to be translated to GLUT_KEY_Xs...
                             */
                            switch( keySym )
                            {
                            /*
                             * First the function keys come:
                             */
                            case XK_F1:     special = GLUT_KEY_F1;     break;
                            case XK_F2:     special = GLUT_KEY_F2;     break;
                            case XK_F3:     special = GLUT_KEY_F3;     break;
                            case XK_F4:     special = GLUT_KEY_F4;     break;
                            case XK_F5:     special = GLUT_KEY_F5;     break;
                            case XK_F6:     special = GLUT_KEY_F6;     break;
                            case XK_F7:     special = GLUT_KEY_F7;     break;
                            case XK_F8:     special = GLUT_KEY_F8;     break;
                            case XK_F9:     special = GLUT_KEY_F9;     break;
                            case XK_F10:    special = GLUT_KEY_F10;    break;
                            case XK_F11:    special = GLUT_KEY_F11;    break;
                            case XK_F12:    special = GLUT_KEY_F12;    break;

                            /*
                             * Then the arrows and stuff:
                             */
                            case XK_Left:   special = GLUT_KEY_LEFT;   break;
                            case XK_Right:  special = GLUT_KEY_RIGHT;  break;
                            case XK_Up:     special = GLUT_KEY_UP;     break;
                            case XK_Down:   special = GLUT_KEY_DOWN;   break;
                            }

                            /*
                             * Execute the callback (if one has been specified),
                             * given that the special code seems to be valid...
                             */
                            if( (window->Callbacks.Special != NULL) && (special != -1) )
                            {
                                /*
                                 * Remember the current modifiers state
                                 */
                                modifiers = 0;
                                if (event.xkey.state & (ShiftMask|LockMask))
                                    modifiers |= GLUT_ACTIVE_SHIFT;
                                if (event.xkey.state & ControlMask)
                                    modifiers |= GLUT_ACTIVE_CTRL;
                                if (event.xkey.state & Mod1Mask)
                                    modifiers |= GLUT_ACTIVE_ALT;
                                window->State.Modifiers = modifiers;

                                window->Callbacks.Special( special, event.xkey.x, event.xkey.y );

                                /*
                                 * Trash the modifiers state
                                 */
                                window->State.Modifiers = 0xffffffff;
                            }
                        }
                    }
                }
                break;
            }
        }
        else
        {
            /*
             * Have all the timers checked.
             */
            fghCheckTimers();

            /*
             * Poll the joystick and notify all windows that want to be notified...
             */
            fghCheckJoystickPolls();

            /*
             * No messages in the queue, which means we are idling...
             */
            if( fgState.IdleCallback != NULL )
                fgState.IdleCallback();

            /*
             * Remember about displaying all the windows that have
             * been marked for a redisplay (possibly in the idle call):
             */
            fghDisplayAll();
        }
    }

#elif TARGET_HOST_WIN32

    GLboolean bLoop = TRUE;
    MSG stMsg;

    /*
     * The windows main loop is considerably smaller
     */
    while( bLoop )
    {
        if( PeekMessage( &stMsg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            /*
             * Grab the message now, checking for WM_QUIT
             */
            if( GetMessage( &stMsg, NULL, 0, 0 ) == 0 )
                bLoop = FALSE;

            /*
             * Translate virtual-key messages and send them to the window...
             */
            TranslateMessage( &stMsg );
            DispatchMessage( &stMsg );
        }
        else
        {
            /*
             * Have all the timers checked.
             */
            fghCheckTimers();

            /*
             * Poll the joystick and notify all windows that want to be notified...
             */
            fghCheckJoystickPolls();

            /*
             * No messages in the queue, which means we are idling...
             */
            if( fgState.IdleCallback != NULL )
                fgState.IdleCallback();

            /*
             * Remember about displaying all the windows that have
             * been marked for a redisplay (possibly in the idle call):
             */
            fghDisplayAll();

            /*
             * We need to terminate the main loop if no windows are left
             */
            bLoop = (g_list_length( fgStructure.Windows ) != 0);
        }
    }

#endif

    /*
     * When this loop terminates, destroy the display, state and structure
     * of a freeglut session, so that another glutInit() call can happen
     */
    fgDeinitialize();
}

/*
 * The window procedure for handling Win32 events
 */
#if TARGET_HOST_WIN32
LRESULT CALLBACK fgWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    SFG_Window* window = fgWindowByHandle( hWnd );
    PAINTSTRUCT ps;
    LONG lRet = 1;

#   define  assert_window_registered  if( window == NULL ) return( DefWindowProc( hWnd, uMsg, wParam, lParam ) );

    /*
     * Check what type of message are we receiving
     */
    switch( uMsg )
    {
    case WM_CREATE:
        /*
         * The window structure is passed as the creation structure paramter...
         */
        window = (SFG_Window *) (((LPCREATESTRUCT) lParam)->lpCreateParams);
        assert( window != NULL );

        /*
         * We can safely store the window's handle now:
         */
        window->Window.Handle = hWnd;

        /*
         * Get the window's device context
         */
        window->Window.Device = GetDC( hWnd );

        /*
         * Setup the pixel format of our window
         */
        fgSetupPixelFormat( window, FALSE );

        /*
         * Create the OpenGL rendering context now
         */
        window->Window.Context = wglCreateContext( window->Window.Device );

        /*
         * Still, we'll be needing to explicitly resize the window
         */
        window->State.NeedToResize = TRUE;

        /*
         * Finally, have the window's device context released
         */
        ReleaseDC( window->Window.Handle, window->Window.Device );
        break;

    case WM_SIZE:
        /*
         * We got resized... But check if the window has been already added...
         */
        fghReshapeWindowByHandle( hWnd, LOWORD(lParam), HIWORD(lParam) );
        break;

    case WM_PAINT:
        /*
         * Start the painting job
         */
        BeginPaint( hWnd, &ps );

        /*
         * Call the engine's main frame drawing method
         */
        fghRedrawWindowByHandle( hWnd );

        /*
         * End the painting job, release the device context
         */
        EndPaint( hWnd, &ps );
        break;

    case WM_CLOSE:
        /*
         * Make sure we don't close a window with current context active
         */
        if( fgStructure.Window == window )
        {
            wglMakeCurrent( NULL, NULL );
            wglDeleteContext( window->Window.Context );
        }

        /*
         * Proceed with the window destruction
         */
        DestroyWindow( window->Window.Handle );
        break;

    case WM_DESTROY:
        /*
         * The window already got destroyed, so forget about it's existence:
         */
        fgDestroyWindow( window, FALSE );
        return( 0 );

    case WM_MOUSEMOVE:
    {
        assert_window_registered;

        /*
         * The mouse cursor has moved. Remember the new mouse cursor's position
         */
        window->State.MouseX = LOWORD( lParam );
        window->State.MouseY = HIWORD( lParam );

        /*
         * Fallback if there's an active menu hooked to this window
         */
        if( window->MenuActive[ 0 ] || window->MenuActive[ 1 ] || window->MenuActive[ 2 ] )
            break;

        /*
         * Remember the current modifiers state.
         */
        window->State.Modifiers = 
            (GetKeyState( VK_LSHIFT   ) || GetKeyState( VK_RSHIFT   )) ? GLUT_ACTIVE_SHIFT : 0 |
            (GetKeyState( VK_LCONTROL ) || GetKeyState( VK_RCONTROL )) ? GLUT_ACTIVE_CTRL  : 0 |
            (GetKeyState( VK_LMENU    ) || GetKeyState( VK_RMENU    )) ? GLUT_ACTIVE_ALT   : 0;

        /*
         * Check if any of the mouse buttons is pressed...
         */
        if( (wParam & MK_LBUTTON) || (wParam & MK_MBUTTON) || (wParam & MK_RBUTTON) )
        {
            /*
             * Yeah, indeed. We need to use the motion callback then:
             */
            if( window->Callbacks.Motion != NULL )
            {
                /*
                 * Make sure the current window is set...
                 */
                glutSetWindow( window->ID );

                /*
                 * Execute the active mouse motion callback now
                 */
                window->Callbacks.Motion( window->State.MouseX, window->State.MouseY );
            }
        }
        else
        {
            /*
             * All mouse buttons are up, execute the passive mouse motion callback
             */
            if( window->Callbacks.Passive != NULL )
            {
                /*
                 * Make sure the current window is set
                 */
                glutSetWindow( window->ID );

                /*
                 * Execute the passive mouse motion callback
                 */
                window->Callbacks.Passive( window->State.MouseX, window->State.MouseY );
            }
        }

        /*
         * Thrash the current modifiers state now
         */
        window->State.Modifiers = 0xffffffff;
    }
    break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    {
        GLboolean pressed = TRUE;
        int button;

        /*
         * A mouse button has been pressed *or* released. Again, break off
         * if the message was not directed towards a freeglut window...
         */
        assert_window_registered;

        /*
         * The mouse cursor has moved. Remember the new mouse cursor's position
         */
        window->State.MouseX = LOWORD( lParam );
        window->State.MouseY = HIWORD( lParam );

        /*
         * We're curious about the GLUT API button name...
         */
        switch( uMsg )
        {
        case WM_LBUTTONDOWN: pressed = TRUE;  button = GLUT_LEFT_BUTTON;   break;
        case WM_MBUTTONDOWN: pressed = TRUE;  button = GLUT_MIDDLE_BUTTON; break;
        case WM_RBUTTONDOWN: pressed = TRUE;  button = GLUT_RIGHT_BUTTON;  break;
        case WM_LBUTTONUP:   pressed = FALSE; button = GLUT_LEFT_BUTTON;   break;
        case WM_MBUTTONUP:   pressed = FALSE; button = GLUT_MIDDLE_BUTTON; break;
        case WM_RBUTTONUP:   pressed = FALSE; button = GLUT_RIGHT_BUTTON;  break;
        default:             pressed = FALSE; button = -1;                 break;
        }

        /*
         * The left and right mouse buttons might have been swapped...
         */
        if( GetSystemMetrics( SM_SWAPBUTTON ) )
            if( button == GLUT_LEFT_BUTTON ) button = GLUT_RIGHT_BUTTON;
            else if( button == GLUT_RIGHT_BUTTON ) button = GLUT_LEFT_BUTTON;

        /*
         * Hey, what's up with you?
         */
        if( button == -1 )
            return( DefWindowProc( hWnd, uMsg, lParam, wParam ) );

        /*
         * Do not execute the callback if a menu is hooked to this key.
         * In that case an appropriate private call should be generated
         */
        if( window->Menu[ button ] != NULL )
        {
            /*
             * Set the current window
             */
            glutSetWindow( window->ID );

            if( pressed == TRUE )
            {
                /*
                 * Activate the appropriate menu structure...
                 */
                fgActivateMenu( button );
            }
            else
            {
                /*
                 * There are two general cases generated when a menu button
                 * is released -- it can provoke a menu call (when released
                 * over a menu area) or just deactivate the menu (when released
                 * somewhere else). Unfortunately, both cases must be checked
                 * recursively due to the submenu possibilities.
                 */
                fgDeactivateMenu( button );
            }
            break;
        }

        /*
         * Check if there is a mouse callback hooked to the window
         */
        if( window->Callbacks.Mouse == NULL )
            break;

        /*
         * Set the current window
         */
        glutSetWindow( window->ID );

        /*
         * Remember the current modifiers state.
         */
        window->State.Modifiers = 
            (GetKeyState( VK_LSHIFT   ) || GetKeyState( VK_RSHIFT   )) ? GLUT_ACTIVE_SHIFT : 0 |
            (GetKeyState( VK_LCONTROL ) || GetKeyState( VK_RCONTROL )) ? GLUT_ACTIVE_CTRL  : 0 |
            (GetKeyState( VK_LMENU    ) || GetKeyState( VK_RMENU    )) ? GLUT_ACTIVE_ALT   : 0;

        /*
         * Finally execute the mouse callback
         */
        window->Callbacks.Mouse(
            button,
            pressed == TRUE ? GLUT_DOWN : GLUT_UP,
            window->State.MouseX,
            window->State.MouseY
        );

        /*
         * Trash the modifiers state
         */
        window->State.Modifiers = 0xffffffff;
    }
    break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        int keypress = -1;

        /*
         * First of all, make sure that there is a window to be notified of this
         */
        assert_window_registered;

        /*
         * Ignore the automatic key repetition if needed:
         */
        if( fgState.IgnoreKeyRepeat && (lParam & KF_REPEAT) )
            break;

        /*
         * Remember the current modifiers state. This is done here in order 
         * to make sure the VK_DELETE keyboard callback is executed properly.
         */
        window->State.Modifiers = 
            (GetKeyState( VK_LSHIFT   ) || GetKeyState( VK_RSHIFT   )) ? GLUT_ACTIVE_SHIFT : 0 |
            (GetKeyState( VK_LCONTROL ) || GetKeyState( VK_RCONTROL )) ? GLUT_ACTIVE_CTRL  : 0 |
            (GetKeyState( VK_LMENU    ) || GetKeyState( VK_RMENU    )) ? GLUT_ACTIVE_ALT   : 0;

        /*
         * Convert the Win32 keystroke codes to GLUTtish way
         */
#       define KEY(a,b) case a: keypress = b; break;

        switch( wParam )
        {
            /*
             * Most of the special characters can be handled automagically...
             */
            KEY( VK_F1,     GLUT_KEY_F1        ); KEY( VK_F2,     GLUT_KEY_F2        );
            KEY( VK_F3,     GLUT_KEY_F3        ); KEY( VK_F4,     GLUT_KEY_F4        );
            KEY( VK_F5,     GLUT_KEY_F5        ); KEY( VK_F6,     GLUT_KEY_F6        );
            KEY( VK_F7,     GLUT_KEY_F7        ); KEY( VK_F8,     GLUT_KEY_F8        );
            KEY( VK_F9,     GLUT_KEY_F9        ); KEY( VK_F10,    GLUT_KEY_F10       );
            KEY( VK_F11,    GLUT_KEY_F11       ); KEY( VK_F12,    GLUT_KEY_F12       );
            KEY( VK_PRIOR,  GLUT_KEY_PAGE_UP   ); KEY( VK_NEXT,   GLUT_KEY_PAGE_DOWN );
            KEY( VK_HOME,   GLUT_KEY_HOME      ); KEY( VK_END,    GLUT_KEY_END       );
            KEY( VK_LEFT,   GLUT_KEY_LEFT      ); KEY( VK_UP,     GLUT_KEY_UP        );
            KEY( VK_RIGHT,  GLUT_KEY_RIGHT     ); KEY( VK_DOWN,   GLUT_KEY_DOWN      );
            KEY( VK_INSERT, GLUT_KEY_INSERT    );

            /*
             * ...yet there is a small exception we need to have handled...
             */
            case VK_DELETE:
                /*
                 * The delete key should be treated as an ASCII keypress:
                 */
                if( window->Callbacks.Keyboard != NULL )
                    window->Callbacks.Keyboard( 127, window->State.MouseX, window->State.MouseY );
        }

        /*
         * Execute the special callback, if present, given the conversion was a success:
         */
        if( (keypress != -1) && (window->Callbacks.Special != NULL) )
        {
            /*
             * Have the special callback executed:
             */
            window->Callbacks.Special( keypress, window->State.MouseX, window->State.MouseY );
        }

        /*
         * Thrash the modifiers register now
         */
        window->State.Modifiers = 0xffffffff;
    }
    break;

    case WM_SYSCHAR:
    case WM_CHAR:
    {
        /*
         * First of all, make sure that there is a window to be notified of this
         */
        assert_window_registered;

        /*
         * Ignore the automatic key repetition if needed:
         */
        if( fgState.IgnoreKeyRepeat && (lParam & KF_REPEAT) )
            break;

        /*
         * Clear to go with the keyboard callback, if registered:
         */
        if( window->Callbacks.Keyboard != NULL )
        {
            /*
             * Remember the current modifiers state
             */
            window->State.Modifiers = 
                (GetKeyState( VK_LSHIFT   ) || GetKeyState( VK_RSHIFT   )) ? GLUT_ACTIVE_SHIFT : 0 |
                (GetKeyState( VK_LCONTROL ) || GetKeyState( VK_RCONTROL )) ? GLUT_ACTIVE_CTRL  : 0 |
                (GetKeyState( VK_LMENU    ) || GetKeyState( VK_RMENU    )) ? GLUT_ACTIVE_ALT   : 0;

            /*
             * Have the special callback executed:
             */
            window->Callbacks.Keyboard( wParam, window->State.MouseX, window->State.MouseY );

            /*
             * Thrash the modifiers register now
             */
            window->State.Modifiers = 0xffffffff;
        }
    }
    break;

    default:
        /*
         * Handle unhandled messages
         */
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;
    }

    return( lRet );
}
#endif

/*** END OF FILE ***/
