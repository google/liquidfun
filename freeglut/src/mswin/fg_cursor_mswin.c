/*
 * freeglut_cursor_mswin.c
 *
 * The Windows-specific mouse cursor related stuff.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by John F. Fay, <fayjf@sourceforge.net>
 * Creation date: Thu Jan 19, 2012
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



void fgPlatformSetCursor ( SFG_Window *window, int cursorID )
{
    /*
     * Joe Krahn is re-writing the following code.
     */
    /* Set the cursor AND change it for this window class. */
#if !defined(__MINGW64__) && _MSC_VER <= 1200
#       define MAP_CURSOR(a,b)                                   \
        case a:                                                  \
            SetCursor( LoadCursor( NULL, b ) );                  \
            SetClassLong( window->Window.Handle,                 \
                          GCL_HCURSOR,                           \
                          ( LONG )LoadCursor( NULL, b ) );       \
        break;
    /* Nuke the cursor AND change it for this window class. */
#       define ZAP_CURSOR(a,b)                                   \
        case a:                                                  \
            SetCursor( NULL );                                   \
            SetClassLong( window->Window.Handle,                 \
                          GCL_HCURSOR, ( LONG )NULL );           \
        break;
#else
#       define MAP_CURSOR(a,b)                                   \
        case a:                                                  \
            SetCursor( LoadCursor( NULL, b ) );                  \
            SetClassLongPtr( window->Window.Handle,              \
                          GCLP_HCURSOR,                          \
                          ( LONG )( LONG_PTR )LoadCursor( NULL, b ) );       \
        break;
    /* Nuke the cursor AND change it for this window class. */
#       define ZAP_CURSOR(a,b)                                   \
        case a:                                                  \
            SetCursor( NULL );                                   \
            SetClassLongPtr( window->Window.Handle,              \
                          GCLP_HCURSOR, ( LONG )( LONG_PTR )NULL );          \
        break;
#endif

    switch( cursorID )
    {
        MAP_CURSOR( GLUT_CURSOR_RIGHT_ARROW,         IDC_ARROW     );
        MAP_CURSOR( GLUT_CURSOR_LEFT_ARROW,          IDC_ARROW     ); /* XXX ToDo */
        MAP_CURSOR( GLUT_CURSOR_INFO,                IDC_HELP      );
        MAP_CURSOR( GLUT_CURSOR_DESTROY,             IDC_CROSS     );
        MAP_CURSOR( GLUT_CURSOR_HELP,                IDC_HELP      );
        MAP_CURSOR( GLUT_CURSOR_CYCLE,               IDC_SIZEALL   );
        MAP_CURSOR( GLUT_CURSOR_SPRAY,               IDC_CROSS     );
        MAP_CURSOR( GLUT_CURSOR_WAIT,                IDC_WAIT      );
        MAP_CURSOR( GLUT_CURSOR_TEXT,                IDC_IBEAM     );
        MAP_CURSOR( GLUT_CURSOR_CROSSHAIR,           IDC_CROSS     );
        MAP_CURSOR( GLUT_CURSOR_UP_DOWN,             IDC_SIZENS    );
        MAP_CURSOR( GLUT_CURSOR_LEFT_RIGHT,          IDC_SIZEWE    );
        MAP_CURSOR( GLUT_CURSOR_TOP_SIDE,            IDC_UPARROW   );
        MAP_CURSOR( GLUT_CURSOR_BOTTOM_SIDE,         IDC_ARROW     ); /* XXX ToDo */
        MAP_CURSOR( GLUT_CURSOR_LEFT_SIDE,           IDC_ARROW     ); /* XXX ToDo */
        MAP_CURSOR( GLUT_CURSOR_RIGHT_SIDE,          IDC_ARROW     ); /* XXX ToDo */
        MAP_CURSOR( GLUT_CURSOR_TOP_LEFT_CORNER,     IDC_SIZENWSE  );
        MAP_CURSOR( GLUT_CURSOR_TOP_RIGHT_CORNER,    IDC_SIZENESW  );
        MAP_CURSOR( GLUT_CURSOR_BOTTOM_RIGHT_CORNER, IDC_SIZENWSE  );
        MAP_CURSOR( GLUT_CURSOR_BOTTOM_LEFT_CORNER,  IDC_SIZENESW  );
        ZAP_CURSOR( GLUT_CURSOR_NONE,                NULL          );
        MAP_CURSOR( GLUT_CURSOR_FULL_CROSSHAIR,      IDC_CROSS     ); /* XXX ToDo */
    case GLUT_CURSOR_INHERIT:
        {
            SFG_Window *temp_window = window;
            while (temp_window->Parent)
            {
                temp_window = temp_window->Parent;
                if (temp_window->State.Cursor != GLUT_CURSOR_INHERIT)
                {
                    fgPlatformSetCursor(window,temp_window->State.Cursor);
                    return;
                }
            }
            /* No parent, or no parent with cursor type set. Fall back to default */
            fgPlatformSetCursor(window,GLUT_CURSOR_LEFT_ARROW);
        }
        break;

    default:
        fgError( "Unknown cursor type: %d", cursorID );
        break;
    }
}


void fgPlatformWarpPointer ( int x, int y )
{
    POINT coords;
    coords.x = x;
    coords.y = y;

    /* ClientToScreen() translates {coords} for us. */
    ClientToScreen( fgStructure.CurrentWindow->Window.Handle, &coords );
    SetCursorPos( coords.x, coords.y );
}


void fghPlatformGetCursorPos(const SFG_Window *window, GLboolean client, SFG_XYUse *mouse_pos)
{
    /* Get current pointer location in screen coordinates (if client is false or window is NULL), else
     * Get current pointer location relative to top-left of client area of window (if client is true and window is not NULL)
     */
    POINT pos;
    GetCursorPos(&pos);

    /* convert to client coords if wanted */
    if (client && window && window->Window.Handle)
        ScreenToClient(window->Window.Handle,&pos);

    mouse_pos->X = pos.x;
    mouse_pos->Y = pos.y;
    mouse_pos->Use = GL_TRUE;
}