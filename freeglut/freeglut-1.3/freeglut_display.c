/*
 * freeglut_display.c
 *
 * Display message posting, context buffer swapping.
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

#define  G_LOG_DOMAIN  "freeglut-display"

#include "../include/GL/freeglut.h"
#include "freeglut_internal.h"


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Marks the current window to have the redisplay performed when possible...
 */
void FGAPIENTRY glutPostRedisplay( void )
{
    /*
     * Is there a current window set?
     */
    freeglut_assert_ready; freeglut_assert_window;

    /*
     * Just mark the window as one that we need to redisplay...
     */
    fgStructure.Window->State.Redisplay = TRUE;
}

/*
 * Swaps the buffers for the current window (if any)
 */
void FGAPIENTRY glutSwapBuffers( void )
{
    /*
     * As long as we've got a current window set...
     */
    freeglut_assert_ready; freeglut_assert_window;

    /*
     * Have the mouse cursor and/or the menus drawn for the current window
     */
    fgDisplayMenu();
    fgDisplayCursor();

    /*
     * Make sure the current context is flushed
     */
    glFlush();

    /*
     * If it's single-buffered, we shouldn't be here.
     */
    if ( ! fgStructure.Window->Window.DoubleBuffered ) return ;

#if TARGET_HOST_UNIX_X11
    /*
     * Issue the glXSwapBuffers call and be done with it
     */
    glXSwapBuffers( fgDisplay.Display, fgStructure.Window->Window.Handle );

#elif TARGET_HOST_WIN32
    /*
     * Swap the window's buffers
     */
    SwapBuffers( fgStructure.Window->Window.Device );

#endif

    /* GLUT_FPS env var support */
    if (fgState.FPSInterval) {
        GLint t = glutGet(GLUT_ELAPSED_TIME);
        fgState.SwapCount++;
        if (fgState.SwapTime == 0)
            fgState.SwapTime = t;
        else if (t - fgState.SwapTime > fgState.FPSInterval) {
            float time = 0.001 * (t - fgState.SwapTime);
            float fps = (float) fgState.SwapCount / time;
            fprintf(stderr, "FreeGLUT: %d frames in %.2f seconds = %.2f FPS\n",
                    fgState.SwapCount, time, fps);
            fgState.SwapTime = t;
            fgState.SwapCount = 0;
        }
    }
}

/*
 * Mark appropriate window to be displayed
 */
void FGAPIENTRY glutPostWindowRedisplay( int windowID )
{
    SFG_Window* window;

    freeglut_assert_ready;

    /*
     * Try looking for the window
     */
    window = fgWindowByID( windowID );

    /*
     * If failed, return
     */
    freeglut_return_if_fail( window != NULL );

    /*
     * Otherwise mark the window for being redisplayed
     */
    window->State.Redisplay = TRUE;
}

/*** END OF FILE ***/
