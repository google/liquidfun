/*
 * freeglut_window_mswin.c
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


/* The following include file is available from SGI but is not standard:
 *   #include <GL/wglext.h>
 * So we copy the necessary parts out of it.
 * XXX: should local definitions for extensions be put in a separate include file?
 */
typedef const char * (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hdc);

typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

#define WGL_DRAW_TO_WINDOW_ARB         0x2001
#define WGL_ACCELERATION_ARB           0x2003
#define WGL_SUPPORT_OPENGL_ARB         0x2010
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_ALPHA_BITS_ARB             0x201B
#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_STENCIL_BITS_ARB           0x2023
#define WGL_FULL_ACCELERATION_ARB      0x2027

#define WGL_SAMPLE_BUFFERS_ARB         0x2041
#define WGL_SAMPLES_ARB                0x2042

#define WGL_TYPE_RGBA_FLOAT_ARB        0x21A0

#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20A9

#ifndef WGL_ARB_create_context
#define WGL_ARB_create_context 1
#ifdef WGL_WGLEXT_PROTOTYPES
extern HGLRC WINAPI wglCreateContextAttribsARB (HDC, HGLRC, const int *);
#endif /* WGL_WGLEXT_PROTOTYPES */
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB    0x2093
#define WGL_CONTEXT_FLAGS_ARB          0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB   0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB      0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define ERROR_INVALID_VERSION_ARB      0x2095
#define ERROR_INVALID_PROFILE_ARB      0x2096
#endif
/* End of copying the necessary parts out of it. */

#ifdef WM_TOUCH
typedef BOOL (WINAPI *pRegisterTouchWindow)(HWND,ULONG);
static pRegisterTouchWindow fghRegisterTouchWindow = (pRegisterTouchWindow)0xDEADBEEF;
#endif

/* 
 * Helper functions for getting client area from the window rect
 * and the window rect from the client area given the style of the window
 * (or a valid window pointer from which the style can be queried).
 */
extern void fghGetBorderWidth(const DWORD windowStyle, int* xBorderWidth, int* yBorderWidth);


/*
 * Setup the pixel format for a Win32 window
 */

#if defined(_WIN32_WCE)
static wchar_t* fghWstrFromStr(const char* str)
{
    int i,len=strlen(str);
    wchar_t* wstr = (wchar_t*)malloc(2*len+2);
    for(i=0; i<len; i++)
        wstr[i] = str[i];
    wstr[len] = 0;
    return wstr;
}
#endif /* defined(_WIN32_WCE) */


static void fghFillContextAttributes( int *attributes ) {
  int where = 0, contextFlags, contextProfile;

  ATTRIB_VAL( WGL_CONTEXT_MAJOR_VERSION_ARB, fgState.MajorVersion );
  ATTRIB_VAL( WGL_CONTEXT_MINOR_VERSION_ARB, fgState.MinorVersion );

  contextFlags =
    fghMapBit( fgState.ContextFlags, GLUT_DEBUG, WGL_CONTEXT_DEBUG_BIT_ARB ) |
    fghMapBit( fgState.ContextFlags, GLUT_FORWARD_COMPATIBLE, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB );
  if ( contextFlags != 0 ) {
    ATTRIB_VAL( WGL_CONTEXT_FLAGS_ARB, contextFlags );
  }

  contextProfile =
    fghMapBit( fgState.ContextProfile, GLUT_CORE_PROFILE, WGL_CONTEXT_CORE_PROFILE_BIT_ARB ) |
    fghMapBit( fgState.ContextProfile, GLUT_COMPATIBILITY_PROFILE, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB );
  if ( contextProfile != 0 ) {
    ATTRIB_VAL( WGL_CONTEXT_PROFILE_MASK_ARB, contextProfile );
  }

  ATTRIB( 0 );
}

static int fghIsExtensionSupported( HDC hdc, const char *extension ) {
    const char *pWglExtString;
    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetEntensionsStringARB =
      (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress("wglGetExtensionsStringARB");
    if ( wglGetEntensionsStringARB == NULL )
    {
      return FALSE;
    }
    pWglExtString = wglGetEntensionsStringARB( hdc );
    return ( pWglExtString != NULL ) && ( strstr(pWglExtString, extension) != NULL );
}

void fgNewWGLCreateContext( SFG_Window* window )
{
    HGLRC context;
    int attributes[9];
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

    /* If nothing fancy has been required, leave the context as it is */
    if ( fghIsLegacyContextRequested() )
    {
        return;
    }

    wglMakeCurrent( window->Window.pContext.Device, window->Window.Context );

    if ( !fghIsExtensionSupported( window->Window.pContext.Device, "WGL_ARB_create_context" ) )
    {
        /* wglCreateContextAttribsARB not found, yet the user has requested the new context creation */
        fgWarning( "OpenGL >2.1 context requested but wglCreateContextAttribsARB is not available! Falling back to legacy context creation" );
        /* Legacy context already created at this point in WM_CREATE path of fgPlatformWindowProc, just return */
        return;
    }

    /* new context creation */
    fghFillContextAttributes( attributes );

    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress( "wglCreateContextAttribsARB" );
    if ( wglCreateContextAttribsARB == NULL )
    {
        /* wglCreateContextAttribsARB not found, yet the user has requested the new context creation */
        fgWarning( "OpenGL >2.1 context requested but wglCreateContextAttribsARB is not available! Falling back to legacy context creation" );
        /* Legacy context already created at this point in WM_CREATE path of fgPlatformWindowProc, just return */
        return;
    }

    context = wglCreateContextAttribsARB( window->Window.pContext.Device, 0, attributes );
    if ( context == NULL )
    {
        fghContextCreationError();
    }

    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( window->Window.Context );
    window->Window.Context = context;
}

#if !defined(_WIN32_WCE)

static void fghFillPFD( PIXELFORMATDESCRIPTOR *ppfd, HDC hdc, unsigned char layer_type )
{
  int flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
  if ( fgState.DisplayMode & GLUT_DOUBLE ) {
        flags |= PFD_DOUBLEBUFFER;
  }
  if ( fgState.DisplayMode & GLUT_STEREO ) {
    flags |= PFD_STEREO;
  }

#if defined(_MSC_VER)
#pragma message( "fgSetupPixelFormat(): there is still some work to do here!" )
#endif

  /* Specify which pixel format do we opt for... */
  ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
  ppfd->nVersion = 1;
  ppfd->dwFlags = flags;

  if( fgState.DisplayMode & GLUT_INDEX ) {
    ppfd->iPixelType = PFD_TYPE_COLORINDEX;
    ppfd->cRedBits = 0;
    ppfd->cGreenBits = 0;
    ppfd->cBlueBits = 0;
    ppfd->cAlphaBits = 0;
  } else {
    ppfd->iPixelType = PFD_TYPE_RGBA;
    ppfd->cRedBits = 8;
    ppfd->cGreenBits = 8;
    ppfd->cBlueBits = 8;
    ppfd->cAlphaBits = ( fgState.DisplayMode & GLUT_ALPHA ) ? 8 : 0;
  }

  ppfd->cColorBits = 24;
  ppfd->cRedShift = 0;
  ppfd->cGreenShift = 0;
  ppfd->cBlueShift = 0;
  ppfd->cAlphaShift = 0;
  ppfd->cAccumBits = ( fgState.DisplayMode & GLUT_ACCUM ) ? 1 : 0;
  ppfd->cAccumRedBits = 0;
  ppfd->cAccumGreenBits = 0;
  ppfd->cAccumBlueBits = 0;
  ppfd->cAccumAlphaBits = 0;

  /* Hmmm, or 32/0 instead of 24/8? */
  ppfd->cDepthBits = 24;
  ppfd->cStencilBits = 8;

  ppfd->cAuxBuffers = fghNumberOfAuxBuffersRequested();
  ppfd->iLayerType = layer_type;
  ppfd->bReserved = 0;
  ppfd->dwLayerMask = 0;
  ppfd->dwVisibleMask = 0;
  ppfd->dwDamageMask = 0;
  
  ppfd->cColorBits = (BYTE) GetDeviceCaps( hdc, BITSPIXEL );
}

static void fghFillPixelFormatAttributes( int *attributes, const PIXELFORMATDESCRIPTOR *ppfd )
{
  int where = 0;

  ATTRIB_VAL( WGL_DRAW_TO_WINDOW_ARB, GL_TRUE );
  ATTRIB_VAL( WGL_SUPPORT_OPENGL_ARB, GL_TRUE );
  ATTRIB_VAL( WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB );

  ATTRIB_VAL( WGL_COLOR_BITS_ARB, ppfd->cColorBits );
  ATTRIB_VAL( WGL_ALPHA_BITS_ARB, ppfd->cAlphaBits );
  ATTRIB_VAL( WGL_DEPTH_BITS_ARB, ppfd->cDepthBits );
  ATTRIB_VAL( WGL_STENCIL_BITS_ARB, ppfd->cStencilBits );

  ATTRIB_VAL( WGL_DOUBLE_BUFFER_ARB, ( fgState.DisplayMode & GLUT_DOUBLE ) != 0 );

  if ( fgState.DisplayMode & GLUT_SRGB ) {
    ATTRIB_VAL( WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, TRUE );
  }

  ATTRIB_VAL( WGL_SAMPLE_BUFFERS_ARB, GL_TRUE );
  ATTRIB_VAL( WGL_SAMPLES_ARB, fgState.SampleNumber );
  ATTRIB( 0 );
}
#endif

GLboolean fgSetupPixelFormat( SFG_Window* window, GLboolean checkOnly,
                              unsigned char layer_type )
{
#if defined(_WIN32_WCE)
    return GL_TRUE;
#else
    PIXELFORMATDESCRIPTOR pfd;
    PIXELFORMATDESCRIPTOR* ppfd = &pfd;
    int pixelformat;
    HDC current_hDC;
    GLboolean success;

    if (checkOnly)
      current_hDC = CreateDC(TEXT("DISPLAY"), NULL ,NULL ,NULL);
    else
      current_hDC = window->Window.pContext.Device;

    fghFillPFD( ppfd, current_hDC, layer_type );
    pixelformat = ChoosePixelFormat( current_hDC, ppfd );

    /* windows hack for multismapling/sRGB */
    if ( ( fgState.DisplayMode & GLUT_MULTISAMPLE ) ||
         ( fgState.DisplayMode & GLUT_SRGB ) )
    {        
        HGLRC rc, rc_before=wglGetCurrentContext();
        HWND hWnd;
        HDC hDC, hDC_before=wglGetCurrentDC();
        WNDCLASS wndCls;

        /* create a dummy window */
        ZeroMemory(&wndCls, sizeof(wndCls));
        wndCls.lpfnWndProc = DefWindowProc;
        wndCls.hInstance = fgDisplay.pDisplay.Instance;
        wndCls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wndCls.lpszClassName = _T("FREEGLUT_dummy");
        RegisterClass( &wndCls );

        hWnd=CreateWindow(_T("FREEGLUT_dummy"), _T(""), WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW , 0,0,0,0, 0, 0, fgDisplay.pDisplay.Instance, 0 );
        hDC=GetDC(hWnd);
        SetPixelFormat( hDC, pixelformat, ppfd );

        rc = wglCreateContext( hDC );
        wglMakeCurrent(hDC, rc);

        if ( fghIsExtensionSupported( hDC, "WGL_ARB_multisample" ) )
        {
            PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARBProc =
              (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
            if ( wglChoosePixelFormatARBProc )
            {
                int attributes[100];
                int iPixelFormat;
                BOOL bValid;
                float fAttributes[] = { 0, 0 };
                UINT numFormats;
                fghFillPixelFormatAttributes( attributes, ppfd );
                bValid = wglChoosePixelFormatARBProc(hDC, attributes, fAttributes, 1, &iPixelFormat, &numFormats);

                if ( bValid && numFormats > 0 )
                {
                    pixelformat = iPixelFormat;
                }
            }
        }

        wglMakeCurrent( hDC_before, rc_before);
        wglDeleteContext(rc);
        ReleaseDC(hWnd, hDC);
        DestroyWindow(hWnd);
        UnregisterClass(_T("FREEGLUT_dummy"), fgDisplay.pDisplay.Instance);
    }

    success = ( pixelformat != 0 ) && ( checkOnly || SetPixelFormat( current_hDC, pixelformat, ppfd ) );

    if (checkOnly)
        DeleteDC(current_hDC);

    return success;
#endif /* defined(_WIN32_WCE) */
}



void fgPlatformSetWindow ( SFG_Window *window )
{
    if ( window != fgStructure.CurrentWindow )
    {
        if( fgStructure.CurrentWindow )
            ReleaseDC( fgStructure.CurrentWindow->Window.Handle,
                       fgStructure.CurrentWindow->Window.pContext.Device );

        if ( window )
        {
            window->Window.pContext.Device = GetDC( window->Window.Handle );
            wglMakeCurrent(
                window->Window.pContext.Device,
                window->Window.Context
            );
        }
    }
}



/* Computes position of corners of window Rect (outer position including
 * decorations) based on the provided client rect and based on the style
 * of the window in question.
 * If posIsOutside is set to true, the input client Rect is taken to follow
 * freeGLUT's window specification convention in which the top-left corner
 * is at the outside of the window, while the size
 * (rect.right-rect.left,rect.bottom-rect.top) is the size of the drawable
 * area.
 */
void fghComputeWindowRectFromClientArea_UseStyle( const DWORD windowStyle, RECT *clientRect, BOOL posIsOutside )
{
    int xBorderWidth = 0, yBorderWidth = 0;

    /* If window has title bar, correct rect for it */
    if (windowStyle & WS_MAXIMIZEBOX) /* Need to query for WS_MAXIMIZEBOX to see if we have a title bar, the WS_CAPTION query is also true for a WS_DLGFRAME only... */
    {
        if (posIsOutside)
            clientRect->bottom += GetSystemMetrics( SM_CYCAPTION );
        else
            clientRect->top -= GetSystemMetrics( SM_CYCAPTION );
    }

    /* get width of window's borders (frame), correct rect for it.
     * Note, borders can be of zero width if style does not specify borders
     */
    fghGetBorderWidth(windowStyle, &xBorderWidth, &yBorderWidth);
    if (posIsOutside)
    {
        clientRect->right  += xBorderWidth * 2;
        clientRect->bottom += yBorderWidth * 2;
    }
    else
    {
        clientRect->left   -= xBorderWidth;
        clientRect->right  += xBorderWidth;
        clientRect->top    -= yBorderWidth;
        clientRect->bottom += yBorderWidth;
    }
}

/* Computes position of corners of window Rect (outer position including
 * decorations) based on the provided client rect and based on the style
 * of the window in question. If the window pointer or the window handle
 * is NULL, a fully decorated window (caption and border) is assumed.
 * Furthermore, if posIsOutside is set to true, the input client Rect is
 * taken to follow freeGLUT's window specification convention in which the
 * top-left corner is at the outside of the window, while the size
 * (rect.right-rect.left,rect.bottom-rect.top) is the size of the drawable
 * area.
*/
void fghComputeWindowRectFromClientArea_QueryWindow( const SFG_Window *window, RECT *clientRect, BOOL posIsOutside )
{
    DWORD windowStyle = 0;

    if (window && window->Window.Handle)
        windowStyle = GetWindowLong(window->Window.Handle, GWL_STYLE);
    else
        windowStyle = WS_OVERLAPPEDWINDOW;

    fghComputeWindowRectFromClientArea_UseStyle(windowStyle, clientRect, posIsOutside);
}

/* Computes position of corners of client area (drawable area) of a window
 * based on the provided window Rect (outer position including decorations)
 * and based on the style of the window in question. If the window pointer
 * or the window handle is NULL, a fully decorated window (caption and
 * border) is assumed.
 * Furthermore, if wantPosOutside is set to true, the output client Rect
 * will follow freeGLUT's window specification convention in which the
 * top-left corner is at the outside of the window, the size
 * (rect.right-rect.left,rect.bottom-rect.top) is the size of the drawable
 * area.
 */
void fghComputeClientAreaFromWindowRect( const SFG_Window *window, RECT *windowRect, BOOL wantPosOutside )
{
    DWORD windowStyle = 0;
    int xBorderWidth = 0, yBorderWidth = 0;

    if (window && window->Window.Handle)
        windowStyle = GetWindowLong(window->Window.Handle, GWL_STYLE);
    else
        windowStyle = WS_OVERLAPPEDWINDOW;

    /* If window has title bar, correct rect for it */
    if (windowStyle & WS_MAXIMIZEBOX) /* Need to query for WS_MAXIMIZEBOX to see if we have a title bar, the WS_CAPTION query is also true for a WS_DLGFRAME only... */
    {
        if (wantPosOutside)
            windowRect->bottom -= GetSystemMetrics( SM_CYCAPTION );
        else
            windowRect->top    += GetSystemMetrics( SM_CYCAPTION );
    }

    /* get width of window's borders (frame), correct rect for it.
     * Note, borders can be of zero width if style does not specify borders
     */
    fghGetBorderWidth(windowStyle, &xBorderWidth, &yBorderWidth);
    if (wantPosOutside)
    {
        windowRect->right  -= xBorderWidth * 2;
        windowRect->bottom -= yBorderWidth * 2;
    }
    else
    {
        windowRect->left   += xBorderWidth;
        windowRect->right  -= xBorderWidth;
        windowRect->top    += yBorderWidth;
        windowRect->bottom -= yBorderWidth;
    }
}

/* Gets the rect describing the client area (drawable area) of the
 * specified window.
 * Returns an empty rect if window pointer or window handle is NULL.
 * If wantPosOutside is set to true, the output client Rect
 * will follow freeGLUT's window specification convention in which the
 * top-left corner is at the outside of the window, while the size
 * (rect.right-rect.left,rect.bottom-rect.top) is the size of the drawable
 * area.
 */
RECT fghGetClientArea( const SFG_Window *window, BOOL wantPosOutside )
{
    RECT windowRect = {0,0,0,0};

    freeglut_return_val_if_fail((window && window->Window.Handle),windowRect);
    
    /*
     * call GetWindowRect()
     * (this returns the pixel coordinates of the outside of the window)
     */
    GetWindowRect( window->Window.Handle, &windowRect );

    /* Then correct the results */
    fghComputeClientAreaFromWindowRect(window, &windowRect, wantPosOutside);

    return windowRect;
}

/* Returns the width of the window borders based on the window's style.
 */
void fghGetBorderWidth(const DWORD windowStyle, int* xBorderWidth, int* yBorderWidth)
{
    if (windowStyle & WS_THICKFRAME)
    {
        *xBorderWidth = GetSystemMetrics(SM_CXSIZEFRAME);
        *yBorderWidth = GetSystemMetrics(SM_CYSIZEFRAME);
    }
    else if (windowStyle & WS_DLGFRAME)
    {
        *xBorderWidth = GetSystemMetrics(SM_CXFIXEDFRAME);
        *yBorderWidth = GetSystemMetrics(SM_CYFIXEDFRAME);
    }
    else
    {
        *xBorderWidth = 0;
        *yBorderWidth = 0;
    }
}

#if(WINVER >= 0x500)
typedef struct
{
      int *x;
      int *y;
      const char *name;
} m_proc_t;

static BOOL CALLBACK m_proc(HMONITOR mon,
			    HDC hdc,
			    LPRECT rect,
			    LPARAM data)
{
      m_proc_t *dp=(m_proc_t *)data;
      MONITORINFOEX info;
      BOOL res;
      info.cbSize=sizeof(info);
      res=GetMonitorInfo(mon,(LPMONITORINFO)&info);
      if( res )
      {
          if( strcmp(dp->name,info.szDevice)==0 )
          {
              *(dp->x)=info.rcMonitor.left;
              *(dp->y)=info.rcMonitor.top;
              return FALSE;
          }
      }
      return TRUE;
}

/* 
 * this function returns the origin of the screen identified by
 * fgDisplay.pDisplay.DisplayName, and 0 otherwise.
 * This is used in fgOpenWindow to open the gamemode window on the screen
 * identified by the -display command line argument. The function should
 * not be called otherwise.
 */

static void get_display_origin(int *xp,int *yp)
{
    *xp = 0;
    *yp = 0;

    if( fgDisplay.pDisplay.DisplayName )
    {
        m_proc_t st;
        st.x=xp;
        st.y=yp;
        st.name=fgDisplay.pDisplay.DisplayName;
        EnumDisplayMonitors(0,0,m_proc,(LPARAM)&st);
    }
}
#else
#pragma message( "-display parameter only works if compiled with WINVER >= 0x0500")

static void get_display_origin(int *xp,int *yp)
{
    *xp = 0;
    *yp = 0;

    if( fgDisplay.pDisplay.DisplayName )
    {
        fgWarning( "for working -display support FreeGLUT must be compiled with WINVER >= 0x0500");
    }
}
#endif



/*
 * Opens a window. Requires a SFG_Window object created and attached
 * to the freeglut structure. OpenGL context is created here.
 */
void fgPlatformOpenWindow( SFG_Window* window, const char* title,
                           GLboolean positionUse, int x, int y,
                           GLboolean sizeUse, int w, int h,
                           GLboolean gameMode, GLboolean isSubWindow )
{

    WNDCLASS wc;
    DWORD flags   = 0;
    DWORD exFlags = 0;
    ATOM atom;

    /* Grab the window class we have registered on glutInit(): */
    atom = GetClassInfo( fgDisplay.pDisplay.Instance, _T("FREEGLUT"), &wc );
    FREEGLUT_INTERNAL_ERROR_EXIT ( atom, "Window Class Info Not Found",
                                   "fgOpenWindow" );

    /* Determine window style flags*/
    if( gameMode )
    {
        FREEGLUT_INTERNAL_ERROR_EXIT ( window->Parent == NULL,
                                       "Game mode being invoked on a subwindow",
                                       "fgOpenWindow" );

        /*
         * Set the window creation flags appropriately to make the window
         * entirely visible:
         */
        flags = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
    }
    else
    {
        flags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

        /*
         * There's a small difference between creating the top, child and
         * menu windows
         */
        if ( window->IsMenu )
        {
            flags |= WS_POPUP;
            exFlags |= WS_EX_TOOLWINDOW;
        }
#if defined(_WIN32_WCE)
        /* no decorations for windows CE */
#else
        /* if this is not a subwindow (child), set its style based on the requested display mode */
        else if( window->Parent == NULL )
            if ( fgState.DisplayMode & GLUT_BORDERLESS )
            {
                /* no window decorations needed */
            }
            else if ( fgState.DisplayMode & GLUT_CAPTIONLESS )
                /* only window decoration is a border, no title bar or buttons */
                flags |= WS_DLGFRAME;
            else
                /* window decoration are a border, title bar and buttons.
                 * NB: we later query whether the window has a title bar or
                 * not by testing for the maximize button, as the test for
                 * WS_CAPTION can be true without the window having a title
                 * bar. This style WS_OVERLAPPEDWINDOW gives you a maximize
                 * button. */
                flags |= WS_OVERLAPPEDWINDOW;
#endif
        else
            /* subwindows always have no decoration, but are marked as a child window to the OS */
            flags |= WS_CHILD;
    }

    /* determine window size and position */
    if( gameMode )
    {
        /* if in gamemode, query the origin of specified by the -display
         * command line parameter (if any) and offset the upper-left corner
         * of the window so we create the window on that screen.
         * The -display argument doesn't do anything if not trying to enter
         * gamemode.
         */
        int xoff=0, yoff=0;
        get_display_origin(&xoff,&yoff);
        x += xoff;
        y += yoff;
    }
    if( !positionUse )
    {
        x = CW_USEDEFAULT;
        y = CW_USEDEFAULT;
    }
    if( !sizeUse )
    {
        if( ! window->IsMenu )
        {
            w = CW_USEDEFAULT;
            h = CW_USEDEFAULT;
        }
        else /* fail safe - Windows can make a window of size (0, 0) */
            w = h = 300; /* default window size */
    }
    /* store requested client area width and height */
    window->State.Width = w;
    window->State.Height = h;

#if !defined(_WIN32_WCE)    /* no decorations for windows CE */
    if( sizeUse )
    {
        RECT windowRect;
        /*
         * Update the window dimensions, taking the window decorations
         * into account.  FreeGLUT is to create the window with the
         * topleft outside corner at (x,y) and with client area
         * dimensions (w,h).
         * note: don't need to do this when w=h=CW_USEDEFAULT, so in the
         * if( sizeUse ) here is convenient.
         */
        windowRect.left     = x;
        windowRect.top      = y;
        windowRect.right    = x+w;
        windowRect.bottom   = y+h;

        fghComputeWindowRectFromClientArea_UseStyle(flags,&windowRect,TRUE);

        w = windowRect.right - windowRect.left;
        h = windowRect.bottom- windowRect.top;
    }
#endif /* !defined(_WIN32_WCE) */

#if defined(_WIN32_WCE)
    {
        wchar_t* wstr = fghWstrFromStr(title);

        window->Window.Handle = CreateWindow(
            _T("FREEGLUT"),
            wstr,
            WS_VISIBLE | WS_POPUP,
            0,0, 240,320,
            NULL,
            NULL,
            fgDisplay.pDisplay.Instance,
            (LPVOID) window
        );

        free(wstr);

        SHFullScreen(window->Window.Handle, SHFS_HIDESTARTICON);
        SHFullScreen(window->Window.Handle, SHFS_HIDESIPBUTTON);
        SHFullScreen(window->Window.Handle, SHFS_HIDETASKBAR);
        MoveWindow(window->Window.Handle, 0, 0, 240, 320, TRUE);
        ShowWindow(window->Window.Handle, SW_SHOW);
        UpdateWindow(window->Window.Handle);
    }
#else
    window->Window.Handle = CreateWindowEx(
        exFlags,
        _T("FREEGLUT"),
        title,
        flags,
        x, y, w, h,
        (HWND) window->Parent == NULL ? NULL : window->Parent->Window.Handle,
        (HMENU) NULL,
        fgDisplay.pDisplay.Instance,
        (LPVOID) window
    );
#endif /* defined(_WIN32_WCE) */

    if( !( window->Window.Handle ) )
        fgError( "Failed to create a window (%s)!", title );

#if !defined(_WIN32_WCE)
    /* Need to set requested style again, apparently Windows doesn't listen when requesting windows without title bar or borders */
    SetWindowLong(window->Window.Handle, GWL_STYLE, flags);
    SetWindowPos(window->Window.Handle, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
#endif /* defined(_WIN32_WCE) */

    /* Make a menu window always on top - fix Feature Request 947118 */
    if( window->IsMenu || gameMode )
        SetWindowPos(
                        window->Window.Handle,
                        HWND_TOPMOST,
                        0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE
                    );

    /* Enable multitouch: additional flag TWF_FINETOUCH, TWF_WANTPALM */
    #ifdef WM_TOUCH
        if (fghRegisterTouchWindow == (pRegisterTouchWindow)0xDEADBEEF) 
			fghRegisterTouchWindow = (pRegisterTouchWindow)GetProcAddress(GetModuleHandle("user32"),"RegisterTouchWindow");
		if (fghRegisterTouchWindow)
             fghRegisterTouchWindow( window->Window.Handle, TWF_FINETOUCH | TWF_WANTPALM );
    #endif

#if defined(_WIN32_WCE)
    ShowWindow( window->Window.Handle, SW_SHOW );
#else
    ShowWindow( window->Window.Handle,
                fgState.ForceIconic ? SW_SHOWMINIMIZED : SW_SHOW );
#endif /* defined(_WIN32_WCE) */

    UpdateWindow( window->Window.Handle );
    ShowCursor( TRUE );  /* XXX Old comments say "hide cursor"! */

}


/*
 * Closes a window, destroying the frame and OpenGL context
 */
void fgPlatformCloseWindow( SFG_Window* window )
{
    /* Make sure we don't close a window with current context active */
    if( fgStructure.CurrentWindow == window )
        wglMakeCurrent( NULL, NULL );

    /*
     * Step through the list of windows.  If the rendering context
     * is not being used by another window, then we delete it.
     */
    {
        int used = FALSE ;
        SFG_Window *iter ;

        for( iter = (SFG_Window *)fgStructure.Windows.First;
             iter;
             iter = (SFG_Window *)iter->Node.Next )
        {
            if( ( iter->Window.Context == window->Window.Context ) &&
                ( iter != window ) )
                used = TRUE;
        }

        if( ! used )
            wglDeleteContext( window->Window.Context );
    }

    DestroyWindow( window->Window.Handle );
}



/*
 * This function makes the current window visible
 */
void fgPlatformGlutShowWindow( void )
{
    ShowWindow( fgStructure.CurrentWindow->Window.Handle, SW_SHOW );
}

/*
 * This function hides the current window
 */
void fgPlatformGlutHideWindow( void )
{
    ShowWindow( fgStructure.CurrentWindow->Window.Handle, SW_HIDE );
}

/*
 * Iconify the current window (top-level windows only)
 */
void fgPlatformGlutIconifyWindow( void )
{
    ShowWindow( fgStructure.CurrentWindow->Window.Handle, SW_MINIMIZE );
}

/*
 * Set the current window's title
 */
void fgPlatformGlutSetWindowTitle( const char* title )
{
#ifdef _WIN32_WCE
    {
        wchar_t* wstr = fghWstrFromStr(title);
        SetWindowText( fgStructure.CurrentWindow->Window.Handle, wstr );
        free(wstr);
    }
#else
    SetWindowText( fgStructure.CurrentWindow->Window.Handle, title );
#endif
}

/*
 * Set the current window's iconified title
 */
void fgPlatformGlutSetIconTitle( const char* title )
{
#ifdef _WIN32_WCE
    {
        wchar_t* wstr = fghWstrFromStr(title);
        SetWindowText( fgStructure.CurrentWindow->Window.Handle, wstr );
        free(wstr);
    }
#else
    SetWindowText( fgStructure.CurrentWindow->Window.Handle, title );
#endif
}

/*
 * Change the current window's position
 */
void fgPlatformGlutPositionWindow( int x, int y )
{
    RECT winRect;

    /* "GetWindowRect" returns the pixel coordinates of the outside of the window */
    GetWindowRect( fgStructure.CurrentWindow->Window.Handle, &winRect );
    MoveWindow(
        fgStructure.CurrentWindow->Window.Handle,
        x,
        y,
        winRect.right - winRect.left,
        winRect.bottom - winRect.top,
        TRUE
    );
}

/*
 * Lowers the current window (by Z order change)
 */
void fgPlatformGlutPushWindow( void )
{
    SetWindowPos(
        fgStructure.CurrentWindow->Window.Handle,
        HWND_BOTTOM,
        0, 0, 0, 0,
        SWP_NOSIZE | SWP_NOMOVE
    );
}

/*
 * Raises the current window (by Z order change)
 */
void fgPlatformGlutPopWindow( void )
{
    SetWindowPos(
        fgStructure.CurrentWindow->Window.Handle,
        HWND_TOP,
        0, 0, 0, 0,
        SWP_NOSIZE | SWP_NOMOVE
    );
}

/*
 * Resize the current window so that it fits the whole screen
 */
void fgPlatformGlutFullScreen( SFG_Window *win )
{
#if !defined(_WIN32_WCE) /* FIXME: what about WinCE */

    if (glutGet(GLUT_FULL_SCREEN))
    {
        /*  Leave full screen state before entering fullscreen again (resizing?) */
        glutLeaveFullScreen();
    }

    {
#if(WINVER >= 0x0500) /* Windows 2000 or later */
        DWORD s;
        RECT rect;
        HMONITOR hMonitor;
        MONITORINFO mi;

        /* For fullscreen mode, first remove all window decoration
         * and set style to popup so it will overlap the taskbar
         * then force to maximize on the screen on which it has the most
         * overlap.
         */

        
        /* store current window rect */
        GetWindowRect( win->Window.Handle, &win->State.pWState.OldRect );

        /* store current window style */
        win->State.pWState.OldStyle = s = GetWindowLong(win->Window.Handle, GWL_STYLE);

        /* remove decorations from style and add popup style*/
        s &= ~WS_OVERLAPPEDWINDOW;
        s |= WS_POPUP;
        SetWindowLong(win->Window.Handle, GWL_STYLE, s);
        SetWindowPos(win->Window.Handle, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

        /* For fullscreen mode, find the monitor that is covered the most
         * by the window and get its rect as the resize target.
	     */
        hMonitor= MonitorFromRect(&win->State.pWState.OldRect, MONITOR_DEFAULTTONEAREST);
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(hMonitor, &mi);
        rect = mi.rcMonitor;
#else   /* if (WINVER >= 0x0500) */
        RECT rect;

        /* For fullscreen mode, force the top-left corner to 0,0
         * and adjust the window rectangle so that the client area
         * covers the whole screen.
         */

        rect.left   = 0;
        rect.top    = 0;
        rect.right  = fgDisplay.ScreenWidth;
        rect.bottom = fgDisplay.ScreenHeight;

        AdjustWindowRect ( &rect, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS |
                                  WS_CLIPCHILDREN, FALSE );
#endif  /* (WINVER >= 0x0500) */

        /*
         * then resize window
         * SWP_NOACTIVATE     Do not activate the window
         * SWP_NOOWNERZORDER  Do not change position in z-order
         * SWP_NOSENDCHANGING Suppress WM_WINDOWPOSCHANGING message
         * SWP_NOZORDER       Retains the current Z order (ignore 2nd param)
         */
        SetWindowPos( fgStructure.CurrentWindow->Window.Handle,
                      HWND_TOP,
                      rect.left,
                      rect.top,
                      rect.right  - rect.left,
                      rect.bottom - rect.top,
                      SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING |
                      SWP_NOZORDER
                    );

        win->State.IsFullscreen = GL_TRUE;
    }
#endif
}

/*
 * If we are fullscreen, resize the current window back to its original size
 */
void fgPlatformGlutLeaveFullScreen( SFG_Window *win )
{
#if !defined(_WIN32_WCE) /* FIXME: what about WinCE */
    if (!glutGet(GLUT_FULL_SCREEN))
    {
        /* nothing to do */
        return;
    }

    /* restore style of window before making it fullscreen */
    SetWindowLong(win->Window.Handle, GWL_STYLE, win->State.pWState.OldStyle);
    SetWindowPos(win->Window.Handle, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    /* Then resize */
    SetWindowPos(win->Window.Handle,
        HWND_TOP,
        win->State.pWState.OldRect.left,
        win->State.pWState.OldRect.top,
        win->State.pWState.OldRect.right  - win->State.pWState.OldRect.left,
        win->State.pWState.OldRect.bottom - win->State.pWState.OldRect.top,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING |
        SWP_NOZORDER
        );

    win->State.IsFullscreen = GL_FALSE;
#endif
}

/*
 * Toggle the window's full screen state.
 */
void fgPlatformGlutFullScreenToggle( SFG_Window *win )
{
    if (!win->State.IsFullscreen)
        glutFullScreen();
    else
        glutLeaveFullScreen();
}


/* -- PLATFORM-SPECIFIC INTERFACE FUNCTION -------------------------------------------------- */

int FGAPIENTRY __glutCreateWindowWithExit( const char *title, void (__cdecl *exit_function)(int) )
{
  __glutExitFunc = exit_function;
  return glutCreateWindow( title );
}

