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



static void fghUpdateWindowStatus(SFG_Window *window, GLboolean visState)
{
    SFG_Window* child;

    if (window->State.Visible != visState)
    {
        window->State.Visible = visState;
        INVOKE_WCB( *window, WindowStatus, ( visState ? GLUT_FULLY_RETAINED:GLUT_HIDDEN ) );
    }

    /* Also set visibility state for children */
    for( child = ( SFG_Window * )window->Children.First;
         child;
         child = ( SFG_Window * )child->Node.Next )
    {
        fghUpdateWindowStatus(child, visState);
    }
}

void fghNotifyWindowStatus(SFG_Window *window)
{
    INVOKE_WCB( *window, WindowStatus, ( window->State.Visible?GLUT_FULLY_RETAINED:GLUT_HIDDEN ) );

    /* Don't notify children, they get their own just before first time they're drawn */
}

void fgPlatformMainLoopPreliminaryWork ( void )
{
    /* no-op */
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

static LRESULT fghWindowProcKeyPress(SFG_Window *window, UINT uMsg, GLboolean keydown, WPARAM wParam, LPARAM lParam)
{
    static unsigned char lControl = 0, lShift = 0, lAlt = 0,
                         rControl = 0, rShift = 0, rAlt = 0;

    int keypress = -1;
    
    /* if keydown, check for repeat */
    /* If repeat is globally switched off, it cannot be switched back on per window.
     * But if it is globally switched on, it can be switched off per window. This matches
     * GLUT's behavior on X11, but not Nate Robbins' win32 GLUT, as he didn't implement the
     * global state switch.
     */
    if( keydown && ( fgState.KeyRepeat==GLUT_KEY_REPEAT_OFF || window->State.IgnoreKeyRepeat==GL_TRUE ) && (HIWORD(lParam) & KF_REPEAT) )
        return 1;
    
    /* Remember the current modifiers state so user can query it from their callback */
    fgState.Modifiers = fgPlatformGetModifiers( );

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

    /* SYSKEY events should be sent to default window proc for system to handle them */
    if (uMsg==WM_SYSKEYDOWN || uMsg==WM_SYSKEYUP)
        return DefWindowProc( window->Window.Handle, uMsg, wParam, lParam );
    else
        return 1;
}

static SFG_Window* fghWindowUnderCursor(SFG_Window *window)
{
    /* Check if the current window that the mouse is over is a child window
     * of the window the message was sent to. Some events only sent to main window,
     * and when handling some messages, we need to make sure that we process
     * callbacks on the child window instead. This mirrors how GLUT does things.
     * returns either the original window or the found child.
     */
    if (window && window->Children.First)   /* This window has childs */
    {
        SFG_WindowHandleType hwnd;
        SFG_Window* child_window;

        /* Get mouse position at time of message */
        DWORD mouse_pos_dw = GetMessagePos();
        POINT mouse_pos = {GET_X_LPARAM(mouse_pos_dw), GET_Y_LPARAM(mouse_pos_dw)};
        ScreenToClient( window->Window.Handle, &mouse_pos );
        
        hwnd = ChildWindowFromPoint(window->Window.Handle, mouse_pos);
        if (hwnd && hwnd!=window->Window.Handle)   /* can be NULL if mouse outside parent by the time we get here, or can be same as parent if we didn't find a child */
        {
            child_window = fgWindowByHandle(hwnd);
            if (child_window)    /* Verify we got a FreeGLUT window */
            {
                /* ChildWindowFromPoint only searches immediate children, so search again to see if actually in grandchild or further descendant */
                window = fghWindowUnderCursor(child_window);
            }
        }
    }

    return window;
}

/*
 * The window procedure for handling Win32 events
 */
LRESULT CALLBACK fgPlatformWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    SFG_Window *window;
    LRESULT lRet = 1;
    static int setCaptureActive = 0;

    FREEGLUT_INTERNAL_ERROR_EXIT_IF_NOT_INITIALISED ( "Event Handler" ) ;

    window = fgWindowByHandle( hWnd );

    if ( ( window == NULL ) && ( uMsg != WM_CREATE ) )
      return DefWindowProc( hWnd, uMsg, wParam, lParam );

    /* printf ( "Window %3d message <%04x> %12d %12d\n", window?window->ID:0,
             uMsg, wParam, lParam ); */

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
        /* printf("WM_SIZE (ID: %i): wParam: %i, new size: %ix%i \n",window->ID,wParam,LOWORD(lParam),HIWORD(lParam)); */

        /* Update visibility state of the window */
        if (wParam==SIZE_MINIMIZED)
            fghUpdateWindowStatus(window,GL_FALSE);
        else if (wParam==SIZE_RESTORED && !window->State.Visible)
            fghUpdateWindowStatus(window,GL_TRUE);

        /* Check window visible, we don't want to resize when the user or glutIconifyWindow minimized the window */
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

        /* according to docs, should return 0 */
        lRet = 0;
        break;

    case WM_MOVE:
        {
            SFG_Window* saved_window = fgStructure.CurrentWindow;
            RECT windowRect;

            /* Check window is minimized, we don't want to call the position callback when the user or glutIconifyWindow minimized the window */
            if (!IsIconic(window->Window.Handle))
            {
                /* Get top-left of non-client area of window, matching coordinates of
                 * glutInitPosition and glutPositionWindow, but not those of 
                 * glutGet(GLUT_WINDOW_X) and glutGet(GLUT_WINDOW_Y), which return
                 * top-left of client area.
                 */
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
        }

        /* according to docs, should return 0 */
        lRet = 0;
        break;

    case WM_SETFOCUS:
/*        printf("WM_SETFOCUS: %p\n", window ); */

        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );

        SetActiveWindow( window->Window.Handle );
        UpdateWindow ( hWnd );

        break;

    case WM_KILLFOCUS:
        {
/*            printf("WM_KILLFOCUS: %p\n", window ); */
            lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );

            /* Check if there are any open menus that need to be closed */
            fgPlatformCheckMenuDeactivate();
        }
        break;

#if 0
    case WM_ACTIVATE:
        //printf("WM_ACTIVATE: %x (ID: %i) %d %d\n",lParam, window->ID, HIWORD(wParam), LOWORD(wParam));
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
        {
            if (!window->State.pWState.MouseTracking)
            {
                TRACKMOUSEEVENT tme;

                /* Cursor just entered window, set cursor look */ 
                fgSetCursor ( window, window->State.Cursor ) ;

                /* If an EntryFunc callback is specified by the user, also
                 * invoke that callback and start mouse tracking so that
                 * we get a WM_MOUSELEAVE message
                 */
                if (FETCH_WCB( *window, Entry ))
                {
                    INVOKE_WCB( *window, Entry, ( GLUT_ENTERED ) );

                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = window->Window.Handle;
                    TrackMouseEvent(&tme);

                    window->State.pWState.MouseTracking = GL_TRUE;
                }
            }
        }
        else
            /* Only pass non-client WM_SETCURSOR to DefWindowProc, or we get WM_SETCURSOR on parents of children as well */
            lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;

    case WM_MOUSELEAVE:
        {
            /* NB: This message is only received when a EntryFunc callback
             * is specified by the user, as that is the only condition under
             * which mouse tracking is setup in WM_SETCURSOR handler above
             */
            SFG_Window* saved_window = fgStructure.CurrentWindow;
            INVOKE_WCB( *window, Entry, ( GLUT_LEFT ) );
            fgSetWindow(saved_window);

            window->State.pWState.MouseTracking = GL_FALSE;
            lRet = 0;   /* As per docs, must return zero */
        }
        break;

    case WM_SHOWWINDOW:
        /* printf("WM_SHOWWINDOW, shown? %i, source: %i\n",wParam,lParam); */
        if (wParam)
        {
            fghUpdateWindowStatus(window, GL_TRUE);
            window->State.Redisplay = GL_TRUE;
        }
        else
        {
            fghUpdateWindowStatus(window, GL_FALSE);
            window->State.Redisplay = GL_FALSE;
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        /* Turn on the visibility in case it was turned off somehow */
        window->State.Visible = GL_TRUE;

        InvalidateRect( hWnd, NULL, GL_FALSE ); /* Make sure whole window is repainted. Bit of a hack, but a safe one from what google turns up... */
        BeginPaint( hWnd, &ps );
        fghRedrawWindow( window );
        EndPaint( hWnd, &ps );
    }
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

        /* Set capture so that the window captures all the mouse messages
         *
         * The mouse is not released from the window until all buttons have
         * been released, even if the user presses a button in another window.
         * This is consistent with the behavior on X11.
         */
        if ( pressed == GL_TRUE )
        {
            if (!setCaptureActive)
                SetCapture ( window->Window.Handle ) ;
            setCaptureActive = 1; /* Set to false in WM_CAPTURECHANGED handler */
        }
        else if (!GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_MBUTTON) && !GetAsyncKeyState(VK_RBUTTON))
          /* Make sure all mouse buttons are released before releasing capture */
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

        /* As per docs, should return zero */
        lRet = 0;
    }
    break;

    case WM_MOUSEWHEEL:
    {
        SFG_Window *child_window = NULL;
        int wheel_number = 0;   /* Only one scroll wheel on windows */
        /* int GET_KEYSTATE_WPARAM( wParam ); */
        short ticks = GET_WHEEL_DELTA_WPARAM( wParam );

        window = fghWindowUnderCursor(window);

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
        /* Per docs, should return zero */
        lRet = 0;
    }
    break ;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        window = fghWindowUnderCursor(window);
        lRet = fghWindowProcKeyPress(window,uMsg,GL_TRUE,wParam,lParam);
    }
    break;

    case WM_SYSKEYUP:
    case WM_KEYUP:
    {
        window = fghWindowUnderCursor(window);
        lRet = fghWindowProcKeyPress(window,uMsg,GL_FALSE,wParam,lParam);
    }
    break;

    case WM_SYSCHAR:
    case WM_CHAR:
    {
      window = fghWindowUnderCursor(window);

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
        if (!lParam || !fgWindowByHandle((HWND)lParam))
            /* Capture released or capture taken by non-FreeGLUT window */
            setCaptureActive = 0;
        /* Docs advise a redraw */
        InvalidateRect( hWnd, NULL, GL_FALSE );
        UpdateWindow(hWnd);
        lRet = 0;   /* Per docs, should return zero */
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
                /* Turning off the visibility is handled in WM_SIZE handler */

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
