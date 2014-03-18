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

  ppfd->cAuxBuffers = (BYTE)fghNumberOfAuxBuffersRequested();
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


void fghGetDefaultWindowStyle(DWORD *flags)
{
    if ( fgState.DisplayMode & GLUT_BORDERLESS )
    {
        /* no window decorations needed, no-op */
    }
    else if ( fgState.DisplayMode & GLUT_CAPTIONLESS )
        /* only window decoration is a border, no title bar or buttons */
        (*flags) |= WS_DLGFRAME;
    else
        /* window decoration are a border, title bar and buttons. */
        (*flags) |= WS_OVERLAPPEDWINDOW;
}

/* Get window style and extended window style of a FreeGLUT window
 * If the window pointer or the window handle is NULL, a fully
 * decorated window (caption and border) is assumed.
 */
void fghGetStyleFromWindow( const SFG_Window *window, DWORD *windowStyle, DWORD *windowExStyle )
{
    if (window && window->Window.Handle)
    {
        *windowStyle   = GetWindowLong(window->Window.Handle, GWL_STYLE);
        *windowExStyle = GetWindowLong(window->Window.Handle, GWL_EXSTYLE);
    }
    else
    {
        *windowStyle   = 0;
        fghGetDefaultWindowStyle(windowStyle);
        /* WindowExStyle==0 is fine/default, exStyle is currently only used for menu windows */
        *windowExStyle = 0;
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
void fghComputeWindowRectFromClientArea_UseStyle( RECT *clientRect, const DWORD windowStyle, const DWORD windowExStyle, BOOL posIsOutside )
{
    RECT windowRect   = {0,0,0,0};
    CopyRect(&windowRect,clientRect);

    /* Get rect including non-client area */
    AdjustWindowRectEx(&windowRect,windowStyle,FALSE,windowExStyle);

    /* Move window right and down by non-client area extent on left and top, if wanted */
    if (posIsOutside)
    {
        windowRect.right   += clientRect->left-windowRect.left;
        windowRect.bottom  += clientRect->top -windowRect.top;
        windowRect.left     = clientRect->left;
        windowRect.top      = clientRect->top;
    }
    
    /* done, copy windowRect to output */
    CopyRect(clientRect,&windowRect);
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
void fghComputeWindowRectFromClientArea_QueryWindow( RECT *clientRect, const SFG_Window *window, BOOL posIsOutside )
{
    DWORD windowStyle = 0, windowExStyle = 0;
    fghGetStyleFromWindow(window,&windowStyle,&windowExStyle);

    fghComputeWindowRectFromClientArea_UseStyle(clientRect, windowStyle, windowExStyle, posIsOutside);
}

/* Gets the rect describing the client area (drawable area) of the
 * specified window. Output is position of corners of client area (drawable area) on the screen.
 * Does not touch clientRect if window pointer or window handle is NULL.
 * (rect.right-rect.left,rect.bottom-rect.top) is the size of the drawable area.
 * if posIsOutside is true, the output client Rect will follow freeGLUT's window
 * specification convention in which the top-left corner is at the outside of
 * the window, while the size (rect.right-rect.left,rect.bottom-rect.top) remains to be the
 * size of the drawable area.
 */
void fghGetClientArea( RECT *clientRect, const SFG_Window *window, BOOL posIsOutside )
{
    POINT topLeftClient = {0,0};

    freeglut_return_if_fail((window && window->Window.Handle));
    
    /* Get size of client rect */
    GetClientRect(window->Window.Handle, clientRect);
    if (posIsOutside)
    {
        RECT windowRect;
        /* Get position of outside of window, including decorations */
        GetWindowRect(window->Window.Handle,&windowRect);
        /* Add top-left offset */
        OffsetRect(clientRect,windowRect.left,windowRect.top);
    }
    else
    {
        /* Get position of top-left of client area on the screen */
        ClientToScreen(window->Window.Handle,&topLeftClient);
        /* Add top-left offset */
        OffsetRect(clientRect,topLeftClient.x,topLeftClient.y);
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
      (void)hdc;
      (void)rect;
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
    atom = (ATOM)GetClassInfo( fgDisplay.pDisplay.Instance, _T("FREEGLUT"), &wc );
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
        /* if this is not a subwindow (child), set its style based on the requested window decorations */
        else if( window->Parent == NULL )
            fghGetDefaultWindowStyle(&flags);
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
        w = CW_USEDEFAULT;
        h = CW_USEDEFAULT;
    }
#if !defined(_WIN32_WCE)    /* no decorations for windows CE, so nothing to do */
    else
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

        fghComputeWindowRectFromClientArea_UseStyle(&windowRect,flags,exFlags,TRUE);

        /* NB: w and h are now width and height of window including non-client area! */
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

    /* WM_CREATE message got sent and was handled by window proc */

    if( !( window->Window.Handle ) )
        fgError( "Failed to create a window (%s)!", title );

    /* Store title */
    window->State.pWState.WindowTitle = strdup(title);

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
    if (!window->IsMenu)    /* Don't show window after creation if its a menu */
    {
        BOOL iconic = fgState.ForceIconic && !gameMode && !isSubWindow;
        ShowWindow( window->Window.Handle,
                    iconic ? SW_SHOWMINIMIZED : SW_SHOWNORMAL );
    }
#endif /* defined(_WIN32_WCE) */

    ShowCursor( TRUE );
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

    /* clean up copied title text(s) */
    if (window->State.pWState.WindowTitle)
        free(window->State.pWState.WindowTitle);
    if (window->State.pWState.IconTitle)
        free(window->State.pWState.IconTitle);
}

/*
 * Hide's specified window. For windows, currently only used
 * to immediately hide menu windows...
 */
void fgPlatformHideWindow( SFG_Window* window )
{
    ShowWindow(window->Window.Handle, SW_HIDE);
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
    if (!IsIconic(fgStructure.CurrentWindow->Window.Handle))
        SetWindowText( fgStructure.CurrentWindow->Window.Handle, title );
#endif

    /* Make copy of string to refer to later */
    if (fgStructure.CurrentWindow->State.pWState.WindowTitle)
        free(fgStructure.CurrentWindow->State.pWState.WindowTitle);
    fgStructure.CurrentWindow->State.pWState.WindowTitle = strdup(title);
}

/*
 * Set the current window's iconified title
 */
void fgPlatformGlutSetIconTitle( const char* title )
{
#ifndef _WIN32_WCE
    if (IsIconic(fgStructure.CurrentWindow->Window.Handle))
        SetWindowText( fgStructure.CurrentWindow->Window.Handle, title );
#endif

    /* Make copy of string to refer to later */
    if (fgStructure.CurrentWindow->State.pWState.IconTitle)
        free(fgStructure.CurrentWindow->State.pWState.IconTitle);
    fgStructure.CurrentWindow->State.pWState.IconTitle = strdup(title);
}


/* -- PLATFORM-SPECIFIC INTERFACE FUNCTION -------------------------------------------------- */

int FGAPIENTRY __glutCreateWindowWithExit( const char *title, void (__cdecl *exit_function)(int) )
{
  __glutExitFunc = exit_function;
  return glutCreateWindow( title );
}

