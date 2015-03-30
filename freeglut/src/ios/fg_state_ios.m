/*
 * fg_state_ios.c
 *
 * iOS-specific freeglut state query methods.
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
#include <stdio.h>
#include "fg_internal.h"
#include "fg_view_controller.h"

int fgPlatformGlutDeviceGet ( GLenum eWhat )
{
    switch ( eWhat )
    {
    case GLUT_HAS_KEYBOARD:
        /* iOS has a keyboard, though it may be virtual. */
        return 1;

    case GLUT_HAS_MOUSE:
        /* iOS has a touchscreen; until we get proper touchscreen
           support, consider it as a mouse. */
        return 1 ;

    case GLUT_NUM_MOUSE_BUTTONS:
        /* iOS has a touchscreen; until we get proper touchscreen
           support, consider it as a 1-button mouse. */
        return 1;

    default:
        fgWarning( "glutDeviceGet(): missing enum handle %d", eWhat );
        break;
    }

    /* And now -- the failure. */
    return -1;
}

int fgPlatformGlutGet ( GLenum eWhat )
{
    switch ( eWhat ) {
    /* One full-screen window only */
    case GLUT_WINDOW_X:
    case GLUT_WINDOW_Y:
    case GLUT_WINDOW_BORDER_WIDTH:
    case GLUT_WINDOW_HEADER_HEIGHT:
        return 0;

    case GLUT_WINDOW_WIDTH:
    case GLUT_WINDOW_HEIGHT:
        if ( fgStructure.CurrentWindow == NULL ||
             fgStructure.CurrentWindow->Window.Handle == NULL )
            return 0;
        {
            GLKView *view =
                       (GLKView *)fgStructure.CurrentWindow->Window.Handle.view;
            switch ( eWhat )
            {
            case GLUT_WINDOW_WIDTH:
                return ( int ) view.drawableWidth;
            case GLUT_WINDOW_HEIGHT:
                return ( int ) view.drawableHeight;
            }
            break;
        }

    case GLUT_WINDOW_COLORMAP_SIZE:
        /* 0 for RGBA/non-indexed mode */
        return 0;

    default:
        break;
    }
    return -1;
}

int* fgPlatformGlutGetModeValues ( GLenum eWhat, int *size )
{
    return NULL;
}
