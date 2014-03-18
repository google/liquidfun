/*
 * freeglut_state_mswin.c
 *
 * The Windows-specific state query methods.
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

#include <GL/freeglut.h>
#include "../fg_internal.h"


extern GLboolean fgSetupPixelFormat( SFG_Window* window, GLboolean checkOnly,
                                     unsigned char layer_type );

/* 
 * Helper functions for getting client area from the window rect
 * and the window rect from the client area given the style of the window
 * (or a valid window pointer from which the style can be queried).
 */
extern void fghGetClientArea( RECT *clientRect, const SFG_Window *window, BOOL posIsOutside );
extern void fghGetStyleFromWindow( const SFG_Window *window, DWORD *windowStyle, DWORD *windowExStyle );
extern void fghComputeWindowRectFromClientArea_UseStyle( RECT *clientRect, const DWORD windowStyle, const DWORD windowExStyle, BOOL posIsOutside );


/* The following include file is available from SGI but is not standard:
 *   #include <GL/wglext.h>
 * So we copy the necessary parts out of it to support the multisampling query
 */
#define WGL_SAMPLES_ARB                0x2042

#if defined(_WIN32_WCE)
#   include <Aygshell.h>
#   ifdef FREEGLUT_LIB_PRAGMAS
#       pragma comment( lib, "Aygshell.lib" )
#   endif
#endif /* defined(_WIN32_WCE) */



int fgPlatformGlutGet ( GLenum eWhat )
{
    int returnValue ;
    GLboolean boolValue ;

    int nsamples = 0;

    switch( eWhat )
    {
    case GLUT_WINDOW_NUM_SAMPLES:
      glGetIntegerv(WGL_SAMPLES_ARB, &nsamples);
      return nsamples;

    /* Handle the OpenGL inquiries */
    case GLUT_WINDOW_RGBA:
#if defined(_WIN32_WCE)
      boolValue = (GLboolean)0;  /* WinCE doesn't support this feature */
#else
      glGetBooleanv ( GL_RGBA_MODE, &boolValue );
      returnValue = boolValue ? 1 : 0;
#endif
      return returnValue;
    case GLUT_WINDOW_DOUBLEBUFFER:
#if defined(_WIN32_WCE)
      boolValue = (GLboolean)0;  /* WinCE doesn't support this feature */
#else
      glGetBooleanv ( GL_DOUBLEBUFFER, &boolValue );
      returnValue = boolValue ? 1 : 0;
#endif
      return returnValue;
    case GLUT_WINDOW_STEREO:
#if defined(_WIN32_WCE)
      boolValue = (GLboolean)0;  /* WinCE doesn't support this feature */
#else
      glGetBooleanv ( GL_STEREO, &boolValue );
      returnValue = boolValue ? 1 : 0;
#endif
      return returnValue;

    case GLUT_WINDOW_RED_SIZE:
      glGetIntegerv ( GL_RED_BITS, &returnValue );
      return returnValue;
    case GLUT_WINDOW_GREEN_SIZE:
      glGetIntegerv ( GL_GREEN_BITS, &returnValue );
      return returnValue;
    case GLUT_WINDOW_BLUE_SIZE:
      glGetIntegerv ( GL_BLUE_BITS, &returnValue );
      return returnValue;
    case GLUT_WINDOW_ALPHA_SIZE:
      glGetIntegerv ( GL_ALPHA_BITS, &returnValue );
      return returnValue;
    case GLUT_WINDOW_ACCUM_RED_SIZE:
#if defined(_WIN32_WCE)
      returnValue = 0;  /* WinCE doesn't support this feature */
#else
      glGetIntegerv ( GL_ACCUM_RED_BITS, &returnValue );
#endif
      return returnValue;
    case GLUT_WINDOW_ACCUM_GREEN_SIZE:
#if defined(_WIN32_WCE)
      returnValue = 0;  /* WinCE doesn't support this feature */
#else
      glGetIntegerv ( GL_ACCUM_GREEN_BITS, &returnValue );
#endif
      return returnValue;
    case GLUT_WINDOW_ACCUM_BLUE_SIZE:
#if defined(_WIN32_WCE)
      returnValue = 0;  /* WinCE doesn't support this feature */
#else
      glGetIntegerv ( GL_ACCUM_BLUE_BITS, &returnValue );
#endif
      return returnValue;
    case GLUT_WINDOW_ACCUM_ALPHA_SIZE:
#if defined(_WIN32_WCE)
      returnValue = 0;  /* WinCE doesn't support this feature */
#else
      glGetIntegerv ( GL_ACCUM_ALPHA_BITS, &returnValue );
#endif
      return returnValue;
    case GLUT_WINDOW_DEPTH_SIZE:
      glGetIntegerv ( GL_DEPTH_BITS, &returnValue );
      return returnValue;

    case GLUT_WINDOW_BUFFER_SIZE:
    {
        PIXELFORMATDESCRIPTOR  pfd;
        HDC hdc = fgStructure.CurrentWindow->Window.pContext.Device;
        int iPixelFormat = GetPixelFormat( hdc );
        DescribePixelFormat(hdc, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
        
        returnValue = pfd.cColorBits;
        if (pfd.iPixelType==PFD_TYPE_RGBA)
            returnValue += pfd.cAlphaBits;

        return returnValue;
    }
    case GLUT_WINDOW_STENCIL_SIZE:
      glGetIntegerv ( GL_STENCIL_BITS, &returnValue );
      return returnValue;

    case GLUT_WINDOW_X:
    case GLUT_WINDOW_Y:
    {
        /*
         *  NB:
         *  - When you create a window with position (x,y) and size
         *    (w,h), the upper left hand corner of the outside of the
         *    window is at (x,y) and the size of the drawable area is
         *    (w,h).
         *  - When you query the size and position of the window--as
         *    is happening here for Windows--"freeglut" will return
         *    the size of the drawable area--the (w,h) that you
         *    specified when you created the window--and the coordinates
         *    of the upper left hand corner of the drawable area, i.e.
         *    of the client rect--which is NOT the (x,y) you specified.
         */

        RECT winRect;
        POINT topLeft = {0,0};

        freeglut_return_val_if_fail( fgStructure.CurrentWindow != NULL, 0 );

#if defined(_WIN32_WCE)
        GetWindowRect( fgStructure.CurrentWindow->Window.Handle, &winRect);
#else
        ClientToScreen(fgStructure.CurrentWindow->Window.Handle, &topLeft);
        
        if (fgStructure.CurrentWindow->Parent)
            /* For child window, we should return relative to upper-left
             * of parent's client area.
             */
            ScreenToClient(fgStructure.CurrentWindow->Parent->Window.Handle,&topLeft);

        winRect.left = topLeft.x;
        winRect.top  = topLeft.y;
#endif /* defined(_WIN32_WCE) */

        switch( eWhat )
        {
        case GLUT_WINDOW_X:      return winRect.left;
        case GLUT_WINDOW_Y:      return winRect.top ;
        }
    }
    break;

    case GLUT_WINDOW_WIDTH:
    case GLUT_WINDOW_HEIGHT:
    {
        RECT winRect;
        freeglut_return_val_if_fail( fgStructure.CurrentWindow != NULL, 0 );

        GetClientRect( fgStructure.CurrentWindow->Window.Handle, &winRect);

        switch( eWhat )
        {
        case GLUT_WINDOW_WIDTH:      return winRect.right-winRect.left;
        case GLUT_WINDOW_HEIGHT:     return winRect.bottom-winRect.top;
        }
    }
    break;

    case GLUT_WINDOW_BORDER_WIDTH :
    case GLUT_WINDOW_BORDER_HEIGHT :
#if defined(_WIN32_WCE)
        return 0;
#else
        {
            /* We can't get the border width or header height in the simple way
             * with some calls to GetSystemMetrics. We'd then have to assume which
             * elements are present for a given decoration, and such calculations
             * wouldn't be valid for every version of Windows. The below should be
             * robust. */
            int borderWidth, captionHeight;
            DWORD windowStyle, windowExStyle;
            RECT clientRect, winRect;

            /* Get style of window, or default style */
            fghGetStyleFromWindow( fgStructure.CurrentWindow, &windowStyle, &windowExStyle );
            /* Get client area if we have a current window, else use dummy rect */
            /* Also get window rect (including non-client area) */
            if (fgStructure.CurrentWindow && fgStructure.CurrentWindow->Window.Handle)
            {
                fghGetClientArea(&clientRect,fgStructure.CurrentWindow, FALSE);
                GetWindowRect(fgStructure.CurrentWindow->Window.Handle,&winRect);
            }
            else
            {
                SetRect(&clientRect,0,0,200,200);
                CopyRect(&winRect,&clientRect);
                fghComputeWindowRectFromClientArea_UseStyle(&winRect,windowStyle,windowExStyle,FALSE);
            }

            /* Calculate border width by taking width of whole window minus width of client area and divide by two
             * NB: we assume horizontal and vertical borders have the same size, which should always be the case
             * unless the user bypassed FreeGLUT and messed with the windowstyle himself.
             * Once borderwidth is known, account for it when comparing height of window to height of client area.
             * all other extra pixels are assumed to be atop the window, forming the caption.
             */
            borderWidth   = ((winRect.right-winRect.left)-(clientRect.right-clientRect.left))/2;
            captionHeight = (winRect.bottom-winRect.top)-(clientRect.bottom-clientRect.top)-borderWidth*2;
            
            switch( eWhat )
            {
            case GLUT_WINDOW_BORDER_WIDTH:
                return borderWidth;
            case GLUT_WINDOW_BORDER_HEIGHT:
                return captionHeight;
            }
        }
#endif /* defined(_WIN32_WCE) */

    case GLUT_DISPLAY_MODE_POSSIBLE:
#if defined(_WIN32_WCE)
        return 0;
#else
        return fgSetupPixelFormat( fgStructure.CurrentWindow, GL_TRUE,
                                    PFD_MAIN_PLANE );
#endif /* defined(_WIN32_WCE) */


    case GLUT_WINDOW_FORMAT_ID:
#if !defined(_WIN32_WCE)
        if( fgStructure.CurrentWindow != NULL )
            return GetPixelFormat( fgStructure.CurrentWindow->Window.pContext.Device );
#endif /* defined(_WIN32_WCE) */
        return 0;

    default:
        fgWarning( "glutGet(): missing enum handle %d", eWhat );
        break;
    }

	return -1;
}


int fgPlatformGlutDeviceGet ( GLenum eWhat )
{
    switch( eWhat )
    {
    case GLUT_HAS_KEYBOARD:
        /*
         * Win32 is assumed a keyboard, and this cannot be queried,
         * except for WindowsCE.
         */
#if defined(_WIN32_CE)
        return ( GetKeyboardStatus() & KBDI_KEYBOARD_PRESENT ) ? 1 : 0;
#   if FREEGLUT_LIB_PRAGMAS
#       pragma comment (lib,"Kbdui.lib")
#   endif

#else
        return 1;
#endif

    case GLUT_HAS_MOUSE:
        /*
         * MS Windows can be booted without a mouse.
         */
        return GetSystemMetrics( SM_MOUSEPRESENT );

    case GLUT_NUM_MOUSE_BUTTONS:
#  if defined(_WIN32_WCE)
        return 1;
#  else
        return GetSystemMetrics( SM_CMOUSEBUTTONS );
#  endif

    default:
        fgWarning( "glutDeviceGet(): missing enum handle %d", eWhat );
        break;
    }

    /* And now -- the failure. */
    return -1;
}

/*
 * This is for querying the number of supported auxiliary or multisample
 * buffers for a (the current?) display mode.
 * see http://old.nabble.com/-GLX--glutGetModeValues-to13514723.html#a13514723
 * Not currently implemented, but we should be able to query the relevant
 * info using
 * http://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt
 * (if supported on the executing machine!)
 */
int *fgPlatformGlutGetModeValues(GLenum eWhat, int *size)
{
  (void)eWhat;
  *size = 0;
  return NULL;
}
