/*
 * freeglut_internal_mswin.h
 *
 * The freeglut library private include file.
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

#ifndef  FREEGLUT_INTERNAL_MSWIN_H
#define  FREEGLUT_INTERNAL_MSWIN_H


/* All Win32 headers depend on the huge windows.h recursive include.
 * Note: Lower-case header names are used, for best cross-platform
 * compatibility.
 */
#if !defined(_WIN32_WCE)
#   include <windows.h>
#   include <windowsx.h>
#   include <mmsystem.h>
/* CYGWIN does not have tchar.h, but has TEXT(x), defined in winnt.h. */
#   ifndef __CYGWIN__
#       include <tchar.h>
#   else
#       define _TEXT(x) TEXT(x)
#       define _T(x)    TEXT(x)
#   endif

#endif


#ifndef HAVE_VFPRINTF
#define  HAVE_VFPRINTF 1
#endif

/* MinGW may lack a prototype for ChangeDisplaySettingsEx() (depending on the version?) */
#if !defined(ChangeDisplaySettingsEx)
LONG WINAPI ChangeDisplaySettingsExA(LPCSTR,LPDEVMODEA,HWND,DWORD,LPVOID);
LONG WINAPI ChangeDisplaySettingsExW(LPCWSTR,LPDEVMODEW,HWND,DWORD,LPVOID);
#   ifdef UNICODE
#       define ChangeDisplaySettingsEx ChangeDisplaySettingsExW
#   else
#       define ChangeDisplaySettingsEx ChangeDisplaySettingsExA
#   endif
#endif


/* Structure Definitions */

typedef struct tagSFG_PlatformDisplay SFG_PlatformDisplay;
struct tagSFG_PlatformDisplay
{
    HINSTANCE       Instance;           /* The application's instance */
    DEVMODE         DisplayMode;        /* Desktop's display settings */
    char           *DisplayName;        /* Display name for multi display support*/ 
};

/*
 * Make "freeglut" window handle and context types so that we don't need so
 * much conditionally-compiled code later in the library.
 */
typedef HWND    SFG_WindowHandleType;
typedef HGLRC   SFG_WindowContextType;
typedef struct tagSFG_PlatformContext SFG_PlatformContext;
struct tagSFG_PlatformContext
{
    HDC             Device;             /* The window's device context */
};


/* Window's state description. This structure should be kept portable. */
typedef struct tagSFG_PlatformWindowState SFG_PlatformWindowState;
struct tagSFG_PlatformWindowState
{
    RECT            OldRect;            /* window rect     - stored before the window is made fullscreen */
    DWORD           OldStyle;           /* window style    - stored before the window is made fullscreen */
    DWORD           OldStyleEx;         /* window Ex style - stored before the window is made fullscreen */
    BOOL            OldMaximized;       /* window maximized state - stored before the window is made fullscreen */

    BOOL            MouseTracking;      /* Needed for generating GLUT_ENTERED and GLUT_LEFT entry func callbacks on windows */

    /* Need to store window titles to emulate
     * glutSetIconTitle/glutSetWindowTitle as Windows has only
     * one title associated with a window and we need to swap
     * them out based on the window's iconic state
     */
    char*           WindowTitle;
    char*           IconTitle;
};



/* Joystick-Specific Definitions */
#if !defined(_WIN32_WCE)
#   define _JS_MAX_AXES  8
typedef struct tagSFG_PlatformJoystick SFG_PlatformJoystick;
struct tagSFG_PlatformJoystick
{
    JOYCAPS     jsCaps;
    JOYINFOEX   js;
    UINT        js_id;
};
#endif


/* Menu font and color definitions */
#define  FREEGLUT_MENU_FONT    GLUT_BITMAP_8_BY_13

#define  FREEGLUT_MENU_PEN_FORE_COLORS   {0.0f,  0.0f,  0.0f,  1.0f}
#define  FREEGLUT_MENU_PEN_BACK_COLORS   {0.85f, 0.85f, 0.85f, 1.0f}
#define  FREEGLUT_MENU_PEN_HFORE_COLORS  {1.0f,  1.0f,  1.0f,  1.0f}
#define  FREEGLUT_MENU_PEN_HBACK_COLORS  {0.15f, 0.15f, 0.45f, 1.0f}


/* Function to be called on exit */
extern void (__cdecl *__glutExitFunc)( int return_value );


#endif  /* FREEGLUT_INTERNAL_MSWIN_H */
