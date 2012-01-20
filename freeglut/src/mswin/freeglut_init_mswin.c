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
#include "freeglut_internal_mswin.h"


/*
 * A call to this function should initialize all the display stuff...
 */
void fghInitialize( const char* displayName )
{
    WNDCLASS wc;
    ATOM atom;

    /* What we need to do is to initialize the fgDisplay global structure here. */
    fgDisplay.Instance = GetModuleHandle( NULL );
    fgDisplay.DisplayName= displayName ? strdup(displayName) : 0 ;
    atom = GetClassInfo( fgDisplay.Instance, _T("FREEGLUT"), &wc );

    if( atom == 0 )
    {
        ZeroMemory( &wc, sizeof(WNDCLASS) );

        /*
         * Each of the windows should have its own device context, and we
         * want redraw events during Vertical and Horizontal Resizes by
         * the user.
         *
         * XXX Old code had "| CS_DBCLCKS" commented out.  Plans for the
         * XXX future?  Dead-end idea?
         */
        wc.lpfnWndProc    = fgWindowProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = 0;
        wc.hInstance      = fgDisplay.Instance;
        wc.hIcon          = LoadIcon( fgDisplay.Instance, _T("GLUT_ICON") );

#if defined(_WIN32_WCE)
        wc.style          = CS_HREDRAW | CS_VREDRAW;
#else
        wc.style          = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        if (!wc.hIcon)
          wc.hIcon        = LoadIcon( NULL, IDI_WINLOGO );
#endif

        wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground  = NULL;
        wc.lpszMenuName   = NULL;
        wc.lpszClassName  = _T("FREEGLUT");

        /* Register the window class */
        atom = RegisterClass( &wc );
        FREEGLUT_INTERNAL_ERROR_EXIT ( atom, "Window Class Not Registered", "fghInitialize" );
    }

    /* The screen dimensions can be obtained via GetSystemMetrics() calls */
    fgDisplay.ScreenWidth  = GetSystemMetrics( SM_CXSCREEN );
    fgDisplay.ScreenHeight = GetSystemMetrics( SM_CYSCREEN );

    {
        HWND desktop = GetDesktopWindow( );
        HDC  context = GetDC( desktop );

        fgDisplay.ScreenWidthMM  = GetDeviceCaps( context, HORZSIZE );
        fgDisplay.ScreenHeightMM = GetDeviceCaps( context, VERTSIZE );

        ReleaseDC( desktop, context );
    }
    /* If we have a DisplayName try to use it for metrics */
    if( fgDisplay.DisplayName )
    {
        HDC context = CreateDC(fgDisplay.DisplayName,0,0,0);
        if( context )
        {
	    fgDisplay.ScreenWidth  = GetDeviceCaps( context, HORZRES );
	    fgDisplay.ScreenHeight = GetDeviceCaps( context, VERTRES );
	    fgDisplay.ScreenWidthMM  = GetDeviceCaps( context, HORZSIZE );
	    fgDisplay.ScreenHeightMM = GetDeviceCaps( context, VERTSIZE );
	    DeleteDC(context);
        }
        else
	    fgWarning("fghInitialize: "
		      "CreateDC failed, Screen size info may be incorrect\n"
          "This is quite likely caused by a bad '-display' parameter");
      
    }
    /* Set the timer granularity to 1 ms */
    timeBeginPeriod ( 1 );


    fgState.Initialised = GL_TRUE;

    /* Avoid registering atexit callback on Win32 as it results in an access
     * violation due to calling into a module which has been unloaded.
     * Any cleanup isn't needed on Windows anyway, the OS takes care of it.c
     * see: http://blogs.msdn.com/b/oldnewthing/archive/2012/01/05/10253268.aspx
     */
/*    atexit(fgDeinitialize); */

    /* InputDevice uses GlutTimerFunc(), so fgState.Initialised must be TRUE */
    fgInitialiseInputDevices();
}



/* Platform-Specific Deinitialization Functions: */
void fghDeinitialiseInputDevices ( void )
{
#if !defined(_WIN32_WCE)
    if ( fgState.JoysticksInitialised )
        fgJoystickClose( );

    if ( fgState.InputDevsInitialised )
        fgInputDeviceClose( );
#endif /* !defined(_WIN32_WCE) */
    fgState.JoysticksInitialised = GL_FALSE;
    fgState.InputDevsInitialised = GL_FALSE;
}

void fghCloseDisplay ( void )
{
    if( fgDisplay.DisplayName )
    {
        free( fgDisplay.DisplayName );
        fgDisplay.DisplayName = NULL;
    }

    /* Reset the timer granularity */
    timeEndPeriod ( 1 );
}





