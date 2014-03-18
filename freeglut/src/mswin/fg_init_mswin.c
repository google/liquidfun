/*
 * freeglut_init_mswin.c
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

#define FREEGLUT_BUILDING_LIB
#include <GL/freeglut.h>
#include "../fg_internal.h"



extern LRESULT CALLBACK fgPlatformWindowProc( HWND hWnd, UINT uMsg,
                               WPARAM wParam, LPARAM lParam );
extern void fgPlatformInitSystemTime();


/*
 * A call to this function should initialize all the display stuff...
 */
void fgPlatformInitialize( const char* displayName )
{
    WNDCLASS wc;
    ATOM atom;

    /* What we need to do is to initialize the fgDisplay global structure here. */
    fgDisplay.pDisplay.Instance = GetModuleHandle( NULL );
    fgDisplay.pDisplay.DisplayName= displayName ? strdup(displayName) : 0 ;
    atom = (ATOM)GetClassInfo( fgDisplay.pDisplay.Instance, _T("FREEGLUT"), &wc );

    if( atom == 0 )
    {
        ZeroMemory( &wc, sizeof(WNDCLASS) );

        /*
         * Each of the windows should have its own device context, and we
         * want redraw events during Vertical and Horizontal Resizes by
         * the user.
         */
        wc.lpfnWndProc    = fgPlatformWindowProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = 0;
        wc.hInstance      = fgDisplay.pDisplay.Instance;
        wc.hIcon          = LoadIcon( fgDisplay.pDisplay.Instance, _T("GLUT_ICON") );

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
        FREEGLUT_INTERNAL_ERROR_EXIT ( atom, "Window Class Not Registered", "fgPlatformInitialize" );
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
    if( fgDisplay.pDisplay.DisplayName )
    {
        HDC context = CreateDC(fgDisplay.pDisplay.DisplayName,0,0,0);
        if( context )
        {
	    fgDisplay.ScreenWidth  = GetDeviceCaps( context, HORZRES );
	    fgDisplay.ScreenHeight = GetDeviceCaps( context, VERTRES );
	    fgDisplay.ScreenWidthMM  = GetDeviceCaps( context, HORZSIZE );
	    fgDisplay.ScreenHeightMM = GetDeviceCaps( context, VERTSIZE );
	    DeleteDC(context);
        }
        else
	    fgWarning("fgPlatformInitialize: "
		      "CreateDC failed, Screen size info may be incorrect\n"
          "This is quite likely caused by a bad '-display' parameter");
      
    }
    /* Set the timer granularity to 1 ms */
    timeBeginPeriod ( 1 );
    /* Init setup to deal with timer wrap, can't query system time before this is done */
    fgPlatformInitSystemTime();
    /* Get start time */
    fgState.Time = fgSystemTime();


    fgState.Initialised = GL_TRUE;

    /* Avoid registering atexit callback on Win32 as it can result in an
     * access violation due to calling into a module which has been
     * unloaded.
     * Any cleanup isn't needed on Windows anyway, the OS takes care of it.
     * see: http://blogs.msdn.com/b/oldnewthing/archive/2012/01/05/10253268.aspx
     */
/*    atexit(fgDeinitialize); */

    /* InputDevice uses GlutTimerFunc(), so fgState.Initialised must be TRUE */
    fgInitialiseInputDevices();
}



/* Platform-Specific Deinitialization Functions: */
extern void fghCloseInputDevices ( void );

void fgPlatformDeinitialiseInputDevices ( void )
{
#if !defined(_WIN32_WCE)
	fghCloseInputDevices ();
#endif /* !defined(_WIN32_WCE) */
    fgState.JoysticksInitialised = GL_FALSE;
    fgState.InputDevsInitialised = GL_FALSE;
}

void fgPlatformCloseDisplay ( void )
{
    if( fgDisplay.pDisplay.DisplayName )
    {
        free( fgDisplay.pDisplay.DisplayName );
        fgDisplay.pDisplay.DisplayName = NULL;
    }

    /* Reset the timer granularity */
    timeEndPeriod ( 1 );
}

void fgPlatformDestroyContext ( SFG_PlatformDisplay pDisplay, SFG_WindowContextType MContext )
{
	/* Do nothing -- this is required for X11 */
    (void)pDisplay;
    (void)MContext;
}

/* -- PLATFORM-SPECIFIC INTERFACE FUNCTION -------------------------------------------------- */

void (__cdecl *__glutExitFunc)( int return_value ) = NULL;

void FGAPIENTRY __glutInitWithExit( int *pargc, char **argv, void (__cdecl *exit_function)(int) )
{
  __glutExitFunc = exit_function;
  glutInit(pargc, argv);
}

