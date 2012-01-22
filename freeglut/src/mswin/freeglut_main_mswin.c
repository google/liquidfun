/*
 * freeglut_main_mswin.c
 *
 * The Windows-specific mouse cursor related stuff.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by John F. Fay, <fayjf@sourceforge.net>
 * Creation date: Sat Jan 21, 2012
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
#include "freeglut_internal_mswin.h"




void fghPlatformReshapeWindow ( SFG_Window *window, int width, int height )
{
    RECT windowRect;

    /*
     * For windowed mode, get the current position of the
     * window and resize taking the size of the frame
     * decorations into account.
     */

    /* "GetWindowRect" returns the pixel coordinates of the outside of the window */
    GetWindowRect( window->Window.Handle, &windowRect );

    /* Create rect in FreeGLUT format, (X,Y) topleft outside window, WxH of client area */
    windowRect.right    = windowRect.left+width;
    windowRect.bottom   = windowRect.top+height;

    if (window->Parent == NULL)
        /* get the window rect from this to feed to SetWindowPos, correct for window decorations */
        fghComputeWindowRectFromClientArea_QueryWindow(window,&windowRect,TRUE);
    else
    {
        /* correct rect for position client area of parent window
         * (SetWindowPos input for child windows is in coordinates
         * relative to the parent's client area).
         * Child windows don't have decoration, so no need to correct
         * for them.
         */
        RECT parentRect;
        parentRect = fghGetClientArea( window->Parent, FALSE );
        windowRect.left   -= parentRect.left;
        windowRect.right  -= parentRect.left;
        windowRect.top    -= parentRect.top;
        windowRect.bottom -= parentRect.top;
    }
    
    /* Do the actual resizing */
    SetWindowPos( window->Window.Handle,
                  HWND_TOP,
                  windowRect.left, windowRect.top,
                  windowRect.right - windowRect.left,
                  windowRect.bottom- windowRect.top,
                  SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING |
                  SWP_NOZORDER
    );
}


void fghcbPlatformDisplayWindow ( SFG_Window *window )
{
    RedrawWindow(
        window->Window.Handle, NULL, NULL,
        RDW_NOERASE | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW
    );
}


void fghPlatformSleepForEvents( long msec )
{
    MsgWaitForMultipleObjects( 0, NULL, FALSE, msec, QS_ALLINPUT );
}


void fghProcessSingleEvent ( void )
{
    MSG stMsg;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutMainLoopEvent" );

    while( PeekMessage( &stMsg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        if( GetMessage( &stMsg, NULL, 0, 0 ) == 0 )
        {
            if( fgState.ActionOnWindowClose == GLUT_ACTION_EXIT )
            {
                fgDeinitialize( );
                exit( 0 );
            }
            else if( fgState.ActionOnWindowClose == GLUT_ACTION_GLUTMAINLOOP_RETURNS )
                fgState.ExecState = GLUT_EXEC_STATE_STOP;

            return;
        }

        TranslateMessage( &stMsg );
        DispatchMessage( &stMsg );
    }
}


