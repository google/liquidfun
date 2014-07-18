/*
 * freeglut_menu_mswin.c
 *
 * The Windows-specific mouse cursor related stuff.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by John F. Fay, <fayjf@sourceforge.net>
 * Creation date: Sun Jan 22, 2012
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

#define FREEGLUT_BUILDING_LIB
#include <GL/freeglut.h>
#include "../fg_internal.h"



GLvoid fgPlatformGetGameModeVMaxExtent( SFG_Window* window, int* x, int* y )
{
    (void)window;
    *x = glutGet ( GLUT_SCREEN_WIDTH );
    *y = glutGet ( GLUT_SCREEN_HEIGHT );
}

void fgPlatformCheckMenuDeactivate(HWND newFocusWnd)
{
    /* User/system switched application focus.
     * If we have an open menu, close it.
     */
    SFG_Menu* menu = NULL;

    if ( fgState.ActiveMenus )
        menu = fgGetActiveMenu();

    if ( menu )
    {
        if (newFocusWnd != menu->Window->Window.Handle)
            /* When in GameMode, the menu's parent window will lose focus when the menu is opened.
             * This is sadly necessary as we need to do an activating ShowWindow() for the menu
             * to pop up over the gamemode window
             */
            fgDeactivateMenu(menu->ParentWindow);
    }
};



/* -- PLATFORM-SPECIFIC INTERFACE FUNCTION -------------------------------------------------- */

int FGAPIENTRY __glutCreateMenuWithExit( void(* callback)( int ), void (__cdecl *exit_function)(int) )
{
  __glutExitFunc = exit_function;
  return glutCreateMenu( callback );
}

