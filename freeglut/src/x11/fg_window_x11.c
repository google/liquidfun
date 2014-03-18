/*
 * freeglut_window_x11.c
 *
 * Window management methods for X11
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Copied for Platform code by Evan Felix <karcaw at gmail.com>
 * Creation date: Thur Feb 2 2012
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
#include <limits.h>     /* LONG_MAX */
#include <unistd.h>     /* usleep, gethostname, getpid */
#include <sys/types.h>  /* pid_t */
#include "../fg_internal.h"

#ifdef EGL_VERSION_1_0
#include "egl/fg_window_egl.h"
#define fghCreateNewContext fghCreateNewContextEGL
#else
#include "x11/fg_window_x11_glx.h"
#endif

#ifdef __APPLE__
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif

static int fghResizeFullscrToggle(void)
{
    XWindowAttributes attributes;
    SFG_Window *win = fgStructure.CurrentWindow;

    if(glutGet(GLUT_FULL_SCREEN)) {
        /* restore original window size */
        fgStructure.CurrentWindow->State.WorkMask = GLUT_SIZE_WORK;
        fgStructure.CurrentWindow->State.DesiredWidth  = win->State.pWState.OldWidth;
        fgStructure.CurrentWindow->State.DesiredHeight = win->State.pWState.OldHeight;

    } else {
        fgStructure.CurrentWindow->State.pWState.OldWidth  = win->State.Width;
        fgStructure.CurrentWindow->State.pWState.OldHeight = win->State.Height;

        /* resize the window to cover the entire screen */
        XGetWindowAttributes(fgDisplay.pDisplay.Display,
                fgStructure.CurrentWindow->Window.Handle,
                &attributes);
        
        /*
         * The "x" and "y" members of "attributes" are the window's coordinates
         * relative to its parent, i.e. to the decoration window.
         */
        XMoveResizeWindow(fgDisplay.pDisplay.Display,
                fgStructure.CurrentWindow->Window.Handle,
                -attributes.x,
                -attributes.y,
                fgDisplay.ScreenWidth,
                fgDisplay.ScreenHeight);
    }
    return 0;
}

#define _NET_WM_STATE_TOGGLE    2
static int fghEwmhFullscrToggle(void)
{
    XEvent xev;
    long evmask = SubstructureRedirectMask | SubstructureNotifyMask;

    if(!fgDisplay.pDisplay.State || !fgDisplay.pDisplay.StateFullScreen) {
        return -1;
    }

    xev.type = ClientMessage;
    xev.xclient.window = fgStructure.CurrentWindow->Window.Handle;
    xev.xclient.message_type = fgDisplay.pDisplay.State;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = _NET_WM_STATE_TOGGLE;
    xev.xclient.data.l[1] = fgDisplay.pDisplay.StateFullScreen;
    xev.xclient.data.l[2] = 0;	/* no second property to toggle */
    xev.xclient.data.l[3] = 1;	/* source indication: application */
    xev.xclient.data.l[4] = 0;	/* unused */

    if(!XSendEvent(fgDisplay.pDisplay.Display, fgDisplay.pDisplay.RootWindow, 0, evmask, &xev)) {
        return -1;
    }
    return 0;
}

static int fghToggleFullscreen(void)
{
    /* first try the EWMH (_NET_WM_STATE) method ... */
    if(fghEwmhFullscrToggle() != -1) {
        return 0;
    }

    /* fall back to resizing the window */
    if(fghResizeFullscrToggle() != -1) {
        return 0;
    }
    return -1;
}

static Bool fghWindowIsVisible( Display *display, XEvent *event, XPointer arg)
{
    Window window = (Window)arg;
    return (event->type == MapNotify) && (event->xmap.window == window);
}

/*
 * Opens a window. Requires a SFG_Window object created and attached
 * to the freeglut structure. OpenGL context is created here.
 */
void fgPlatformOpenWindow( SFG_Window* window, const char* title,
                           GLboolean positionUse, int x, int y,
                           GLboolean sizeUse, int w, int h,
                           GLboolean gameMode, GLboolean isSubWindow )
{
    XVisualInfo * visualInfo = NULL;
    XSetWindowAttributes winAttr;
    XTextProperty textProperty;
    XSizeHints sizeHints;
    XWMHints wmHints;
    XEvent eventReturnBuffer; /* return buffer required for a call */
    unsigned long mask;
    unsigned int current_DisplayMode = fgState.DisplayMode ;
    XConfigureEvent fakeEvent = {0};

    /* Save the display mode if we are creating a menu window */
    if( window->IsMenu && ( ! fgStructure.MenuContext ) )
        fgState.DisplayMode = GLUT_DOUBLE | GLUT_RGB ;

#ifdef EGL_VERSION_1_0
#define WINDOW_CONFIG window->Window.pContext.egl.Config
#else
#define WINDOW_CONFIG window->Window.pContext.FBConfig
#endif
    fghChooseConfig(&WINDOW_CONFIG);

    if( window->IsMenu && ( ! fgStructure.MenuContext ) )
        fgState.DisplayMode = current_DisplayMode ;

    if( ! WINDOW_CONFIG )
    {
        /*
         * The "fghChooseConfig" returned a null meaning that the visual
         * context is not available.
         * Try a couple of variations to see if they will work.
         */
#ifndef EGL_VERSION_1_0
        if( !( fgState.DisplayMode & GLUT_DOUBLE ) )
        {
            fgState.DisplayMode |= GLUT_DOUBLE ;
            fghChooseConfig(&WINDOW_CONFIG);
            fgState.DisplayMode &= ~GLUT_DOUBLE;
        }
#endif

        if( fgState.DisplayMode & GLUT_MULTISAMPLE )
        {
            fgState.DisplayMode &= ~GLUT_MULTISAMPLE ;
            fghChooseConfig(&WINDOW_CONFIG);
            fgState.DisplayMode |= GLUT_MULTISAMPLE;
        }
    }

    FREEGLUT_INTERNAL_ERROR_EXIT( WINDOW_CONFIG != NULL,
                                  "FBConfig with necessary capabilities not found", "fgOpenWindow" );

    /*  Get the X visual.  */
#ifdef EGL_VERSION_1_0
    EGLint vid = 0;
    XVisualInfo visualTemplate;
    int num_visuals;
    if (!eglGetConfigAttrib(fgDisplay.pDisplay.egl.Display, window->Window.pContext.egl.Config, EGL_NATIVE_VISUAL_ID, &vid))
      fgError("eglGetConfigAttrib(EGL_NATIVE_VISUAL_ID) failed");
    visualTemplate.visualid = vid;
    visualInfo = XGetVisualInfo(fgDisplay.pDisplay.Display, VisualIDMask, &visualTemplate, &num_visuals);
#else
    visualInfo = glXGetVisualFromFBConfig( fgDisplay.pDisplay.Display,
					   window->Window.pContext.FBConfig );
#endif

    FREEGLUT_INTERNAL_ERROR_EXIT( visualInfo != NULL,
                                  "visualInfo could not be retrieved from FBConfig", "fgOpenWindow" );
    if (!visualInfo) return;

    /*
     * XXX HINT: the masks should be updated when adding/removing callbacks.
     * XXX       This might speed up message processing. Is that true?
     * XXX
     * XXX A: Not appreciably, but it WILL make it easier to debug.
     * XXX    Try tracing old GLUT and try tracing freeglut.  Old GLUT
     * XXX    turns off events that it doesn't need and is a whole lot
     * XXX    more pleasant to trace.  (Think mouse-motion!  Tons of
     * XXX    ``bonus'' GUI events stream in.)
     */
    winAttr.event_mask        =
        StructureNotifyMask | SubstructureNotifyMask | ExposureMask |
        ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask |
        VisibilityChangeMask | EnterWindowMask | LeaveWindowMask |
        PointerMotionMask | ButtonMotionMask;
    winAttr.background_pixmap = None;
    winAttr.background_pixel  = 0;
    winAttr.border_pixel      = 0;

    winAttr.colormap = XCreateColormap(
        fgDisplay.pDisplay.Display, fgDisplay.pDisplay.RootWindow,
        visualInfo->visual, AllocNone
    );

    mask = CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask;

    if( window->IsMenu || ( gameMode == GL_TRUE ) )
    {
        winAttr.override_redirect = True;
        mask |= CWOverrideRedirect;
    }

    if( ! positionUse )
        x = y = -1; /* default window position */
    if( ! sizeUse )
        w = h = 300; /* default window size */

    window->Window.Handle = XCreateWindow(
        fgDisplay.pDisplay.Display,
        window->Parent == NULL ? fgDisplay.pDisplay.RootWindow :
        window->Parent->Window.Handle,
        x, y, w, h, 0,
        visualInfo->depth, InputOutput,
        visualInfo->visual, mask,
        &winAttr
    );

    /* Fake configure event to force viewport setup
     * even with no window manager.
     */
    fakeEvent.type = ConfigureNotify;
    fakeEvent.display = fgDisplay.pDisplay.Display;
    fakeEvent.window = window->Window.Handle;
    fakeEvent.x = x;
    fakeEvent.y = y;
    fakeEvent.width = w;
    fakeEvent.height = h;
    XPutBackEvent(fgDisplay.pDisplay.Display, (XEvent*)&fakeEvent);

    /*
     * The GLX context creation, possibly trying the direct context rendering
     *  or else use the current context if the user has so specified
     */

    if( window->IsMenu )
    {
        /*
         * If there isn't already an OpenGL rendering context for menu
         * windows, make one
         */
        if( !fgStructure.MenuContext )
        {
            fgStructure.MenuContext =
                (SFG_MenuContext *)malloc( sizeof(SFG_MenuContext) );
            fgStructure.MenuContext->MContext = fghCreateNewContext( window );
        }

        /* window->Window.Context = fgStructure.MenuContext->MContext; */
        window->Window.Context = fghCreateNewContext( window );
    }
    else if( fgState.UseCurrentContext )
    {

#ifdef EGL_VERSION_1_0
        window->Window.Context = eglGetCurrentContext( );
#else
        window->Window.Context = glXGetCurrentContext( );
#endif

        if( ! window->Window.Context )
            window->Window.Context = fghCreateNewContext( window );
    }
    else
        window->Window.Context = fghCreateNewContext( window );

#if !defined( __FreeBSD__ ) && !defined( __NetBSD__ ) && !defined(EGL_VERSION_1_0)
    if(  !glXIsDirect( fgDisplay.pDisplay.Display, window->Window.Context ) )
    {
      if( fgState.DirectContext == GLUT_FORCE_DIRECT_CONTEXT )
        fgError( "Unable to force direct context rendering for window '%s'",
                 title );
    }
#endif

    sizeHints.flags = 0;
    if ( positionUse )
        sizeHints.flags |= USPosition;
    if ( sizeUse )
        sizeHints.flags |= USSize;

    /*
     * Fill in the size hints values now (the x, y, width and height
     * settings are obsolete, are there any more WMs that support them?)
     * Unless the X servers actually stop supporting these, we should
     * continue to fill them in.  It is *not* our place to tell the user
     * that they should replace a window manager that they like, and which
     * works, just because *we* think that it's not "modern" enough.
     */
    sizeHints.x      = x;
    sizeHints.y      = y;
    sizeHints.width  = w;
    sizeHints.height = h;

    wmHints.flags = StateHint;
    wmHints.initial_state = fgState.ForceIconic ? IconicState : NormalState;
    /* Prepare the window and iconified window names... */
    XStringListToTextProperty( (char **) &title, 1, &textProperty );

    XSetWMProperties(
        fgDisplay.pDisplay.Display,
        window->Window.Handle,
        &textProperty,
        &textProperty,
        0,
        0,
        &sizeHints,
        &wmHints,
        NULL
    );
    XFree( textProperty.value );

    XSetWMProtocols( fgDisplay.pDisplay.Display, window->Window.Handle,
                     &fgDisplay.pDisplay.DeleteWindow, 1 );

    if (fgDisplay.pDisplay.NetWMSupported
        && fgDisplay.pDisplay.NetWMPid != None
        && fgDisplay.pDisplay.ClientMachine != None)
    {
      char hostname[HOST_NAME_MAX];
      pid_t pid = getpid();

      if (pid > 0 && gethostname(hostname, sizeof(hostname)) > -1)
      {
        hostname[sizeof(hostname) - 1] = '\0';

        XChangeProperty(
            fgDisplay.pDisplay.Display,
            window->Window.Handle,
            fgDisplay.pDisplay.NetWMPid,
            XA_CARDINAL,
            32,
            PropModeReplace,
            (unsigned char *) &pid,
            1
        );

        XChangeProperty(
            fgDisplay.pDisplay.Display,
            window->Window.Handle,
            fgDisplay.pDisplay.ClientMachine,
            XA_STRING,
            8,
            PropModeReplace,
            (unsigned char *) hostname,
            strlen(hostname)
        );
      }
    }

#ifdef EGL_VERSION_1_0
    fghPlatformOpenWindowEGL(window);
#else
    glXMakeContextCurrent(
        fgDisplay.pDisplay.Display,
        window->Window.Handle,
        window->Window.Handle,
        window->Window.Context
    );
#endif

    /* register extension events _before_ window is mapped */
    #ifdef HAVE_X11_EXTENSIONS_XINPUT2_H
       fgRegisterDevices( fgDisplay.pDisplay.Display, &(window->Window.Handle) );
    #endif

    if (!window->IsMenu)    /* Don't show window after creation if its a menu */
    {
        XMapWindow( fgDisplay.pDisplay.Display, window->Window.Handle );
        window->State.Visible = GL_TRUE;
    }

    XFree(visualInfo);

    /* wait till window visible */
    if( !isSubWindow && !window->IsMenu)
        XPeekIfEvent( fgDisplay.pDisplay.Display, &eventReturnBuffer, &fghWindowIsVisible, (XPointer)(window->Window.Handle) );
#undef WINDOW_CONFIG
}


/*
 * Request a window resize
 */
void fgPlatformReshapeWindow ( SFG_Window *window, int width, int height )
{
    XResizeWindow( fgDisplay.pDisplay.Display, window->Window.Handle,
                   width, height );
    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
}


/*
 * Closes a window, destroying the frame and OpenGL context
 */
void fgPlatformCloseWindow( SFG_Window* window )
{
#ifdef EGL_VERSION_1_0
    fghPlatformCloseWindowEGL(window);
#else
    if( window->Window.Context )
        glXDestroyContext( fgDisplay.pDisplay.Display, window->Window.Context );
    window->Window.pContext.FBConfig = NULL;
#endif

    if( window->Window.Handle ) {
        XDestroyWindow( fgDisplay.pDisplay.Display, window->Window.Handle );
    }
    /* XFlush( fgDisplay.pDisplay.Display ); */ /* XXX Shouldn't need this */
}


/*
 * This function makes the specified window visible
 */
void fgPlatformShowWindow( SFG_Window *window )
{
    XMapWindow( fgDisplay.pDisplay.Display, window->Window.Handle );
    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
}

/*
 * This function hides the specified window
 */
void fgPlatformHideWindow( SFG_Window *window )
{
    if( window->Parent == NULL )
        XWithdrawWindow( fgDisplay.pDisplay.Display,
                         window->Window.Handle,
                         fgDisplay.pDisplay.Screen );
    else
        XUnmapWindow( fgDisplay.pDisplay.Display,
                      window->Window.Handle );
    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
}

/*
 * Iconify the specified window (top-level windows only)
 */
void fgPlatformIconifyWindow( SFG_Window *window )
{
    XIconifyWindow( fgDisplay.pDisplay.Display, window->Window.Handle,
                    fgDisplay.pDisplay.Screen );
    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */

    fgStructure.CurrentWindow->State.Visible   = GL_FALSE;
}

/*
 * Set the current window's title
 */
void fgPlatformGlutSetWindowTitle( const char* title )
{
    XTextProperty text;

    text.value = (unsigned char *) title;
    text.encoding = XA_STRING;
    text.format = 8;
    text.nitems = strlen( title );

    XSetWMName(
        fgDisplay.pDisplay.Display,
        fgStructure.CurrentWindow->Window.Handle,
        &text
    );

    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
}

/*
 * Set the current window's iconified title
 */
void fgPlatformGlutSetIconTitle( const char* title )
{
    XTextProperty text;

    text.value = (unsigned char *) title;
    text.encoding = XA_STRING;
    text.format = 8;
    text.nitems = strlen( title );

    XSetWMIconName(
        fgDisplay.pDisplay.Display,
        fgStructure.CurrentWindow->Window.Handle,
        &text
    );

    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
}

/*
 * Change the specified window's position
 */
void fgPlatformPositionWindow( SFG_Window *window, int x, int y )
{
    XMoveWindow( fgDisplay.pDisplay.Display, window->Window.Handle,
                 x, y );
    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
}

/*
 * Lowers the specified window (by Z order change)
 */
void fgPlatformPushWindow( SFG_Window *window )
{
    XLowerWindow( fgDisplay.pDisplay.Display, window->Window.Handle );
}

/*
 * Raises the specified window (by Z order change)
 */
void fgPlatformPopWindow( SFG_Window *window )
{
    XRaiseWindow( fgDisplay.pDisplay.Display, window->Window.Handle );
}

/*
 * Toggle the window's full screen state.
 */
void fgPlatformFullScreenToggle( SFG_Window *win )
{
    if(fghToggleFullscreen() != -1) {
        win->State.IsFullscreen = !win->State.IsFullscreen;
    }
}

