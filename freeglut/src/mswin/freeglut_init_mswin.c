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
#include "../Common/freeglut_internal.h"



extern LRESULT CALLBACK fgPlatformWindowProc( HWND hWnd, UINT uMsg,
                               WPARAM wParam, LPARAM lParam );


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
    atom = GetClassInfo( fgDisplay.pDisplay.Instance, _T("FREEGLUT"), &wc );

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
}

/*
 * Everything down to the end of the next two functions is copied from the X sources.
 */

/*

Copyright 1985, 1986, 1987,1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/

#define NoValue         0x0000
#define XValue          0x0001
#define YValue          0x0002
#define WidthValue      0x0004
#define HeightValue     0x0008
#define AllValues       0x000F
#define XNegative       0x0010
#define YNegative       0x0020

/*
 *    XParseGeometry parses strings of the form
 *   "=<width>x<height>{+-}<xoffset>{+-}<yoffset>", where
 *   width, height, xoffset, and yoffset are unsigned integers.
 *   Example:  "=80x24+300-49"
 *   The equal sign is optional.
 *   It returns a bitmask that indicates which of the four values
 *   were actually found in the string.  For each value found,
 *   the corresponding argument is updated;  for each value
 *   not found, the corresponding argument is left unchanged.
 */

static int
ReadInteger(char *string, char **NextString)
{
    register int Result = 0;
    int Sign = 1;

    if (*string == '+')
        string++;
    else if (*string == '-')
    {
        string++;
        Sign = -1;
    }
    for (; (*string >= '0') && (*string <= '9'); string++)
    {
        Result = (Result * 10) + (*string - '0');
    }
    *NextString = string;
    if (Sign >= 0)
        return Result;
    else
        return -Result;
}

int XParseGeometry (
    const char *string,
    int *x,
    int *y,
    unsigned int *width,    /* RETURN */
    unsigned int *height)    /* RETURN */
{
    int mask = NoValue;
    register char *strind;
    unsigned int tempWidth = 0, tempHeight = 0;
    int tempX = 0, tempY = 0;
    char *nextCharacter;

    if ( (string == NULL) || (*string == '\0'))
      return mask;
    if (*string == '=')
        string++;  /* ignore possible '=' at beg of geometry spec */

    strind = (char *)string;
    if (*strind != '+' && *strind != '-' && *strind != 'x') {
        tempWidth = ReadInteger(strind, &nextCharacter);
        if (strind == nextCharacter)
            return 0;
        strind = nextCharacter;
        mask |= WidthValue;
    }

    if (*strind == 'x' || *strind == 'X') {
        strind++;
        tempHeight = ReadInteger(strind, &nextCharacter);
        if (strind == nextCharacter)
            return 0;
        strind = nextCharacter;
        mask |= HeightValue;
    }

    if ((*strind == '+') || (*strind == '-')) {
        if (*strind == '-') {
            strind++;
            tempX = -ReadInteger(strind, &nextCharacter);
            if (strind == nextCharacter)
                return 0;
            strind = nextCharacter;
            mask |= XNegative;
        }
        else
        {
            strind++;
            tempX = ReadInteger(strind, &nextCharacter);
            if (strind == nextCharacter)
                return 0;
            strind = nextCharacter;
        }
        mask |= XValue;
        if ((*strind == '+') || (*strind == '-')) {
            if (*strind == '-') {
                strind++;
                tempY = -ReadInteger(strind, &nextCharacter);
                if (strind == nextCharacter)
                    return 0;
                strind = nextCharacter;
                mask |= YNegative;
            }
            else
            {
                strind++;
                tempY = ReadInteger(strind, &nextCharacter);
                if (strind == nextCharacter)
                    return 0;
                strind = nextCharacter;
            }
            mask |= YValue;
        }
    }

    /* If strind isn't at the end of the string the it's an invalid
       geometry specification. */

    if (*strind != '\0') return 0;

    if (mask & XValue)
        *x = tempX;
    if (mask & YValue)
        *y = tempY;
    if (mask & WidthValue)
        *width = tempWidth;
    if (mask & HeightValue)
        *height = tempHeight;
    return mask;
}



/* -- PLATFORM-SPECIFIC INTERFACE FUNCTION -------------------------------------------------- */

void (__cdecl *__glutExitFunc)( int return_value ) = NULL;

void FGAPIENTRY __glutInitWithExit( int *pargc, char **argv, void (__cdecl *exit_function)(int) )
{
  __glutExitFunc = exit_function;
  glutInit(pargc, argv);
}

