/*
 * freeglut_internal_x11.h
 *
 * The freeglut library private include file.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by Diederick C. Niehorster, <dcnieho@gmail.com>
 * Creation date: Fri Jan 20, 2012
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

#ifndef  FREEGLUT_INTERNAL_X11_H
#define  FREEGLUT_INTERNAL_X11_H


/* -- PLATFORM-SPECIFIC INCLUDES ------------------------------------------- */
#ifdef EGL_VERSION_1_0
#include "egl/fg_internal_egl.h"
#else
#include <GL/glx.h>
#include "x11/fg_internal_x11_glx.h"
#endif
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput.h>
#ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
#    include <X11/extensions/xf86vmode.h>
#endif
#ifdef HAVE_X11_EXTENSIONS_XRANDR_H
#    include <X11/extensions/Xrandr.h>
#endif


/* -- GLOBAL TYPE DEFINITIONS ---------------------------------------------- */
/* The structure used by display initialization in freeglut_init.c */
typedef struct tagSFG_PlatformDisplay SFG_PlatformDisplay;
struct tagSFG_PlatformDisplay
{
    Display*        Display;            /* The display we are being run in.  */
    int             Screen;             /* The screen we are about to use.   */
    Window          RootWindow;         /* The screen's root window.         */
    int             Connection;         /* The display's connection number   */
    Atom            DeleteWindow;       /* The window deletion atom          */
    Atom            State;              /* The state atom                    */
    Atom            StateFullScreen;    /* The full screen atom              */
    int             NetWMSupported;     /* Flag for EWMH Window Managers     */
    Atom            NetWMPid;           /* The _NET_WM_PID atom              */
    Atom            ClientMachine;      /* The client machine name atom      */

#ifdef HAVE_X11_EXTENSIONS_XRANDR_H
    int prev_xsz, prev_ysz;
    int prev_refresh;
    int prev_size_valid;
#endif	/* HAVE_X11_EXTENSIONS_XRANDR_H */

#ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
    /*
     * XF86VidMode may be compilable even if it fails at runtime.  Therefore,
     * the validity of the VidMode has to be tracked
     */
    int             DisplayModeValid;   /* Flag that indicates runtime status*/
    XF86VidModeModeLine DisplayMode;    /* Current screen's display settings */
    int             DisplayModeClock;   /* The display mode's refresh rate   */
    int             DisplayViewPortX;   /* saved X location of the viewport  */
    int             DisplayViewPortY;   /* saved Y location of the viewport  */
#endif /* HAVE_X11_EXTENSIONS_XF86VMODE_H */

#ifdef EGL_VERSION_1_0
    struct tagSFG_PlatformDisplayEGL egl;
#endif

    int             DisplayPointerX;    /* saved X location of the pointer   */
    int             DisplayPointerY;    /* saved Y location of the pointer   */
};


/*
 * Make "freeglut" window handle and context types so that we don't need so
 * much conditionally-compiled code later in the library.
 */
#ifndef EGL_VERSION_1_0
typedef Window     SFG_WindowHandleType ;
typedef GLXContext SFG_WindowContextType ;
#endif
typedef struct tagSFG_PlatformContext SFG_PlatformContext;
struct tagSFG_PlatformContext
{
#ifdef EGL_VERSION_1_0
    struct tagSFG_PlatformContextEGL egl;
#else
    GLXFBConfig    FBConfig;        /* The window's FBConfig               */
#endif
};


/* Window's state description. This structure should be kept portable. */
typedef struct tagSFG_PlatformWindowState SFG_PlatformWindowState;
struct tagSFG_PlatformWindowState
{
    int             OldWidth;           /* Window width from before a resize */
    int             OldHeight;          /*   "    height  "    "    "   "    */
    GLboolean       KeyRepeating;       /* Currently in repeat mode?         */    
};


/* -- JOYSTICK-SPECIFIC STRUCTURES AND TYPES ------------------------------- */
/*
 * Initial defines from "js.h" starting around line 33 with the existing "freeglut_joystick.c"
 * interspersed
 */
#    ifdef HAVE_SYS_IOCTL_H
#        include <sys/ioctl.h>
#    endif
#    ifdef HAVE_FCNTL_H
#        include <fcntl.h>
#    endif
#    ifdef HAVE_ERRNO_H
#        include <errno.h>
#        include <string.h>
#    endif
#    if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
/* XXX The below hack is done until freeglut's autoconf is updated. */
#        define HAVE_USB_JS    1

#        if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#            include <sys/joystick.h>
#        else
/*
 * XXX NetBSD/amd64 systems may find that they have to steal the
 * XXX /usr/include/machine/joystick.h from a NetBSD/i386 system.
 * XXX I cannot comment whether that works for the interface, but
 * XXX it lets you compile...(^&  I do not think that we can do away
 * XXX with this header.
 */
#            include <machine/joystick.h>         /* For analog joysticks */
#        endif
#        define JS_DATA_TYPE joystick
#        define JS_RETURN (sizeof(struct JS_DATA_TYPE))
#    endif

#    if defined(__linux__)
#        include <linux/joystick.h>

/* check the joystick driver version */
#        if defined(JS_VERSION) && JS_VERSION >= 0x010000
#            define JS_NEW
#        endif
#    else  /* Not BSD or Linux */
#        ifndef JS_RETURN

  /*
   * We'll put these values in and that should
   * allow the code to at least compile when there is
   * no support. The JS open routine should error out
   * and shut off all the code downstream anyway and if
   * the application doesn't use a joystick we'll be fine.
   */

  struct JS_DATA_TYPE
  {
    int buttons;
    int x;
    int y;
  };

#            define JS_RETURN (sizeof(struct JS_DATA_TYPE))
#        endif
#    endif

/* XXX It might be better to poll the operating system for the numbers of buttons and
 * XXX axes and then dynamically allocate the arrays.
 */
#    define _JS_MAX_AXES 16
typedef struct tagSFG_PlatformJoystick SFG_PlatformJoystick;
struct tagSFG_PlatformJoystick
{
#   if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
       struct os_specific_s *os;
#   endif

#   ifdef JS_NEW
       struct js_event     js;
       int          tmp_buttons;
       float        tmp_axes [ _JS_MAX_AXES ];
#   else
       struct JS_DATA_TYPE js;
#   endif

    char         fname [ 128 ];
    int          fd;
};


/* Menu font and color definitions */
#define  FREEGLUT_MENU_FONT    GLUT_BITMAP_HELVETICA_18

#define  FREEGLUT_MENU_PEN_FORE_COLORS   {0.0f,  0.0f,  0.0f,  1.0f}
#define  FREEGLUT_MENU_PEN_BACK_COLORS   {0.70f, 0.70f, 0.70f, 1.0f}
#define  FREEGLUT_MENU_PEN_HFORE_COLORS  {0.0f,  0.0f,  0.0f,  1.0f}
#define  FREEGLUT_MENU_PEN_HBACK_COLORS  {1.0f,  1.0f,  1.0f,  1.0f}




/* -- PRIVATE FUNCTION DECLARATIONS ---------------------------------------- */
/* spaceball device functions, defined in freeglut_spaceball.c */
int             fgIsSpaceballXEvent( const XEvent *ev );
void            fgSpaceballHandleXEvent( const XEvent *ev );

/*
 * Check if "hint" is present in "property" for "window".  See freeglut_init.c
 */
int             fgHintPresent(Window window, Atom property, Atom hint);

/* Handler for X extension Events */
#ifdef HAVE_X11_EXTENSIONS_XINPUT2_H
  void          fgHandleExtensionEvents( XEvent * ev );
  void          fgRegisterDevices( Display* dpy, Window* win );
#endif


#endif  /* FREEGLUT_INTERNAL_X11_H */
