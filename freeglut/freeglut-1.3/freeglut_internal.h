/*
 * freeglut_internal.h
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

/*
 * Freeglut is meant to be available under all Unix/X11 and Win32 platforms.
 */
#if !defined(_WIN32)
#   define  TARGET_HOST_UNIX_X11    1
#   define  TARGET_HOST_WIN32       0
#else
#   define  TARGET_HOST_UNIX_X11    0
#   define  TARGET_HOST_WIN32       1
#endif

#define  FREEGLUT_MAX_MENUS         3
#define  FREEGLUT_DEBUG             1

#if FREEGLUT_DEBUG
    #undef   G_DISABLE_ASSERT
    #undef   G_DISABLE_CHECKS
#else
    #define  G_DISABLE_ASSERT
    #define  G_DISABLE_CHECKS
#endif

/*
 * Somehow all Win32 include headers depend on this one:
 */
#if TARGET_HOST_WIN32
#include <windows.h>
#include <windowsx.h>

#define strdup   _strdup
#endif

/*
 * Those files should be available on every platform.
 */
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#if TARGET_HOST_UNIX_X11
#include <sys/time.h>
#endif

/*
 * The system-dependant include files should go here:
 */
#if TARGET_HOST_UNIX_X11
    #include <GL/glx.h>
    #include <X11/Xlib.h>
    #include <X11/Xatom.h>
    #include <X11/keysym.h>

    /*
     * This will generate errors, but I don't have any idea how to fix it (will autoconf help?)
     */
    #include <X11/extensions/xf86vmode.h>
#endif

/*
 * Microsoft VisualC++ 5.0's <math.h> does not define the PI
 */
#ifndef M_PI
#    define  M_PI  3.14159265358979323846
#endif

#ifndef TRUE
#    define  TRUE  1
#endif

#ifndef FALSE
#    define  FALSE  0
#endif

/* -- GLOBAL TYPE DEFINITIONS ---------------------------------------------- */

/*
 * Freeglut callbacks type definitions
 */
typedef void (* FGCBdisplay       )( void );
typedef void (* FGCBreshape       )( int, int );
typedef void (* FGCBvisibility    )( int );
typedef void (* FGCBkeyboard      )( unsigned char, int, int );
typedef void (* FGCBspecial       )( int, int, int );
typedef void (* FGCBmouse         )( int, int, int, int );
typedef void (* FGCBmotion        )( int, int );
typedef void (* FGCBpassive       )( int, int );
typedef void (* FGCBentry         )( int );
typedef void (* FGCBwindowStatus  )( int );
typedef void (* FGCBmenuState     )( int );
typedef void (* FGCBmenuStatus    )( int, int, int );
typedef void (* FGCBselect        )( int, int, int );
typedef void (* FGCBjoystick      )( unsigned int, int, int, int );
typedef void (* FGCBkeyboardUp    )( unsigned char, int, int );
typedef void (* FGCBspecialUp     )( int, int, int );
typedef void (* FGCBoverlayDisplay)( void );
typedef void (* FGCBspaceMotion   )( int, int, int );
typedef void (* FGCBspaceRotate   )( int, int, int );
typedef void (* FGCBspaceButton   )( int, int );
typedef void (* FGCBdials         )( int, int );
typedef void (* FGCBbuttonBox     )( int, int );
typedef void (* FGCBtabletMotion  )( int, int );
typedef void (* FGCBtabletButton  )( int, int, int, int );
typedef void (* FGCBdestroy       )( void );

/*
 * The global callbacks type definitions
 */
typedef void (* FGCBidle          )( void );
typedef void (* FGCBtimer         )( int );

/*
 * The callback used when creating/using menus
 */
typedef void (* FGCBmenu          )( int );


/*
 * A list structure
 */
typedef struct tagSFG_List SFG_List;
struct tagSFG_List
{
    void *First;
    void *Last;
};

/*
 * A list node structure
 */
typedef struct tagSFG_Node SFG_Node;
struct tagSFG_Node
{
    void *Next;
    void *Prev;
};

/*
 * A helper structure holding two ints and a boolean
 */
typedef struct tagSFG_XYUse SFG_XYUse;
struct tagSFG_XYUse
{
    GLint           X, Y;               /* The two integers...               */
    GLboolean       Use;                /* ...and a single boolean.          */
};

/*
 * A helper structure holding a timeval and a boolean
 */
typedef struct tagSFG_Time SFG_Time;
struct tagSFG_Time
{
#ifdef WIN32
    DWORD Value;
#else
    struct timeval  Value;
#endif
    GLboolean       Set;
};

/*
 * An enumeration containing the state of the GLUT execution:  initializing, running, or stopping
 */
typedef enum {
  GLUT_EXEC_STATE_INIT,
  GLUT_EXEC_STATE_RUNNING,
  GLUT_EXEC_STATE_STOP
} fgExecutionState ;

/*
 * This structure holds different freeglut settings
 */
typedef struct tagSFG_State SFG_State;
struct tagSFG_State
{
    SFG_XYUse        Position;             /* The default windows' position     */
    SFG_XYUse        Size;                 /* The default windows' size         */
    unsigned int     DisplayMode;          /* The display mode for new windows  */

    GLboolean        ForceDirectContext;   /* Should we force direct contexts?  */
    GLboolean        TryDirectContext;     /* What about giving a try to?       */

    GLboolean        ForceIconic;          /* All new top windows are iconified */

    GLboolean        GLDebugSwitch;        /* OpenGL state debugging switch     */
    GLboolean        XSyncSwitch;          /* X11 sync protocol switch          */

    GLboolean        IgnoreKeyRepeat;      /* Whether to ignore key repeat...   */

    SFG_Time         Time;                 /* The time that glutInit was called */
    SFG_List         Timers;               /* The freeglut timer hooks          */

    FGCBidle         IdleCallback;         /* The global idle callback          */

    FGCBmenuState    MenuStateCallback;    /* Menu callbacks are global         */
    FGCBmenuStatus   MenuStatusCallback;

    SFG_XYUse        GameModeSize;         /* The game mode screen's dimensions */
    int              GameModeDepth;        /* The pixel depth for game mode     */
    int              GameModeRefresh;      /* The refresh rate for game mode    */

    int              ActionOnWindowClose ; /* Action when user clicks "x" on window header bar */

    fgExecutionState ExecState ;           /* Current state of the GLUT execution */
};

/*
 * The structure used by display initialization in freeglut_init.c
 */
typedef struct tagSFG_Display SFG_Display;
struct tagSFG_Display
{
#if TARGET_HOST_UNIX_X11
    Display*        Display;            /* The display we are being run in.  */
    int             Screen;             /* The screen we are about to use.   */
    Window          RootWindow;         /* The screen's root window.         */
    int             Connection;         /* The display's connection number   */
    Atom            DeleteWindow;       /* The window deletion atom          */

#ifdef X_XF86VidModeGetModeLine
    XF86VidModeModeLine DisplayMode;    /* Current screen's display settings */
    int             DisplayModeClock;   /* The display mode's refresh rate   */
#endif

#elif TARGET_HOST_WIN32
    HINSTANCE        Instance;          /* The application's instance        */
    DEVMODE         DisplayMode;        /* Desktop's display settings        */

#endif

    int             ScreenWidth;        /* The screen's width in pixels      */
    int             ScreenHeight;       /* The screen's height in pixels     */
    int             ScreenWidthMM;      /* The screen's width in milimeters  */
    int             ScreenHeightMM;     /* The screen's height in milimeters */
};


/*
 * The user can create any number of timer hooks
 */
typedef struct tagSFG_Timer SFG_Timer;
struct tagSFG_Timer
{
    SFG_Node        Node;
    int             ID;                 /* The timer ID integer              */
    FGCBtimer       Callback;           /* The timer callback                */
    long            TriggerTime;        /* The timer trigger time            */
};

/*
 * A window and its OpenGL context. The contents of this structure
 * are highly dependant on the target operating system we aim at...
 */
typedef struct tagSFG_Context SFG_Context;
struct tagSFG_Context
{
#if TARGET_HOST_UNIX_X11
    Window          Handle;             /* The window's handle               */
    GLXContext      Context;            /* The OpenGL context                */
    XVisualInfo*    VisualInfo;         /* The window's visual information   */

#elif TARGET_HOST_WIN32
    HWND            Handle;             /* The window's handle               */
    HDC             Device;             /* The window's device context       */
    HGLRC           Context;            /* The window's WGL context          */

#endif
};

/*
 * Window's state description. This structure should be kept portable.
 */
typedef struct tagSFG_WindowState SFG_WindowState;
struct tagSFG_WindowState
{
    int             Width;              /* Window's width in pixels          */
    int             Height;             /* The same about the height         */

    GLboolean       Redisplay;          /* Do we have to redisplay?          */
    GLboolean       Visible;            /* Is the window visible now         */

    int             Cursor;             /* The currently selected cursor     */
    int             Modifiers;          /* The current ALT/SHIFT/CTRL state  */

    long            JoystickPollRate;   /* The joystick polling rate         */
    long            JoystickLastPoll;   /* When the last poll has happened   */

    int             MouseX, MouseY;     /* The most recent mouse position    */

    GLboolean       IsGameMode;         /* Is this the game mode window?     */

#if TARGET_HOST_WIN32
    GLboolean       NeedToResize;       /* Do we need to explicitly resize?  */
#endif
};

/*
 * The window callbacks the user can supply us with. Should be kept portable.
 */
typedef struct tagSFG_WindowCallbacks SFG_WindowCallbacks;
struct tagSFG_WindowCallbacks
{
    /*
     * Following callbacks are fully supported right now
     * and are ready to be tested for GLUT conformance:
     */
    FGCBdisplay         Display;
    FGCBreshape         Reshape;
    FGCBkeyboard        Keyboard;
    FGCBkeyboardUp      KeyboardUp;
    FGCBspecial         Special;
    FGCBspecialUp       SpecialUp;
    FGCBmouse           Mouse;
    FGCBmotion          Motion;
    FGCBpassive         Passive;
    FGCBentry           Entry;
    FGCBvisibility      Visibility;
    FGCBwindowStatus    WindowStatus;
    FGCBjoystick        Joystick;
    FGCBdestroy         Destroy;

    /*
     * Those callbacks are being ignored for the moment
     */
    FGCBselect          Select;
    FGCBoverlayDisplay  OverlayDisplay;
    FGCBspaceMotion     SpaceMotion;
    FGCBspaceRotate     SpaceRotation;
    FGCBspaceButton     SpaceButton;
    FGCBdials           Dials;
    FGCBbuttonBox       ButtonBox;
    FGCBtabletMotion    TabletMotion;
    FGCBtabletButton    TabletButton;
};

/*
 * This structure describes a menu
 */
typedef struct tagSFG_Menu SFG_Menu;
struct tagSFG_Menu
{
    SFG_Node            Node;
    int                 ID;                     /* The global menu ID        */
    SFG_List            Entries;                /* The menu entries list     */
    FGCBmenu            Callback;               /* The menu callback         */
    GLboolean           IsActive;               /* Is the menu selected?     */
    int                 Width;                  /* Menu box width in pixels  */
    int                 Height;                 /* Menu box height in pixels */
    int                 X, Y;                   /* Menu box raster position  */
};

/*
 * This is a menu entry
 */
typedef struct tagSFG_MenuEntry SFG_MenuEntry;
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
 * A window, making part of FreeGLUT windows hierarchy. Should be kept portable.
 */
typedef struct tagSFG_Window SFG_Window;
struct tagSFG_Window
{
    SFG_Node            Node;
    int                 ID;                     /* Window's ID number        */

    SFG_Context         Window;                 /* Window and OpenGL context */
    SFG_WindowState     State;                  /* The window state          */
    SFG_WindowCallbacks Callbacks;              /* The window callbacks      */

    SFG_Menu*       Menu[ FREEGLUT_MAX_MENUS ]; /* Menus appended to window  */
    SFG_Menu*       ActiveMenu;                 /* The window's active menu  */

    SFG_Window*         Parent;                 /* The parent to this window */
    SFG_List            Children;               /* The subwindows d.l. list  */
};

/*
 * A linked list structure of windows
 */
typedef struct tagSFG_WindowList SFG_WindowList ;
struct tagSFG_WindowList
{
  SFG_Window *window ;
  GLboolean needToClose ;
  SFG_WindowList *next ;
} ;

/*
 * This holds information about all the windows, menus etc.
 */
typedef struct tagSFG_Structure SFG_Structure;
struct tagSFG_Structure
{
    SFG_List            Windows;                /* The global windows list   */
    SFG_List            Menus;                  /* The global menus list     */

    SFG_Window*         Window;                 /* The currently active win. */
    SFG_Menu*           Menu;                   /* Same, but menu...         */

    SFG_Window*         GameMode;               /* The game mode window      */

    int                 WindowID;               /* The new current window ID */
    int                 MenuID;                 /* The new current menu ID   */
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
typedef void (* FGCBenumerator  )( SFG_Window *, SFG_Enumerator * );

/*
 * The bitmap font structure
 */
typedef struct tagSFG_Font SFG_Font;
struct tagSFG_Font
{
    char*           Name;                       /* The source font name      */
    int             Quantity;                   /* Number of chars in font   */
    int             Height;                     /* Height of the characters  */
    const GLubyte** Characters;                 /* The characters mapping    */

    float           xorig, yorig ;              /* The origin of the character relative to the draw location */
};

/*
 * The stroke font structures
 */

typedef struct tagSFG_StrokeVertex SFG_StrokeVertex;
struct tagSFG_StrokeVertex
{
    GLfloat         X, Y;
};

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

/* -- GLOBAL VARIABLES EXPORTS --------------------------------------------- */

/*
 * Freeglut display related stuff (initialized once per session)
 */
extern SFG_Display fgDisplay;

/*
 * Freeglut internal structure
 */
extern SFG_Structure fgStructure;

/*
 * The current freeglut settings
 */
extern SFG_State fgState;


/* -- PRIVATE FUNCTION DECLARATIONS ---------------------------------------- */

/*
 * A call to this function makes us sure that the Display and Structure
 * subsystems have been properly initialized and are ready to be used
 */
#define  freeglut_assert_ready                      assert( fgState.Time.Set );

/*
 * Following definitions are somewhat similiar to GLib's,
 * but do not generate any log messages:
 */
#define  freeglut_return_if_fail( expr )            if( !(expr) ) return;
#define  freeglut_return_val_if_fail( expr, val )   if( !(expr) ) return( val );

/*
 * A call to those macros assures us that there is a current
 * window and menu set, respectively:
 */
#define  freeglut_assert_window                     assert( fgStructure.Window != NULL );
#define  freeglut_assert_menu                       assert( fgStructure.Menu != NULL );

/*
 * The initialize and deinitialize functions get called on glutInit()
 * and glutMainLoop() end respectively. They should create/clean up
 * everything inside of the freeglut
 */
void fgInitialize( const char* displayName );
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
 * A helper function to check if a display mode is possible to use
 */
#if TARGET_HOST_UNIX_X11
XVisualInfo* fgChooseVisual( void );
#endif

/*
 * The window procedure for Win32 events handling
 */
#if TARGET_HOST_WIN32
LRESULT CALLBACK fgWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
GLboolean fgSetupPixelFormat( SFG_Window* window, GLboolean checkOnly, unsigned char layer_type );
#endif

/*
 * Window creation, opening, closing and destruction.
 * Defined in freeglut_structure.c, freeglut_window.c.
 */
SFG_Window* fgCreateWindow( SFG_Window* parent, const char* title, int x, int y, int w, int h, GLboolean gameMode );
void        fgSetWindow ( SFG_Window *window ) ;
void        fgOpenWindow( SFG_Window* window, const char* title, int x, int y, int w, int h, GLboolean gameMode, int isSubWindow );
void        fgCloseWindow( SFG_Window* window );
void        fgAddToWindowDestroyList ( SFG_Window* window, GLboolean needToClose ) ;
void        fgCloseWindows () ;
void        fgDestroyWindow( SFG_Window* window, GLboolean needToClose );

/*
 * Menu creation and destruction. Defined in freeglut_structure.c
 */
SFG_Menu*   fgCreateMenu( FGCBmenu menuCallback );
void        fgDestroyMenu( SFG_Menu* menu );

/*
 * Joystick device management functions, defined in freeglut_joystick.c
 */
void        fgJoystickInit( int ident );
void        fgJoystickClose( void );
void        fgJoystickPollWindow( SFG_Window* window );

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
void fgEnumWindows( FGCBenumerator enumCallback, SFG_Enumerator* enumerator );
void fgEnumSubWindows( SFG_Window* window, FGCBenumerator enumCallback, SFG_Enumerator* enumerator );

/*
 * fgWindowByHandle returns a (SFG_Window *) value pointing to the
 * first window in the queue matching the specified window handle.
 * The function is defined in freeglut_structure.c file.
 */
#if TARGET_HOST_UNIX_X11
    SFG_Window* fgWindowByHandle( Window hWindow );
#elif TARGET_HOST_WIN32

    SFG_Window* fgWindowByHandle( HWND hWindow );
#endif

/*
 * This function is similiar to the previous one, except it is
 * looking for a specified (sub)window identifier. The function
 * is defined in freeglut_structure.c file.
 */
SFG_Window* fgWindowByID( int windowID );

/*
 * Looks up a menu given its ID. This is easier that fgWindowByXXX
 * as all menus are placed in a single doubly linked list...
 */
SFG_Menu* fgMenuByID( int menuID );

/*
 * The menu activation and deactivation the code. This is the meat
 * of the menu user interface handling code...
 */
void fgActivateMenu( SFG_Window* window, int button );
void fgExecuteMenuCallback( SFG_Menu* menu ) ;
GLboolean fgCheckActiveMenu ( SFG_Window *window, SFG_Menu *menu ) ;
void fgDeactivateMenu( SFG_Window *window );

/*
 * This function gets called just before the buffers swap, so that
 * freeglut can display the pull-down menus via OpenGL. The function
 * is defined in freeglut_menu.c file.
 */
void fgDisplayMenu( void );

/*
 * Display the mouse cursor using OpenGL calls. The function
 * is defined in freeglut_cursor.c file.
 */
void fgDisplayCursor( void );

/*
 * Elapsed time as per glutGet(GLUT_ELAPSED_TIME).
 */
long fgElapsedTime( void );

/*
 * List functions
 */
void fgListInit(SFG_List *list);
void fgListAppend(SFG_List *list, SFG_Node *node);
void fgListRemove(SFG_List *list, SFG_Node *node);
int fgListLength(SFG_List *list);

/*
 * Error Messages functions
 */
void fgError( const char *fmt, ... );
void fgWarning( const char *fmt, ... );

#endif /* FREEGLUT_INTERNAL_H */

/*** END OF FILE ***/
