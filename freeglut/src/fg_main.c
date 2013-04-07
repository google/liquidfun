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

#include <GL/freeglut.h>
#include "fg_internal.h"
#ifdef HAVE_ERRNO_H
#    include <errno.h>
#endif
#include <stdarg.h>
#ifdef  HAVE_VFPRINTF
#    define VFPRINTF(s,f,a) vfprintf((s),(f),(a))
#elif defined(HAVE__DOPRNT)
#    define VFPRINTF(s,f,a) _doprnt((f),(a),(s))
#else
#    define VFPRINTF(s,f,a)
#endif

/*
 * Try to get the maximum value allowed for ints, falling back to the minimum
 * guaranteed by ISO C99 if there is no suitable header.
 */
#ifdef HAVE_LIMITS_H
#    include <limits.h>
#endif
#ifndef INT_MAX
#    define INT_MAX 32767
#endif

#ifndef MIN
#    define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif

extern void fgPlatformProcessWork   ( SFG_Window *window );
extern fg_time_t fgPlatformSystemTime ( void );
extern void fgPlatformSleepForEvents( fg_time_t msec );
extern void fgPlatformProcessSingleEvent ( void );
extern void fgPlatformMainLoopPreliminaryWork ( void );




/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

void fghOnReshapeNotify(SFG_Window *window, int width, int height, GLboolean forceNotify)
{
    GLboolean notify = GL_FALSE;

    if( width  != window->State.Width ||
        height != window->State.Height )
    {
        window->State.Width = width;
        window->State.Height = height;

        notify = GL_TRUE;
    }

    if (notify || forceNotify)
    {
        SFG_Window *saved_window = fgStructure.CurrentWindow;

        INVOKE_WCB( *window, Reshape, ( width, height ) );

        /*
         * Force a window redraw.  In Windows at least this is only a partial
         * solution:  if the window is increasing in size in either dimension,
         * the already-drawn part does not get drawn again and things look funny.
         * But without this we get this bad behaviour whenever we resize the
         * window.
         * DN: Hmm.. the above sounds like a concern only in single buffered mode...
         */
        glutPostRedisplay( );
        if( window->IsMenu )
            fgSetWindow( saved_window );
    }
}

void fghOnPositionNotify(SFG_Window *window, int x, int y, GLboolean forceNotify)
{
    GLboolean notify = GL_FALSE;

    if( x  != window->State.Xpos ||
        y != window->State.Ypos )
    {
        window->State.Xpos = x;
        window->State.Ypos = y;

        notify = GL_TRUE;
    }

    if (notify || forceNotify)
    {
        SFG_Window *saved_window = fgStructure.CurrentWindow;
        INVOKE_WCB( *window, Position, ( x, y ) );
        fgSetWindow( saved_window );
    }
}

/*
 * Calls a window's redraw method. This is used when
 * a redraw is forced by the incoming window messages,
 * or if a redisplay is otherwise pending.
 * this is lean and mean without checks as it is
 * currently only called from fghcbDisplayWindow which
 * only calls this if the window is visible and needs
 * a redisplay.
 * Note that the fgSetWindow call on Windows makes the
 * right device context current on windows, allowing
 * direct drawing without BeginPaint/EndPaint in the
 * WM_PAINT handler.
 */
void fghRedrawWindow ( SFG_Window *window )
{
    SFG_Window *current_window = fgStructure.CurrentWindow;

    fgSetWindow( window );
    INVOKE_WCB( *window, Display, ( ) );

    fgSetWindow( current_window );
}

void fghRedrawWindowAndChildren ( SFG_Window *window )
{
    SFG_Window* child;

    fghRedrawWindow(window);

    for( child = ( SFG_Window * )window->Children.First;
         child;
         child = ( SFG_Window * )child->Node.Next )
    {
        fghRedrawWindowAndChildren(child);
    }
}


static void fghcbProcessWork( SFG_Window *window,
                              SFG_Enumerator *enumerator )
{
    if( window->State.WorkMask )
		fgPlatformProcessWork ( window );

    fgEnumSubWindows( window, fghcbProcessWork, enumerator );
}

/*
 * Make all windows process their work list
 */
static void fghProcessWork( void )
{
    SFG_Enumerator enumerator;

    enumerator.found = GL_FALSE;
    enumerator.data  =  NULL;

    fgEnumWindows( fghcbProcessWork, &enumerator );
}


static void fghcbDisplayWindow( SFG_Window *window,
                                SFG_Enumerator *enumerator )
{
    if( window->State.Redisplay &&
        window->State.Visible )
    {
        window->State.Redisplay = GL_FALSE;
		fghRedrawWindow ( window );
    }

    fgEnumSubWindows( window, fghcbDisplayWindow, enumerator );
}

/*
 * Make all windows perform a display call
 */
static void fghDisplayAll( void )
{
    SFG_Enumerator enumerator;

    enumerator.found = GL_FALSE;
    enumerator.data  =  NULL;

    fgEnumWindows( fghcbDisplayWindow, &enumerator );
}

/*
 * Window enumerator callback to check for the joystick polling code
 */
static void fghcbCheckJoystickPolls( SFG_Window *window,
                                     SFG_Enumerator *enumerator )
{
    fg_time_t checkTime;
    
    if (window->State.JoystickPollRate > 0 && FETCH_WCB( *window, Joystick ))
    {
        /* This window has a joystick to be polled (if pollrate <= 0, user needs to poll manually with glutForceJoystickFunc */
        checkTime= fgElapsedTime( );

        if( window->State.JoystickLastPoll + window->State.JoystickPollRate <=
            checkTime )
        {
#if !defined(_WIN32_WCE)
            fgJoystickPollWindow( window );
#endif /* !defined(_WIN32_WCE) */
            window->State.JoystickLastPoll = checkTime;
        }
    }

    fgEnumSubWindows( window, fghcbCheckJoystickPolls, enumerator );
}

/*
 * Check all windows for joystick polling
 * 
 * The real way to do this is to make use of the glutTimer() API
 * to more cleanly re-implement the joystick API.  Then, this code
 * and all other "joystick timer" code can be yanked.
 */
static void fghCheckJoystickPolls( void )
{
    SFG_Enumerator enumerator;

    enumerator.found = GL_FALSE;
    enumerator.data  =  NULL;

    fgEnumWindows( fghcbCheckJoystickPolls, &enumerator );
}

/*
 * Check the global timers
 */
static void fghCheckTimers( void )
{
    fg_time_t checkTime = fgElapsedTime( );

    while( fgState.Timers.First )
    {
        SFG_Timer *timer = fgState.Timers.First;

        if( timer->TriggerTime > checkTime )
            /* Timers are sorted by triggerTime */
            break;

        fgListRemove( &fgState.Timers, &timer->Node );
        fgListAppend( &fgState.FreeTimers, &timer->Node );

        timer->Callback( timer->ID );
    }
}

 
/* Platform-dependent time in milliseconds, as an unsigned 64-bit integer.
 * This doesn't overflow in any reasonable time, so no need to worry about
 * that. The GLUT API return value will however overflow after 49.7 days,
 * which means you will still get in trouble when running the
 * application for more than 49.7 days.
 */  
fg_time_t fgSystemTime(void)
{
	return fgPlatformSystemTime();
}
  
/*
 * Elapsed Time
 */
fg_time_t fgElapsedTime( void )
{
    return fgSystemTime() - fgState.Time;
}

/*
 * Error Messages.
 */
void fgError( const char *fmt, ... )
{
    va_list ap;

    if (fgState.ErrorFunc) {

        va_start( ap, fmt );

        /* call user set error handler here */
        fgState.ErrorFunc(fmt, ap);

        va_end( ap );

    } else {
#ifdef FREEGLUT_PRINT_ERRORS
        va_start( ap, fmt );

        fprintf( stderr, "freeglut ");
        if( fgState.ProgramName )
            fprintf( stderr, "(%s): ", fgState.ProgramName );
        VFPRINTF( stderr, fmt, ap );
        fprintf( stderr, "\n" );

        va_end( ap );
#endif

        if ( fgState.Initialised )
            fgDeinitialize ();

        exit( 1 );
    }
}

void fgWarning( const char *fmt, ... )
{
    va_list ap;

    if (fgState.WarningFunc) {

        va_start( ap, fmt );

        /* call user set warning handler here */
        fgState.WarningFunc(fmt, ap);

        va_end( ap );

    } else {
#ifdef FREEGLUT_PRINT_WARNINGS
        va_start( ap, fmt );

        fprintf( stderr, "freeglut ");
        if( fgState.ProgramName )
            fprintf( stderr, "(%s): ", fgState.ProgramName );
        VFPRINTF( stderr, fmt, ap );
        fprintf( stderr, "\n" );

        va_end( ap );
#endif
    }
}


/*
 * Indicates whether a redisplay is pending for ANY window.
 *
 * The current mechanism is to walk all of the windows and ask if
 * a redisplay is pending. We have a short-circuit early
 * return if we find any.
 */
static void fghHavePendingRedisplaysCallback( SFG_Window* w, SFG_Enumerator* e)
{
    if( w->State.Redisplay && w->State.Visible )
    {
        e->found = GL_TRUE;
        e->data = w;
        return;
    }
    fgEnumSubWindows( w, fghHavePendingRedisplaysCallback, e );
}
static int fghHavePendingRedisplays (void)
{
    SFG_Enumerator enumerator;

    enumerator.found = GL_FALSE;
    enumerator.data = NULL;
    fgEnumWindows( fghHavePendingRedisplaysCallback, &enumerator );
    return !!enumerator.data;
}

/*
 * Returns the number of GLUT ticks (milliseconds) till the next timer event.
 */
static fg_time_t fghNextTimer( void )
{
    fg_time_t currentTime;
    SFG_Timer *timer = fgState.Timers.First;    /* timers are sorted by trigger time, so only have to check the first */

    if( !timer )
        return INT_MAX;

    currentTime = fgElapsedTime();
    if( timer->TriggerTime < currentTime )
        return 0;
    else
        return timer->TriggerTime - currentTime;
}

static void fghSleepForEvents( void )
{
    fg_time_t msec;

    if( fghHavePendingRedisplays( ) )
        return;

    msec = fghNextTimer( );
    /* XXX Should use GLUT timers for joysticks... */
    /* XXX Dumb; forces granularity to .01sec */
    if( fgState.NumActiveJoysticks>0 && ( msec > 10 ) )     
        msec = 10;

	fgPlatformSleepForEvents ( msec );
}


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Executes a single iteration in the freeglut processing loop.
 */
void FGAPIENTRY glutMainLoopEvent( void )
{
    /* Process input */
	fgPlatformProcessSingleEvent ();

    if( fgState.Timers.First )
        fghCheckTimers( );
    if (fgState.NumActiveJoysticks>0)   /* If zero, don't poll joysticks */
        fghCheckJoystickPolls( );

    /* Perform work on the window (position, reshape, etc) */
    fghProcessWork( );

    /* Display */
    fghDisplayAll( );

    fgCloseWindows( );
}

/*
 * Enters the freeglut processing loop.
 * Stays until the "ExecState" changes to "GLUT_EXEC_STATE_STOP".
 */
void FGAPIENTRY glutMainLoop( void )
{
    int action;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutMainLoop" );

    if (!fgStructure.Windows.First)
        fgError(" ERROR:  glutMainLoop called with no windows created.");

	fgPlatformMainLoopPreliminaryWork ();

    fgState.ExecState = GLUT_EXEC_STATE_RUNNING ;
    while( fgState.ExecState == GLUT_EXEC_STATE_RUNNING )
    {
        SFG_Window *window;

        glutMainLoopEvent( );
        /*
         * Step through the list of windows, seeing if there are any
         * that are not menus
         */
        for( window = ( SFG_Window * )fgStructure.Windows.First;
             window;
             window = ( SFG_Window * )window->Node.Next )
            if ( ! ( window->IsMenu ) )
                break;

        if( ! window )
            fgState.ExecState = GLUT_EXEC_STATE_STOP;
        else
        {
            if( fgState.IdleCallback )
            {
                if( fgStructure.CurrentWindow &&
                    fgStructure.CurrentWindow->IsMenu )
                    /* fail safe */
                    fgSetWindow( window );
                fgState.IdleCallback( );
            }
            else
                fghSleepForEvents( );
        }
    }

    /*
     * When this loop terminates, destroy the display, state and structure
     * of a freeglut session, so that another glutInit() call can happen
     *
     * Save the "ActionOnWindowClose" because "fgDeinitialize" resets it.
     */
    action = fgState.ActionOnWindowClose;
    fgDeinitialize( );
    if( action == GLUT_ACTION_EXIT )
        exit( 0 );
}

/*
 * Leaves the freeglut processing loop.
 */
void FGAPIENTRY glutLeaveMainLoop( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutLeaveMainLoop" );
    fgState.ExecState = GLUT_EXEC_STATE_STOP ;
}



/*** END OF FILE ***/
