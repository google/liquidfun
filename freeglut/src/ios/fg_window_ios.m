/*
 * fg_window_ios.c
 *
 * Window management methods for iOS
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

/*
 * Opens a window. Requires a SFG_Window object created and attached
 * to the freeglut structure. OpenGL context is created here.
 */
void fgPlatformOpenWindow ( SFG_Window* window, const char* title,
                            GLboolean positionUse, int x, int y,
                            GLboolean sizeUse, int w, int h,
                            GLboolean gameMode, GLboolean isSubWindow )
{
}

/*
 * Request a window resize
 */
void fgPlatformReshapeWindow ( SFG_Window *window, int width, int height )
{
    fprintf(stderr, "fgPlatformReshapeWindow: STUB\n");
}

/*
 * Sets current window
 */
void fgPlatformSetWindow ( SFG_Window *window )
{
    [EAGLContext setCurrentContext:window->Window.Context];
}

/*
 * Closes a window, destroying the frame and OpenGL context
 */
void fgPlatformCloseWindow ( SFG_Window* window )
{
    fprintf(stderr, "fgPlatformCloseWindow: STUB\n");
}

/*
 * This function makes the specified window visible
 */
void fgPlatformShowWindow ( void )
{
    fprintf(stderr, "fgPlatformShowWindow: STUB\n");
}

/*
 * This function hides the specified window
 */
void fgPlatformHideWindow ( SFG_Window *window )
{
    fprintf(stderr, "fgPlatformHideWindow: STUB\n");
}

/*
 * Iconify the specified window (top-level windows only)
 */
void fgPlatformIconifyWindow ( SFG_Window *window )
{
    fprintf(stderr, "fgPlatformGlutIconifyWindow: STUB\n");
}

/*
 * Set the current window's title
 */
void fgPlatformGlutSetWindowTitle ( const char* title )
{
    fprintf(stderr, "fgPlatformGlutSetWindowTitle: STUB\n");
}

/*
 * Set the current window's iconified title
 */
void fgPlatformGlutSetIconTitle ( const char* title )
{
    fprintf(stderr, "fgPlatformGlutSetIconTitle: STUB\n");
}

/*
 * Change the specified window's position
 */
void fgPlatformPositionWindow ( SFG_Window *window, int x, int y )
{
    fprintf(stderr, "fgPlatformPositionWindow: STUB\n");
}

/*
 * Lowers the specified window (by Z order change)
 */
void fgPlatformPushWindow ( SFG_Window *window )
{
    fprintf(stderr, "fgPlatformPushWindow: STUB\n");
}

/*
 * Raises the specified window (by Z order change)
 */
void fgPlatformPopWindow ( SFG_Window *window )
{
    fprintf(stderr, "fgPlatformPopWindow: STUB\n");
}

/*
 * Toggle the window's full screen state.
 */
void fgPlatformFullScreenToggle ( SFG_Window *win )
{
    fprintf(stderr, "fgPlatformFullScreenToggle: STUB\n");
}
