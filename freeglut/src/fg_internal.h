/*
 * fg_internal.h
 *
 * The freeglut library private include file.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Thu Dec 2 1999
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

#ifndef  FREEGLUT_INTERNAL_H
#define  FREEGLUT_INTERNAL_H

#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

/* Freeglut is intended to function under all Unix/X11 and Win32 platforms. */
/* XXX: Don't all MS-Windows compilers (except Cygwin) have _WIN32 defined?
 * XXX: If so, remove the first set of defined()'s below.
 */
#if !defined(TARGET_HOST_POSIX_X11) && !defined(TARGET_HOST_MS_WINDOWS) && !defined(TARGET_HOST_MAC_OSX) && !defined(TARGET_HOST_SOLARIS)
#if defined(_MSC_VER) || defined(__WATCOMC__) || defined(__MINGW32__) \
    || defined(_WIN32) || defined(_WIN32_WCE) \
    || ( defined(__CYGWIN__) && defined(X_DISPLAY_MISSING) )
#   define  TARGET_HOST_MS_WINDOWS 1

#elif defined (__ANDROID__)
#   define  TARGET_HOST_ANDROID  1

#elif defined(__posix__) || defined(__unix__) || defined(__linux__) || defined(__sun)
#   define  TARGET_HOST_POSIX_X11  1

#elif defined(__APPLE__)
#   include <TargetConditionals.h>
#   if TARGET_OS_IPHONE
#      define TARGET_HOST_IOS  1
#   else
/* This is a placeholder until we get native OSX support ironed out -- JFF 11/18/09 */
#      define  TARGET_HOST_POSIX_X11  1
/* #      define  TARGET_HOST_MAC_OSX    1 */
#   endif

#else
#   error "Unrecognized target host!"

#endif
#endif

/* Detect both SunPro and gcc compilers on Sun Solaris */
#if defined (__SVR4) && defined (__sun)
#   define TARGET_HOST_SOLARIS 1
#endif

#ifndef TARGET_HOST_MS_WINDOWS
#   define  TARGET_HOST_MS_WINDOWS 0
#endif

#ifndef  TARGET_HOST_POSIX_X11
#   define  TARGET_HOST_POSIX_X11  0
#endif

#ifndef  TARGET_HOST_MAC_OSX
#   define  TARGET_HOST_MAC_OSX    0
#endif

#ifndef  TARGET_HOST_IOS
#   define  TARGET_HOST_IOS    0
#endif

#ifndef  TARGET_HOST_SOLARIS
#   define  TARGET_HOST_SOLARIS    0
#endif

/* Compile time assert macro for compilers that don't support static_assert().
 * e.g COMPILE_TIME_ASSERT(sizeof(long) == 4, long_is_4_bytes); would verify
 * that the size of the long type is 4 bytes at compile time. */
#define COMPILE_TIME_ASSERT_EXPAND_IDENTIFIER(a, b, c)  a ## b ##c
#define COMPILE_TIME_ASSERT_IDENTIFIER(a, b, c) \
  COMPILE_TIME_ASSERT_EXPAND_IDENTIFIER(a, b, c)
#define COMPILE_TIME_ASSERT_MESSAGE(msg) \
  COMPILE_TIME_ASSERT_IDENTIFIER(compile_time_assert_failed_at_line_, \
                                 __LINE__, _ ## msg)
#define COMPILE_TIME_ASSERT(expr, msg) \
  typedef struct { int COMPILE_TIME_ASSERT_MESSAGE(msg) : !!(expr); } \
    COMPILE_TIME_ASSERT_MESSAGE(msg)


/* -- FIXED CONFIGURATION LIMITS ------------------------------------------- */

#define  FREEGLUT_MAX_MENUS         3

/* These files should be available on every platform. */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>

/* These are included based on autoconf directives. */
#ifdef HAVE_SYS_TYPES_H
#    include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif
#ifdef TIME_WITH_SYS_TIME
#    include <sys/time.h>
#    include <time.h>
#elif defined(HAVE_SYS_TIME_H)
#    include <sys/time.h>
#else
#    include <time.h>
#endif

/* -- AUTOCONF HACKS --------------------------------------------------------*/

/* XXX: Update autoconf to avoid these.
 * XXX: Are non-POSIX platforms intended not to use autoconf?
 * If so, perhaps there should be a config_guess.h for them. Alternatively,
 * config guesses could be placed above, just after the config.h exclusion.
 */
#if defined(__FreeBSD__) || defined(__NetBSD__)
#    define HAVE_USB_JS 1
#    if defined(__NetBSD__) || ( defined(__FreeBSD__) && __FreeBSD_version >= 500000)
#        define HAVE_USBHID_H 1
#    endif
#endif

#if defined(_MSC_VER) || defined(__WATCOMC__)
/* strdup() is non-standard, for all but POSIX-2001 */
#define strdup   _strdup
#endif

/* M_PI is non-standard (defined by BSD, not ISO-C) */
#ifndef M_PI
#    define  M_PI  3.14159265358979323846
#endif

#ifdef HAVE_STDBOOL_H
#    include <stdbool.h>
#    ifndef TRUE
#        define TRUE true
#    endif
#    ifndef FALSE
#        define FALSE false
#    endif
#else
#    ifndef TRUE
#        define  TRUE  1
#    endif
#    ifndef FALSE
#        define  FALSE  0
#    endif
#endif

/* General defines */
#define INVALID_MODIFIERS 0xffffffff

/* FreeGLUT internal time type */
#if defined(HAVE_STDINT_H)
#   include <stdint.h>
    typedef uint64_t fg_time_t;
#elif defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
    typedef uint64_t fg_time_t;
#elif defined(HAVE_U__INT64)
    typedef unsigned __int64 fg_time_t;
#elif defined(HAVE_ULONG_LONG)
    typedef unsigned long long fg_time_t;
#else
    typedef unsigned long fg_time_t;
#endif

#ifndef __fg_unused
# ifdef __GNUC__
#  define __fg_unused __attribute__((unused))
# else
#  define __fg_unused
# endif
#endif

/* Platform-specific includes */
#if TARGET_HOST_POSIX_X11
#include "x11/fg_internal_x11.h"
#endif
#if TARGET_HOST_MS_WINDOWS
#include "mswin/fg_internal_mswin.h"
#endif
#if TARGET_HOST_ANDROID
#include "android/fg_internal_android.h"
#endif
#if TARGET_HOST_IOS
#include "ios/fg_internal_ios.h"
#endif


/* -- GLOBAL TYPE DEFINITIONS ---------------------------------------------- */

/* Freeglut callbacks type definitions */
typedef void (* FGCBDisplay       )( void );
typedef void (* FGCBReshape       )( int, int );
typedef void (* FGCBPosition      )( int, int );
typedef void (* FGCBVisibility    )( int );
typedef void (* FGCBKeyboard      )( unsigned char, int, int );
typedef void (* FGCBKeyboardUp    )( unsigned char, int, int );
typedef void (* FGCBSpecial       )( int, int, int );
typedef void (* FGCBSpecialUp     )( int, int, int );
typedef void (* FGCBMouse         )( int, int, int, int );
typedef void (* FGCBMouseWheel    )( int, int, int, int );
typedef void (* FGCBMotion        )( int, int );
typedef void (* FGCBPassive       )( int, int );
typedef void (* FGCBEntry         )( int );
typedef void (* FGCBWindowStatus  )( int );
typedef void (* FGCBJoystick      )( unsigned int, int, int, int );
typedef void (* FGCBOverlayDisplay)( void );
typedef void (* FGCBSpaceMotion   )( int, int, int );
typedef void (* FGCBSpaceRotation )( int, int, int );
typedef void (* FGCBSpaceButton   )( int, int );
typedef void (* FGCBDials         )( int, int );
typedef void (* FGCBButtonBox     )( int, int );
typedef void (* FGCBTabletMotion  )( int, int );
typedef void (* FGCBTabletButton  )( int, int, int, int );
typedef void (* FGCBDestroy       )( void );    /* Used for both window and menu destroy callbacks */

typedef void (* FGCBMultiEntry   )( int, int );
typedef void (* FGCBMultiButton  )( int, int, int, int, int );
typedef void (* FGCBMultiMotion  )( int, int, int );
typedef void (* FGCBMultiPassive )( int, int, int );

typedef void (* FGCBInitContext)();
typedef void (* FGCBAppStatus)(int);

/* The global callbacks type definitions */
typedef void (* FGCBIdle          )( void );
typedef void (* FGCBTimer         )( int );
typedef void (* FGCBMenuState     )( int );
typedef void (* FGCBMenuStatus    )( int, int, int );

/* The callback used when creating/using menus */
typedef void (* FGCBMenu          )( int );

/* The FreeGLUT error/warning handler type definition */
typedef void (* FGError           ) ( const char *fmt, va_list ap);
typedef void (* FGWarning         ) ( const char *fmt, va_list ap);


/* A list structure */
typedef struct tagSFG_List SFG_List;
struct tagSFG_List
{
    void *First;
    void *Last;
};

/* A list node structure */
typedef struct tagSFG_Node SFG_Node;
struct tagSFG_Node
{
    void *Next;
    void *Prev;
};

/* A helper structure holding two ints and a boolean */
typedef struct tagSFG_XYUse SFG_XYUse;
struct tagSFG_XYUse
{
    GLint           X, Y;               /* The two integers...               */
    GLboolean       Use;                /* ...and a single boolean.          */
};

/*
 * An enumeration containing the state of the GLUT execution:
 * initializing, running, or stopping
 */
typedef enum
{
  GLUT_EXEC_STATE_INIT,
  GLUT_EXEC_STATE_RUNNING,
  GLUT_EXEC_STATE_STOP
} fgExecutionState ;

/* This structure holds different freeglut settings */
typedef struct tagSFG_State SFG_State;
struct tagSFG_State
{
    SFG_XYUse        Position;             /* The default windows' position  */
    SFG_XYUse        Size;                 /* The default windows' size      */
    unsigned int     DisplayMode;          /* Display mode for new windows   */

    GLboolean        Initialised;          /* freeglut has been initialised  */

    int              DirectContext;        /* Direct rendering state         */

    GLboolean        ForceIconic;          /* New top windows are iconified  */
    GLboolean        UseCurrentContext;    /* New windows share with current */

    GLboolean        GLDebugSwitch;        /* OpenGL state debugging switch  */
    GLboolean        XSyncSwitch;          /* X11 sync protocol switch       */

    int              KeyRepeat;            /* Global key repeat mode.        */
    int              Modifiers;            /* Current ALT/SHIFT/CTRL state   */

    GLuint           FPSInterval;          /* Interval between FPS printfs   */
    GLuint           SwapCount;            /* Count of glutSwapBuffer calls  */
    GLuint           SwapTime;             /* Time of last SwapBuffers       */

    fg_time_t        Time;                 /* Time that glutInit was called  */
    SFG_List         Timers;               /* The freeglut timer hooks       */
    SFG_List         FreeTimers;           /* The unused timer hooks         */

    FGCBIdle         IdleCallback;         /* The global idle callback       */

    int              ActiveMenus;          /* Num. of currently active menus */
    FGCBMenuState    MenuStateCallback;    /* Menu callbacks are global      */
    FGCBMenuStatus   MenuStatusCallback;
    void*            MenuFont;             /* Font to be used for newly created menus */

    SFG_XYUse        GameModeSize;         /* Game mode screen's dimensions  */
    int              GameModeDepth;        /* The pixel depth for game mode  */
    int              GameModeRefresh;      /* The refresh rate for game mode */

    int              ActionOnWindowClose; /* Action when user closes window  */

    fgExecutionState ExecState;           /* Used for GLUT termination       */
    char            *ProgramName;         /* Name of the invoking program    */
    GLboolean        JoysticksInitialised;  /* Only initialize if application calls for them */
    int              NumActiveJoysticks;    /* Number of active joysticks (callback defined and positive pollrate) -- if zero, don't poll joysticks */
    GLboolean        InputDevsInitialised;  /* Only initialize if application calls for them */

	int              MouseWheelTicks;      /* Number of ticks the mouse wheel has turned */

    int              AuxiliaryBufferNumber;  /* Number of auxiliary buffers */
    int              SampleNumber;         /*  Number of samples per pixel  */

    GLboolean        SkipStaleMotion;      /* skip stale motion events */

    int              MajorVersion;         /* Major OpenGL context version  */
    int              MinorVersion;         /* Minor OpenGL context version  */
    int              ContextFlags;         /* OpenGL context flags          */
    int              ContextProfile;       /* OpenGL context profile        */
    int              HasOpenGL20;          /* fgInitGL2 could find all OpenGL 2.0 functions */
    FGError          ErrorFunc;            /* User defined error handler    */
    FGWarning        WarningFunc;          /* User defined warning handler  */
};

/* The structure used by display initialization in freeglut_init.c */
typedef struct tagSFG_Display SFG_Display;
struct tagSFG_Display
{
	SFG_PlatformDisplay pDisplay;

    int             ScreenWidth;        /* The screen's width in pixels      */
    int             ScreenHeight;       /* The screen's height in pixels     */
    int             ScreenWidthMM;      /* The screen's width in milimeters  */
    int             ScreenHeightMM;     /* The screen's height in milimeters */
};


/* The user can create any number of timer hooks */
typedef struct tagSFG_Timer SFG_Timer;
struct tagSFG_Timer
{
    SFG_Node        Node;
    int             ID;                 /* The timer ID integer              */
    FGCBTimer       Callback;           /* The timer callback                */
    fg_time_t       TriggerTime;        /* The timer trigger time            */
};

/*
 * A window and its OpenGL context. The contents of this structure
 * are highly dependant on the target operating system we aim at...
 */
typedef struct tagSFG_Context SFG_Context;
struct tagSFG_Context
{
    SFG_WindowHandleType  Handle;    /* The window's handle                 */
    SFG_WindowContextType Context;   /* The window's OpenGL/WGL context     */

	SFG_PlatformContext pContext;    /* The window's FBConfig (X11) or device context (Windows) */

    int             DoubleBuffered;  /* Treat the window as double-buffered */

    /* When drawing geometry to vertex attribute buffers, user specifies 
     * the attribute indices for vertices, normals and/or texture coords
     * to freeglut. Those are stored here
     */
    GLint           attribute_v_coord;
    GLint           attribute_v_normal;
    GLint           attribute_v_texture;
};


/*
 * Bitmasks indicating the different kinds of
 * actions that can be scheduled for a window.
 */
#define GLUT_INIT_WORK        (1<<0)
#define GLUT_VISIBILITY_WORK  (1<<1)
#define GLUT_POSITION_WORK    (1<<2)
#define GLUT_SIZE_WORK        (1<<3)
#define GLUT_ZORDER_WORK      (1<<4)
#define GLUT_FULL_SCREEN_WORK (1<<5)
#define GLUT_DISPLAY_WORK     (1<<6)

/*
 * An enumeration containing the state of the GLUT execution:
 * initializing, running, or stopping
 */
typedef enum
{
  DesireHiddenState,
  DesireIconicState,
  DesireNormalState
} fgDesiredVisibility ;

/*
 *  There is considerable confusion about the "right thing to
 *  do" concerning window  size and position.  GLUT itself is
 *  not consistent between Windows and UNIX/X11; since
 *  platform independence is a virtue for "freeglut", we
 *  decided to break with GLUT's behaviour.
 *
 *  Under UNIX/X11, it is apparently not possible to get the
 *  window border sizes in order to subtract them off the
 *  window's initial position until some time after the window
 *  has been created.  Therefore we decided on the following
 *  behaviour, both under Windows and under UNIX/X11:
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
typedef struct tagSFG_WindowState SFG_WindowState;
struct tagSFG_WindowState   /* as per notes above, sizes always refer to the client area (thus without the window decorations) */
{
    /* window state - size, position, look */
    int             Xpos;               /* Window's top-left of client area, X-coordinate */
    int             Ypos;               /* Window's top-left of client area, Y-coordinate */
    int             Width;              /* Window's width in pixels          */
    int             Height;             /* The same about the height         */
    GLboolean       Visible;            /* Is the window visible now? Not using fgVisibilityState as we only care if visible or not */
    int             Cursor;             /* The currently selected cursor style */
    GLboolean       IsFullscreen;       /* is the window fullscreen?         */

    /* FreeGLUT operations are deferred, that is, window moving, resizing,
     * Z-order changing, making full screen or not do not happen immediately
     * upon the user's request, but only in the next iteration of the main
     * loop, before the display callback is called. This allows multiple
     * reshape, position, etc requests to be combined into one and is
     * compatible with the way GLUT does things. Callbacks get triggered
     * based on the feedback/messages/notifications from the window manager.
     * Below here we define what work should be done, as well as the relevant
     * parameters for this work.
     */
    unsigned int    WorkMask;           /* work (resize, etc) to be done on the window */
    int             DesiredXpos;        /* desired X location */
    int             DesiredYpos;        /* desired Y location */
    int             DesiredWidth;       /* desired window width */
    int             DesiredHeight;      /* desired window height */
    int             DesiredZOrder;      /* desired window Z Order position */
    fgDesiredVisibility DesiredVisibility;/* desired visibility (hidden, iconic, shown/normal) */

	SFG_PlatformWindowState pWState;    /* Window width/height (X11) or rectangle/style (Windows) from before a resize, and other stuff only needed on specific platforms */

    long            JoystickPollRate;   /* The joystick polling rate         */
    fg_time_t       JoystickLastPoll;   /* When the last poll happened       */

    int             MouseX, MouseY;     /* The most recent mouse position    */

    GLboolean       IgnoreKeyRepeat;    /* Whether to ignore key repeat.     */

    GLboolean       VisualizeNormals;   /* When drawing objects, draw vectors representing the normals as well? */
};


/*
 * A generic function pointer.  We should really use the GLUTproc type
 * defined in freeglut_ext.h, but if we include that header in this file
 * a bunch of other stuff (font-related) blows up!
 */
typedef void (*SFG_Proc)();

#if TARGET_HOST_MS_WINDOWS
#define FG_MACRO_START {
#define FG_MACRO_END }
#else
#define FG_MACRO_START do {
#define FG_MACRO_END } while(0)
#endif /* TARGET_HOST_MS_WINDOWS */

/*
 * SET_WCB() is used as:
 *
 *     SET_WCB( window, cbname, func );
 *
 * ...where {window} is the freeglut window to set the callback,
 *          {cbname} is the window-specific callback to set,
 *          {func} is a function-pointer.
 *
 * Originally, {FETCH_WCB( ... ) = func} was rather sloppily used,
 * but this can cause warnings because the FETCH_WCB() macro type-
 * casts its result, and a type-cast value shouldn't be an lvalue.
 *
 * The {if( FETCH_WCB( ... ) != func )} test is to do type-checking
 * and for no other reason.  Since it's hidden in the macro, the
 * ugliness is felt to be rather benign.
 */
#define SET_WCB(window,cbname,func)                            \
FG_MACRO_START                                                 \
    if( FETCH_WCB( window, cbname ) != (SFG_Proc)(func) )      \
        (((window).CallBacks[WCB_ ## cbname]) = (SFG_Proc)(func)); \
FG_MACRO_END

/*
 * FETCH_WCB() is used as:
 *
 *     FETCH_WCB( window, cbname );
 *
 * ...where {window} is the freeglut window to fetch the callback from,
 *          {cbname} is the window-specific callback to fetch.
 *
 * The result is correctly type-cast to the callback function pointer
 * type.
 */
#define FETCH_WCB(window,cbname) \
    ((window).CallBacks[WCB_ ## cbname])

/*
 * INVOKE_WCB() is used as:
 *
 *     INVOKE_WCB( window, cbname, ( arg_list ) );
 *
 * ...where {window} is the freeglut window,
 *          {cbname} is the window-specific callback to be invoked,
 *          {(arg_list)} is the parameter list.
 *
 * The callback is invoked as:
 *
 *    callback( arg_list );
 *
 * ...so the parentheses are REQUIRED in the {arg_list}.
 *
 * NOTE that it does a sanity-check and also sets the
 * current window.
 *
 */
#if TARGET_HOST_MS_WINDOWS && !defined(_WIN32_WCE) /* FIXME: also WinCE? */
#define INVOKE_WCB(window,cbname,arg_list)    \
FG_MACRO_START                                \
    if( FETCH_WCB( window, cbname ) )         \
    {                                         \
        FGCB ## cbname func = (FGCB ## cbname)(FETCH_WCB( window, cbname )); \
        fgSetWindow( &window );               \
        func arg_list;                        \
    }                                         \
FG_MACRO_END
#else
#define INVOKE_WCB(window,cbname,arg_list)    \
FG_MACRO_START                                \
    if( FETCH_WCB( window, cbname ) )         \
    {                                         \
        fgSetWindow( &window );               \
        ((FGCB ## cbname)FETCH_WCB( window, cbname )) arg_list; \
    }                                         \
FG_MACRO_END
#endif

/*
 * The window callbacks the user can supply us with. Should be kept portable.
 *
 * This enumeration provides the freeglut CallBack numbers.
 * The symbolic constants are indices into a window's array of
 * function callbacks.  The names are formed by splicing a common
 * prefix onto the callback's base name.  (This was originally
 * done so that an early stage of development could live side-by-
 * side with the old callback code.  The old callback code used
 * the bare callback's name as a structure member, so I used a
 * prefix for the array index name.)
 */
enum
{
    WCB_Display,
    WCB_Reshape,
    WCB_Position,
    WCB_Keyboard,
    WCB_KeyboardUp,
    WCB_Special,
    WCB_SpecialUp,
    WCB_Mouse,
    WCB_MouseWheel,
    WCB_Motion,
    WCB_Passive,
    WCB_Entry,
    WCB_Visibility,
    WCB_WindowStatus,
    WCB_Joystick,
    WCB_Destroy,

    /* Multi-Pointer X and touch related */
    WCB_MultiEntry,
    WCB_MultiButton,
    WCB_MultiMotion,
    WCB_MultiPassive,

    /* Mobile platforms LifeCycle */
    WCB_InitContext,
    WCB_AppStatus,

    /* Presently ignored */
    WCB_Select,
    WCB_OverlayDisplay,
    WCB_SpaceMotion,     /* presently implemented only on UNIX/X11 */
    WCB_SpaceRotation,   /* presently implemented only on UNIX/X11 */
    WCB_SpaceButton,     /* presently implemented only on UNIX/X11 */
    WCB_Dials,
    WCB_ButtonBox,
    WCB_TabletMotion,
    WCB_TabletButton,

    /* Always make this the LAST one */
    TOTAL_CALLBACKS
};


/* This structure holds the OpenGL rendering context for all the menu windows */
typedef struct tagSFG_MenuContext SFG_MenuContext;
struct tagSFG_MenuContext
{
    SFG_WindowContextType MContext;       /* The menu window's WGL context   */
};

/* This structure describes a menu */
typedef struct tagSFG_Window SFG_Window;
typedef struct tagSFG_MenuEntry SFG_MenuEntry;
typedef struct tagSFG_Menu SFG_Menu;
struct tagSFG_Menu
{
    SFG_Node            Node;
    void               *UserData;     /* User data passed back at callback   */
    int                 ID;           /* The global menu ID                  */
    SFG_List            Entries;      /* The menu entries list               */
    FGCBMenu            Callback;     /* The menu callback                   */
    FGCBDestroy         Destroy;      /* Destruction callback                */
    GLboolean           IsActive;     /* Is the menu selected?               */
    void*               Font;         /* Font to be used for displaying this menu */
    int                 Width;        /* Menu box width in pixels            */
    int                 Height;       /* Menu box height in pixels           */
    int                 X, Y;         /* Menu box raster position            */

    SFG_MenuEntry      *ActiveEntry;  /* Currently active entry in the menu  */
    SFG_Window         *Window;       /* Window for menu                     */
    SFG_Window         *ParentWindow; /* Window in which the menu is invoked */
};

/* This is a menu entry */
struct tagSFG_MenuEntry
{
    SFG_Node            Node;
    int                 ID;                     /* The menu entry ID (local) */
    int                 Ordinal;                /* The menu's ordinal number */
    char*               Text;                   /* The text to be displayed  */
    SFG_Menu*           SubMenu;                /* Optional sub-menu tree    */
    GLboolean           IsActive;               /* Is the entry highlighted? */
    int                 Width;                  /* Label's width in pixels   */
};

/*
 * A window, making part of freeglut windows hierarchy.
 * Should be kept portable.
 *
 * NOTE that ActiveMenu is set to menu itself if the window is a menu.
 */
struct tagSFG_Window
{
    SFG_Node            Node;
    int                 ID;                     /* Window's ID number        */

    SFG_Context         Window;                 /* Window and OpenGL context */
    SFG_WindowState     State;                  /* The window state          */
    SFG_Proc            CallBacks[ TOTAL_CALLBACKS ]; /* Array of window callbacks */
    void               *UserData ;              /* For use by user           */

    SFG_Menu*       Menu[ FREEGLUT_MAX_MENUS ]; /* Menus appended to window  */
    SFG_Menu*       ActiveMenu;                 /* The window's active menu  */

    SFG_Window*         Parent;                 /* The parent to this window */
    SFG_List            Children;               /* The subwindows d.l. list  */

    GLboolean           IsMenu;                 /* Set to 1 if we are a menu */
};


/* A linked list structure of windows */
typedef struct tagSFG_WindowList SFG_WindowList ;
struct tagSFG_WindowList
{
    SFG_Node node;
    SFG_Window *window ;
};

/* This holds information about all the windows, menus etc. */
typedef struct tagSFG_Structure SFG_Structure;
struct tagSFG_Structure
{
    SFG_List        Windows;         /* The global windows list            */
    SFG_List        Menus;           /* The global menus list              */
    SFG_List        WindowsToDestroy;

    SFG_Window*     CurrentWindow;   /* The currently set window          */
    SFG_Menu*       CurrentMenu;     /* Same, but menu...                 */

    SFG_MenuContext* MenuContext;    /* OpenGL rendering context for menus */

    SFG_Window*      GameModeWindow; /* The game mode window               */

    int              WindowID;       /* The window ID for the next window to be created */
    int              MenuID;         /* The menu ID for the next menu to be created */
};

/*
 * This structure is used for the enumeration purposes.
 * You can easily extend its functionalities by declaring
 * a structure containing enumerator's contents and custom
 * data, then casting its pointer to (SFG_Enumerator *).
 */
typedef struct tagSFG_Enumerator SFG_Enumerator;
struct tagSFG_Enumerator
{
    GLboolean   found;                          /* Used to terminate search  */
    void*       data;                           /* Custom data pointer       */
};
typedef void (* FGCBWindowEnumerator  )( SFG_Window *, SFG_Enumerator * );
typedef void (* FGCBMenuEnumerator  )( SFG_Menu *, SFG_Enumerator * );

/* The bitmap font structure */
typedef struct tagSFG_Font SFG_Font;
struct tagSFG_Font
{
    char*           Name;         /* The source font name             */
    int             Quantity;     /* Number of chars in font          */
    int             Height;       /* Height of the characters         */
    const GLubyte** Characters;   /* The characters mapping           */

    float           xorig, yorig; /* Relative origin of the character */
};

/* The stroke font structures */

typedef struct tagSFG_StrokeVertex SFG_StrokeVertex;
struct tagSFG_StrokeVertex
{
    GLfloat         X, Y;
};

/* Verify this structure can safely be used cast to an array of floats for use
   with glVertexPointer(). */
COMPILE_TIME_ASSERT(sizeof(SFG_StrokeVertex) == sizeof(GLfloat) * 2,
                    SFG_StrokeVertex_can_be_used_with_glVertexPointer);

typedef struct tagSFG_StrokeStrip SFG_StrokeStrip;
struct tagSFG_StrokeStrip
{
    int             Number;
    const SFG_StrokeVertex* Vertices;
};

typedef struct tagSFG_StrokeChar SFG_StrokeChar;
struct tagSFG_StrokeChar
{
    GLfloat         Right;
    int             Number;
    const SFG_StrokeStrip* Strips;
};

typedef struct tagSFG_StrokeFont SFG_StrokeFont;
struct tagSFG_StrokeFont
{
    char*           Name;                       /* The source font name      */
    int             Quantity;                   /* Number of chars in font   */
    GLfloat         Height;                     /* Height of the characters  */
    const SFG_StrokeChar** Characters;          /* The characters mapping    */
};


/* -- JOYSTICK-SPECIFIC STRUCTURES AND TYPES ------------------------------- */
/*
 * Initial defines from "js.h" starting around line 33 with the existing "freeglut_joystick.c"
 * interspersed
 */

#if TARGET_HOST_MACINTOSH
#    include <InputSprocket.h>
#endif

#if TARGET_HOST_MAC_OSX
#    include <mach/mach.h>
#    include <IOKit/IOkitLib.h>
#    include <IOKit/hid/IOHIDLib.h>
#endif

/* XXX It might be better to poll the operating system for the numbers of buttons and
 * XXX axes and then dynamically allocate the arrays.
 */
#define _JS_MAX_BUTTONS 32

#if TARGET_HOST_MACINTOSH
#    define _JS_MAX_AXES  9
typedef struct tagSFG_PlatformJoystick SFG_PlatformJoystick;
struct tagSFG_PlatformJoystick
{
#define  ISP_NUM_AXIS    9
#define  ISP_NUM_NEEDS  41
    ISpElementReference isp_elem  [ ISP_NUM_NEEDS ];
    ISpNeed             isp_needs [ ISP_NUM_NEEDS ];
};
#endif

#if TARGET_HOST_MAC_OSX
#    define _JS_MAX_AXES 16
typedef struct tagSFG_PlatformJoystick SFG_PlatformJoystick;
struct tagSFG_PlatformJoystick
{
    IOHIDDeviceInterface ** hidDev;
    IOHIDElementCookie buttonCookies[41];
    IOHIDElementCookie axisCookies[_JS_MAX_AXES];
/* The next two variables are not used anywhere */
/*    long minReport[_JS_MAX_AXES],
 *         maxReport[_JS_MAX_AXES];
 */
};
#endif


/*
 * Definition of "SFG_Joystick" structure -- based on JS's "jsJoystick" object class.
 * See "js.h" lines 80-178.
 */
typedef struct tagSFG_Joystick SFG_Joystick;
struct tagSFG_Joystick
{
	SFG_PlatformJoystick pJoystick;

    int          id;
    GLboolean    error;
    char         name [ 128 ];
    int          num_axes;
    int          num_buttons;

    float dead_band[ _JS_MAX_AXES ];
    float saturate [ _JS_MAX_AXES ];
    float center   [ _JS_MAX_AXES ];
    float max      [ _JS_MAX_AXES ];
    float min      [ _JS_MAX_AXES ];
};



/* -- GLOBAL VARIABLES EXPORTS --------------------------------------------- */

/* Freeglut display related stuff (initialized once per session) */
extern SFG_Display fgDisplay;

/* Freeglut internal structure */
extern SFG_Structure fgStructure;

/* The current freeglut settings */
extern SFG_State fgState;


/* -- PRIVATE FUNCTION DECLARATIONS ---------------------------------------- */

/*
 * A call to this function makes us sure that the Display and Structure
 * subsystems have been properly initialized and are ready to be used
 */
#define  FREEGLUT_EXIT_IF_NOT_INITIALISED( string )               \
  if ( ! fgState.Initialised )                                    \
  {                                                               \
    fgError ( " ERROR:  Function <%s> called"                     \
              " without first calling 'glutInit'.", (string) ) ;  \
  }

#define  FREEGLUT_INTERNAL_ERROR_EXIT_IF_NOT_INITIALISED( string )  \
  if ( ! fgState.Initialised )                                      \
  {                                                                 \
    fgError ( " ERROR:  Internal <%s> function called"              \
              " without first calling 'glutInit'.", (string) ) ;    \
  }

#define  FREEGLUT_INTERNAL_ERROR_EXIT( cond, string, function )  \
  if ( ! ( cond ) )                                              \
  {                                                              \
    fgError ( " ERROR:  Internal error <%s> in function %s",     \
              (string), (function) ) ;                           \
  }

/*
 * Following definitions are somewhat similiar to GLib's,
 * but do not generate any log messages:
 */
#define  freeglut_return_if_fail( expr ) \
    if( !(expr) )                        \
        return;
#define  freeglut_return_val_if_fail( expr, val ) \
    if( !(expr) )                                 \
        return val ;

#if defined(__APPLE__)
#define FREEGLUT_DO_NOT_CONTINUE_EXECUTION 1
#else
#define FREEGLUT_DO_NOT_CONTINUE_EXECUTION \
    ( fgState.ActionOnWindowClose != GLUT_ACTION_CONTINUE_EXECUTION )
#endif // defined(__APPLE__)

/*
 * A call to those macros assures us that there is a current
 * window set, respectively:
 */
#define  FREEGLUT_EXIT_IF_NO_WINDOW( string )                               \
  if ( ! fgStructure.CurrentWindow && FREEGLUT_DO_NOT_CONTINUE_EXECUTION )  \
  {                                                                         \
    fgError ( " ERROR:  Function <%s> called"                               \
              " with no current window defined.", (string) ) ;              \
  }

/*
 * The deinitialize function gets called on glutMainLoop() end. It should clean up
 * everything inside of the freeglut
 */
void fgDeinitialize( void );

/*
 * Those two functions are used to create/destroy the freeglut internal
 * structures. This actually happens when calling glutInit() and when
 * quitting the glutMainLoop() (which actually happens, when all windows
 * have been closed).
 */
void fgCreateStructure( void );
void fgDestroyStructure( void );

/*
 * Window creation, opening, closing and destruction.
 * Also CallBack clearing/initialization.
 * Defined in freeglut_structure.c, freeglut_window.c.
 */
SFG_Window* fgCreateWindow( SFG_Window* parent, const char* title,
                            GLboolean positionUse, int x, int y,
                            GLboolean sizeUse, int w, int h,
                            GLboolean gameMode, GLboolean isMenu );
void        fgSetWindow ( SFG_Window *window );
void        fgOpenWindow( SFG_Window* window, const char* title,
                          GLboolean positionUse, int x, int y,
                          GLboolean sizeUse, int w, int h,
                          GLboolean gameMode, GLboolean isSubWindow );
void        fgCloseWindow( SFG_Window* window );
void        fgAddToWindowDestroyList ( SFG_Window* window );
void        fgCloseWindows ();
void        fgDestroyWindow( SFG_Window* window );

/* Menu creation and destruction. Defined in freeglut_structure.c */
SFG_Menu*   fgCreateMenu( FGCBMenu menuCallback );
void        fgDestroyMenu( SFG_Menu* menu );

/* Joystick device management functions, defined in freeglut_joystick.c */
int         fgJoystickDetect( void );
void        fgInitialiseJoysticks( void );
void        fgJoystickClose( void );
void        fgJoystickPollWindow( SFG_Window* window );

/* InputDevice Initialisation and Closure */
int         fgInputDeviceDetect( void );
void        fgInitialiseInputDevices( void );
void        fgInputDeviceClose( void );

/* spaceball device functions, defined in freeglut_spaceball.c */
void        fgInitialiseSpaceball( void );
void        fgSpaceballClose( void );
void        fgSpaceballSetWindow( SFG_Window *window );

int         fgHasSpaceball( void );
int         fgSpaceballNumButtons( void );

/* Setting the cursor for a given window */
void fgSetCursor ( SFG_Window *window, int cursorID );

/*
 * Helper function to enumerate through all registered windows
 * and one to enumerate all of a window's subwindows...
 *
 * The GFunc callback for those functions will be defined as:
 *
 *      void enumCallback( gpointer window, gpointer enumerator );
 *
 * where window is the enumerated (sub)window pointer (SFG_Window *),
 * and userData is the a custom user-supplied pointer. Functions
 * are defined and exported from freeglut_structure.c file.
 */
void fgEnumWindows( FGCBWindowEnumerator enumCallback, SFG_Enumerator* enumerator );
void fgEnumSubWindows( SFG_Window* window, FGCBWindowEnumerator enumCallback,
                       SFG_Enumerator* enumerator );

/*
 * fgWindowByHandle returns a (SFG_Window *) value pointing to the
 * first window in the queue matching the specified window handle.
 * The function is defined in freeglut_structure.c file.
 */
SFG_Window* fgWindowByHandle( SFG_WindowHandleType hWindow );

/*
 * This function is similiar to the previous one, except it is
 * looking for a specified (sub)window identifier. The function
 * is defined in freeglut_structure.c file.
 */
SFG_Window* fgWindowByID( int windowID );

/*
 * Looks up a menu given its ID. This is easier than fgWindowByXXX
 * as all menus are placed in a single doubly linked list...
 */
SFG_Menu* fgMenuByID( int menuID );

/*
 * Returns active menu, if any. Assumption: only one menu active throughout application at any one time.
 * This is easier than fgWindowByXXX as all menus are placed in one doubly linked list...
 */
SFG_Menu* fgGetActiveMenu( );

/*
 * The menu activation and deactivation the code. This is the meat
 * of the menu user interface handling code...
 */
void fgUpdateMenuHighlight ( SFG_Menu *menu );
GLboolean fgCheckActiveMenu ( SFG_Window *window, int button, GLboolean pressed,
                              int mouse_x, int mouse_y );
void fgDeactivateMenu( SFG_Window *window );

/*
 * This function gets called just before the buffers swap, so that
 * freeglut can display the pull-down menus via OpenGL. The function
 * is defined in freeglut_menu.c file.
 */
void fgDisplayMenu( void );

/* Elapsed time as per glutGet(GLUT_ELAPSED_TIME). */
fg_time_t fgElapsedTime( void );

/* System time in milliseconds */
fg_time_t fgSystemTime(void);

/* List functions */
void fgListInit(SFG_List *list);
void fgListAppend(SFG_List *list, SFG_Node *node);
void fgListRemove(SFG_List *list, SFG_Node *node);
int fgListLength(SFG_List *list);
void fgListInsert(SFG_List *list, SFG_Node *next, SFG_Node *node);

/* Error Message functions */
void fgError( const char *fmt, ... );
void fgWarning( const char *fmt, ... );

SFG_Proc fgPlatformGetProcAddress( const char *procName );

/* pushing attribute/value pairs into an array */
#define ATTRIB(a) attributes[where++]=(a)
#define ATTRIB_VAL(a,v) {ATTRIB(a); ATTRIB(v);}

int fghMapBit( int mask, int from, int to );
int fghIsLegacyContextRequested( void );
void fghContextCreationError( void );
int fghNumberOfAuxBuffersRequested( void );

#endif /* FREEGLUT_INTERNAL_H */

/*** END OF FILE ***/
