/*
 * freeglut_callbacks.c
 *
 * The callbacks setting methods.
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

#include <GL/freeglut.h>
#include "freeglut_internal.h"


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * All of the callbacks setting methods can be generalized to this:
 */
#define SET_CALLBACK(a)              \
    if( fgStructure.Window == NULL ) \
        return;                      \
    SET_WCB( ( *( fgStructure.Window ) ), a, callback );

/*
 * Sets the Display callback for the current window
 */
void FGAPIENTRY glutDisplayFunc( void (* callback)( void ) )
{
    if( !callback )
        fgError( "Fatal error in program.  NULL display callback not "
                 "permitted in GLUT 3.0+ or freeglut 2.0.1+\n" );
    SET_CALLBACK( Display );
}

/*
 * Sets the Reshape callback for the current window
 */
void FGAPIENTRY glutReshapeFunc( void (* callback)( int, int ) )
{
    SET_CALLBACK( Reshape );
}

/*
 * Sets the Keyboard callback for the current window
 */
void FGAPIENTRY glutKeyboardFunc( void (* callback)
                                  ( unsigned char, int, int ) )
{
    SET_CALLBACK( Keyboard );
}

/*
 * Sets the Special callback for the current window
 */
void FGAPIENTRY glutSpecialFunc( void (* callback)( int, int, int ) )
{
    SET_CALLBACK( Special );
}

/*
 * Sets the global idle callback
 */
void FGAPIENTRY glutIdleFunc( void (* callback)( void ) )
{
    freeglut_assert_ready;
    fgState.IdleCallback = callback;
}

/*
 * Sets the Timer callback for the current window
 */
void FGAPIENTRY glutTimerFunc( unsigned int timeOut, void (* callback)( int ),
                               int timerID )
{
    SFG_Timer *timer, *node;

    freeglut_assert_ready;

    if( (timer = fgState.FreeTimers.Last) )
    {
        fgListRemove( &fgState.FreeTimers, &timer->Node );
    }
    else
    {
        if( ! (timer = malloc(sizeof(SFG_Timer))) )
            fgError( "Fatal error: "
                     "Memory allocation failure in glutTimerFunc()\n" );
    }

    timer->Callback  = callback;
    timer->ID        = timerID;
    timer->TriggerTime = fgElapsedTime() + timeOut;

    for( node = fgState.Timers.First; node; node = node->Node.Next )
    {
        if( node->TriggerTime > timer->TriggerTime )
            break;
    }

    fgListInsert( &fgState.Timers, &node->Node, &timer->Node );
}

/*
 * Sets the Visibility callback for the current window.
 */
static void fghVisibility( int status )
{
    int glut_status = GLUT_VISIBLE;

    freeglut_assert_ready;
    freeglut_return_if_fail( fgStructure.Window );

    if( ( GLUT_HIDDEN == status )  || ( GLUT_FULLY_COVERED == status ) )
        glut_status = GLUT_NOT_VISIBLE;
    INVOKE_WCB( *( fgStructure.Window ), Visibility, ( glut_status ) );
}

void FGAPIENTRY glutVisibilityFunc( void (* callback)( int ) )
{
    SET_CALLBACK( Visibility );

    if( callback )
        glutWindowStatusFunc( fghVisibility );
    else
        glutWindowStatusFunc( NULL );
}

/*
 * Sets the keyboard key release callback for the current window
 */
void FGAPIENTRY glutKeyboardUpFunc( void (* callback)
                                    ( unsigned char, int, int ) )
{
    SET_CALLBACK( KeyboardUp );
}

/*
 * Sets the special key release callback for the current window
 */
void FGAPIENTRY glutSpecialUpFunc( void (* callback)( int, int, int ) )
{
    SET_CALLBACK( SpecialUp );
}

/*
 * Sets the joystick callback and polling rate for the current window
 */
void FGAPIENTRY glutJoystickFunc( void (* callback)
                                  ( unsigned int, int, int, int ),
                                  int pollInterval )
{
    SET_CALLBACK( Joystick );
    fgStructure.Window->State.JoystickPollRate = pollInterval;

    fgStructure.Window->State.JoystickLastPoll =
        fgElapsedTime() - fgStructure.Window->State.JoystickPollRate;

    if( fgStructure.Window->State.JoystickLastPoll < 0 )
        fgStructure.Window->State.JoystickLastPoll = 0;
}

/*
 * Sets the mouse callback for the current window
 */
void FGAPIENTRY glutMouseFunc( void (* callback)( int, int, int, int ) )
{
    SET_CALLBACK( Mouse );
}

/*
 * Sets the mouse wheel callback for the current window
 */
void FGAPIENTRY glutMouseWheelFunc( void (* callback)( int, int, int, int ) )
{
    SET_CALLBACK( MouseWheel );
}

/*
 * Sets the mouse motion callback for the current window (one or more buttons
 * are pressed)
 */
void FGAPIENTRY glutMotionFunc( void (* callback)( int, int ) )
{
    SET_CALLBACK( Motion );
}

/*
 * Sets the passive mouse motion callback for the current window (no mouse
 * buttons are pressed)
 */
void FGAPIENTRY glutPassiveMotionFunc( void (* callback)( int, int ) )
{
    SET_CALLBACK( Passive );
}

/*
 * Window mouse entry/leave callback
 */
void FGAPIENTRY glutEntryFunc( void (* callback)( int ) )
{
    SET_CALLBACK( Entry );
}

/*
 * Window destruction callbacks
 */
void FGAPIENTRY glutCloseFunc( void (* callback)( void ) )
{
    SET_CALLBACK( Destroy );
}

void FGAPIENTRY glutWMCloseFunc( void (* callback)( void ) )
{
    glutCloseFunc( callback );
}

/* A. Donev: Destruction callback for menus */
void FGAPIENTRY glutMenuDestroyFunc( void (* callback)( void ) )
{
    if( fgStructure.Menu )
        fgStructure.Menu->Destroy = callback;
}

/*
 * Deprecated version of glutMenuStatusFunc callback setting method
 */
void FGAPIENTRY glutMenuStateFunc( void (* callback)( int ) )
{
    freeglut_assert_ready;
    fgState.MenuStateCallback = callback;
}

/*
 * Sets the global menu status callback for the current window
 */
void FGAPIENTRY glutMenuStatusFunc( void (* callback)( int, int, int ) )
{
    freeglut_assert_ready;
    fgState.MenuStatusCallback = callback;
}

/*
 * Sets the overlay display callback for the current window
 */
void FGAPIENTRY glutOverlayDisplayFunc( void (* callback)( void ) )
{
    SET_CALLBACK( OverlayDisplay );
}

/*
 * Sets the window status callback for the current window
 */
void FGAPIENTRY glutWindowStatusFunc( void (* callback)( int ) )
{
    SET_CALLBACK( WindowStatus );
}

/*
 * Sets the spaceball motion callback for the current window
 */
void FGAPIENTRY glutSpaceballMotionFunc( void (* callback)( int, int, int ) )
{
    SET_CALLBACK( SpaceMotion );
}

/*
 * Sets the spaceball rotate callback for the current window
 */
void FGAPIENTRY glutSpaceballRotateFunc( void (* callback)( int, int, int ) )
{
    SET_CALLBACK( SpaceRotation );
}

/*
 * Sets the spaceball button callback for the current window
 */
void FGAPIENTRY glutSpaceballButtonFunc( void (* callback)( int, int ) )
{
    SET_CALLBACK( SpaceButton );
}

/*
 * Sets the button box callback for the current window
 */
void FGAPIENTRY glutButtonBoxFunc( void (* callback)( int, int ) )
{
    SET_CALLBACK( ButtonBox );
}

/*
 * Sets the dials box callback for the current window
 */
void FGAPIENTRY glutDialsFunc( void (* callback)( int, int ) )
{
    SET_CALLBACK( Dials );
}

/*
 * Sets the tablet motion callback for the current window
 */
void FGAPIENTRY glutTabletMotionFunc( void (* callback)( int, int ) )
{
    SET_CALLBACK( TabletMotion );
}

/*
 * Sets the tablet buttons callback for the current window
 */
void FGAPIENTRY glutTabletButtonFunc( void (* callback)( int, int, int, int ) )
{
    SET_CALLBACK( TabletButton );
}

/*** END OF FILE ***/
