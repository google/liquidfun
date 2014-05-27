/*
 * fg_main_ios.c
 *
 * The iOS-specific windows message processing methods.
 *
 * Copyright (C) 2014 Google, Inc.
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
#include "fg_main_ios.h"
#import "fg_app_delegate.h"

extern void fghOnReshapeNotify ( SFG_Window *window,
                                 int width, int height, GLboolean forceNotify);
extern void fghOnPositionNotify ( SFG_Window *window,
                                  int x, int y, GLboolean forceNotify);
extern void fgPlatformFullScreenToggle ( SFG_Window *win );
extern void fgPlatformPositionWindow ( SFG_Window *window, int x, int y );
extern void fgPlatformReshapeWindow ( SFG_Window *window, int width, int height );
extern void fgPlatformPushWindow ( SFG_Window *window );
extern void fgPlatformPopWindow ( SFG_Window *window );
extern void fgPlatformHideWindow ( SFG_Window *window );
extern void fgPlatformIconifyWindow ( SFG_Window *window );
extern void fgPlatformShowWindow ( SFG_Window *window );

unsigned long fgPlatformSystemTime ( void )
{
    static NSTimeInterval firstTime = 0;
    NSTimeInterval currentTime = [NSDate timeIntervalSinceReferenceDate];
    if (firstTime == 0)
    {
        firstTime = currentTime;
    }
    return (unsigned long)(1000*(currentTime-firstTime));
}

void fgPlatformSleepForEvents ( long msec )
{
}

void fgPlatformProcessSingleEvent ( void )
{
}

void fgPlatformMainLoopPreliminaryWork ( void )
{
    fgState.ExecState = GLUT_EXEC_STATE_RUNNING ;
    @autoreleasepool
    {
        UIApplicationMain(0, NULL, nil, NSStringFromClass([FGAppDelegate class]));
    }
}

/* deal with work list items */
void fgPlatformInitWork ( SFG_Window* window )
{
    /* notify windowStatus/visibility */
    INVOKE_WCB( *window, WindowStatus, ( GLUT_FULLY_RETAINED ) );

    /* Position callback, always at 0,0 */
    fghOnPositionNotify(window, 0, 0, GL_TRUE);

    /* Size gets notified on window creation with size detection in mainloop above
     * XXX CHECK: does this messages happen too early like on windows,
     * so client code cannot have registered a callback yet and the message
     * is thus never received by client?
     */
}

void fgPlatformPosResZordWork ( SFG_Window* window, unsigned int workMask )
{
    if (workMask & GLUT_FULL_SCREEN_WORK)
        fgPlatformFullScreenToggle( window );
    if (workMask & GLUT_POSITION_WORK)
        fgPlatformPositionWindow( window, window->State.DesiredXpos, window->State.DesiredYpos );
    if (workMask & GLUT_SIZE_WORK)
        fgPlatformReshapeWindow ( window, window->State.DesiredWidth, window->State.DesiredHeight );
    if (workMask & GLUT_ZORDER_WORK)
    {
        if (window->State.DesiredZOrder < 0)
            fgPlatformPushWindow( window );
        else
            fgPlatformPopWindow( window );
    }
}

void fgPlatformVisibilityWork ( SFG_Window* window )
{
    /* Visibility status of window should get updated in the window message handlers
     * For now, none of these functions called below do anything, so don't worry
     * about it
     */
    SFG_Window *win = window;
    switch (window->State.DesiredVisibility)
    {
    case DesireHiddenState:
        fgPlatformHideWindow( window );
        break;
    case DesireIconicState:
        /* Call on top-level window */
        while (win->Parent)
            win = win->Parent;
        fgPlatformIconifyWindow( win );
        break;
    case DesireNormalState:
        fgPlatformShowWindow( window );
        break;
    }
}

