/*
 * freeglut_cursor.c
 *
 * The mouse cursor related stuff.
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

#include <GL/freeglut.h>
#include "fg_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *  glutSetCursor()     -- Win32 mappings are incomplete.
 *
 * It would be good to use custom mouse cursor shapes, and introduce
 * an option to display them using glBitmap() and/or texture mapping,
 * apart from the windowing system version.
 */

/* -- PRIVATE FUNCTIONS --------------------------------------------------- */

extern void fgPlatformSetCursor ( SFG_Window *window, int cursorID );
extern void fgPlatformWarpPointer ( int x, int y );



/* -- INTERNAL FUNCTIONS ---------------------------------------------------- */
void fgSetCursor ( SFG_Window *window, int cursorID )
{
    fgPlatformSetCursor ( window, cursorID );
}


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Set the cursor image to be used for the current window
 */
void FGAPIENTRY glutSetCursor( int cursorID )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSetCursor" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutSetCursor" );

    fgPlatformSetCursor ( fgStructure.CurrentWindow, cursorID );
    fgStructure.CurrentWindow->State.Cursor = cursorID;
}

/*
 * Moves the mouse pointer to given window coordinates
 */
void FGAPIENTRY glutWarpPointer( int x, int y )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutWarpPointer" );
    FREEGLUT_EXIT_IF_NO_WINDOW ( "glutWarpPointer" );

    fgPlatformWarpPointer ( x, y );
}

/*** END OF FILE ***/
