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
#include <limits.h>  /* LONG_MAX */
#include <unistd.h>  /* usleep */
#include "../fg_internal.h"

/* pushing attribute/value pairs into an array */
#define ATTRIB(a) attributes[where++]=(a)
#define ATTRIB_VAL(a,v) {ATTRIB(a); ATTRIB(v);}


#ifndef GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB
#define GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20B2
#endif

#ifndef GLX_CONTEXT_MAJOR_VERSION_ARB
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#endif

#ifndef GLX_CONTEXT_MINOR_VERSION_ARB
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#endif

#ifndef GLX_CONTEXT_FLAGS_ARB
#define GLX_CONTEXT_FLAGS_ARB 0x2094
#endif

#ifndef GLX_CONTEXT_PROFILE_MASK_ARB
#define GLX_CONTEXT_PROFILE_MASK_ARB 0x9126
#endif

#ifndef GLX_CONTEXT_DEBUG_BIT_ARB
#define GLX_CONTEXT_DEBUG_BIT_ARB 0x0001
#endif

#ifndef GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002
#endif

#ifndef GLX_CONTEXT_CORE_PROFILE_BIT_ARB
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#endif

#ifndef GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
#define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif

#ifndef GLX_RGBA_FLOAT_TYPE
#define GLX_RGBA_FLOAT_TYPE 0x20B9
#endif

#ifndef GLX_RGBA_FLOAT_BIT
#define GLX_RGBA_FLOAT_BIT 0x00000004
#endif


/*
 * Chooses a visual basing on the current display mode settings
 */

GLXFBConfig* fgPlatformChooseFBConfig( int *numcfgs )
{
  GLboolean wantIndexedMode = GL_FALSE;
  int attributes[ 100 ];
  int where = 0, numAuxBuffers;

  /* First we have to process the display mode settings... */
  if( fgState.DisplayMode & GLUT_INDEX ) {
    ATTRIB_VAL( GLX_BUFFER_SIZE, 8 );
    /*  Buffer size is selected later.  */

    ATTRIB_VAL( GLX_RENDER_TYPE, GLX_COLOR_INDEX_BIT );
    wantIndexedMode = GL_TRUE;
  } else {
    ATTRIB_VAL( GLX_RED_SIZE,   1 );
    ATTRIB_VAL( GLX_GREEN_SIZE, 1 );
    ATTRIB_VAL( GLX_BLUE_SIZE,  1 );
    if( fgState.DisplayMode & GLUT_ALPHA ) {
      ATTRIB_VAL( GLX_ALPHA_SIZE, 1 );
    }
  }

  if( fgState.DisplayMode & GLUT_DOUBLE ) {
    ATTRIB_VAL( GLX_DOUBLEBUFFER, True );
  }

  if( fgState.DisplayMode & GLUT_STEREO ) {
    ATTRIB_VAL( GLX_STEREO, True );
  }

  if( fgState.DisplayMode & GLUT_DEPTH ) {
    ATTRIB_VAL( GLX_DEPTH_SIZE, 1 );
  }

  if( fgState.DisplayMode & GLUT_STENCIL ) {
    ATTRIB_VAL( GLX_STENCIL_SIZE, 1 );
  }

  if( fgState.DisplayMode & GLUT_ACCUM ) {
    ATTRIB_VAL( GLX_ACCUM_RED_SIZE, 1 );
    ATTRIB_VAL( GLX_ACCUM_GREEN_SIZE, 1 );
    ATTRIB_VAL( GLX_ACCUM_BLUE_SIZE, 1 );
    if( fgState.DisplayMode & GLUT_ALPHA ) {
      ATTRIB_VAL( GLX_ACCUM_ALPHA_SIZE, 1 );
    }
  }

  numAuxBuffers = fghNumberOfAuxBuffersRequested();
  if ( numAuxBuffers > 0 ) {
    ATTRIB_VAL( GLX_AUX_BUFFERS, numAuxBuffers );
  }

  if( fgState.DisplayMode & GLUT_SRGB ) {
    ATTRIB_VAL( GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, True );
  }

  if (fgState.DisplayMode & GLUT_MULTISAMPLE) {
    ATTRIB_VAL(GLX_SAMPLE_BUFFERS, 1);
    ATTRIB_VAL(GLX_SAMPLES, fgState.SampleNumber);
  }

  /* Push a terminator at the end of the list */
  ATTRIB( None );

    {
        GLXFBConfig * fbconfigArray;  /*  Array of FBConfigs  */
        GLXFBConfig * fbconfig;       /*  The FBConfig we want  */
        int fbconfigArraySize;        /*  Number of FBConfigs in the array  */


        /*  Get all FBConfigs that match "attributes".  */
        fbconfigArray = glXChooseFBConfig( fgDisplay.pDisplay.Display,
                                           fgDisplay.pDisplay.Screen,
                                           attributes,
                                           &fbconfigArraySize );

        if (fbconfigArray != NULL)
        {
            int result;  /* Returned by glXGetFBConfigAttrib, not checked. */


            if( wantIndexedMode )
            {
                /*
                 * In index mode, we want the largest buffer size, i.e. visual
                 * depth.  Here, FBConfigs are sorted by increasing buffer size
                 * first, so FBConfigs with the largest size come last.
                 */

                int bufferSizeMin, bufferSizeMax;

                /*  Get bufferSizeMin.  */
                result =
                  glXGetFBConfigAttrib( fgDisplay.pDisplay.Display,
                                        fbconfigArray[0],
                                        GLX_BUFFER_SIZE,
                                        &bufferSizeMin );
                /*  Get bufferSizeMax.  */
                result =
                  glXGetFBConfigAttrib( fgDisplay.pDisplay.Display,
                                        fbconfigArray[fbconfigArraySize - 1],
                                        GLX_BUFFER_SIZE,
                                        &bufferSizeMax );

                if (bufferSizeMax > bufferSizeMin)
                {
                    /* 
                     * Free and reallocate fbconfigArray, keeping only FBConfigs
                     * with the largest buffer size.
                     */
                    XFree(fbconfigArray);

                    /*  Add buffer size token at the end of the list.  */
                    where--;
                    ATTRIB_VAL( GLX_BUFFER_SIZE, bufferSizeMax );
                    ATTRIB( None );

                    fbconfigArray = glXChooseFBConfig( fgDisplay.pDisplay.Display,
                                                       fgDisplay.pDisplay.Screen,
                                                       attributes,
                                                       &fbconfigArraySize );
                }
            }

            /*
             * We now have an array of FBConfigs, the first one being the "best"
             * one.  So we should return only this FBConfig:
             *
             * int fbconfigXID;
             *
             *  - pick the XID of the FBConfig we want
             * result = glXGetFBConfigAttrib( fgDisplay.pDisplay.Display,
             *                                fbconfigArray[0],
             *                                GLX_FBCONFIG_ID,
             *                                &fbconfigXID );
             *
             * - free the array
             * XFree(fbconfigArray);
             *
             * - reset "attributes" with the XID
             * where = 0;
             * ATTRIB_VAL( GLX_FBCONFIG_ID, fbconfigXID );
             * ATTRIB( None );
             *
             * - get our FBConfig only
             * fbconfig = glXChooseFBConfig( fgDisplay.pDisplay.Display,
             *                               fgDisplay.pDisplay.Screen,
             *                               attributes,
             *                               &fbconfigArraySize );
             *
             * However, for some configurations (for instance multisampling with
             * Mesa 6.5.2 and ATI drivers), this does not work:
             * glXChooseFBConfig returns NULL, whereas fbconfigXID is a valid
             * XID.  Further investigation is needed.
             *
             * So, for now, we return the whole array of FBConfigs.  This should
             * not produce any side effects elsewhere.
             */
            fbconfig = fbconfigArray;
        }
        else
        {
           fbconfig = NULL;
        }

	if (numcfgs)
		*numcfgs = fbconfigArraySize;

        return fbconfig;
    }
}


static void fghFillContextAttributes( int *attributes ) {
  int where = 0, contextFlags, contextProfile;

  if ( !fghIsLegacyContextVersionRequested() ) {
    ATTRIB_VAL( GLX_CONTEXT_MAJOR_VERSION_ARB, fgState.MajorVersion );
    ATTRIB_VAL( GLX_CONTEXT_MINOR_VERSION_ARB, fgState.MinorVersion );
  }

  contextFlags =
    fghMapBit( fgState.ContextFlags, GLUT_DEBUG, GLX_CONTEXT_DEBUG_BIT_ARB ) |
    fghMapBit( fgState.ContextFlags, GLUT_FORWARD_COMPATIBLE, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB );
  if ( contextFlags != 0 ) {
    ATTRIB_VAL( GLX_CONTEXT_FLAGS_ARB, contextFlags );
  }

  contextProfile =
    fghMapBit( fgState.ContextProfile, GLUT_CORE_PROFILE, GLX_CONTEXT_CORE_PROFILE_BIT_ARB ) |
    fghMapBit( fgState.ContextProfile, GLUT_COMPATIBILITY_PROFILE, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB );
  if ( contextProfile != 0 ) {
    ATTRIB_VAL( GLX_CONTEXT_PROFILE_MASK_ARB, contextProfile );
  }

  ATTRIB( 0 );
}

typedef GLXContext (*CreateContextAttribsProc)(Display *dpy, GLXFBConfig config,
					       GLXContext share_list, Bool direct,
					       const int *attrib_list);

static GLXContext fghCreateNewContext( SFG_Window* window )
{
  /* for color model calculation */
  int menu = ( window->IsMenu && !fgStructure.MenuContext );
  int index_mode = ( fgState.DisplayMode & GLUT_INDEX );

  /* "classic" context creation */
  Display *dpy = fgDisplay.pDisplay.Display;
  GLXFBConfig config = *(window->Window.pContext.FBConfig);
  int render_type = ( !menu && index_mode ) ? GLX_COLOR_INDEX_TYPE : GLX_RGBA_TYPE;
  GLXContext share_list = NULL;
  Bool direct = ( fgState.DirectContext != GLUT_FORCE_INDIRECT_CONTEXT );
  GLXContext context;

  /* new context creation */
  int attributes[9];
  CreateContextAttribsProc createContextAttribs = (CreateContextAttribsProc) fgPlatformGetProcAddress( "glXCreateContextAttribsARB" );
 
  /* glXCreateContextAttribsARB not found, yet the user has requested the new context creation */
  if ( !createContextAttribs && !fghIsLegacyContextRequested() ) {
    fgWarning( "OpenGL >2.1 context requested but glXCreateContextAttribsARB is not available! Falling back to legacy context creation" );
	fgState.MajorVersion = 2;
	fgState.MinorVersion = 1;
  }

  /* If nothing fancy has been required, simply use the old context creation GLX API entry */
  if ( fghIsLegacyContextRequested() || !createContextAttribs )
  {
    context = glXCreateNewContext( dpy, config, render_type, share_list, direct );
    if ( context == NULL ) {
      fghContextCreationError();
    }
    return context;
  }

  /* color index mode is not available anymore with OpenGL 3.0 */
  if ( render_type == GLX_COLOR_INDEX_TYPE ) {
    fgWarning( "color index mode is deprecated, using RGBA mode" );
  }

  fghFillContextAttributes( attributes );

  context = createContextAttribs( dpy, config, share_list, direct, attributes );
  if ( context == NULL ) {
    fghContextCreationError();
  }
  return context;
}


#define _NET_WM_STATE_TOGGLE    2
static int fghResizeFullscrToggle(void)
{
    XWindowAttributes attributes;

    if(glutGet(GLUT_FULL_SCREEN)) {
        /* restore original window size */
        SFG_Window *win = fgStructure.CurrentWindow;
        fgStructure.CurrentWindow->State.NeedToResize = GL_TRUE;
        fgStructure.CurrentWindow->State.Width  = win->State.pWState.OldWidth;
        fgStructure.CurrentWindow->State.Height = win->State.pWState.OldHeight;

    } else {
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

void fgPlatformSetWindow ( SFG_Window *window )
{
    if ( window )
    {
        glXMakeContextCurrent(
            fgDisplay.pDisplay.Display,
            window->Window.Handle,
            window->Window.Handle,
            window->Window.Context
        );
    }
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
    int num_FBConfigs, i;
    unsigned int current_DisplayMode = fgState.DisplayMode ;

    /* Save the display mode if we are creating a menu window */
    if( window->IsMenu && ( ! fgStructure.MenuContext ) )
        fgState.DisplayMode = GLUT_DOUBLE | GLUT_RGB ;

    window->Window.pContext.FBConfig = fgPlatformChooseFBConfig( &num_FBConfigs );

    if( window->IsMenu && ( ! fgStructure.MenuContext ) )
        fgState.DisplayMode = current_DisplayMode ;

    if( ! window->Window.pContext.FBConfig )
    {
        /*
         * The "fgPlatformChooseFBConfig" returned a null meaning that the visual
         * context is not available.
         * Try a couple of variations to see if they will work.
         */
        if( !( fgState.DisplayMode & GLUT_DOUBLE ) )
        {
            fgState.DisplayMode |= GLUT_DOUBLE ;
            window->Window.pContext.FBConfig = fgPlatformChooseFBConfig( &num_FBConfigs );
            fgState.DisplayMode &= ~GLUT_DOUBLE;
        }

        if( fgState.DisplayMode & GLUT_MULTISAMPLE )
        {
            fgState.DisplayMode &= ~GLUT_MULTISAMPLE ;
            window->Window.pContext.FBConfig = fgPlatformChooseFBConfig( &num_FBConfigs );
            fgState.DisplayMode |= GLUT_MULTISAMPLE;
        }
    }

    FREEGLUT_INTERNAL_ERROR_EXIT( window->Window.pContext.FBConfig != NULL,
                                  "FBConfig with necessary capabilities not found", "fgOpenWindow" );

    /*  Get the X visual.  */
    for (i = 0; i < num_FBConfigs; i++) {
	    visualInfo = glXGetVisualFromFBConfig( fgDisplay.pDisplay.Display,
						   window->Window.pContext.FBConfig[i] );
	    if (visualInfo)
		break;
    }

    FREEGLUT_INTERNAL_ERROR_EXIT( visualInfo != NULL,
                                  "visualInfo could not be retrieved from FBConfig", "fgOpenWindow" );

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
        window->Window.Context = glXGetCurrentContext( );

        if( ! window->Window.Context )
            window->Window.Context = fghCreateNewContext( window );
    }
    else
        window->Window.Context = fghCreateNewContext( window );

#if !defined( __FreeBSD__ ) && !defined( __NetBSD__ )
    if(  !glXIsDirect( fgDisplay.pDisplay.Display, window->Window.Context ) )
    {
      if( fgState.DirectContext == GLUT_FORCE_DIRECT_CONTEXT )
        fgError( "Unable to force direct context rendering for window '%s'",
                 title );
    }
#endif

    /*
     * XXX Assume the new window is visible by default
     * XXX Is this a  safe assumption?
     */
    window->State.Visible = GL_TRUE;

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

    glXMakeContextCurrent(
        fgDisplay.pDisplay.Display,
        window->Window.Handle,
        window->Window.Handle,
        window->Window.Context
    );

    /* register extension events _before_ window is mapped */
    #ifdef HAVE_X11_EXTENSIONS_XINPUT2_H
       fgRegisterDevices( fgDisplay.pDisplay.Display, &(window->Window.Handle) );
    #endif

    XMapWindow( fgDisplay.pDisplay.Display, window->Window.Handle );

    XFree(visualInfo);

    if( !isSubWindow)
        XPeekIfEvent( fgDisplay.pDisplay.Display, &eventReturnBuffer, &fghWindowIsVisible, (XPointer)(window->Window.Handle) );
}


/*
 * Closes a window, destroying the frame and OpenGL context
 */
void fgPlatformCloseWindow( SFG_Window* window )
{
    if( window->Window.Context )
        glXDestroyContext( fgDisplay.pDisplay.Display, window->Window.Context );
    XFree( window->Window.pContext.FBConfig );

    if( window->Window.Handle ) {
        XDestroyWindow( fgDisplay.pDisplay.Display, window->Window.Handle );
    }
    /* XFlush( fgDisplay.pDisplay.Display ); */ /* XXX Shouldn't need this */
}


/*
 * This function makes the current window visible
 */
void fgPlatformGlutShowWindow( void )
{
    XMapWindow( fgDisplay.pDisplay.Display, fgStructure.CurrentWindow->Window.Handle );
    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
}

/*
 * This function hides the current window
 */
void fgPlatformGlutHideWindow( void )
{
    if( fgStructure.CurrentWindow->Parent == NULL )
        XWithdrawWindow( fgDisplay.pDisplay.Display,
                         fgStructure.CurrentWindow->Window.Handle,
                         fgDisplay.pDisplay.Screen );
    else
        XUnmapWindow( fgDisplay.pDisplay.Display,
                      fgStructure.CurrentWindow->Window.Handle );
    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
}

/*
 * Iconify the current window (top-level windows only)
 */
void fgPlatformGlutIconifyWindow( void )
{
    XIconifyWindow( fgDisplay.pDisplay.Display, fgStructure.CurrentWindow->Window.Handle,
                    fgDisplay.pDisplay.Screen );
    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
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
 * Change the current window's position
 */
void fgPlatformGlutPositionWindow( int x, int y )
{
    XMoveWindow( fgDisplay.pDisplay.Display, fgStructure.CurrentWindow->Window.Handle,
                 x, y );
    XFlush( fgDisplay.pDisplay.Display ); /* XXX Shouldn't need this */
}

/*
 * Lowers the current window (by Z order change)
 */
void fgPlatformGlutPushWindow( void )
{
    XLowerWindow( fgDisplay.pDisplay.Display, fgStructure.CurrentWindow->Window.Handle );
}

/*
 * Raises the current window (by Z order change)
 */
void fgPlatformGlutPopWindow( void )
{
    XRaiseWindow( fgDisplay.pDisplay.Display, fgStructure.CurrentWindow->Window.Handle );
}

/*
 * Resize the current window so that it fits the whole screen
 */
void fgPlatformGlutFullScreen( SFG_Window *win )
{
    if(!glutGet(GLUT_FULL_SCREEN)) {
        if(fghToggleFullscreen() != -1) {
            win->State.IsFullscreen = GL_TRUE;
        }
    }
}

/*
 * If we are fullscreen, resize the current window back to its original size
 */
void fgPlatformGlutLeaveFullScreen( SFG_Window *win )
{
    if(glutGet(GLUT_FULL_SCREEN)) {
        if(fghToggleFullscreen() != -1) {
            win->State.IsFullscreen = GL_FALSE;
        }
    }
}

/*
 * Toggle the window's full screen state.
 */
void fgPlatformGlutFullScreenToggle( SFG_Window *win )
{
    if(fghToggleFullscreen() != -1) {
        win->State.IsFullscreen = !win->State.IsFullscreen;
    }
}

