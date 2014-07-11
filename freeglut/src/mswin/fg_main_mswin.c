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
extern void fghRedrawWindowAndChildren ( SFG_Window *window );
extern void fghOnReshapeNotify(SFG_Window *window, int width, int height, GLboolean forceNotify);
extern void fghOnPositionNotify(SFG_Window *window, int x, int y, GLboolean forceNotify);
extern void fghComputeWindowRectFromClientArea_QueryWindow( RECT *clientRect, const SFG_Window *window, BOOL posIsOutside );
extern void fghGetClientArea( RECT *clientRect, const SFG_Window *window, BOOL posIsOutside );

extern void fgNewWGLCreateContext( SFG_Window* window );
extern GLboolean fgSetupPixelFormat( SFG_Window* window, GLboolean checkOnly,
                                     unsigned char layer_type );

extern void fgPlatformCheckMenuDeactivate(HWND newFocusWnd);

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

#ifdef _DEBUG
/* 
 * WM_ message to string, for debugging
 * This is taken from the 8.0 SDK, so Windows 8 API and everything earlier is included
 */
struct WM_MESSAGE_MAP
{
    UINT    nMsg;
    LPCSTR  lpszMsg;
};
#define DEFINE_MESSAGE(wm){ wm, #wm }
static struct WM_MESSAGE_MAP allMessages[] =
{
    DEFINE_MESSAGE(WM_NULL),
    DEFINE_MESSAGE(WM_CREATE),
    DEFINE_MESSAGE(WM_DESTROY),
    DEFINE_MESSAGE(WM_MOVE),
    DEFINE_MESSAGE(WM_SIZE),

    DEFINE_MESSAGE(WM_ACTIVATE),
    DEFINE_MESSAGE(WM_SETFOCUS),
    DEFINE_MESSAGE(WM_KILLFOCUS),
    DEFINE_MESSAGE(WM_ENABLE),
    DEFINE_MESSAGE(WM_SETREDRAW),
    DEFINE_MESSAGE(WM_SETTEXT),
    DEFINE_MESSAGE(WM_GETTEXT),
    DEFINE_MESSAGE(WM_GETTEXTLENGTH),
    DEFINE_MESSAGE(WM_PAINT),
    DEFINE_MESSAGE(WM_CLOSE),
#   ifndef _WIN32_WCE
        DEFINE_MESSAGE(WM_QUERYENDSESSION),
        DEFINE_MESSAGE(WM_QUERYOPEN),
        DEFINE_MESSAGE(WM_ENDSESSION),
#   endif
    DEFINE_MESSAGE(WM_QUIT),
    DEFINE_MESSAGE(WM_ERASEBKGND),
    DEFINE_MESSAGE(WM_SYSCOLORCHANGE),
    DEFINE_MESSAGE(WM_SHOWWINDOW),
    DEFINE_MESSAGE(WM_WININICHANGE),

    DEFINE_MESSAGE(WM_DEVMODECHANGE),
    DEFINE_MESSAGE(WM_ACTIVATEAPP),
    DEFINE_MESSAGE(WM_FONTCHANGE),
    DEFINE_MESSAGE(WM_TIMECHANGE),
    DEFINE_MESSAGE(WM_CANCELMODE),
    DEFINE_MESSAGE(WM_SETCURSOR),
    DEFINE_MESSAGE(WM_MOUSEACTIVATE),
    DEFINE_MESSAGE(WM_CHILDACTIVATE),
    DEFINE_MESSAGE(WM_QUEUESYNC),

    DEFINE_MESSAGE(WM_GETMINMAXINFO),

    DEFINE_MESSAGE(WM_PAINTICON),
    DEFINE_MESSAGE(WM_ICONERASEBKGND),
    DEFINE_MESSAGE(WM_NEXTDLGCTL),
    DEFINE_MESSAGE(WM_SPOOLERSTATUS),
    DEFINE_MESSAGE(WM_DRAWITEM),
    DEFINE_MESSAGE(WM_MEASUREITEM),
    DEFINE_MESSAGE(WM_DELETEITEM),
    DEFINE_MESSAGE(WM_VKEYTOITEM),
    DEFINE_MESSAGE(WM_CHARTOITEM),
    DEFINE_MESSAGE(WM_SETFONT),
    DEFINE_MESSAGE(WM_GETFONT),
    DEFINE_MESSAGE(WM_SETHOTKEY),
    DEFINE_MESSAGE(WM_GETHOTKEY),
    DEFINE_MESSAGE(WM_QUERYDRAGICON),
    DEFINE_MESSAGE(WM_COMPAREITEM),
#   if(WINVER >= 0x0500)
#       ifndef _WIN32_WCE
            DEFINE_MESSAGE(WM_GETOBJECT),
    #   endif
#   endif /* WINVER >= 0x0500 */
    DEFINE_MESSAGE(WM_COMPACTING),
    DEFINE_MESSAGE(WM_COMMNOTIFY),
    DEFINE_MESSAGE(WM_WINDOWPOSCHANGING),
    DEFINE_MESSAGE(WM_WINDOWPOSCHANGED),

    DEFINE_MESSAGE(WM_POWER),

    DEFINE_MESSAGE(WM_COPYDATA),
    DEFINE_MESSAGE(WM_CANCELJOURNAL),

#   if(WINVER >= 0x0400)
        DEFINE_MESSAGE(WM_NOTIFY),
        DEFINE_MESSAGE(WM_INPUTLANGCHANGEREQUEST),
        DEFINE_MESSAGE(WM_INPUTLANGCHANGE),
        DEFINE_MESSAGE(WM_TCARD),
        DEFINE_MESSAGE(WM_HELP),
        DEFINE_MESSAGE(WM_USERCHANGED),
        DEFINE_MESSAGE(WM_NOTIFYFORMAT),

        DEFINE_MESSAGE(WM_CONTEXTMENU),
        DEFINE_MESSAGE(WM_STYLECHANGING),
        DEFINE_MESSAGE(WM_STYLECHANGED),
        DEFINE_MESSAGE(WM_DISPLAYCHANGE),
        DEFINE_MESSAGE(WM_GETICON),
        DEFINE_MESSAGE(WM_SETICON),
#   endif /* WINVER >= 0x0400 */

    DEFINE_MESSAGE(WM_NCCREATE),
    DEFINE_MESSAGE(WM_NCDESTROY),
    DEFINE_MESSAGE(WM_NCCALCSIZE),
    DEFINE_MESSAGE(WM_NCHITTEST),
    DEFINE_MESSAGE(WM_NCPAINT),
    DEFINE_MESSAGE(WM_NCACTIVATE),
    DEFINE_MESSAGE(WM_GETDLGCODE),
#   ifndef _WIN32_WCE
        DEFINE_MESSAGE(WM_SYNCPAINT),
#   endif
    DEFINE_MESSAGE(WM_NCMOUSEMOVE),
    DEFINE_MESSAGE(WM_NCLBUTTONDOWN),
    DEFINE_MESSAGE(WM_NCLBUTTONUP),
    DEFINE_MESSAGE(WM_NCLBUTTONDBLCLK),
    DEFINE_MESSAGE(WM_NCRBUTTONDOWN),
    DEFINE_MESSAGE(WM_NCRBUTTONUP),
    DEFINE_MESSAGE(WM_NCRBUTTONDBLCLK),
    DEFINE_MESSAGE(WM_NCMBUTTONDOWN),
    DEFINE_MESSAGE(WM_NCMBUTTONUP),
    DEFINE_MESSAGE(WM_NCMBUTTONDBLCLK),



#   if(_WIN32_WINNT >= 0x0500)
        DEFINE_MESSAGE(WM_NCXBUTTONDOWN),
        DEFINE_MESSAGE(WM_NCXBUTTONUP),
        DEFINE_MESSAGE(WM_NCXBUTTONDBLCLK),
#   endif /* _WIN32_WINNT >= 0x0500 */


#   if(_WIN32_WINNT >= 0x0501)
        DEFINE_MESSAGE(WM_INPUT_DEVICE_CHANGE),
#   endif /* _WIN32_WINNT >= 0x0501 */

#   if(_WIN32_WINNT >= 0x0501)
        DEFINE_MESSAGE(WM_INPUT),
#   endif /* _WIN32_WINNT >= 0x0501 */

    DEFINE_MESSAGE(WM_KEYDOWN),
    DEFINE_MESSAGE(WM_KEYUP),
    DEFINE_MESSAGE(WM_CHAR),
    DEFINE_MESSAGE(WM_DEADCHAR),
    DEFINE_MESSAGE(WM_SYSKEYDOWN),
    DEFINE_MESSAGE(WM_SYSKEYUP),
    DEFINE_MESSAGE(WM_SYSCHAR),
    DEFINE_MESSAGE(WM_SYSDEADCHAR),
#   if(_WIN32_WINNT >= 0x0501)
        DEFINE_MESSAGE(WM_UNICHAR),
#   endif /* _WIN32_WINNT >= 0x0501 */

#   if(WINVER >= 0x0400)
        DEFINE_MESSAGE(WM_IME_STARTCOMPOSITION),
        DEFINE_MESSAGE(WM_IME_ENDCOMPOSITION),
        DEFINE_MESSAGE(WM_IME_COMPOSITION),
        DEFINE_MESSAGE(WM_IME_KEYLAST),
#   endif /* WINVER >= 0x0400 */

    DEFINE_MESSAGE(WM_INITDIALOG),
    DEFINE_MESSAGE(WM_COMMAND),
    DEFINE_MESSAGE(WM_SYSCOMMAND),
    DEFINE_MESSAGE(WM_TIMER),
    DEFINE_MESSAGE(WM_HSCROLL),
    DEFINE_MESSAGE(WM_VSCROLL),
    DEFINE_MESSAGE(WM_INITMENU),
    DEFINE_MESSAGE(WM_INITMENUPOPUP),
#   if(WINVER >= 0x0601)
        DEFINE_MESSAGE(WM_GESTURE),
        DEFINE_MESSAGE(WM_GESTURENOTIFY),
#   endif /* WINVER >= 0x0601 */
    DEFINE_MESSAGE(WM_MENUSELECT),
    DEFINE_MESSAGE(WM_MENUCHAR),
    DEFINE_MESSAGE(WM_ENTERIDLE),
#   if(WINVER >= 0x0500)
#       ifndef _WIN32_WCE
            DEFINE_MESSAGE(WM_MENURBUTTONUP),
            DEFINE_MESSAGE(WM_MENUDRAG),
            DEFINE_MESSAGE(WM_MENUGETOBJECT),
            DEFINE_MESSAGE(WM_UNINITMENUPOPUP),
            DEFINE_MESSAGE(WM_MENUCOMMAND),

#           if(_WIN32_WINNT >= 0x0500)
                DEFINE_MESSAGE(WM_CHANGEUISTATE),
                DEFINE_MESSAGE(WM_UPDATEUISTATE),
                DEFINE_MESSAGE(WM_QUERYUISTATE),
#           endif /* _WIN32_WINNT >= 0x0500 */

#       endif
#   endif /* WINVER >= 0x0500 */

    DEFINE_MESSAGE(WM_CTLCOLORMSGBOX),
    DEFINE_MESSAGE(WM_CTLCOLOREDIT),
    DEFINE_MESSAGE(WM_CTLCOLORLISTBOX),
    DEFINE_MESSAGE(WM_CTLCOLORBTN),
    DEFINE_MESSAGE(WM_CTLCOLORDLG),
    DEFINE_MESSAGE(WM_CTLCOLORSCROLLBAR),
    DEFINE_MESSAGE(WM_CTLCOLORSTATIC),
#   define MN_GETHMENU                     0x01E1

    DEFINE_MESSAGE(WM_MOUSEMOVE),
    DEFINE_MESSAGE(WM_LBUTTONDOWN),
    DEFINE_MESSAGE(WM_LBUTTONUP),
    DEFINE_MESSAGE(WM_LBUTTONDBLCLK),
    DEFINE_MESSAGE(WM_RBUTTONDOWN),
    DEFINE_MESSAGE(WM_RBUTTONUP),
    DEFINE_MESSAGE(WM_RBUTTONDBLCLK),
    DEFINE_MESSAGE(WM_MBUTTONDOWN),
    DEFINE_MESSAGE(WM_MBUTTONUP),
    DEFINE_MESSAGE(WM_MBUTTONDBLCLK),
#   if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
        DEFINE_MESSAGE(WM_MOUSEWHEEL),
#   endif
#   if (_WIN32_WINNT >= 0x0500)
        DEFINE_MESSAGE(WM_XBUTTONDOWN),
        DEFINE_MESSAGE(WM_XBUTTONUP),
        DEFINE_MESSAGE(WM_XBUTTONDBLCLK),
#   endif
#   if (_WIN32_WINNT >= 0x0600)
        DEFINE_MESSAGE(WM_MOUSEHWHEEL),
#   endif



    DEFINE_MESSAGE(WM_PARENTNOTIFY),
    DEFINE_MESSAGE(WM_ENTERMENULOOP),
    DEFINE_MESSAGE(WM_EXITMENULOOP),

#   if(WINVER >= 0x0400)
        DEFINE_MESSAGE(WM_NEXTMENU),
        DEFINE_MESSAGE(WM_SIZING),
        DEFINE_MESSAGE(WM_CAPTURECHANGED),
        DEFINE_MESSAGE(WM_MOVING),
#   endif /* WINVER >= 0x0400 */

#   if(WINVER >= 0x0400)
        DEFINE_MESSAGE(WM_POWERBROADCAST),
        DEFINE_MESSAGE(WM_DEVICECHANGE),
#   endif /* WINVER >= 0x0400 */

    DEFINE_MESSAGE(WM_MDICREATE),
    DEFINE_MESSAGE(WM_MDIDESTROY),
    DEFINE_MESSAGE(WM_MDIACTIVATE),
    DEFINE_MESSAGE(WM_MDIRESTORE),
    DEFINE_MESSAGE(WM_MDINEXT),
    DEFINE_MESSAGE(WM_MDIMAXIMIZE),
    DEFINE_MESSAGE(WM_MDITILE),
    DEFINE_MESSAGE(WM_MDICASCADE),
    DEFINE_MESSAGE(WM_MDIICONARRANGE),
    DEFINE_MESSAGE(WM_MDIGETACTIVE),


    DEFINE_MESSAGE(WM_MDISETMENU),
    DEFINE_MESSAGE(WM_ENTERSIZEMOVE),
    DEFINE_MESSAGE(WM_EXITSIZEMOVE),
    DEFINE_MESSAGE(WM_DROPFILES),
    DEFINE_MESSAGE(WM_MDIREFRESHMENU),

#   if(WINVER >= 0x0602)
        DEFINE_MESSAGE(WM_POINTERDEVICECHANGE),
        DEFINE_MESSAGE(WM_POINTERDEVICEINRANGE),
        DEFINE_MESSAGE(WM_POINTERDEVICEOUTOFRANGE),
#   endif /* WINVER >= 0x0602 */

#   if(WINVER >= 0x0601)
        DEFINE_MESSAGE(WM_TOUCH),
#   endif /* WINVER >= 0x0601 */

#   if(WINVER >= 0x0602)
        DEFINE_MESSAGE(WM_NCPOINTERUPDATE),
        DEFINE_MESSAGE(WM_NCPOINTERDOWN),
        DEFINE_MESSAGE(WM_NCPOINTERUP),
        DEFINE_MESSAGE(WM_POINTERUPDATE),
        DEFINE_MESSAGE(WM_POINTERDOWN),
        DEFINE_MESSAGE(WM_POINTERUP),
        DEFINE_MESSAGE(WM_POINTERENTER),
        DEFINE_MESSAGE(WM_POINTERLEAVE),
        DEFINE_MESSAGE(WM_POINTERACTIVATE),
        DEFINE_MESSAGE(WM_POINTERCAPTURECHANGED),
        DEFINE_MESSAGE(WM_TOUCHHITTESTING),
        DEFINE_MESSAGE(WM_POINTERWHEEL),
        DEFINE_MESSAGE(WM_POINTERHWHEEL),
#   endif /* WINVER >= 0x0602 */


#   if(WINVER >= 0x0400)
        DEFINE_MESSAGE(WM_IME_SETCONTEXT),
        DEFINE_MESSAGE(WM_IME_NOTIFY),
        DEFINE_MESSAGE(WM_IME_CONTROL),
        DEFINE_MESSAGE(WM_IME_COMPOSITIONFULL),
        DEFINE_MESSAGE(WM_IME_SELECT),
        DEFINE_MESSAGE(WM_IME_CHAR),
#   endif /* WINVER >= 0x0400 */
#   if(WINVER >= 0x0500)
        DEFINE_MESSAGE(WM_IME_REQUEST),
#   endif /* WINVER >= 0x0500 */
#   if(WINVER >= 0x0400)
        DEFINE_MESSAGE(WM_IME_KEYDOWN),
        DEFINE_MESSAGE(WM_IME_KEYUP),
#   endif /* WINVER >= 0x0400 */

#   if((_WIN32_WINNT >= 0x0400) || (WINVER >= 0x0500))
        DEFINE_MESSAGE(WM_MOUSEHOVER),
        DEFINE_MESSAGE(WM_MOUSELEAVE),
#   endif
#   if(WINVER >= 0x0500)
        DEFINE_MESSAGE(WM_NCMOUSEHOVER),
        DEFINE_MESSAGE(WM_NCMOUSELEAVE),
#   endif /* WINVER >= 0x0500 */

#   if(_WIN32_WINNT >= 0x0501)
        DEFINE_MESSAGE(WM_WTSSESSION_CHANGE),
#   endif /* _WIN32_WINNT >= 0x0501 */

    DEFINE_MESSAGE(WM_CUT),
    DEFINE_MESSAGE(WM_COPY),
    DEFINE_MESSAGE(WM_PASTE),
    DEFINE_MESSAGE(WM_CLEAR),
    DEFINE_MESSAGE(WM_UNDO),
    DEFINE_MESSAGE(WM_RENDERFORMAT),
    DEFINE_MESSAGE(WM_RENDERALLFORMATS),
    DEFINE_MESSAGE(WM_DESTROYCLIPBOARD),
    DEFINE_MESSAGE(WM_DRAWCLIPBOARD),
    DEFINE_MESSAGE(WM_PAINTCLIPBOARD),
    DEFINE_MESSAGE(WM_VSCROLLCLIPBOARD),
    DEFINE_MESSAGE(WM_SIZECLIPBOARD),
    DEFINE_MESSAGE(WM_ASKCBFORMATNAME),
    DEFINE_MESSAGE(WM_CHANGECBCHAIN),
    DEFINE_MESSAGE(WM_HSCROLLCLIPBOARD),
    DEFINE_MESSAGE(WM_QUERYNEWPALETTE),
    DEFINE_MESSAGE(WM_PALETTEISCHANGING),
    DEFINE_MESSAGE(WM_PALETTECHANGED),
    DEFINE_MESSAGE(WM_HOTKEY),

#   if(WINVER >= 0x0400)
        DEFINE_MESSAGE(WM_PRINT),
        DEFINE_MESSAGE(WM_PRINTCLIENT),
#   endif /* WINVER >= 0x0400 */

#   if(_WIN32_WINNT >= 0x0500)
        DEFINE_MESSAGE(WM_APPCOMMAND),
#   endif /* _WIN32_WINNT >= 0x0500 */

#   if(_WIN32_WINNT >= 0x0501)
        DEFINE_MESSAGE(WM_THEMECHANGED),
#   endif /* _WIN32_WINNT >= 0x0501 */


#   if(_WIN32_WINNT >= 0x0501)
        DEFINE_MESSAGE(WM_CLIPBOARDUPDATE),
#   endif /* _WIN32_WINNT >= 0x0501 */

#   if(_WIN32_WINNT >= 0x0600)
        DEFINE_MESSAGE(WM_DWMCOMPOSITIONCHANGED),
        DEFINE_MESSAGE(WM_DWMNCRENDERINGCHANGED),
        DEFINE_MESSAGE(WM_DWMCOLORIZATIONCOLORCHANGED),
        DEFINE_MESSAGE(WM_DWMWINDOWMAXIMIZEDCHANGE),
#   endif /* _WIN32_WINNT >= 0x0600 */

#   if(_WIN32_WINNT >= 0x0601)
        DEFINE_MESSAGE(WM_DWMSENDICONICTHUMBNAIL),
        DEFINE_MESSAGE(WM_DWMSENDICONICLIVEPREVIEWBITMAP),
#   endif /* _WIN32_WINNT >= 0x0601 */


#   if(WINVER >= 0x0600)
        DEFINE_MESSAGE(WM_GETTITLEBARINFOEX),
#   endif /* WINVER >= 0x0600 */
    { 0, NULL, }    /* end of message list */
};
#undef DEFINE_MESSAGE

char* WMMsg2Str(DWORD dwMessage)
{
    struct WM_MESSAGE_MAP* pMapMsg = allMessages;
    for (/*null*/; pMapMsg->lpszMsg != NULL; pMapMsg++)
    {
        if (pMapMsg->nMsg == dwMessage )
        {
            return (char *)pMapMsg->lpszMsg;
        }
    }
    return "";
}
#endif /* _DEBUG */


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



static void fghPlatformOnWindowStatusNotify(SFG_Window *window, GLboolean visState, GLboolean forceNotify)
{
    GLboolean notify = GL_FALSE;
    SFG_Window* child;

    if (window->State.Visible != visState)
    {
        window->State.Visible = visState;

        /* If top level window (not a subwindow/child), and icon title text available, switch titles based on visibility state */
        if (!window->Parent && window->State.pWState.IconTitle)
        {
            if (visState)
                /* visible, set window title */
                SetWindowText( window->Window.Handle, window->State.pWState.WindowTitle );
            else
                /* not visible, set icon title */
                SetWindowText( window->Window.Handle, window->State.pWState.IconTitle );
        }

        notify = GL_TRUE;
    }

    if (notify || forceNotify)
    {
        SFG_Window *saved_window = fgStructure.CurrentWindow;

        /* On win32 we only have two states, window displayed and window not displayed (iconified) 
         * We map these to GLUT_FULLY_RETAINED and GLUT_HIDDEN respectively.
         */
        INVOKE_WCB( *window, WindowStatus, ( visState ? GLUT_FULLY_RETAINED:GLUT_HIDDEN ) );
        fgSetWindow( saved_window );
    }

    /* Also set windowStatus/visibility state for children */
    for( child = ( SFG_Window * )window->Children.First;
         child;
         child = ( SFG_Window * )child->Node.Next )
    {
        fghPlatformOnWindowStatusNotify(child, visState, GL_FALSE); /* No need to propagate forceNotify. Childs get this from their own INIT_WORK */
    }
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
        {
            INVOKE_WCB( *window, Keyboard,
                        ( 127, window->State.MouseX, window->State.MouseY )
            );
        }
        else
        {
            INVOKE_WCB( *window, KeyboardUp,
                        ( 127, window->State.MouseX, window->State.MouseY )
            );
        }
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
                   ( (char)(wParam & 0xFF), /* and with 0xFF to indicate to runtime that we want to strip out higher bits - otherwise we get a runtime error when "Smaller Type Checks" is enabled */
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
        {
            INVOKE_WCB( *window, Special,
                        ( keypress,
                            window->State.MouseX, window->State.MouseY )
            );
        }
        else
        {
            INVOKE_WCB( *window, SpecialUp,
                        ( keypress,
                            window->State.MouseX, window->State.MouseY )
            );
        }

    fgState.Modifiers = INVALID_MODIFIERS;

    /* SYSKEY events should be sent to default window proc for system to handle them */
    if (uMsg==WM_SYSKEYDOWN || uMsg==WM_SYSKEYUP)
        return DefWindowProc( window->Window.Handle, uMsg, wParam, lParam );
    else
        return 1;
}

SFG_Window* fghWindowUnderCursor(SFG_Window *window)
{
    /* Check if the current window that the mouse is over is a child window
     * of the window the message was sent to. Some events only sent to main window,
     * and when handling some messages, we need to make sure that we process
     * callbacks on the child window instead. This mirrors how GLUT does things.
     * returns either the original window or the found child.
     */
    if (window && window->Children.First)   /* This window has childs */
    {
        HWND hwnd;
        SFG_Window* child_window;

        /* Get mouse position at time of message */
        DWORD mouse_pos_dw = GetMessagePos();
        POINT mouse_pos;
        mouse_pos.x = GET_X_LPARAM(mouse_pos_dw);
        mouse_pos.y = GET_Y_LPARAM(mouse_pos_dw);
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

static GLboolean fgAnyMouseButtonDown()
{
	// Note: GetAsyncKeyState returns an unreliable value in the least-significant bit.
	// We have to the most-significant bit to know if the mouse button is down.
	// See "Remarks" here, http://msdn.microsoft.com/en-us/library/windows/desktop/ms646293(v=vs.85).aspx
	static const int ASYNC_KEY_DOWN_BIT = 0x8000;
	return (GetAsyncKeyState(VK_LBUTTON) & ASYNC_KEY_DOWN_BIT) != 0
		|| (GetAsyncKeyState(VK_MBUTTON) & ASYNC_KEY_DOWN_BIT) != 0
		|| (GetAsyncKeyState(VK_RBUTTON) & ASYNC_KEY_DOWN_BIT) != 0;
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

    /* printf ( "Window %3d message %s (<%04x>) %12d %12d\n", window?window->ID:0,
             WMMsg2Str(uMsg), uMsg, wParam, lParam ); */

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
            fghPlatformOnWindowStatusNotify(window,GL_FALSE,GL_FALSE);
        else if (wParam==SIZE_RESTORED && !window->State.Visible)
            fghPlatformOnWindowStatusNotify(window,GL_TRUE,GL_FALSE);

        /* Check window visible, we don't want do anything when we get a WM_SIZE because the user or glutIconifyWindow minimized the window */
        if( window->State.Visible )
        {
            int width, height;
#if defined(_WIN32_WCE)
            width  = HIWORD(lParam);
            height = LOWORD(lParam);
#else
            width  = LOWORD(lParam);
            height = HIWORD(lParam);
#endif /* defined(_WIN32_WCE) */
            
            /* Update state and call callback, if there was a change */
            fghOnReshapeNotify(window, width, height, GL_FALSE);
        }

        /* according to docs, should return 0 */
        lRet = 0;
        break;

    case WM_SIZING:
        {
            /* User resize-dragging the window, call reshape callback and
             * force redisplay so display keeps running during dragging.
             * Screen still wont update when not moving the cursor though...
             */
            RECT rect;
            /* PRECT prect = (PRECT) lParam;
               printf("WM_SIZING: nc-area: %i,%i\n",prect->right-prect->left,prect->bottom-prect->top); */
            /* Get client area, the rect in lParam is including non-client area. */
            fghGetClientArea(&rect,window,FALSE);

            /* We'll get a WM_SIZE as well, but as state has
             * already been updated here, the fghOnReshapeNotify
             * in the handler for that message doesn't do anything.
             */
            fghOnReshapeNotify(window, rect.right-rect.left, rect.bottom-rect.top, GL_FALSE);

            /* Now directly call the drawing function to update
             * window and window's childs.
             * This mimics the WM_PAINT messages that are received during
             * resizing. Note that we don't have a WM_MOVING handler
             * as move-dragging doesn't generate WM_MOVE or WM_PAINT
             * messages until the mouse is released.
             */
            fghRedrawWindowAndChildren(window);
        }

        /* according to docs, should return TRUE */
        lRet = TRUE;
        break;

    case WM_MOVE:
        {
            /* Check window is minimized, we don't want to call the position callback when the user or glutIconifyWindow minimized the window */
            if (!IsIconic(window->Window.Handle))
            {
                RECT windowRect;
                
                /* lParam contains coordinates of top-left of client area.
                 * Get top-left of non-client area of window, matching coordinates of
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
                    POINT topleft;
                    topleft.x = windowRect.left;
                    topleft.y = windowRect.top;

                    ScreenToClient(window->Parent->Window.Handle,&topleft);
                    windowRect.left = topleft.x;
                    windowRect.top  = topleft.y;
                }

                /* Update state and call callback, if there was a change */
                fghOnPositionNotify(window, windowRect.left, windowRect.top, GL_FALSE);
            }
        }

        /* according to docs, should return 0 */
        lRet = 0;
        break;

    case WM_SETFOCUS:
        /*printf("WM_SETFOCUS: %p\n", window );*/
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );

        SetActiveWindow( window->Window.Handle );
        UpdateWindow ( hWnd );

        break;

    case WM_KILLFOCUS:
        /*printf("WM_KILLFOCUS: %p\n", window ); */
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );

        /* Check if there are any open menus that need to be closed */
        fgPlatformCheckMenuDeactivate((HWND)wParam);
        break;

    case WM_MOUSEACTIVATE:
        /* Clicks should not activate the menu.
         * Especially important when clicking on a menu's submenu item which has no effect.
         */
        /*printf("WM_MOUSEACTIVATE\n");*/
        if (window->IsMenu)
            lRet = MA_NOACTIVATEANDEAT;
        else
            lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;

    case WM_NCLBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
        {
            SFG_Menu *menu;
            if (fgState.ActiveMenus)
            {
                menu = fgGetActiveMenu();
                if (menu)
                  /* user clicked non-client area of window while a menu is open. Close menu */
                  fgDeactivateMenu(menu->ParentWindow);
            }

            /* and always pass to DefWindowProc */
            lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        }
        break;

#if 0
    case WM_ACTIVATE:
        /* printf("WM_ACTIVATE: %x (ID: %i) %d %d\n",lParam, window->ID, HIWORD(wParam), LOWORD(wParam)); */
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
                    SFG_Window* saved_window = fgStructure.CurrentWindow;
                    INVOKE_WCB( *window, Entry, ( GLUT_ENTERED ) );
                    fgSetWindow(saved_window);

                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = window->Window.Handle;
                    TrackMouseEvent(&tme);

                    window->State.pWState.MouseTracking = TRUE;
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

            window->State.pWState.MouseTracking = FALSE;
            lRet = 0;   /* As per docs, must return zero */
        }
        break;

    case WM_SHOWWINDOW:
        /* printf("WM_SHOWWINDOW, shown? %i, source: %i\n",wParam,lParam); */
        if (wParam)
        {
            fghPlatformOnWindowStatusNotify(window, GL_TRUE, GL_FALSE);
            window->State.WorkMask |= GLUT_DISPLAY_WORK;
        }
        else
        {
            fghPlatformOnWindowStatusNotify(window, GL_FALSE, GL_FALSE);
            window->State.WorkMask &= ~GLUT_DISPLAY_WORK;
        }
        break;

    case WM_PAINT:
    {
        RECT rect;
        
        /* As per docs, upon receiving WM_PAINT, first check if the update region is not empty before you call BeginPaint */
        if (GetUpdateRect(hWnd,&rect,FALSE))
        {
            /* Dummy begin/end paint to validate rect that needs
             * redrawing, then signal that a redisplay is needed.
             * This allows us full control about when we do any
             * redrawing, and is the same as what original GLUT
             * does.
             */
            PAINTSTRUCT ps;
            BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );

            window->State.WorkMask |= GLUT_DISPLAY_WORK;
        }
        lRet = 0;   /* As per docs, should return 0 */
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
        /* Per docs, use LOWORD/HIWORD for WinCE and GET_X_LPARAM/GET_Y_LPARAM for desktop windows */
#if defined(_WIN32_WCE)
        window->State.MouseX = 320-HIWORD( lParam );    /* XXX: Docs say x should be loword and y hiword? */
        window->State.MouseY = LOWORD( lParam );
#else
        window->State.MouseX = GET_X_LPARAM( lParam );
        window->State.MouseY = GET_Y_LPARAM( lParam );
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
        {
            INVOKE_WCB( *window, Motion, ( window->State.MouseX,
                                           window->State.MouseY ) );
        }
        else
        {
            INVOKE_WCB( *window, Passive, ( window->State.MouseX,
                                            window->State.MouseY ) );
        }

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

        /* Per docs, use LOWORD/HIWORD for WinCE and GET_X_LPARAM/GET_Y_LPARAM for desktop windows */
#if defined(_WIN32_WCE)
        window->State.MouseX = 320-HIWORD( lParam );    /* XXX: Docs say x should be loword and y hiword? */
        window->State.MouseY = LOWORD( lParam );
#else
        window->State.MouseX = GET_X_LPARAM( lParam );
        window->State.MouseY = GET_Y_LPARAM( lParam );
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
        else if (!fgAnyMouseButtonDown())
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
        int wheel_number = 0;   /* Only one scroll wheel on windows */
#if defined(_WIN32_WCE)
        int modkeys = LOWORD(wParam); 
        short ticks = (short)HIWORD(wParam);
        /* commented out as should not be needed here, mouse motion is processed in WM_MOUSEMOVE first:
        xPos = LOWORD(lParam);  -- straight from docs, not consistent with mouse nutton and mouse motion above (which i think is wrong)
        yPos = HIWORD(lParam);
        */
#else
        /* int modkeys = GET_KEYSTATE_WPARAM( wParam ); */
        short ticks = GET_WHEEL_DELTA_WPARAM( wParam );
        /* commented out as should not be needed here, mouse motion is processed in WM_MOUSEMOVE first:
        window->State.MouseX = GET_X_LPARAM( lParam );
        window->State.MouseY = GET_Y_LPARAM( lParam );
        */
#endif /* defined(_WIN32_WCE) */

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
                {
                    INVOKE_WCB( *window, MouseWheel,
                                ( wheel_number,
                                  direction,
                                  window->State.MouseX,
                                  window->State.MouseY
                                )
                    );
                }
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

#if !defined(_WIN32_WCE)
    case WM_SYNCPAINT:  /* 0x0088 */
        /* Another window has moved, need to update this one */
        window->State.WorkMask |= GLUT_DISPLAY_WORK;
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        /* Help screen says this message must be passed to "DefWindowProc" */
        break;

    case WM_DISPLAYCHANGE: /* 0x007E */
        /* The system display resolution/depth has changed */
        fgDisplay.ScreenWidth = LOWORD(lParam);
        fgDisplay.ScreenHeight = HIWORD(lParam);
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


/* deal with work list items */
void fgPlatformInitWork(SFG_Window* window)
{
    RECT windowRect;

    /* Notify windowStatus/visibility */
    fghPlatformOnWindowStatusNotify(window, window->State.Visible, GL_TRUE);

    /* get and notify window's position */
    GetWindowRect(window->Window.Handle,&windowRect);
    fghOnPositionNotify(window, windowRect.left, windowRect.top, GL_TRUE);

    /* get and notify window's size */
    GetClientRect(window->Window.Handle,&windowRect);
    fghOnReshapeNotify(window, windowRect.right-windowRect.left, windowRect.bottom-windowRect.top, GL_TRUE);
}

/* On windows we can position, resize and change z order at the same time */
void fgPlatformPosResZordWork(SFG_Window* window, unsigned int workMask)
{
    UINT flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOZORDER;
    HWND insertAfter = HWND_TOP;
    RECT clientRect;

#if !defined(_WIN32_WCE) /* FIXME: what about WinCE */
    if (workMask & GLUT_FULL_SCREEN_WORK)
    {
        /* This asks us to toggle fullscreen mode */
        flags |= SWP_FRAMECHANGED;

        if (window->State.IsFullscreen)
        {
            /* If we are fullscreen, resize the current window back to its original size */
            /* printf("OldRect %i,%i to %i,%i\n",window->State.pWState.OldRect.left,window->State.pWState.OldRect.top,window->State.pWState.OldRect.right,window->State.pWState.OldRect.bottom); */

            /* restore style of window before making it fullscreen */
            SetWindowLong(window->Window.Handle, GWL_STYLE, window->State.pWState.OldStyle);
            SetWindowLong(window->Window.Handle, GWL_EXSTYLE, window->State.pWState.OldStyleEx);

            /* Then set up resize/reposition, unless user already queued up reshape/position work */
            if (!(workMask & GLUT_POSITION_WORK))
            {
                workMask |= GLUT_POSITION_WORK;
                window->State.DesiredXpos   = window->State.pWState.OldRect.left;
                window->State.DesiredYpos   = window->State.pWState.OldRect.top;
            }
            if (!(workMask & GLUT_SIZE_WORK))
            {
                workMask |= GLUT_SIZE_WORK;
                window->State.DesiredWidth  = window->State.pWState.OldRect.right  - window->State.pWState.OldRect.left;
                window->State.DesiredHeight = window->State.pWState.OldRect.bottom - window->State.pWState.OldRect.top;
            }
                
            /* We'll finish off the fullscreen operation below after the other GLUT_POSITION_WORK|GLUT_SIZE_WORK|GLUT_ZORDER_WORK */
        }
        else
        {
            /* we are currently not fullscreen, go to fullscreen:
                * remove window decoration and then maximize
                */
            RECT rect;
            HMONITOR hMonitor;
            MONITORINFO mi;
        
            /* save current window rect, style, exstyle and maximized state */
            window->State.pWState.OldMaximized = !!IsZoomed(window->Window.Handle);
            if (window->State.pWState.OldMaximized)
                /* We force the window into restored mode before going
                    * fullscreen because Windows doesn't seem to hide the
                    * taskbar if the window is in the maximized state.
                    */
                SendMessage(window->Window.Handle, WM_SYSCOMMAND, SC_RESTORE, 0);

            fghGetClientArea( &window->State.pWState.OldRect, window, GL_TRUE );
            window->State.pWState.OldStyle   = GetWindowLong(window->Window.Handle, GWL_STYLE);
            window->State.pWState.OldStyleEx = GetWindowLong(window->Window.Handle, GWL_EXSTYLE);

            /* remove decorations from style */
            SetWindowLong(window->Window.Handle, GWL_STYLE,
                            window->State.pWState.OldStyle & ~(WS_CAPTION | WS_THICKFRAME));
            SetWindowLong(window->Window.Handle, GWL_EXSTYLE,
                            window->State.pWState.OldStyleEx & ~(WS_EX_DLGMODALFRAME |
                            WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

            /* For fullscreen mode, find the monitor that is covered the most
                * by the window and get its rect as the resize target.
	            */
            GetWindowRect(window->Window.Handle, &rect);
            hMonitor= MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
            mi.cbSize = sizeof(mi);
            GetMonitorInfo(hMonitor, &mi);
            rect = mi.rcMonitor;

            /* then setup window resize, overwriting other work queued on the window */
            window->State.WorkMask |= GLUT_POSITION_WORK | GLUT_SIZE_WORK;
            window->State.WorkMask &= ~GLUT_ZORDER_WORK;
            window->State.DesiredXpos   = rect.left;
            window->State.DesiredYpos   = rect.top;
            window->State.DesiredWidth  = rect.right  - rect.left;
            window->State.DesiredHeight = rect.bottom - rect.top;
        }
    }
#endif /*!defined(_WIN32_WCE) */

    /* Now deal with normal position, reshape and z order requests (some might have been set when handling GLUT_FULLSCREEN_WORK above */
    {
        /* get rect describing window's current position and size, 
            * in screen coordinates and in FreeGLUT format
            * (size (right-left, bottom-top) is client area size, top and left
            * are outside of window including decorations).
            */
        fghGetClientArea( &clientRect, window, TRUE );

        if (workMask & GLUT_POSITION_WORK)
        {
            flags &= ~SWP_NOMOVE;
                
            /* Move rect so that top-left is at requested position */
            /* This also automatically makes sure that child window requested coordinates are relative
                * to top-left of parent's client area (needed input for SetWindowPos on child windows),
                * so no need to further correct rect for child windows below (childs don't have decorations either).
                */
            OffsetRect(&clientRect,window->State.DesiredXpos-clientRect.left,window->State.DesiredYpos-clientRect.top);
        }
        if (workMask & GLUT_SIZE_WORK)
        {
            flags &= ~SWP_NOSIZE;
                
            /* Note on maximizing behavior of Windows: the resize borders are off
                * the screen such that the client area extends all the way from the
                * leftmost corner to the rightmost corner to maximize screen real
                * estate. A caption is still shown however to allow interaction with
                * the window controls. This is default behavior of Windows that
                * FreeGLUT sticks with. To alter, one would have to check if
                * WS_MAXIMIZE style is set when a resize event is triggered, and
                * then manually correct the windowRect to put the borders back on
                * screen.
                */

            /* Set new size of window, WxH specify client area */
            clientRect.right    = clientRect.left + window->State.DesiredWidth;
            clientRect.bottom   = clientRect.top  + window->State.DesiredHeight;
        }
        if (workMask & GLUT_ZORDER_WORK)
        {
            flags &= ~SWP_NOZORDER;

            /* Could change this to push it down or up one window at a time with some
                * more code using GetWindow with GW_HWNDPREV and GW_HWNDNEXT.
                * What would be consistent with X11? Win32 GLUT does what we do here...
                */
            if (window->State.DesiredZOrder < 0)
                insertAfter = HWND_BOTTOM;
        }
    }

    /* Adjust for window decorations
        * Child windows don't have decoration, so no need to correct
        */
    if (!window->Parent)
        /* get the window rect from this to feed to SetWindowPos, correct for window decorations */
        fghComputeWindowRectFromClientArea_QueryWindow(&clientRect,window,TRUE);
    
    /* Do the requested positioning, moving, and z order push/pop. */
    SetWindowPos( window->Window.Handle,
                    insertAfter,
                    clientRect.left, clientRect.top,
                    clientRect.right - clientRect.left,
                    clientRect.bottom- clientRect.top,
                    flags
    );

    /* Finish off the fullscreen operation we were doing, if any */
    if (workMask & GLUT_FULL_SCREEN_WORK)
    {
        if (window->State.IsFullscreen)
        {
            /* leaving fullscreen, restore maximized state, if any */
            if (window->State.pWState.OldMaximized)
                SendMessage(window->Window.Handle, WM_SYSCOMMAND, SC_MAXIMIZE, 0);

            window->State.IsFullscreen = GL_FALSE;
        }
        else
            window->State.IsFullscreen = GL_TRUE;
    }
}


void fgPlatformVisibilityWork(SFG_Window* window)
{
    /* Visibility status of window gets updated in the WM_SHOWWINDOW and WM_SIZE handlers */
    int cmdShow = 0;
    SFG_Window *win = window;
    switch (window->State.DesiredVisibility)
    {
    case DesireHiddenState:
        cmdShow = SW_HIDE;
        break;
    case DesireIconicState:
        cmdShow = SW_MINIMIZE;
        /* Call on top-level window */
        while (win->Parent)
            win = win->Parent;
        break;
    case DesireNormalState:
        if (win->IsMenu && (!fgStructure.GameModeWindow || win->ActiveMenu->ParentWindow != fgStructure.GameModeWindow))
            cmdShow = SW_SHOWNA;    /* Just show, don't activate window if its a menu. Only exception is when the parent is a gamemode window as the menu would pop under it when we do this... */
        else
            cmdShow = SW_SHOW;
        break;
    }

    ShowWindow( win->Window.Handle, cmdShow );
}
