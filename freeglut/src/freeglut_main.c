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
#include "freeglut_internal.h"

#include <limits.h>
#if TARGET_HOST_UNIX_X11
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#elif TARGET_HOST_WIN32
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif


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
    SFG_Window* window = fgWindowByHandle( handle );

    freeglut_return_if_fail( window != NULL );
    freeglut_return_if_fail( window->Callbacks.Display != NULL );
    freeglut_return_if_fail( window->State.Visible == TRUE );
    fgSetWindow( window );
    window->State.Redisplay = FALSE;

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
    SFG_Window *current_window = fgStructure.Window ;

    SFG_Window* window = fgWindowByHandle( handle );
    freeglut_return_if_fail( window != NULL );
    fgSetWindow( window );
    if( window->Callbacks.Reshape != NULL )
        window->Callbacks.Reshape( width, height );
    else
        glViewport( 0, 0, width, height );

    /*
     * Force a window redraw.  In Windows at least this is only a partial
     * solution:  if the window is increasing in size in either dimension,
     * the already-drawn part does not get drawn again and things look funny.
     * But without this we get this bad behaviour whenever we resize the
     * window.
     */
    window->State.Redisplay = TRUE ;

    if ( window->IsMenu )
	fgSetWindow ( current_window ) ;
}

/*
 * A static helper function to execute display callback for a window
 */
static void fghcbDisplayWindow( SFG_Window *window, SFG_Enumerator *enumerator )
{
#if TARGET_HOST_UNIX_X11
    if( (window->Callbacks.Display != NULL) &&
        (window->State.Redisplay == TRUE) &&
        (window->State.Visible == TRUE) )
    {
        SFG_Window *current_window = fgStructure.Window ;

        fgSetWindow( window );
        window->State.Redisplay = FALSE;
        window->Callbacks.Display();
        fgSetWindow ( current_window ) ;
    }

#elif TARGET_HOST_WIN32

    if( window->State.NeedToResize )
    {
        SFG_Window *current_window = fgStructure.Window ;

        fgSetWindow( window );

        fghReshapeWindowByHandle( 
            window->Window.Handle,
            glutGet( GLUT_WINDOW_WIDTH ),
            glutGet( GLUT_WINDOW_HEIGHT )
        );

        window->State.NeedToResize = FALSE;
        fgSetWindow ( current_window ) ;
    }

    if( (window->Callbacks.Display != NULL) &&
        (window->State.Redisplay == TRUE) &&
        (window->State.Visible == TRUE) )
    {
      window->State.Redisplay = FALSE;
      RedrawWindow( 
        window->Window.Handle, NULL, NULL, 
        RDW_NOERASE | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW
	);
    }

#endif

    fgEnumSubWindows( window, fghcbDisplayWindow, enumerator );
}

/*
 * Make all windows perform a display call
 */
static void fghDisplayAll( void )
{
    SFG_Enumerator enumerator;

    enumerator.found = FALSE;
    enumerator.data  =  NULL;
    fgEnumWindows( fghcbDisplayWindow, &enumerator );
}

/*
 * Window enumerator callback to check for the joystick polling code
 */
static void fghcbCheckJoystickPolls( SFG_Window *window, SFG_Enumerator *enumerator )
{
    long int checkTime = fgElapsedTime();

    if( window->State.JoystickLastPoll + window->State.JoystickPollRate <= checkTime )
    {
        fgJoystickPollWindow( window );
        window->State.JoystickLastPoll = checkTime;
    }
    fgEnumSubWindows( window, fghcbCheckJoystickPolls, enumerator );
}

/*
 * Check all windows for joystick polling
 */
static void fghCheckJoystickPolls( void )
{
    SFG_Enumerator enumerator;

    enumerator.found = FALSE;
    enumerator.data  =  NULL;
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
    for( timer = (SFG_Timer *)fgState.Timers.First;
	 timer;
	 timer = (SFG_Timer *)next )
    {
	next = (SFG_Timer *)timer->Node.Next;
        if( timer->TriggerTime <= checkTime )
        {
	    fgListRemove( &fgState.Timers, &timer->Node );
            fgListAppend( &timedOut, &timer->Node );
        }
    }

    /*
     * Now feel free to execute all the hooked and timed out timer callbacks
     * And delete the timed out timers...
     */
    while ( (timer = (SFG_Timer *)timedOut.First) )
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
#if TARGET_HOST_UNIX_X11
    struct timeval now;
    long elapsed;

    gettimeofday( &now, NULL );
    
    elapsed = (now.tv_usec - fgState.Time.Value.tv_usec) / 1000;
    elapsed += (now.tv_sec - fgState.Time.Value.tv_sec) * 1000;

    return( elapsed );
#elif TARGET_HOST_WIN32
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

    fprintf( stderr, "freeglut (%s): ", fgState.ProgramName || "");
    vfprintf( stderr, fmt, ap );
    fprintf( stderr, "\n" );

    va_end( ap );

    exit( 1 );
}

void fgWarning( const char *fmt, ... )
{
    va_list ap;

    va_start( ap, fmt );

    fprintf( stderr, "freeglut (%s): ", fgState.ProgramName || "");
    vfprintf( stderr, fmt, ap );
    fprintf( stderr, "\n" );

    va_end( ap );
}

/*
 * Indicates whether Joystick events are being used by ANY window.
 *
 * The current mechanism is to walk all of the windows and ask if
 * there is a joystick callback.  Certainly in some cases, maybe
 * in all cases, the joystick is attached to the system and accessed
 * from ONE point by GLUT/freeglut, so this is not the right way,
 * in general, to do this.  However, the Joystick code is segregated
 * in its own little world, so we can't access the information that
 * we need in order to do that nicely.
 *
 * Some alternatives:
 *  * Store Joystick data into freeglut global state.
 *  * Provide NON-static functions or data from Joystick *.c file.
 *
 * Basically, the RIGHT way to do this requires knowing something
 * about the Joystick.  Right now, the Joystick code is behind
 * an opaque wall.
 *
 */
static void fgCheckJoystickCallback( SFG_Window* w, SFG_Enumerator* e)
{
    if( w->Callbacks.Joystick )
    {
	e->found = TRUE;
	e->data = w;
    }
    fgEnumSubWindows( w, fgCheckJoystickCallback, e );
}
static int fgHaveJoystick( void )
{
    SFG_Enumerator enumerator;
    enumerator.found = FALSE;
    enumerator.data = NULL;
    fgEnumWindows( fgCheckJoystickCallback, &enumerator );
    return !!enumerator.data;
}
static void fgHavePendingRedisplaysCallback( SFG_Window* w, SFG_Enumerator* e)
{
    if( w->State.Redisplay )
    {
	e->found = TRUE;
	e->data = w;
    }
    fgEnumSubWindows( w, fgHavePendingRedisplaysCallback, e );
}	
static int fgHavePendingRedisplays (void)
{
    SFG_Enumerator enumerator;
    enumerator.found = FALSE;
    enumerator.data = NULL;
    fgEnumWindows( fgHavePendingRedisplaysCallback, &enumerator );
    return !!enumerator.data;
}
/*
 * Indicates whether there are any outstanding timers.
 */
static int fgHaveTimers( void )
{
    return !!fgState.Timers.First;
}
/*
 * Returns the number of GLUT ticks (milliseconds) till the next timer event.
 */
static long fgNextTimer( void )
{
    long now = fgElapsedTime();
    long ret = INT_MAX;
    SFG_Timer *timer;

    for( timer = (SFG_Timer *)fgState.Timers.First;
	 timer;
	 timer = (SFG_Timer *)timer->Node.Next )
	ret = MIN( ret, MAX( 0, (timer->TriggerTime) - now ) );

    return ret;
}
/*
 * Does the magic required to relinquish the CPU until something interesting
 * happens.
 */
static void fgSleepForEvents( void )
{
#if TARGET_HOST_UNIX_X11
    fd_set fdset;
    int err;
    int socket;
    struct timeval wait;
    long msec;    
    
    if( fgState.IdleCallback ||
	fgHavePendingRedisplays() )
	return;
    socket = ConnectionNumber( fgDisplay.Display );
    FD_ZERO( &fdset );
    FD_SET( socket, &fdset );
    
    msec = fgNextTimer();
    if( fgHaveJoystick() )
	msec = MIN( msec, 10 );
    
    wait.tv_sec = msec / 1000;
    wait.tv_usec = (msec % 1000) * 1000;
    err = select( socket+1, &fdset, NULL, NULL, &wait );

    if( -1 == err )
	printf( "freeglut select() error: %d\n", errno );
    
#elif TARGET_HOST_WIN32
#endif
}

/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Executes a single iteration in the freeglut processing loop.
 */
void FGAPIENTRY glutMainLoopEvent( void )
{
#if TARGET_HOST_UNIX_X11
  SFG_Window* window;
  XEvent event;
  int modifiers;

  /*
   * This code was repeated constantly, so here it goes into a definition:
   */
# define GETWINDOW(a)                          \
  window = fgWindowByHandle( event.a.window ); \
  if( window == NULL )                         \
    break;
# define GETMOUSE(a)                           \
  window->State.MouseX = event.a.x;            \
  window->State.MouseY = event.a.y;

  freeglut_assert_ready;
  while( XPending( fgDisplay.Display ) )
  {
    XNextEvent( fgDisplay.Display, &event );
    switch( event.type )
    {
    case DestroyNotify:
      /*
       * This is sent to confirm the XDestroyWindow call.
       */
      /*
       * Call the window closure callback, remove from the structure, etc.
       */
/*      fgAddToWindowDestroyList ( window, FALSE ); */

      break;

    case ClientMessage:
      /*
       * Destroy the window when the WM_DELETE_WINDOW message arrives
       */
      if( (Atom) event.xclient.data.l[ 0 ] == fgDisplay.DeleteWindow )
      {
        GETWINDOW( xclient ); 

        /*
         * Call the XWindows functions to close the window
         */
        fgCloseWindow ( window ) ;

        /*
         * Call the window closure callback, remove from the structure, etc.
         */
        fgAddToWindowDestroyList ( window, FALSE );
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

      /*
       * CreateNotify causes a configure-event so that sub-windows are
       * handled compatibly with GLUT.
       *
       * NOTE that it is possible that you will more than one Reshape
       * event for your top-level window, but something like this appears
       * to be required for compatbility.
       *
       * GLUT presumably does this because it generally tries to treat
       * sub-windows the same as windows.
       *
       */
    case CreateNotify:
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
        fgSetWindow( window );

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
           * Set the current window
           */
          fgSetWindow ( window ) ;

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
           * Set the current window
           */
          fgSetWindow ( window ) ;

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
         * Fallback if there's an active menu hooked to this window
         */
        if( window->ActiveMenu != NULL )
        {
        		if ( window == window->ActiveMenu->ParentWindow )
        		{
        			window->ActiveMenu->Window->State.MouseX = event.xmotion.x_root - window->ActiveMenu->X ;
        			window->ActiveMenu->Window->State.MouseY = event.xmotion.y_root - window->ActiveMenu->Y ;
            }

            /*
             * Let's make the window redraw as a result of the mouse motion.
             */
            window->ActiveMenu->Window->State.Redisplay = TRUE ;

            /*
             * Since the window is a menu, make the parent window current
             */
            fgSetWindow ( window->ActiveMenu->ParentWindow ) ;

            break;  /* I think this should stay in -- an active menu should absorb the mouse motion */
        }

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
             * Set the current window
             */
            fgSetWindow ( window ) ;

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
             * Set the current window
             */
            fgSetWindow ( window ) ;

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
        GLboolean pressed = TRUE ;
        int button;

        if ( event.type == ButtonRelease ) pressed = FALSE ;

        /*
         * A mouse button has been pressed or released. Traditionally,
         * break if the window was found within the freeglut structures.
         */
        GETWINDOW( xbutton ); GETMOUSE( xbutton );

	/*
	 * An X button (at least in XFree86) is numbered from 1.
	 * A GLUT button is numbered from 0.
	 * Old GLUT passed through buttons other than just the first
	 * three, though it only gave symbolic names and official
	 * support to the first three.
	 *
	 */
	button = event.xbutton.button - 1;

        /*
         * Do not execute the application's mouse callback if a menu is hooked to this button.
         * In that case an appropriate private call should be generated.
         * Near as I can tell, this is the menu behaviour:
         *  - Down-click the menu button, menu not active:  activate the menu with its upper left-hand corner at the mouse location.
         *  - Down-click any button outside the menu, menu active:  deactivate the menu
         *  - Down-click any button inside the menu, menu active:  select the menu entry and deactivate the menu
         *  - Up-click the menu button, menu not active:  nothing happens
         *  - Up-click the menu button outside the menu, menu active:  nothing happens
         *  - Up-click the menu button inside the menu, menu active:  select the menu entry and deactivate the menu
         */
        if ( window->ActiveMenu != NULL )  /* Window has an active menu, it absorbs any mouse click */
        {
      		if ( window == window->ActiveMenu->ParentWindow )
      		{
      			window->ActiveMenu->Window->State.MouseX = event.xbutton.x_root - window->ActiveMenu->X ;
      			window->ActiveMenu->Window->State.MouseY = event.xbutton.y_root - window->ActiveMenu->Y ;
      		}

          if ( fgCheckActiveMenu ( window->ActiveMenu->Window, window->ActiveMenu ) == TRUE )  /* Inside the menu, invoke the callback and deactivate the menu*/
          {
            /* Save the current window and menu and set the current window to the window whose menu this is */
            SFG_Window *save_window = fgStructure.Window ;
            SFG_Menu *save_menu = fgStructure.Menu ;
            SFG_Window *parent_window = window->ActiveMenu->ParentWindow ;
            fgSetWindow ( parent_window ) ;
            fgStructure.Menu = window->ActiveMenu ;

            /* Execute the menu callback */
            fgExecuteMenuCallback ( window->ActiveMenu ) ;
            fgDeactivateMenu ( parent_window ) ;

            /* Restore the current window and menu */
            fgSetWindow ( save_window ) ;
            fgStructure.Menu = save_menu ;
          }
          else  /* Outside the menu, deactivate the menu if it's a downclick */
          {
            if ( pressed == TRUE ) fgDeactivateMenu ( window->ActiveMenu->ParentWindow ) ;
          }

          /*
           * Let's make the window redraw as a result of the mouse click and menu activity.
           */
          window->State.Redisplay = TRUE ;

          break ;
        }

        /*
         * No active menu, let's check whether we need to activate one.
         */
        if (( 0 <= button ) && ( 2 >= button ) &&
	    ( window->Menu[ button ] != NULL ) && ( pressed == TRUE ) )
        {
          /*
           * Let's make the window redraw as a result of the mouse click.
           */
          window->State.Redisplay = TRUE ;

          /*
           * Set the current window
           */
          fgSetWindow( window );

          /*
           * Activate the appropriate menu structure...
           */
          fgActivateMenu( window, button );

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
        fgSetWindow ( window );

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
        fgStructure.Window->State.Modifiers = modifiers;

        /*
         * Finally execute the mouse callback
         */
        fgStructure.Window->Callbacks.Mouse(
            button,
            event.type == ButtonPress ? GLUT_DOWN : GLUT_UP,
            event.xbutton.x,
            event.xbutton.y
        );

        /*
         * Trash the modifiers state
         */
        fgStructure.Window->State.Modifiers = 0xffffffff;
      }
      break;

    case KeyRelease:
    case KeyPress:
      {
        FGCBkeyboard keyboard_cb;
        FGCBspecial special_cb;

        /*
         * A key has been pressed, find the window that had the focus:
         */
        GETWINDOW( xkey ); GETMOUSE( xkey );

        if( event.type == KeyPress )
        {
          keyboard_cb = window->Callbacks.Keyboard;
          special_cb = window->Callbacks.Special;
        }
        else
        {
          keyboard_cb = window->Callbacks.KeyboardUp;
          special_cb = window->Callbacks.SpecialUp;
        }

        /*
         * Is there a keyboard/special callback hooked for this window?
         */
        if( (keyboard_cb != NULL) || (special_cb != NULL) )
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
           * GLUT API tells us to have two separate callbacks...
           */
          if( len > 0 )
          {
            /*
             * ...one for the ASCII translateable keypresses...
             */
            if( keyboard_cb != NULL )
            {
              /*
               * Set the current window
               */
              fgSetWindow( window );

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
              keyboard_cb( asciiCode[ 0 ], event.xkey.x, event.xkey.y );

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

            case XK_KP_Prior:
            case XK_Prior:  special = GLUT_KEY_PAGE_UP; break;
            case XK_KP_Next:
            case XK_Next:   special = GLUT_KEY_PAGE_DOWN; break;
            case XK_KP_Home:
            case XK_Home:   special = GLUT_KEY_HOME;   break;
            case XK_KP_End:
            case XK_End:    special = GLUT_KEY_END;    break;
            case XK_KP_Insert:
            case XK_Insert: special = GLUT_KEY_INSERT; break;
            }

            /*
             * Execute the callback (if one has been specified),
             * given that the special code seems to be valid...
             */
            if( (special_cb != NULL) && (special != -1) )
            {
              /*
               * Set the current window
               */
              fgSetWindow( window );

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

              special_cb( special, event.xkey.x, event.xkey.y );

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

#elif TARGET_HOST_WIN32

  MSG stMsg;

  /*
   * The windows processing is considerably smaller
   */
  while( PeekMessage( &stMsg, NULL, 0, 0, PM_NOREMOVE ) )
  {
    /*
     * Grab the message now, checking for WM_QUIT
     */
    if( GetMessage( &stMsg, NULL, 0, 0 ) == 0 )
      fgState.ExecState = GLUT_EXEC_STATE_STOP ;

    /*
     * Translate virtual-key messages and send them to the window...
     */
    TranslateMessage( &stMsg );
    DispatchMessage( &stMsg );
  }

#endif

    fghCheckTimers ();
    fghCheckJoystickPolls ();
    fghDisplayAll ();
    fgCloseWindows () ;
}

/*
 * Enters the freeglut processing loop. Stays until the "ExecState" changes to "GLUT_EXEC_STATE_STOP".
 */
void FGAPIENTRY glutMainLoop( void )
{
#if TARGET_HOST_WIN32
  SFG_Window *window = (SFG_Window *)fgStructure.Windows.First ;
#endif

  freeglut_assert_ready;

#if TARGET_HOST_WIN32
  /*
   * Processing before the main loop:  If there is a window which is open and
   * which has a visibility callback, call it.  I know this is an ugly hack,
   * but I'm not sure what else to do about it.  Ideally we should leave
   * something uninitialized in the create window code and initialize it in
   * the main loop, and have that initialization create a "WM_ACTIVATE"
   * message.  Then we would put the visibility callback code in the
   * "case WM_ACTIVATE" block below.         - John Fay -- 10/24/02
   */
  while ( window != NULL )
  {
    if ( window->Callbacks.Visibility != NULL )
    {
      SFG_Window *current_window = fgStructure.Window ;
      fgSetWindow( window );
      window->Callbacks.Visibility ( window->State.Visible ) ;
      fgSetWindow( current_window );
    }

    window = (SFG_Window *)window->Node.Next ;
  }
#endif

  fgState.ExecState = GLUT_EXEC_STATE_RUNNING ;

  while ( fgState.ExecState == GLUT_EXEC_STATE_RUNNING )
  {
    glutMainLoopEvent () ;
    if ( fgStructure.Windows.First == NULL )
      fgState.ExecState = GLUT_EXEC_STATE_STOP ;
    else
    {
      if ( fgState.IdleCallback )
        fgState.IdleCallback ();
      fgSleepForEvents();
    }
  }

  {
    fgExecutionState execState = fgState.ActionOnWindowClose;

    /*
     * When this loop terminates, destroy the display, state and structure
     * of a freeglut session, so that another glutInit() call can happen
     */
    fgDeinitialize();

    if ( execState == GLUT_ACTION_EXIT )
      exit ( 0 ) ;
  }
}

/*
 * Leaves the freeglut processing loop.
 */
void FGAPIENTRY glutLeaveMainLoop( void )
{
  fgState.ExecState = GLUT_EXEC_STATE_STOP ;
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

    if ( ( window == NULL ) && ( uMsg != WM_CREATE ) )
      return( DefWindowProc( hWnd, uMsg, wParam, lParam ) );

/*    printf ( "Window %3d message <%04x> %12d %12d\n", window?window->ID:0, uMsg, wParam, lParam ) ; */
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
         * Create or get the OpenGL rendering context now
         */
        if ( fgState.BuildingAMenu )
        {
          /*
           * Setup the pixel format of our window
           */
          unsigned int current_DisplayMode = fgState.DisplayMode ;
          fgState.DisplayMode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH ;
          fgSetupPixelFormat( window, FALSE, PFD_MAIN_PLANE );
          fgState.DisplayMode = current_DisplayMode ;

          /*
           * If there isn't already an OpenGL rendering context for menu windows, make one
           */
          if ( !fgStructure.MenuContext )
          {
            fgStructure.MenuContext = (SFG_MenuContext *)malloc ( sizeof(SFG_MenuContext) ) ;
            fgStructure.MenuContext->Context = wglCreateContext( window->Window.Device );
          }
          else
            wglMakeCurrent ( window->Window.Device, fgStructure.MenuContext->Context ) ;

/*          window->Window.Context = wglGetCurrentContext () ;   */
          window->Window.Context = wglCreateContext( window->Window.Device );
        }
        else
        {
          /*
           * Setup the pixel format of our window
           */
          fgSetupPixelFormat( window, FALSE, PFD_MAIN_PLANE );

          if ( fgState.UseCurrentContext == TRUE )
          {
            window->Window.Context = wglGetCurrentContext();
            if ( ! window->Window.Context )
              window->Window.Context = wglCreateContext( window->Window.Device );
          }
          else
            window->Window.Context = wglCreateContext( window->Window.Device );
        }

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
#if 0
    case WM_SETFOCUS: 
        printf("WM_SETFOCUS: %p\n", window );
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;

    case WM_ACTIVATE: 
        if (LOWORD(wParam) != WA_INACTIVE)
        {
          /* glutSetCursor( fgStructure.Window->State.Cursor ); */
	        printf("WM_ACTIVATE: glutSetCursor( %p, %d)\n", window, window->State.Cursor );

          glutSetCursor( window->State.Cursor );
        }

        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;
#endif

    case WM_SETCURSOR: 
        /*
         * Windows seems to need reminding to erase the cursor for NONE.
         */
#if 0
        if ((LOWORD(lParam) == HTCLIENT) &&
	    (fgStructure.Window->State.Cursor == GLUT_CURSOR_NONE))
	  SetCursor( NULL );
#else
	/* Set the cursor AND change it for this window class. */
#	define MAP_CURSOR(a,b) case a: SetCursor( LoadCursor( NULL, b ) ); \
        break;
	/* Nuke the cursor AND change it for this window class. */
#	define ZAP_CURSOR(a,b) case a: SetCursor( NULL ); \
        break;

        if (LOWORD(lParam) == HTCLIENT)
	  switch( window->State.Cursor )
	  {
		MAP_CURSOR( GLUT_CURSOR_RIGHT_ARROW, IDC_ARROW     );
		MAP_CURSOR( GLUT_CURSOR_LEFT_ARROW,  IDC_ARROW     );
		MAP_CURSOR( GLUT_CURSOR_INFO,        IDC_HELP      );
		MAP_CURSOR( GLUT_CURSOR_DESTROY,     IDC_CROSS     );
		MAP_CURSOR( GLUT_CURSOR_HELP,        IDC_HELP	   );
		MAP_CURSOR( GLUT_CURSOR_CYCLE,       IDC_SIZEALL   );
		MAP_CURSOR( GLUT_CURSOR_SPRAY,       IDC_CROSS     );
		MAP_CURSOR( GLUT_CURSOR_WAIT,		 IDC_WAIT      );
		MAP_CURSOR( GLUT_CURSOR_TEXT,        IDC_UPARROW   );
		MAP_CURSOR( GLUT_CURSOR_CROSSHAIR,   IDC_CROSS     );
		/* MAP_CURSOR( GLUT_CURSOR_NONE,        IDC_NO		   ); */
		ZAP_CURSOR( GLUT_CURSOR_NONE,        NULL	   );

		default:
		MAP_CURSOR( GLUT_CURSOR_UP_DOWN,     IDC_ARROW     );
	  }
#endif
	else
	  lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
	break;

    case WM_SHOWWINDOW:
        /*
         * We are now Visible!
         */
        window->State.Visible = TRUE;
        window->State.Redisplay = TRUE;
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
          int used = FALSE ;
          SFG_Window *iter ;

            wglMakeCurrent( NULL, NULL );
            /* Step through the list of windows.  If the rendering context is notbeing used
             * by another window, then we delete it.
             */
            for ( iter = (SFG_Window *)fgStructure.Windows.First; iter; iter = (SFG_Window *)iter->Node.Next )
            {
              if ( ( iter->Window.Context == window->Window.Context ) && ( iter != window ) )
                used = TRUE ;
            }

            if ( used == FALSE ) wglDeleteContext( window->Window.Context );
        }

        /*
         * Put on a linked list of windows to be removed after all the callbacks have returned
         */
        fgAddToWindowDestroyList ( window, FALSE ) ;

        /*
         * Proceed with the window destruction
         */
        DestroyWindow( hWnd );
        break;

    case WM_DESTROY:
        /*
         * The window already got destroyed, so don't bother with it.
         */
        return( 0 );

    case WM_MOUSEMOVE:
    {
        /*
         * The mouse cursor has moved. Remember the new mouse cursor's position
         */
        window->State.MouseX = LOWORD( lParam );
        window->State.MouseY = HIWORD( lParam );

        /*
         * Fallback if there's an active menu hooked to this window
         */
        if ( window->ActiveMenu != NULL )
        {
            /*
             * Let's make the window redraw as a result of the mouse motion.
             */
            window->State.Redisplay = TRUE ;

            /*
             * Since the window is a menu, make the parent window current
             */
            fgSetWindow ( window->ActiveMenu->ParentWindow ) ;

            break;
        }

        /*
         * Remember the current modifiers state.
         */
        window->State.Modifiers = 
            ( ( (GetKeyState( VK_LSHIFT   ) < 0 ) || ( GetKeyState( VK_RSHIFT   ) < 0 )) ? GLUT_ACTIVE_SHIFT : 0 ) |
            ( ( (GetKeyState( VK_LCONTROL ) < 0 ) || ( GetKeyState( VK_RCONTROL ) < 0 )) ? GLUT_ACTIVE_CTRL  : 0 ) |
            ( ( (GetKeyState( VK_LMENU    ) < 0 ) || ( GetKeyState( VK_RMENU    ) < 0 )) ? GLUT_ACTIVE_ALT   : 0 );

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
                fgSetWindow( window );

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
                fgSetWindow( window );

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
         * Do not execute the application's mouse callback if a menu is hooked to this button.
         * In that case an appropriate private call should be generated.
         * Near as I can tell, this is the menu behaviour:
         *  - Down-click the menu button, menu not active:  activate the menu with its upper left-hand corner at the mouse location.
         *  - Down-click any button outside the menu, menu active:  deactivate the menu
         *  - Down-click any button inside the menu, menu active:  select the menu entry and deactivate the menu
         *  - Up-click the menu button, menu not active:  nothing happens
         *  - Up-click the menu button outside the menu, menu active:  nothing happens
         *  - Up-click the menu button inside the menu, menu active:  select the menu entry and deactivate the menu
         */
        if ( window->ActiveMenu != NULL )  /* Window has an active menu, it absorbs any mouse click */
        {
          if ( fgCheckActiveMenu ( window, window->ActiveMenu ) == TRUE )  /* Inside the menu, invoke the callback and deactivate the menu*/
          {
            /* Save the current window and menu and set the current window to the window whose menu this is */
            SFG_Window *save_window = fgStructure.Window ;
            SFG_Menu *save_menu = fgStructure.Menu ;
            SFG_Window *parent_window = window->ActiveMenu->ParentWindow ;
            fgSetWindow ( parent_window ) ;
            fgStructure.Menu = window->ActiveMenu ;

            /* Execute the menu callback */
            fgExecuteMenuCallback ( window->ActiveMenu ) ;
            fgDeactivateMenu ( parent_window ) ;

            /* Restore the current window and menu */
            fgSetWindow ( save_window ) ;
            fgStructure.Menu = save_menu ;
          }
          else  /* Outside the menu, deactivate the menu if it's a downclick */
          {
            if ( pressed == TRUE ) fgDeactivateMenu ( window->ActiveMenu->ParentWindow ) ;
          }

          /*
           * Let's make the window redraw as a result of the mouse click and menu activity.
           */
          if ( ! window->IsMenu ) window->State.Redisplay = TRUE ;

          break ;
        }

        /*
         * No active menu, let's check whether we need to activate one.
         */
        if ( ( window->Menu[ button ] != NULL ) && ( pressed == TRUE ) )
        {
            /*
             * Let's make the window redraw as a result of the mouse click.
             */
            window->State.Redisplay = TRUE ;

            /*
             * Set the current window
             */
            fgSetWindow( window );

            /*
             * Activate the appropriate menu structure...
             */
            fgActivateMenu( window, button );

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
        fgSetWindow ( window );

        /*
         * Remember the current modifiers state.
         */
        fgStructure.Window->State.Modifiers = 
            ( ( (GetKeyState( VK_LSHIFT   ) < 0 ) || ( GetKeyState( VK_RSHIFT   ) < 0 )) ? GLUT_ACTIVE_SHIFT : 0 ) |
            ( ( (GetKeyState( VK_LCONTROL ) < 0 ) || ( GetKeyState( VK_RCONTROL ) < 0 )) ? GLUT_ACTIVE_CTRL  : 0 ) |
            ( ( (GetKeyState( VK_LMENU    ) < 0 ) || ( GetKeyState( VK_RMENU    ) < 0 )) ? GLUT_ACTIVE_ALT   : 0 );

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
        fgStructure.Window->State.Modifiers = 0xffffffff;
    }
    break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        int keypress = -1;
        POINT mouse_pos ;

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
            ( ( (GetKeyState( VK_LSHIFT   ) < 0 ) || ( GetKeyState( VK_RSHIFT   ) < 0 )) ? GLUT_ACTIVE_SHIFT : 0 ) |
            ( ( (GetKeyState( VK_LCONTROL ) < 0 ) || ( GetKeyState( VK_RCONTROL ) < 0 )) ? GLUT_ACTIVE_CTRL  : 0 ) |
            ( ( (GetKeyState( VK_LMENU    ) < 0 ) || ( GetKeyState( VK_RMENU    ) < 0 )) ? GLUT_ACTIVE_ALT   : 0 );

        /*
         * Set the mouse position
         */
        GetCursorPos ( &mouse_pos ) ;
        ScreenToClient ( window->Window.Handle, &mouse_pos ) ;

        window->State.MouseX = mouse_pos.x ;
        window->State.MouseY = mouse_pos.y ;

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
                {
                    fgSetWindow( window );
                    window->Callbacks.Keyboard( 127, window->State.MouseX, window->State.MouseY );
                }
        }

        /*
         * Execute the special callback, if present, given the conversion was a success:
         */
        if( (keypress != -1) && (window->Callbacks.Special != NULL) )
        {
            /*
             * Set the current window
             */
            fgSetWindow( window );

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

    case WM_SYSKEYUP:
    case WM_KEYUP:
    {
        int keypress = -1;
        POINT mouse_pos ;

        /*
         * Remember the current modifiers state. This is done here in order 
         * to make sure the VK_DELETE keyboard callback is executed properly.
         */
        window->State.Modifiers = 
            ( ( (GetKeyState( VK_LSHIFT   ) < 0 ) || ( GetKeyState( VK_RSHIFT   ) < 0 )) ? GLUT_ACTIVE_SHIFT : 0 ) |
            ( ( (GetKeyState( VK_LCONTROL ) < 0 ) || ( GetKeyState( VK_RCONTROL ) < 0 )) ? GLUT_ACTIVE_CTRL  : 0 ) |
            ( ( (GetKeyState( VK_LMENU    ) < 0 ) || ( GetKeyState( VK_RMENU    ) < 0 )) ? GLUT_ACTIVE_ALT   : 0 );

        /*
         * Set the mouse position
         */
        GetCursorPos ( &mouse_pos ) ;
        ScreenToClient ( window->Window.Handle, &mouse_pos ) ;

        window->State.MouseX = mouse_pos.x ;
        window->State.MouseY = mouse_pos.y ;

        /*
         * Convert the Win32 keystroke codes to GLUTtish way.  "KEY(a,b)" was defined under "WM_KEYDOWN"
         */

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
            if( window->Callbacks.KeyboardUp != NULL )
            {
                fgSetWindow ( window ) ;
                window->Callbacks.KeyboardUp( 127, window->State.MouseX, window->State.MouseY );
            }

            break ;
          default:
            {
              /*
               * Call the KeyboardUp callback for a regular character if there is one.
               */
              BYTE state[ 256 ];
              WORD code[ 2 ];

              GetKeyboardState(state);

              if ( ToAscii( wParam, 0, state, code, 0 ) == 1 )
                wParam=code[ 0 ];

              if( window->Callbacks.KeyboardUp != NULL )
              {
                /*
                 * Set the current window
                 */
                fgSetWindow( window );

                window->Callbacks.KeyboardUp( (char)wParam, window->State.MouseX, window->State.MouseY );
              }
            }
        }

        /*
         * Execute the special callback, if present, given the conversion was a success:
         */
        if( (keypress != -1) && (window->Callbacks.SpecialUp != NULL) )
        {
            /*
             * Set the current window
             */
            fgSetWindow( window );

            /*
             * Have the special callback executed:
             */
            window->Callbacks.SpecialUp( keypress, window->State.MouseX, window->State.MouseY );
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
             * Set the current window
             */
            fgSetWindow( window );

            /*
             * Remember the current modifiers state
             */
            window->State.Modifiers = 
                ( ( (GetKeyState( VK_LSHIFT   ) < 0 ) || ( GetKeyState( VK_RSHIFT   ) < 0 )) ? GLUT_ACTIVE_SHIFT : 0 ) |
                ( ( (GetKeyState( VK_LCONTROL ) < 0 ) || ( GetKeyState( VK_RCONTROL ) < 0 )) ? GLUT_ACTIVE_CTRL  : 0 ) |
                ( ( (GetKeyState( VK_LMENU    ) < 0 ) || ( GetKeyState( VK_RMENU    ) < 0 )) ? GLUT_ACTIVE_ALT   : 0 );

            /*
             * Have the special callback executed:
             */
            window->Callbacks.Keyboard( (char)wParam, window->State.MouseX, window->State.MouseY );

            /*
             * Thrash the modifiers register now
             */
            window->State.Modifiers = 0xffffffff;
        }
    }
    break;

    case WM_CAPTURECHANGED :  /* User has finished resizing the window, force a redraw */
      if ( window->Callbacks.Display )
      {
        /*
         * Set the current window
         */
        fgSetWindow( window );

        window->Callbacks.Display () ;
      }

/*      lRet = DefWindowProc( hWnd, uMsg, wParam, lParam ) ; */
      break ;

      /*
       * Other messages that I have seen and which are not handled already
       */
    case WM_SETTEXT :  /* 0x000c */
      lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );  /* Pass it on to "DefWindowProc" to set the window text */
      break ;

    case WM_GETTEXT :  /* 0x000d */
      /* Ideally we would copy the title of the window into "lParam" */
/*      strncpy ( (char *)lParam, "Window Title", wParam ) ;
      lRet = ( wParam > 12 ) ? 12 : wParam ;  */ /* the number of characters copied */
      lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
      break ;

    case WM_GETTEXTLENGTH :  /* 0x000e */
      /* Ideally we would get the length of the title of the window */
      lRet = 12 ;  /* the number of characters in "Window Title\0" (see above) */
      break ;

    case WM_ERASEBKGND :  /* 0x0014 */
      lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
      break ;

    case WM_SYNCPAINT :  /* 0x0088 */
      /* Another window has moved, need to update this one */
      window->State.Redisplay = TRUE ;
      lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );  /* Help screen says this message must be passed to "DefWindowProc" */
      break ;

    case WM_NCPAINT :  /* 0x0085 */
      /* Need to update the border of this window */
      lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );  /* Pass it on to "DefWindowProc" to repaint a standard border */
      break ;

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
