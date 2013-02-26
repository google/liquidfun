/*
 * freeglut_main_mswin.c
 *
 * The Windows-specific mouse cursor related stuff.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by John F. Fay, <fayjf@sourceforge.net>
 * Creation date: Sat Jan 21, 2012
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


extern void fghRedrawWindow ( SFG_Window *window );

extern void fgNewWGLCreateContext( SFG_Window* window );
extern GLboolean fgSetupPixelFormat( SFG_Window* window, GLboolean checkOnly,
                                     unsigned char layer_type );

extern void fgPlatformCheckMenuDeactivate();

#ifdef WM_TOUCH
typedef BOOL (WINAPI *pGetTouchInputInfo)(HTOUCHINPUT,UINT,PTOUCHINPUT,int);
typedef BOOL (WINAPI *pCloseTouchInputHandle)(HTOUCHINPUT);
static pGetTouchInputInfo fghGetTouchInputInfo = (pGetTouchInputInfo)0xDEADBEEF;
static pCloseTouchInputHandle fghCloseTouchInputHandle = (pCloseTouchInputHandle)0xDEADBEEF;
#endif

#ifdef _WIN32_WCE
typedef struct GXDisplayProperties GXDisplayProperties;
typedef struct GXKeyList GXKeyList;
#include <gx.h>

typedef struct GXKeyList (*GXGETDEFAULTKEYS)(int);
typedef int (*GXOPENINPUT)();

GXGETDEFAULTKEYS GXGetDefaultKeys_ = NULL;
GXOPENINPUT GXOpenInput_ = NULL;

struct GXKeyList gxKeyList;
#endif /* _WIN32_WCE */


/* Get system time, taking special precautions against 32bit timer wrap.
   We use timeGetTime and not GetTickCount because of its better stability,
   and because we can increase its granularity (to 1 ms in
   fgPlatformInitialize). For that reason we can't use GetTickCount64 which
   wouldn't have the wrap issue.
   Credit: this is based on code in glibc (https://mail.gnome.org/archives/commits-list/2011-November/msg04588.html)
   */
static fg_time_t lastTime32 = 0;
static fg_time_t timeEpoch = 0;
void fgPlatformInitSystemTime()
{
#if defined(_WIN32_WCE)
    lastTime32 = GetTickCount();
#else
    lastTime32 = timeGetTime();
#endif
}
fg_time_t fgPlatformSystemTime ( void )
{
    fg_time_t currTime32;
#if defined(_WIN32_WCE)
    currTime32 = GetTickCount();
#else
    currTime32 = timeGetTime();
#endif
    /* Check if we just wrapped */
    if (currTime32 < lastTime32)
        timeEpoch++;
    
    lastTime32 = currTime32;

    return currTime32 | timeEpoch << 32;
}


void fgPlatformSleepForEvents( fg_time_t msec )
{
    MsgWaitForMultipleObjects( 0, NULL, FALSE, (DWORD) msec, QS_ALLINPUT );
}


void fgPlatformProcessSingleEvent ( void )
{
    MSG stMsg;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutMainLoopEvent" );

    while( PeekMessage( &stMsg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        if( GetMessage( &stMsg, NULL, 0, 0 ) == 0 )
        {
            if( fgState.ActionOnWindowClose == GLUT_ACTION_EXIT )
            {
                fgDeinitialize( );
                exit( 0 );
            }
            else if( fgState.ActionOnWindowClose == GLUT_ACTION_GLUTMAINLOOP_RETURNS )
                fgState.ExecState = GLUT_EXEC_STATE_STOP;

            return;
        }

        TranslateMessage( &stMsg );
        DispatchMessage( &stMsg );
    }
}



void fgPlatformMainLoopPreliminaryWork ( void )
{
    SFG_Window *window = (SFG_Window *)fgStructure.Windows.First ;

    /*
     * Processing before the main loop:  If there is a window which is open and
     * which has a visibility callback, call it.  I know this is an ugly hack,
     * but I'm not sure what else to do about it.  Ideally we should leave
     * something uninitialized in the create window code and initialize it in
     * the main loop, and have that initialization create a "WM_ACTIVATE"
     * message.  Then we would put the visibility callback code in the
     * "case WM_ACTIVATE" block below.         - John Fay -- 10/24/02
     */
    while( window )
    {
        if ( FETCH_WCB( *window, Visibility ) )
        {
            SFG_Window *current_window = fgStructure.CurrentWindow ;

            INVOKE_WCB( *window, Visibility, ( window->State.Visible ) );
            fgSetWindow( current_window );
        }

        window = (SFG_Window *)window->Node.Next ;
    }
}


/*
 * Determine a GLUT modifier mask based on MS-WINDOWS system info.
 */
static int fgPlatformGetModifiers (void)
{
    return
        ( ( ( GetKeyState( VK_LSHIFT   ) < 0 ) ||
            ( GetKeyState( VK_RSHIFT   ) < 0 )) ? GLUT_ACTIVE_SHIFT : 0 ) |
        ( ( ( GetKeyState( VK_LCONTROL ) < 0 ) ||
            ( GetKeyState( VK_RCONTROL ) < 0 )) ? GLUT_ACTIVE_CTRL  : 0 ) |
        ( ( ( GetKeyState( VK_LMENU    ) < 0 ) ||
            ( GetKeyState( VK_RMENU    ) < 0 )) ? GLUT_ACTIVE_ALT   : 0 );
}

static void fghKeyPress(SFG_Window *window, GLboolean keydown, WPARAM wParam, LPARAM lParam)
{
    static unsigned char lControl = 0, lShift = 0, lAlt = 0,
                         rControl = 0, rShift = 0, rAlt = 0;

    int keypress = -1;
    POINT mouse_pos ;
    
    /* if keydown, check for repeat */
    if( keydown && ( fgState.KeyRepeat==GLUT_KEY_REPEAT_OFF || window->State.IgnoreKeyRepeat==GL_TRUE ) && (HIWORD(lParam) & KF_REPEAT) )
        return;
    
    /* Remember the current modifiers state so user can query it from their callback */
    fgState.Modifiers = fgPlatformGetModifiers( );

    /* Get mouse position roughly at time of keypress */
    GetCursorPos( &mouse_pos );
    ScreenToClient( window->Window.Handle, &mouse_pos );
    window->State.MouseX = mouse_pos.x;
    window->State.MouseY = mouse_pos.y;

    /* Convert the Win32 keystroke codes to GLUTtish way */
#   define KEY(a,b) case a: keypress = b; break;

    switch( wParam )
    {
        KEY( VK_F1,     GLUT_KEY_F1        );
        KEY( VK_F2,     GLUT_KEY_F2        );
        KEY( VK_F3,     GLUT_KEY_F3        );
        KEY( VK_F4,     GLUT_KEY_F4        );
        KEY( VK_F5,     GLUT_KEY_F5        );
        KEY( VK_F6,     GLUT_KEY_F6        );
        KEY( VK_F7,     GLUT_KEY_F7        );
        KEY( VK_F8,     GLUT_KEY_F8        );
        KEY( VK_F9,     GLUT_KEY_F9        );
        KEY( VK_F10,    GLUT_KEY_F10       );
        KEY( VK_F11,    GLUT_KEY_F11       );
        KEY( VK_F12,    GLUT_KEY_F12       );
        KEY( VK_PRIOR,  GLUT_KEY_PAGE_UP   );
        KEY( VK_NEXT,   GLUT_KEY_PAGE_DOWN );
        KEY( VK_HOME,   GLUT_KEY_HOME      );
        KEY( VK_END,    GLUT_KEY_END       );
        KEY( VK_LEFT,   GLUT_KEY_LEFT      );
        KEY( VK_UP,     GLUT_KEY_UP        );
        KEY( VK_RIGHT,  GLUT_KEY_RIGHT     );
        KEY( VK_DOWN,   GLUT_KEY_DOWN      );
        KEY( VK_INSERT, GLUT_KEY_INSERT    );

    /* handle control, alt and shift. For GLUT, we want to distinguish between left and right presses.
     * The VK_L* & VK_R* left and right Alt, Ctrl and Shift virtual keys are however only used as parameters to GetAsyncKeyState() and GetKeyState()
     * so when we get an alt, shift or control keypress here, we manually check whether it was the left or the right
     */
#define ASYNC_KEY_EVENT(winKey,glutKey,keyStateVar)\
    if (!keyStateVar && GetAsyncKeyState ( winKey ))\
    {\
        keypress   = glutKey;\
        keyStateVar = 1;\
    }\
    else if (keyStateVar && !GetAsyncKeyState ( winKey ))\
    {\
        keypress   = glutKey;\
        keyStateVar = 0;\
    }
    case VK_CONTROL:
        ASYNC_KEY_EVENT(VK_LCONTROL,GLUT_KEY_CTRL_L,lControl);
        ASYNC_KEY_EVENT(VK_RCONTROL,GLUT_KEY_CTRL_R,rControl);
        break;
    case VK_SHIFT:
        ASYNC_KEY_EVENT(VK_LSHIFT,GLUT_KEY_SHIFT_L,lShift);
        ASYNC_KEY_EVENT(VK_RSHIFT,GLUT_KEY_SHIFT_R,rShift);
        break;
    case VK_MENU:
        ASYNC_KEY_EVENT(VK_LMENU,GLUT_KEY_ALT_L,lAlt);
        ASYNC_KEY_EVENT(VK_RMENU,GLUT_KEY_ALT_R,rAlt);
        break;
#undef ASYNC_KEY_EVENT

    case VK_DELETE:
        /* The delete key should be treated as an ASCII keypress: */
        if (keydown)
            INVOKE_WCB( *window, Keyboard,
                        ( 127, window->State.MouseX, window->State.MouseY )
            );
        else
            INVOKE_WCB( *window, KeyboardUp,
                        ( 127, window->State.MouseX, window->State.MouseY )
            );
        break;

#if !defined(_WIN32_WCE)
    default:
        /* keydown displayable characters are handled with WM_CHAR message, but no corresponding up is generated. So get that here. */
        if (!keydown)
        {
            BYTE state[ 256 ];
            WORD code[ 2 ];

            GetKeyboardState( state );

            if( ToAscii( (UINT)wParam, 0, state, code, 0 ) == 1 )
                wParam=code[ 0 ];

            INVOKE_WCB( *window, KeyboardUp,
                   ( (char)wParam,
                        window->State.MouseX, window->State.MouseY )
            );
        }
#endif
    }

#if defined(_WIN32_WCE)
    if(keydown && !(lParam & 0x40000000)) /* Prevent auto-repeat */
    {
        if(wParam==(unsigned)gxKeyList.vkRight)
            keypress = GLUT_KEY_RIGHT;
        else if(wParam==(unsigned)gxKeyList.vkLeft)
            keypress = GLUT_KEY_LEFT;
        else if(wParam==(unsigned)gxKeyList.vkUp)
            keypress = GLUT_KEY_UP;
        else if(wParam==(unsigned)gxKeyList.vkDown)
            keypress = GLUT_KEY_DOWN;
        else if(wParam==(unsigned)gxKeyList.vkA)
            keypress = GLUT_KEY_F1;
        else if(wParam==(unsigned)gxKeyList.vkB)
            keypress = GLUT_KEY_F2;
        else if(wParam==(unsigned)gxKeyList.vkC)
            keypress = GLUT_KEY_F3;
        else if(wParam==(unsigned)gxKeyList.vkStart)
            keypress = GLUT_KEY_F4;
    }
#endif
    
    if( keypress != -1 )
        if (keydown)
            INVOKE_WCB( *window, Special,
                        ( keypress,
                            window->State.MouseX, window->State.MouseY )
            );
        else
            INVOKE_WCB( *window, SpecialUp,
                        ( keypress,
                            window->State.MouseX, window->State.MouseY )
            );

    fgState.Modifiers = INVALID_MODIFIERS;
}

/*
 * The window procedure for handling Win32 events
 */
LRESULT CALLBACK fgPlatformWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    SFG_Window *window, *child_window = NULL;
    PAINTSTRUCT ps;
    LRESULT lRet = 1;

    FREEGLUT_INTERNAL_ERROR_EXIT_IF_NOT_INITIALISED ( "Event Handler" ) ;

    window = fgWindowByHandle( hWnd );

    if ( ( window == NULL ) && ( uMsg != WM_CREATE ) )
      return DefWindowProc( hWnd, uMsg, wParam, lParam );

    /* printf ( "Window %3d message <%04x> %12d %12d\n", window?window->ID:0,
             uMsg, wParam, lParam ); */

    /* Some events only sent to main window. Check if the current window that
     * the mouse is over is a child window. Below when handling some messages,
     * we make sure that we process callbacks on the child window instead.
     * This mirrors how GLUT does things.
     */
    if (window && window->Children.First)
    {
        POINT mouse_pos;
        SFG_WindowHandleType hwnd;
        SFG_Window* temp_window;

        GetCursorPos( &mouse_pos );
        ScreenToClient( window->Window.Handle, &mouse_pos );
        hwnd = ChildWindowFromPoint(window->Window.Handle, mouse_pos);
        if (hwnd)   /* can be NULL if mouse outside parent by the time we get here */
        {
            temp_window = fgWindowByHandle(hwnd);
            if (temp_window && temp_window->Parent)    /* Verify we got a child window */
                child_window = temp_window;
        }
    }
    
    switch( uMsg )
    {
    case WM_CREATE:
        /* The window structure is passed as the creation structure parameter... */
        window = (SFG_Window *) (((LPCREATESTRUCT) lParam)->lpCreateParams);
        FREEGLUT_INTERNAL_ERROR_EXIT ( ( window != NULL ), "Cannot create window",
                                       "fgPlatformWindowProc" );

        window->Window.Handle = hWnd;
        window->Window.pContext.Device = GetDC( hWnd );
        if( window->IsMenu )
        {
            unsigned int current_DisplayMode = fgState.DisplayMode;
            fgState.DisplayMode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
#if !defined(_WIN32_WCE)
            fgSetupPixelFormat( window, GL_FALSE, PFD_MAIN_PLANE );
#endif
            fgState.DisplayMode = current_DisplayMode;

            if( fgStructure.MenuContext )
                wglMakeCurrent( window->Window.pContext.Device,
                                fgStructure.MenuContext->MContext
                );
            else
            {
                fgStructure.MenuContext =
                    (SFG_MenuContext *)malloc( sizeof(SFG_MenuContext) );
                fgStructure.MenuContext->MContext =
                    wglCreateContext( window->Window.pContext.Device );
            }

            /* window->Window.Context = wglGetCurrentContext ();   */
            window->Window.Context = wglCreateContext( window->Window.pContext.Device );
        }
        else
        {
#if !defined(_WIN32_WCE)
            fgSetupPixelFormat( window, GL_FALSE, PFD_MAIN_PLANE );
#endif

            if( ! fgState.UseCurrentContext )
                window->Window.Context =
                    wglCreateContext( window->Window.pContext.Device );
            else
            {
                window->Window.Context = wglGetCurrentContext( );
                if( ! window->Window.Context )
                    window->Window.Context =
                        wglCreateContext( window->Window.pContext.Device );
            }

#if !defined(_WIN32_WCE)
            fgNewWGLCreateContext( window );
#endif
        }

        window->State.NeedToResize = GL_TRUE;
        /* if we used CW_USEDEFAULT (thats a negative value) for the size
         * of the window, query the window now for the size at which it
         * was created.
         */
        if( ( window->State.Width < 0 ) || ( window->State.Height < 0 ) )
        {
            SFG_Window *current_window = fgStructure.CurrentWindow;

            fgSetWindow( window );
            window->State.Width = glutGet( GLUT_WINDOW_WIDTH );
            window->State.Height = glutGet( GLUT_WINDOW_HEIGHT );
            fgSetWindow( current_window );
        }

        ReleaseDC( window->Window.Handle, window->Window.pContext.Device );

#if defined(_WIN32_WCE)
        /* Take over button handling */
        {
            HINSTANCE dxDllLib=LoadLibrary(_T("gx.dll"));
            if (dxDllLib)
            {
                GXGetDefaultKeys_=(GXGETDEFAULTKEYS)GetProcAddress(dxDllLib, _T("?GXGetDefaultKeys@@YA?AUGXKeyList@@H@Z"));
                GXOpenInput_=(GXOPENINPUT)GetProcAddress(dxDllLib, _T("?GXOpenInput@@YAHXZ"));
            }

            if(GXOpenInput_)
                (*GXOpenInput_)();
            if(GXGetDefaultKeys_)
                gxKeyList = (*GXGetDefaultKeys_)(GX_LANDSCAPEKEYS);
        }

#endif /* defined(_WIN32_WCE) */
        break;

    case WM_SIZE:
        /*
         * If the window is visible, then it is the user manually resizing it.
         * If it is not, then it is the system sending us a dummy resize with
         * zero dimensions on a "glutIconifyWindow" call.
         */
        if( window->State.Visible )
        {
            /* get old values first to compare to below */
            int width = window->State.Width, height=window->State.Height;
#if defined(_WIN32_WCE)
            window->State.Width  = HIWORD(lParam);
            window->State.Height = LOWORD(lParam);
#else
            window->State.Width  = LOWORD(lParam);
            window->State.Height = HIWORD(lParam);
#endif /* defined(_WIN32_WCE) */
            
            if (width!=window->State.Width || height!=window->State.Height)
                /* Something changed, need to resize */
                window->State.NeedToResize = GL_TRUE;
        }

        break;

    case WM_MOVE:
        {
            SFG_Window* saved_window = fgStructure.CurrentWindow;
            RECT windowRect;
            GetWindowRect( window->Window.Handle, &windowRect );
            
            if (window->Parent)
            {
                /* For child window, we should return relative to upper-left
                * of parent's client area.
                */
                POINT topleft = {windowRect.left,windowRect.top};

                ScreenToClient(window->Parent->Window.Handle,&topleft);
                windowRect.left = topleft.x;
                windowRect.top  = topleft.y;
            }

            INVOKE_WCB( *window, Position, ( windowRect.left, windowRect.top ) );
            fgSetWindow(saved_window);
        }
        break;

    case WM_SETFOCUS:
/*        printf("WM_SETFOCUS: %p\n", window ); */

        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );

        if (child_window)
        {
            /* If we're dealing with a child window, make sure it has input focus instead, set it here. */
            SetFocus(child_window->Window.Handle);
            SetActiveWindow( child_window->Window.Handle );
            INVOKE_WCB( *child_window, Entry, ( GLUT_ENTERED ) );
            UpdateWindow ( child_window->Window.Handle );
        }
        else
        {
            SetActiveWindow( window->Window.Handle );
            INVOKE_WCB( *window, Entry, ( GLUT_ENTERED ) );
        }
        /* Always request update on main window to be safe */
        UpdateWindow ( hWnd );

        break;

    case WM_KILLFOCUS:
        {
            SFG_Window* saved_window = fgStructure.CurrentWindow;
/*            printf("WM_KILLFOCUS: %p\n", window ); */
            lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
            INVOKE_WCB( *window, Entry, ( GLUT_LEFT ) );
            fgSetWindow(saved_window);

            /* Check if there are any open menus that need to be closed */
            fgPlatformCheckMenuDeactivate();
        }
        break;

#if 0
    case WM_ACTIVATE:
        if (LOWORD(wParam) != WA_INACTIVE)
        {
/*            printf("WM_ACTIVATE: fgSetCursor( %p, %d)\n", window,
                   window->State.Cursor ); */
            fgSetCursor( window, window->State.Cursor );
        }

        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;
#endif

    case WM_SETCURSOR:
/*      printf ( "Cursor event %x %x %x %x\n", window, window->State.Cursor, lParam, wParam ) ; */
        if( LOWORD( lParam ) == HTCLIENT )
            fgSetCursor ( window, window->State.Cursor ) ;
        else
            lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;

    case WM_SHOWWINDOW:
        window->State.Visible = GL_TRUE;
        window->State.Redisplay = GL_TRUE;
        break;

    case WM_PAINT:
        /* Turn on the visibility in case it was turned off somehow */
        window->State.Visible = GL_TRUE;
        InvalidateRect( hWnd, NULL, GL_FALSE ); /* Make sure whole window is repainted. Bit of a hack, but a safe one from what google turns up... */
        BeginPaint( hWnd, &ps );
        fghRedrawWindow( window );
        EndPaint( hWnd, &ps );
        break;

    case WM_CLOSE:
        fgDestroyWindow ( window );
        if ( fgState.ActionOnWindowClose != GLUT_ACTION_CONTINUE_EXECUTION )
            PostQuitMessage(0);
        break;

    case WM_DESTROY:
        /*
         * The window already got destroyed, so don't bother with it.
         */
        return 0;

    case WM_MOUSEMOVE:
    {
#if defined(_WIN32_WCE)
        window->State.MouseX = 320-HIWORD( lParam );
        window->State.MouseY = LOWORD( lParam );
#else
        window->State.MouseX = LOWORD( lParam );
        window->State.MouseY = HIWORD( lParam );
#endif /* defined(_WIN32_WCE) */
        /* Restrict to [-32768, 32767] to match X11 behaviour       */
        /* See comment in "freeglut_developer" mailing list 10/4/04 */
        if ( window->State.MouseX > 32767 ) window->State.MouseX -= 65536;
        if ( window->State.MouseY > 32767 ) window->State.MouseY -= 65536;

        if ( window->ActiveMenu )
        {
            fgUpdateMenuHighlight( window->ActiveMenu );
            break;
        }

        fgState.Modifiers = fgPlatformGetModifiers( );

        if( ( wParam & MK_LBUTTON ) ||
            ( wParam & MK_MBUTTON ) ||
            ( wParam & MK_RBUTTON ) )
            INVOKE_WCB( *window, Motion, ( window->State.MouseX,
                                           window->State.MouseY ) );
        else
            INVOKE_WCB( *window, Passive, ( window->State.MouseX,
                                            window->State.MouseY ) );

        fgState.Modifiers = INVALID_MODIFIERS;
    }
    break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    {
        GLboolean pressed = GL_TRUE;
        int button;

#if defined(_WIN32_WCE)
        window->State.MouseX = 320-HIWORD( lParam );
        window->State.MouseY = LOWORD( lParam );
#else
        window->State.MouseX = LOWORD( lParam );
        window->State.MouseY = HIWORD( lParam );
#endif /* defined(_WIN32_WCE) */

        /* Restrict to [-32768, 32767] to match X11 behaviour       */
        /* See comment in "freeglut_developer" mailing list 10/4/04 */
        if ( window->State.MouseX > 32767 ) window->State.MouseX -= 65536;
        if ( window->State.MouseY > 32767 ) window->State.MouseY -= 65536;

        switch( uMsg )
        {
        case WM_LBUTTONDOWN:
            pressed = GL_TRUE;
            button = GLUT_LEFT_BUTTON;
            break;
        case WM_MBUTTONDOWN:
            pressed = GL_TRUE;
            button = GLUT_MIDDLE_BUTTON;
            break;
        case WM_RBUTTONDOWN:
            pressed = GL_TRUE;
            button = GLUT_RIGHT_BUTTON;
            break;
        case WM_LBUTTONUP:
            pressed = GL_FALSE;
            button = GLUT_LEFT_BUTTON;
            break;
        case WM_MBUTTONUP:
            pressed = GL_FALSE;
            button = GLUT_MIDDLE_BUTTON;
            break;
        case WM_RBUTTONUP:
            pressed = GL_FALSE;
            button = GLUT_RIGHT_BUTTON;
            break;
        default:
            pressed = GL_FALSE;
            button = -1;
            break;
        }

#if !defined(_WIN32_WCE)
        if( GetSystemMetrics( SM_SWAPBUTTON ) )
        {
            if( button == GLUT_LEFT_BUTTON )
                button = GLUT_RIGHT_BUTTON;
            else
                if( button == GLUT_RIGHT_BUTTON )
                    button = GLUT_LEFT_BUTTON;
        }
#endif /* !defined(_WIN32_WCE) */

        if( button == -1 )
            return DefWindowProc( hWnd, uMsg, lParam, wParam );

        /*
         * Do not execute the application's mouse callback if a menu
         * is hooked to this button.  In that case an appropriate
         * private call should be generated.
         */
        if( fgCheckActiveMenu( window, button, pressed,
                               window->State.MouseX, window->State.MouseY ) )
            break;

        /* Set capture so that the window captures all the mouse messages */
        /*
         * XXX - Multiple button support:  Under X11, the mouse is not released
         * XXX - from the window until all buttons have been released, even if the
         * XXX - user presses a button in another window.  This will take more
         * XXX - code changes than I am up to at the moment (10/5/04).  The present
         * XXX - is a 90 percent solution.
         */
        if ( pressed == GL_TRUE )
          SetCapture ( window->Window.Handle ) ;
        else
          ReleaseCapture () ;

        if( ! FETCH_WCB( *window, Mouse ) )
            break;

        fgSetWindow( window );
        fgState.Modifiers = fgPlatformGetModifiers( );

        INVOKE_WCB(
            *window, Mouse,
            ( button,
              pressed ? GLUT_DOWN : GLUT_UP,
              window->State.MouseX,
              window->State.MouseY
            )
        );

        fgState.Modifiers = INVALID_MODIFIERS;
    }
    break;

    case WM_MOUSEWHEEL:
    {
        int wheel_number = LOWORD( wParam );
        short ticks = ( short )HIWORD( wParam );
		fgState.MouseWheelTicks += ticks;

        if ( abs ( fgState.MouseWheelTicks ) >= WHEEL_DELTA )
		{
			int direction = ( fgState.MouseWheelTicks > 0 ) ? 1 : -1;

            if( ! FETCH_WCB( *window, MouseWheel ) &&
                ! FETCH_WCB( *window, Mouse ) )
                break;

            fgSetWindow( window );
            fgState.Modifiers = fgPlatformGetModifiers( );

            while( abs ( fgState.MouseWheelTicks ) >= WHEEL_DELTA )
			{
                if( FETCH_WCB( *window, MouseWheel ) )
                    INVOKE_WCB( *window, MouseWheel,
                                ( wheel_number,
                                  direction,
                                  window->State.MouseX,
                                  window->State.MouseY
                                )
                    );
                else  /* No mouse wheel, call the mouse button callback twice */
				{
                    /*
                     * Map wheel zero to button 3 and 4; +1 to 3, -1 to 4
                     *  "    "   one                     +1 to 5, -1 to 6, ...
                     *
                     * XXX The below assumes that you have no more than 3 mouse
                     * XXX buttons.  Sorry.
                     */
                    int button = wheel_number * 2 + 3;
                    if( direction < 0 )
                        ++button;
                    INVOKE_WCB( *window, Mouse,
                                ( button, GLUT_DOWN,
                                  window->State.MouseX, window->State.MouseY )
                    );
                    INVOKE_WCB( *window, Mouse,
                                ( button, GLUT_UP,
                                  window->State.MouseX, window->State.MouseY )
                    );
				}

				fgState.MouseWheelTicks -= WHEEL_DELTA * direction;
			}

            fgState.Modifiers = INVALID_MODIFIERS;
		}
    }
    break ;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        if (child_window)
            window = child_window;
        fghKeyPress(window,GL_TRUE,wParam,lParam);
    break;

    case WM_SYSKEYUP:
    case WM_KEYUP:
        if (child_window)
            window = child_window;
        fghKeyPress(window,GL_FALSE,wParam,lParam);
    break;

    case WM_SYSCHAR:
    case WM_CHAR:
    {
      if (child_window)
        window = child_window;

      if( (fgState.KeyRepeat==GLUT_KEY_REPEAT_OFF || window->State.IgnoreKeyRepeat==GL_TRUE) && (HIWORD(lParam) & KF_REPEAT) )
            break;

        fgState.Modifiers = fgPlatformGetModifiers( );
        INVOKE_WCB( *window, Keyboard,
                    ( (char)wParam,
                      window->State.MouseX, window->State.MouseY )
        );
        fgState.Modifiers = INVALID_MODIFIERS;
    }
    break;

    case WM_CAPTURECHANGED:
        /* User has finished resizing the window, force a redraw */
        INVOKE_WCB( *window, Display, ( ) );

        /*lRet = DefWindowProc( hWnd, uMsg, wParam, lParam ); */
        break;

        /* Other messages that I have seen and which are not handled already */
    case WM_SETTEXT:  /* 0x000c */
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        /* Pass it on to "DefWindowProc" to set the window text */
        break;

    case WM_GETTEXT:  /* 0x000d */
        /* Ideally we would copy the title of the window into "lParam" */
        /* strncpy ( (char *)lParam, "Window Title", wParam );
           lRet = ( wParam > 12 ) ? 12 : wParam;  */
        /* the number of characters copied */
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;

    case WM_GETTEXTLENGTH:  /* 0x000e */
        /* Ideally we would get the length of the title of the window */
        lRet = 12;
        /* the number of characters in "Window Title\0" (see above) */
        break;

    case WM_ERASEBKGND:  /* 0x0014 */
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;

#if !defined(_WIN32_WCE)
    case WM_SYNCPAINT:  /* 0x0088 */
        /* Another window has moved, need to update this one */
        window->State.Redisplay = GL_TRUE;
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        /* Help screen says this message must be passed to "DefWindowProc" */
        break;

    case WM_NCPAINT:  /* 0x0085 */
      /* Need to update the border of this window */
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        /* Pass it on to "DefWindowProc" to repaint a standard border */
        break;

    case WM_SYSCOMMAND :  /* 0x0112 */
        {
          /*
           * We have received a system command message.  Try to act on it.
           * The commands are passed in through the "wParam" parameter:
           * The least significant digit seems to be which edge of the window
           * is being used for a resize event:
           *     4  3  5
           *     1     2
           *     7  6  8
           * Congratulations and thanks to Richard Rauch for figuring this out..
           */
            switch ( wParam & 0xfff0 )
            {
            case SC_SIZE       :
                break ;

            case SC_MOVE       :
                break ;

            case SC_MINIMIZE   :
                /* User has clicked on the "-" to minimize the window */
                /* Turn off the visibility */
                window->State.Visible = GL_FALSE ;

                break ;

            case SC_MAXIMIZE   :
                break ;

            case SC_NEXTWINDOW :
                break ;

            case SC_PREVWINDOW :
                break ;

            case SC_CLOSE      :
                /* Followed very closely by a WM_CLOSE message */
                break ;

            case SC_VSCROLL    :
                break ;

            case SC_HSCROLL    :
                break ;

            case SC_MOUSEMENU  :
                break ;

            case SC_KEYMENU    :
                break ;

            case SC_ARRANGE    :
                break ;

            case SC_RESTORE    :
                break ;

            case SC_TASKLIST   :
                break ;

            case SC_SCREENSAVE :
                break ;

            case SC_HOTKEY     :
                break ;

#if(WINVER >= 0x0400)
            case SC_DEFAULT    :
                break ;

            case SC_MONITORPOWER    :
                break ;

            case SC_CONTEXTHELP    :
                break ;
#endif /* WINVER >= 0x0400 */

            default:
#if _DEBUG
                fgWarning( "Unknown wParam type 0x%x", wParam );
#endif
                break;
            }
        }
#endif /* !defined(_WIN32_WCE) */

        /* We need to pass the message on to the operating system as well */
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;

#ifdef WM_TOUCH
	/* handle multi-touch messages */
	case WM_TOUCH:
	{
		unsigned int numInputs = (unsigned int)wParam;
		unsigned int i = 0;
		TOUCHINPUT* ti = (TOUCHINPUT*)malloc( sizeof(TOUCHINPUT)*numInputs);

		if (fghGetTouchInputInfo == (pGetTouchInputInfo)0xDEADBEEF) {
		    fghGetTouchInputInfo = (pGetTouchInputInfo)GetProcAddress(GetModuleHandle("user32"),"GetTouchInputInfo");
		    fghCloseTouchInputHandle = (pCloseTouchInputHandle)GetProcAddress(GetModuleHandle("user32"),"CloseTouchInputHandle");
		}

		if (!fghGetTouchInputInfo) { 
			free( (void*)ti );
			break;
		}

		if (fghGetTouchInputInfo( (HTOUCHINPUT)lParam, numInputs, ti, sizeof(TOUCHINPUT) )) {
			/* Handle each contact point */
			for (i = 0; i < numInputs; ++i ) {

				POINT tp;
				tp.x = TOUCH_COORD_TO_PIXEL(ti[i].x);
				tp.y = TOUCH_COORD_TO_PIXEL(ti[i].y);
				ScreenToClient( hWnd, &tp );

				ti[i].dwID = ti[i].dwID * 2;

				if (ti[i].dwFlags & TOUCHEVENTF_DOWN) {
					INVOKE_WCB( *window, MultiEntry,  ( ti[i].dwID, GLUT_ENTERED ) );
					INVOKE_WCB( *window, MultiButton, ( ti[i].dwID, tp.x, tp.y, 0, GLUT_DOWN ) );
				} else if (ti[i].dwFlags & TOUCHEVENTF_MOVE) {
					INVOKE_WCB( *window, MultiMotion, ( ti[i].dwID, tp.x, tp.y ) );
				} else if (ti[i].dwFlags & TOUCHEVENTF_UP)   { 
					INVOKE_WCB( *window, MultiButton, ( ti[i].dwID, tp.x, tp.y, 0, GLUT_UP ) );
					INVOKE_WCB( *window, MultiEntry,  ( ti[i].dwID, GLUT_LEFT ) );
				}
			}
		}
		fghCloseTouchInputHandle((HTOUCHINPUT)lParam);
		free( (void*)ti );
		lRet = 0; /*DefWindowProc( hWnd, uMsg, wParam, lParam );*/
		break;
	}
#endif
    default:
        /* Handle unhandled messages */
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;
    }

    return lRet;
}
