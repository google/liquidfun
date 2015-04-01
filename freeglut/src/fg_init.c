/*
 * freeglut_init.c
 *
 * Various freeglut initialization functions.
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

#define FREEGLUT_BUILDING_LIB
#include <GL/freeglut.h>
#include "fg_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  fgDeinitialize()        -- Win32's OK, X11 needs the OS-specific
 *                             deinitialization done
 *  glutInitDisplayString() -- display mode string parsing
 *
 * Wouldn't it be cool to use gettext() for error messages? I just love
 * bash saying  "nie znaleziono pliku" instead of "file not found" :)
 * Is gettext easily portable?
 */

/* -- GLOBAL VARIABLES ----------------------------------------------------- */

/*
 * A structure pointed by fgDisplay holds all information
 * regarding the display, screen, root window etc.
 */
SFG_Display fgDisplay;

/*
 * The settings for the current freeglut session
 */
SFG_State fgState = { { -1, -1, GL_FALSE },  /* Position */
                      { 300, 300, GL_TRUE }, /* Size */
                      GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH,  /* DisplayMode */
                      GL_FALSE,              /* Initialised */
                      GLUT_TRY_DIRECT_CONTEXT,  /* DirectContext */
                      GL_FALSE,              /* ForceIconic */
                      GL_FALSE,              /* UseCurrentContext */
                      GL_FALSE,              /* GLDebugSwitch */
                      GL_FALSE,              /* XSyncSwitch */
                      GLUT_KEY_REPEAT_ON,    /* KeyRepeat */
                      INVALID_MODIFIERS,     /* Modifiers */
                      0,                     /* FPSInterval */
                      0,                     /* SwapCount */
                      0,                     /* SwapTime */
                      0,                     /* Time */
                      { NULL, NULL },         /* Timers */
                      { NULL, NULL },         /* FreeTimers */
                      NULL,                   /* IdleCallback */
                      0,                      /* ActiveMenus */
                      NULL,                   /* MenuStateCallback */
                      NULL,                   /* MenuStatusCallback */
                      FREEGLUT_MENU_FONT,
                      { -1, -1, GL_TRUE },    /* GameModeSize */
                      -1,                     /* GameModeDepth */
                      -1,                     /* GameModeRefresh */
                      GLUT_ACTION_EXIT,       /* ActionOnWindowClose */
                      GLUT_EXEC_STATE_INIT,   /* ExecState */
                      NULL,                   /* ProgramName */
                      GL_FALSE,               /* JoysticksInitialised */
                      0,                      /* NumActiveJoysticks */
                      GL_FALSE,               /* InputDevsInitialised */
                      0,                      /* MouseWheelTicks */
                      1,                      /* AuxiliaryBufferNumber */
                      4,                      /* SampleNumber */
                      GL_FALSE,               /* SkipStaleMotion */
                      1,                      /* OpenGL context MajorVersion */
                      0,                      /* OpenGL context MinorVersion */
                      0,                      /* OpenGL ContextFlags */
                      0,                      /* OpenGL ContextProfile */
                      0,                      /* HasOpenGL20 */
                      NULL,                   /* ErrorFunc */
                      NULL                    /* WarningFunc */
};


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

extern void fgPlatformInitialize( const char* displayName );
extern void fgPlatformDeinitialiseInputDevices ( void );
extern void fgPlatformCloseDisplay ( void );
extern void fgPlatformDestroyContext ( SFG_PlatformDisplay pDisplay, SFG_WindowContextType MContext );

void fghParseCommandLineArguments ( int* pargc, char** argv, char **pDisplayName, char **pGeometry )
{
#ifndef _WIN32_WCE
    int i, j, argc = *pargc;

    {
	    /* check if GLUT_FPS env var is set */
        const char *fps = getenv( "GLUT_FPS" );

        if( fps )
        {
            int interval;
            sscanf( fps, "%d", &interval );

            if( interval <= 0 )
                fgState.FPSInterval = 5000;  /* 5000 millisecond default */
            else
                fgState.FPSInterval = interval;
        }
    }

    *pDisplayName = getenv( "DISPLAY" );

    for( i = 1; i < argc; i++ )
    {
        if( strcmp( argv[ i ], "-display" ) == 0 )
        {
            if( ++i >= argc )
                fgError( "-display parameter must be followed by display name" );

            *pDisplayName = argv[ i ];

            argv[ i - 1 ] = NULL;
            argv[ i     ] = NULL;
            ( *pargc ) -= 2;
        }
        else if( strcmp( argv[ i ], "-geometry" ) == 0 )
        {
            if( ++i >= argc )
                fgError( "-geometry parameter must be followed by window "
                         "geometry settings" );

            *pGeometry = argv[ i ];

            argv[ i - 1 ] = NULL;
            argv[ i     ] = NULL;
            ( *pargc ) -= 2;
        }
        else if( strcmp( argv[ i ], "-direct" ) == 0)
        {
            if( fgState.DirectContext == GLUT_FORCE_INDIRECT_CONTEXT )
                fgError( "parameters ambiguity, -direct and -indirect "
                    "cannot be both specified" );

            fgState.DirectContext = GLUT_FORCE_DIRECT_CONTEXT;
            argv[ i ] = NULL;
            ( *pargc )--;
        }
        else if( strcmp( argv[ i ], "-indirect" ) == 0 )
        {
            if( fgState.DirectContext == GLUT_FORCE_DIRECT_CONTEXT )
                fgError( "parameters ambiguity, -direct and -indirect "
                    "cannot be both specified" );

            fgState.DirectContext = GLUT_FORCE_INDIRECT_CONTEXT;
            argv[ i ] = NULL;
            (*pargc)--;
        }
        else if( strcmp( argv[ i ], "-iconic" ) == 0 )
        {
            fgState.ForceIconic = GL_TRUE;
            argv[ i ] = NULL;
            ( *pargc )--;
        }
        else if( strcmp( argv[ i ], "-gldebug" ) == 0 )
        {
            fgState.GLDebugSwitch = GL_TRUE;
            argv[ i ] = NULL;
            ( *pargc )--;
        }
        else if( strcmp( argv[ i ], "-sync" ) == 0 )
        {
            fgState.XSyncSwitch = GL_TRUE;
            argv[ i ] = NULL;
            ( *pargc )--;
        }
    }

    /* Compact {argv}. */
    for( i = j = 1; i < *pargc; i++, j++ )
    {
        /* Guaranteed to end because there are "*pargc" arguments left */
        while ( argv[ j ] == NULL )
            j++;
        if ( i != j )
            argv[ i ] = argv[ j ];
    }

#endif /* _WIN32_WCE */

}


void fghCloseInputDevices ( void )
{
    if ( fgState.JoysticksInitialised )
        fgJoystickClose( );

    if ( fgState.InputDevsInitialised )
        fgInputDeviceClose( );
}


/*
 * Perform the freeglut deinitialization...
 */
void fgDeinitialize( void )
{
    SFG_Timer *timer;

    if( !fgState.Initialised )
    {
        return;
    }

    fgState.Initialised = GL_FALSE;  // fgState.Initialised needs to be set here to prevent fgDeinitialize from entering an infinite loop in its error handling.

	/* If we're in game mode, we want to leave game mode */
    if( fgStructure.GameModeWindow ) {
        glutLeaveGameMode();
    }

    /* If there was a menu created, destroy the rendering context */
    if( fgStructure.MenuContext )
    {
		fgPlatformDestroyContext (fgDisplay.pDisplay, fgStructure.MenuContext->MContext );
        free( fgStructure.MenuContext );
        fgStructure.MenuContext = NULL;
    }

    fgDestroyStructure( );

    for (timer = fgState.Timers.First; timer; timer = fgState.Timers.First)
    {
        fgListRemove( &fgState.Timers, &timer->Node );
        free( timer );
    }

    for (timer = fgState.FreeTimers.First; timer; timer = fgState.FreeTimers.First)
    {
        fgListRemove( &fgState.FreeTimers, &timer->Node );
        free( timer );
    }

	fgPlatformDeinitialiseInputDevices ();

	fgState.MouseWheelTicks = 0;

    fgState.MajorVersion = 1;
    fgState.MinorVersion = 0;
    fgState.ContextFlags = 0;
    fgState.ContextProfile = 0;

    fgState.Position.X = -1;
    fgState.Position.Y = -1;
    fgState.Position.Use = GL_FALSE;

    fgState.Size.X = 300;
    fgState.Size.Y = 300;
    fgState.Size.Use = GL_TRUE;

    fgState.DisplayMode = GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH;

    fgState.DirectContext  = GLUT_TRY_DIRECT_CONTEXT;
    fgState.ForceIconic         = GL_FALSE;
    fgState.UseCurrentContext   = GL_FALSE;
    fgState.GLDebugSwitch       = GL_FALSE;
    fgState.XSyncSwitch         = GL_FALSE;
    fgState.ActionOnWindowClose = GLUT_ACTION_EXIT;
    fgState.ExecState           = GLUT_EXEC_STATE_INIT;

    fgState.KeyRepeat       = GLUT_KEY_REPEAT_ON;
    fgState.Modifiers       = INVALID_MODIFIERS;

    fgState.GameModeSize.X  = -1;
    fgState.GameModeSize.Y  = -1;
    fgState.GameModeDepth   = -1;
    fgState.GameModeRefresh = -1;

    fgListInit( &fgState.Timers );
    fgListInit( &fgState.FreeTimers );

    fgState.IdleCallback = NULL;
    fgState.MenuStateCallback = ( FGCBMenuState )NULL;
    fgState.MenuStatusCallback = ( FGCBMenuStatus )NULL;

    fgState.SwapCount   = 0;
    fgState.SwapTime    = 0;
    fgState.FPSInterval = 0;

    if( fgState.ProgramName )
    {
        free( fgState.ProgramName );
        fgState.ProgramName = NULL;
    }

	fgPlatformCloseDisplay ();

    fgState.Initialised = GL_FALSE;
}


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */
#if defined(NEED_XPARSEGEOMETRY_IMPL)
#   include "util/xparsegeometry_repl.h"
#endif

/*
 * Perform initialization. This usually happens on the program startup
 * and restarting after glutMainLoop termination...
 */
void FGAPIENTRY glutInit( int* pargc, char** argv )
{
    char* displayName = NULL;
    char* geometry = NULL;
    if( fgState.Initialised )
        fgError( "illegal glutInit() reinitialization attempt" );

    if (pargc && *pargc && argv && *argv && **argv)
    {
        fgState.ProgramName = strdup (*argv);

        if( !fgState.ProgramName )
            fgError ("Could not allocate space for the program's name.");
    }

    if (!pargc)
    {
        static int empty_argc = 0;
        pargc = &empty_argc;
    }
    if (!argv)
    {
        static char empty_string = '\0';
        static char *empty_argv[] = { &empty_string };
        argv = (char**)&empty_argv;
    }

    fgCreateStructure( );

	fghParseCommandLineArguments ( pargc, argv, &displayName, &geometry );

    /*
     * Have the display created now. If there wasn't a "-display"
     * in the program arguments, we will use the DISPLAY environment
     * variable for opening the X display (see code above):
     */
    fgPlatformInitialize( displayName );

    /*
     * Geometry parsing deferred until here because we may need the screen
     * size.
     */

    if (geometry )
    {
        unsigned int parsedWidth, parsedHeight;
        int mask = XParseGeometry( geometry,
                                   &fgState.Position.X, &fgState.Position.Y,
                                   &parsedWidth, &parsedHeight );
        /* TODO: Check for overflow? */
        fgState.Size.X = parsedWidth;
        fgState.Size.Y = parsedHeight;

        if( (mask & (WidthValue|HeightValue)) == (WidthValue|HeightValue) )
            fgState.Size.Use = GL_TRUE;

        if( mask & XNegative )
            fgState.Position.X += fgDisplay.ScreenWidth - fgState.Size.X;

        if( mask & YNegative )
            fgState.Position.Y += fgDisplay.ScreenHeight - fgState.Size.Y;

        if( (mask & (XValue|YValue)) == (XValue|YValue) )
            fgState.Position.Use = GL_TRUE;
    }
}

/*
 * Undoes all the "glutInit" stuff
 */
void FGAPIENTRY glutExit ( void )
{
  fgDeinitialize ();
}

/*
 * Sets the default initial window position for new windows
 */
void FGAPIENTRY glutInitWindowPosition( int x, int y )
{
    fgState.Position.X = x;
    fgState.Position.Y = y;

    if( ( x >= 0 ) && ( y >= 0 ) )
        fgState.Position.Use = GL_TRUE;
    else
        fgState.Position.Use = GL_FALSE;
}

/*
 * Sets the default initial window size for new windows
 */
void FGAPIENTRY glutInitWindowSize( int width, int height )
{
    fgState.Size.X = width;
    fgState.Size.Y = height;

    if( ( width > 0 ) && ( height > 0 ) )
        fgState.Size.Use = GL_TRUE;
    else
        fgState.Size.Use = GL_FALSE;
}

/*
 * Sets the default display mode for all new windows
 */
void FGAPIENTRY glutInitDisplayMode( unsigned int displayMode )
{
    /* We will make use of this value when creating a new OpenGL context... */
    fgState.DisplayMode = displayMode;
}


/* -- INIT DISPLAY STRING PARSING ------------------------------------------ */

static char* Tokens[] =
{
    "alpha", "acca", "acc", "blue", "buffer", "conformant", "depth", "double",
    "green", "index", "num", "red", "rgba", "rgb", "luminance", "stencil",
    "single", "stereo", "samples", "slow", "win32pdf", "win32pfd", "xvisual",
    "xstaticgray", "xgrayscale", "xstaticcolor", "xpseudocolor",
    "xtruecolor", "xdirectcolor",
    "xstaticgrey", "xgreyscale", "xstaticcolour", "xpseudocolour",
    "xtruecolour", "xdirectcolour", "borderless", "aux"
};
#define NUM_TOKENS             (sizeof(Tokens) / sizeof(*Tokens))

void FGAPIENTRY glutInitDisplayString( const char* displayMode )
{
    int glut_state_flag = 0 ;
    /*
     * Unpack a lot of options from a character string.  The options are
     * delimited by blanks or tabs.
     */
    char *token ;
    size_t len = strlen ( displayMode );
    char *buffer = (char *)malloc ( (len+1) * sizeof(char) );
    memcpy ( buffer, displayMode, len );
    buffer[len] = '\0';

    token = strtok ( buffer, " \t" );

    while ( token )
    {
        /* Process this token */
        int i ;

        /* Temporary fix:  Ignore any length specifications and at least
         * process the basic token
         * TODO:  Fix this permanently
         */
        size_t cleanlength = strcspn ( token, "=<>~!" );

        for ( i = 0; i < NUM_TOKENS; i++ )
        {
            if ( strncmp ( token, Tokens[i], cleanlength ) == 0 ) break ;
        }

        switch ( i )
        {
        case 0 :  /* "alpha":  Alpha color buffer precision in bits */
            glut_state_flag |= GLUT_ALPHA ;  /* Somebody fix this for me! */
            break ;

        case 1 :  /* "acca":  Red, green, blue, and alpha accumulation buffer
                     precision in bits */
            break ;

        case 2 :  /* "acc":  Red, green, and blue accumulation buffer precision
                     in bits with zero bits alpha */
            glut_state_flag |= GLUT_ACCUM ;  /* Somebody fix this for me! */
            break ;

        case 3 :  /* "blue":  Blue color buffer precision in bits */
            break ;

        case 4 :  /* "buffer":  Number of bits in the color index color buffer
                   */
            break ;

        case 5 :  /* "conformant":  Boolean indicating if the frame buffer
                     configuration is conformant or not */
            break ;

        case 6 : /* "depth":  Number of bits of precision in the depth buffer */
            glut_state_flag |= GLUT_DEPTH ;  /* Somebody fix this for me! */
            break ;

        case 7 :  /* "double":  Boolean indicating if the color buffer is
                     double buffered */
            glut_state_flag |= GLUT_DOUBLE ;
            break ;

        case 8 :  /* "green":  Green color buffer precision in bits */
            break ;

        case 9 :  /* "index":  Boolean if the color model is color index or not
                   */
            glut_state_flag |= GLUT_INDEX ;
            break ;

        case 10 :  /* "num":  A special capability  name indicating where the
                      value represents the Nth frame buffer configuration
                      matching the description string */
            break ;

        case 11 :  /* "red":  Red color buffer precision in bits */
            break ;

        case 12 :  /* "rgba":  Number of bits of red, green, blue, and alpha in
                      the RGBA color buffer */
            glut_state_flag |= GLUT_RGBA ;  /* Somebody fix this for me! */
            break ;

        case 13 :  /* "rgb":  Number of bits of red, green, and blue in the
                      RGBA color buffer with zero bits alpha */
            glut_state_flag |= GLUT_RGB ;  /* Somebody fix this for me! */
            break ;

        case 14 :  /* "luminance":  Number of bits of red in the RGBA and zero
                      bits of green, blue (alpha not specified) of color buffer
                      precision */
            glut_state_flag |= GLUT_LUMINANCE ; /* Somebody fix this for me! */
            break ;

        case 15 :  /* "stencil":  Number of bits in the stencil buffer */
            glut_state_flag |= GLUT_STENCIL;  /* Somebody fix this for me! */
            break ;

        case 16 :  /* "single":  Boolean indicate the color buffer is single
                      buffered */
            glut_state_flag |= GLUT_SINGLE ;
            break ;

        case 17 :  /* "stereo":  Boolean indicating the color buffer supports
                      OpenGL-style stereo */
            glut_state_flag |= GLUT_STEREO ;
            break ;

        case 18 :  /* "samples":  Indicates the number of multisamples to use
                      based on GLX's SGIS_multisample extension (for
                      antialiasing) */
            glut_state_flag |= GLUT_MULTISAMPLE ; /*Somebody fix this for me!*/
            break ;

        case 19 :  /* "slow":  Boolean indicating if the frame buffer
                      configuration is slow or not */
            break ;

        case 20 :  /* "win32pdf": (incorrect spelling but was there before */
        case 21 :  /* "win32pfd":  matches the Win32 Pixel Format Descriptor by
                      number */
#if TARGET_HOST_MS_WINDOWS
#endif
            break ;

        case 22 :  /* "xvisual":  matches the X visual ID by number */
#if TARGET_HOST_POSIX_X11
#endif
            break ;

        case 23 :  /* "xstaticgray": */
        case 29 :  /* "xstaticgrey":  boolean indicating if the frame buffer
                      configuration's X visual is of type StaticGray */
#if TARGET_HOST_POSIX_X11
#endif
            break ;

        case 24 :  /* "xgrayscale": */
        case 30 :  /* "xgreyscale":  boolean indicating if the frame buffer
                      configuration's X visual is of type GrayScale */
#if TARGET_HOST_POSIX_X11
#endif
            break ;

        case 25 :  /* "xstaticcolor": */
        case 31 :  /* "xstaticcolour":  boolean indicating if the frame buffer
                      configuration's X visual is of type StaticColor */
#if TARGET_HOST_POSIX_X11
#endif
            break ;

        case 26 :  /* "xpseudocolor": */
        case 32 :  /* "xpseudocolour":  boolean indicating if the frame buffer
                      configuration's X visual is of type PseudoColor */
#if TARGET_HOST_POSIX_X11
#endif
            break ;

        case 27 :  /* "xtruecolor": */
        case 33 :  /* "xtruecolour":  boolean indicating if the frame buffer
                      configuration's X visual is of type TrueColor */
#if TARGET_HOST_POSIX_X11
#endif
            break ;

        case 28 :  /* "xdirectcolor": */
        case 34 :  /* "xdirectcolour":  boolean indicating if the frame buffer
                      configuration's X visual is of type DirectColor */
#if TARGET_HOST_POSIX_X11
#endif
            break ;

        case 35 :  /* "borderless":  windows should not have borders */
            glut_state_flag |= GLUT_BORDERLESS;
            break ;

        case 36 :  /* "aux":  some number of aux buffers */
            glut_state_flag |= GLUT_AUX;
            break ;

        case 37 :  /* Unrecognized */
            fgWarning ( "WARNING - Display string token not recognized:  %s",
                        token );
            break ;
        }

        token = strtok ( NULL, " \t" );
    }

    free ( buffer );

    /* We will make use of this value when creating a new OpenGL context... */
    fgState.DisplayMode = glut_state_flag;
}

/* -- SETTING OPENGL 3.0 CONTEXT CREATION PARAMETERS ---------------------- */

void FGAPIENTRY glutInitContextVersion( int majorVersion, int minorVersion )
{
    /* We will make use of these value when creating a new OpenGL context... */
    fgState.MajorVersion = majorVersion;
    fgState.MinorVersion = minorVersion;
}


void FGAPIENTRY glutInitContextFlags( int flags )
{
    /* We will make use of this value when creating a new OpenGL context... */
    fgState.ContextFlags = flags;
}

void FGAPIENTRY glutInitContextProfile( int profile )
{
    /* We will make use of this value when creating a new OpenGL context... */
    fgState.ContextProfile = profile;
}

/* -------------- User Defined Error/Warning Handler Support -------------- */

/*
 * Sets the user error handler (note the use of va_list for the args to the fmt)
 */
void FGAPIENTRY glutInitErrorFunc( FGError callback )
{
    /* This allows user programs to handle freeglut errors */
    fgState.ErrorFunc = callback;
}

/*
 * Sets the user warning handler (note the use of va_list for the args to the fmt)
 */
void FGAPIENTRY glutInitWarningFunc( FGWarning callback )
{
    /* This allows user programs to handle freeglut warnings */
    fgState.WarningFunc = callback;
}

/*** END OF FILE ***/
