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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../include/GL/freeglut.h"
#include "freeglut_internal.h"

#if TARGET_HOST_UNIX_X11
  #include <X11/cursorfont.h>
#endif

/*
 * TODO BEFORE THE STABLE RELEASE:
 *  glutSetCursor()     -- Win32 mappings are incomplete.
 *
 * It would be good to use custom mouse cursor shapes, and introduce
 * an option to display them using glBitmap() and/or texture mapping,
 * apart from the windowing system version.
 */

/* -- INTERNAL FUNCTIONS --------------------------------------------------- */

#if TARGET_HOST_UNIX_X11

fgGetCursorError( Cursor cursor )
{
    int ret = 0;
    char buf[ 256 ];
    
    switch( cursor )
    {
    case BadAlloc:
    case BadFont:
    case BadMatch:
    case BadPixmap:
    case BadValue:
        XGetErrorText( fgDisplay.Display, cursor, buf, sizeof buf );
        fgWarning( "Error in setting cursor:\n %s.", buf );
        ret = cursor;
        break;
    default:
        /* no error */
        break;
    }

    return ret;
}

#endif


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Set the cursor image to be used for the current window
 */
void FGAPIENTRY glutSetCursor( int cursorID )
{
    freeglut_assert_ready;  /* XXX WHY do we need the timer active for this? */
    freeglut_assert_window;

#if TARGET_HOST_UNIX_X11
    /*
     * Open issues:
     * (a) Partial error checking.  Is that a problem?
     *     Is fgGetCursorError() correct?  Should we abort on errors?
     *     Should there be a freeglut-wide X error handler?  Should
     *     we use the X error-handler mechanism?
     * (b) FULL_CROSSHAIR demotes to plain CROSSHAIR.  Old GLUT allows
     *     for this, but if there is a system that easily supports a full-
     *     window (or full-screen) crosshair, we might consider it.
     * (c) Out-of-range cursor-types generate warnings.  Should we abort?
     */
    {
        Cursor cursor;
        Pixmap no_cursor;  /* Used for GLUT_CURSOR_NONE */
        int error = 0;

#define MAP_CURSOR(a,b)                                     \
    case a:                                                 \
        cursor = XCreateFontCursor( fgDisplay.Display, b ); \
        break;

        if( GLUT_CURSOR_FULL_CROSSHAIR == cursorID )
            cursorID = GLUT_CURSOR_CROSSHAIR;
        
        switch( cursorID )
        {
            MAP_CURSOR( GLUT_CURSOR_RIGHT_ARROW, XC_right_ptr);
            MAP_CURSOR( GLUT_CURSOR_LEFT_ARROW,  XC_left_ptr);
            MAP_CURSOR( GLUT_CURSOR_INFO,        XC_hand1);
            MAP_CURSOR( GLUT_CURSOR_DESTROY,     XC_pirate);
            MAP_CURSOR( GLUT_CURSOR_HELP,        XC_question_arrow);
            MAP_CURSOR( GLUT_CURSOR_CYCLE,       XC_exchange);
            MAP_CURSOR( GLUT_CURSOR_SPRAY,       XC_spraycan);
            MAP_CURSOR( GLUT_CURSOR_WAIT,        XC_watch);
            MAP_CURSOR( GLUT_CURSOR_TEXT,        XC_xterm);
            MAP_CURSOR( GLUT_CURSOR_CROSSHAIR,   XC_crosshair);
            MAP_CURSOR( GLUT_CURSOR_UP_DOWN,     XC_sb_v_double_arrow);
            MAP_CURSOR( GLUT_CURSOR_LEFT_RIGHT,  XC_sb_h_double_arrow);
            MAP_CURSOR( GLUT_CURSOR_TOP_SIDE,    XC_top_side);
            MAP_CURSOR( GLUT_CURSOR_BOTTOM_SIDE, XC_bottom_side);
            MAP_CURSOR( GLUT_CURSOR_LEFT_SIDE,   XC_left_side);
            MAP_CURSOR( GLUT_CURSOR_RIGHT_SIDE,  XC_right_side);
            MAP_CURSOR( GLUT_CURSOR_TOP_LEFT_CORNER,     XC_top_left_corner);
            MAP_CURSOR( GLUT_CURSOR_TOP_RIGHT_CORNER,    XC_top_right_corner);
            MAP_CURSOR( GLUT_CURSOR_BOTTOM_RIGHT_CORNER,
                        XC_bottom_right_corner);
            MAP_CURSOR( GLUT_CURSOR_BOTTOM_LEFT_CORNER, XC_bottom_left_corner);
            /* MAP_CURSOR( GLUT_CURSOR_NONE,        XC_bogosity); */

        case GLUT_CURSOR_NONE:
        {
            static unsigned char no_cursor_bits[ 32 ];
            XColor black;
            no_cursor = XCreatePixmapFromBitmapData( fgDisplay.Display,
                                                     fgDisplay.RootWindow,
                                                     no_cursor_bits,
                                                     16, 16,
                                                     1, 0, 1 );
            XParseColor( fgDisplay.Display,
                         DefaultColormap( fgDisplay.Display,
                                          DefaultScreen( fgDisplay.Display ) ),
                         "black",
                         &black );
            cursor = XCreatePixmapCursor( fgDisplay.Display,
                                          no_cursor, no_cursor,
                                          &black, &black,
                                          0, 0 );
            break;
        }
        
        case GLUT_CURSOR_INHERIT:
            break;

        default:
            fgWarning( "Unknown cursor type: %d\n", cursorID );
            return;
        }

        error = fgGetCursorError( cursor );

        if( GLUT_CURSOR_INHERIT == cursorID )
            XUndefineCursor( fgDisplay.Display,
                             fgStructure.Window->Window.Handle );
        else
        {
            XDefineCursor( fgDisplay.Display,
                           fgStructure.Window->Window.Handle, cursor );
            XFreeCursor( fgDisplay.Display, cursor );
            if( GLUT_CURSOR_NONE == cursorID )
                XFreePixmap( fgDisplay.Display, no_cursor );
        }
    }

#elif TARGET_HOST_WIN32

    /*
     * This is a temporary solution only...
     */
    /* Set the cursor AND change it for this window class. */
#       define MAP_CURSOR(a,b)                                   \
        case a:                                                  \
            SetCursor( LoadCursor( NULL, b ) );                  \
            SetClassLong( fgStructure.Window->Window.Handle,     \
                          GCL_HCURSOR,                           \
                          ( LONG )LoadCursor( NULL, b ) );       \
        break;

        /* Nuke the cursor AND change it for this window class. */
#       define ZAP_CURSOR(a,b)                                   \
        case a:                                                  \
            SetCursor( NULL );                                   \
            SetClassLong( fgStructure.Window->Window.Handle,     \
                          GCL_HCURSOR, ( LONG )NULL );           \
        break;

        switch( cursorID )
        {
            MAP_CURSOR( GLUT_CURSOR_RIGHT_ARROW, IDC_ARROW     );
            MAP_CURSOR( GLUT_CURSOR_LEFT_ARROW,  IDC_ARROW     );
            MAP_CURSOR( GLUT_CURSOR_INFO,        IDC_HELP      );
            MAP_CURSOR( GLUT_CURSOR_DESTROY,     IDC_CROSS     );
            MAP_CURSOR( GLUT_CURSOR_HELP,        IDC_HELP      );
            MAP_CURSOR( GLUT_CURSOR_CYCLE,       IDC_SIZEALL   );
            MAP_CURSOR( GLUT_CURSOR_SPRAY,       IDC_CROSS     );
            MAP_CURSOR( GLUT_CURSOR_WAIT,        IDC_WAIT      );
            MAP_CURSOR( GLUT_CURSOR_TEXT,        IDC_UPARROW   );
            MAP_CURSOR( GLUT_CURSOR_CROSSHAIR,   IDC_CROSS     );
            /* MAP_CURSOR( GLUT_CURSOR_NONE,        IDC_NO        ); */
            ZAP_CURSOR( GLUT_CURSOR_NONE,        NULL           );
            
        default:
            MAP_CURSOR( GLUT_CURSOR_UP_DOWN,     IDC_ARROW     );
        }
#endif

  fgStructure.Window->State.Cursor = cursorID;
}

/*
 * Moves the mouse pointer to given window coordinates
 */
void FGAPIENTRY glutWarpPointer( int x, int y )
{
    freeglut_assert_ready; /* XXX WHY do we need the timer active for this? */
    freeglut_assert_window;

#if TARGET_HOST_UNIX_X11

    XWarpPointer(
        fgDisplay.Display,
        None,
        fgStructure.Window->Window.Handle,
        0, 0, 0, 0,
        x, y
    );
    XFlush( fgDisplay.Display ); /* XXX Is this really necessary? */

#elif TARGET_HOST_WIN32

    {
        POINT coords = { x, y };
        /*
         * ClientToScreen() translates {coords} for us.
         */
        ClientToScreen( fgStructure.Window->Window.Handle, &coords );
        SetCursorPos( coords.x, coords.y );
    }

#endif
}

/*** END OF FILE ***/
