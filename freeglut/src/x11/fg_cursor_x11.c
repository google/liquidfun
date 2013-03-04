/*
 * freeglut_cursor_x11.c
 *
 * The Windows-specific mouse cursor related stuff.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by John F. Fay, <fayjf@sourceforge.net>
 * Creation date: Sun Feb 5, 2012
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
#include "../fg_internal.h"

/* This code is for Posix/X11, Solaris, and OSX */
#include <X11/cursorfont.h>

/*
 * A factory method for an empty cursor
 */
static Cursor getEmptyCursor( void )
{
    static Cursor cursorNone = None;
    if( cursorNone == None ) {
        char cursorNoneBits[ 32 ];
        XColor dontCare;
        Pixmap cursorNonePixmap;
        memset( cursorNoneBits, 0, sizeof( cursorNoneBits ) );
        memset( &dontCare, 0, sizeof( dontCare ) );
        cursorNonePixmap = XCreateBitmapFromData ( fgDisplay.pDisplay.Display,
                                                   fgDisplay.pDisplay.RootWindow,
                                                   cursorNoneBits, 16, 16 );
        if( cursorNonePixmap != None ) {
            cursorNone = XCreatePixmapCursor( fgDisplay.pDisplay.Display,
                                              cursorNonePixmap, cursorNonePixmap,
                                              &dontCare, &dontCare, 0, 0 );
            XFreePixmap( fgDisplay.pDisplay.Display, cursorNonePixmap );
        }
    }
    return cursorNone;
}

typedef struct tag_cursorCacheEntry cursorCacheEntry;
struct tag_cursorCacheEntry {
    unsigned int cursorShape;    /* an XC_foo value */
    Cursor cachedCursor;         /* None if the corresponding cursor has
                                    not been created yet */
};

/*
 * Note: The arrangement of the table below depends on the fact that
 * the "normal" GLUT_CURSOR_* values start a 0 and are consecutive.
 */ 
static cursorCacheEntry cursorCache[] = {
    { XC_arrow,               None }, /* GLUT_CURSOR_RIGHT_ARROW */
    { XC_top_left_arrow,      None }, /* GLUT_CURSOR_LEFT_ARROW */
    { XC_hand1,               None }, /* GLUT_CURSOR_INFO */
    { XC_pirate,              None }, /* GLUT_CURSOR_DESTROY */
    { XC_question_arrow,      None }, /* GLUT_CURSOR_HELP */
    { XC_exchange,            None }, /* GLUT_CURSOR_CYCLE */
    { XC_spraycan,            None }, /* GLUT_CURSOR_SPRAY */
    { XC_watch,               None }, /* GLUT_CURSOR_WAIT */
    { XC_xterm,               None }, /* GLUT_CURSOR_TEXT */
    { XC_crosshair,           None }, /* GLUT_CURSOR_CROSSHAIR */
    { XC_sb_v_double_arrow,   None }, /* GLUT_CURSOR_UP_DOWN */
    { XC_sb_h_double_arrow,   None }, /* GLUT_CURSOR_LEFT_RIGHT */
    { XC_top_side,            None }, /* GLUT_CURSOR_TOP_SIDE */
    { XC_bottom_side,         None }, /* GLUT_CURSOR_BOTTOM_SIDE */
    { XC_left_side,           None }, /* GLUT_CURSOR_LEFT_SIDE */
    { XC_right_side,          None }, /* GLUT_CURSOR_RIGHT_SIDE */
    { XC_top_left_corner,     None }, /* GLUT_CURSOR_TOP_LEFT_CORNER */
    { XC_top_right_corner,    None }, /* GLUT_CURSOR_TOP_RIGHT_CORNER */
    { XC_bottom_right_corner, None }, /* GLUT_CURSOR_BOTTOM_RIGHT_CORNER */
    { XC_bottom_left_corner,  None }  /* GLUT_CURSOR_BOTTOM_LEFT_CORNER */
};

void fgPlatformSetCursor ( SFG_Window *window, int cursorID )
{
    Cursor cursor;
    /*
     * XXX FULL_CROSSHAIR demotes to plain CROSSHAIR. Old GLUT allows
     * for this, but if there is a system that easily supports a full-
     * window (or full-screen) crosshair, we might consider it.
     */
    int cursorIDToUse =
        ( cursorID == GLUT_CURSOR_FULL_CROSSHAIR ) ? GLUT_CURSOR_CROSSHAIR : cursorID;

    if( ( cursorIDToUse >= 0 ) &&
        ( cursorIDToUse < sizeof( cursorCache ) / sizeof( cursorCache[0] ) ) ) {
        cursorCacheEntry *entry = &cursorCache[ cursorIDToUse ];
        if( entry->cachedCursor == None ) {
            entry->cachedCursor =
                XCreateFontCursor( fgDisplay.pDisplay.Display, entry->cursorShape );
        }
        cursor = entry->cachedCursor;
    } else {
        switch( cursorIDToUse )
        {
        case GLUT_CURSOR_NONE:
            cursor = getEmptyCursor( );
            break;

        case GLUT_CURSOR_INHERIT:
            cursor = None;
            break;

        default:
            fgError( "Unknown cursor type: %d", cursorIDToUse );
            return;
        }
    }

    if ( cursorIDToUse == GLUT_CURSOR_INHERIT ) {
        XUndefineCursor( fgDisplay.pDisplay.Display, window->Window.Handle );
    } else if ( cursor != None ) {
        XDefineCursor( fgDisplay.pDisplay.Display, window->Window.Handle, cursor );
    } else if ( cursorIDToUse != GLUT_CURSOR_NONE ) {
        fgError( "Failed to create cursor" );
    }
}


void fgPlatformWarpPointer ( int x, int y )
{
    XWarpPointer(
        fgDisplay.pDisplay.Display,
        None,
        fgStructure.CurrentWindow->Window.Handle,
        0, 0, 0, 0,
        x, y
    );
    /* Make the warp visible immediately. */
    XFlush( fgDisplay.pDisplay.Display );
}

void fghPlatformGetCursorPos(const SFG_Window *window, GLboolean client, SFG_XYUse *mouse_pos)
{
    /* Get current pointer location in screen coordinates (if client is false or window is NULL), else
     * Get current pointer location relative to top-left of client area of window (if client is true and window is not NULL)
     */
    Window w = (client && window && window->Window.Handle)? window->Window.Handle: fgDisplay.pDisplay.RootWindow;
    Window junk_window;
    unsigned int junk_mask;
    int clientX, clientY;

    XQueryPointer(fgDisplay.pDisplay.Display, w,
            &junk_window, &junk_window,
            &mouse_pos->X, &mouse_pos->Y, /* Screen coords relative to root window's top-left */
            &clientX, &clientY,           /* Client coords relative to window's top-left */
            &junk_mask);

    if (client && window && window->Window.Handle)
    {
        mouse_pos->X = clientX;
        mouse_pos->Y = clientY;
    }

    mouse_pos->Use = GL_TRUE;
}
