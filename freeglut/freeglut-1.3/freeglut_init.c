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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define G_LOG_DOMAIN "freeglut-init"

#include "../include/GL/freeglut.h"
#include "../include/GL/freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  fgDeinitialize()        -- Win32's OK, X11 needs the OS-specific deinitialization done
 *  glutInitDisplayString() -- display mode string parsing
 *
 * Wouldn't it be cool to use gettext() for error messages? I just love bash saying 
 * "nie znaleziono pliku" instead of "file not found" :) Is gettext easily portable?
 */

/* -- GLOBAL VARIABLES ----------------------------------------------------- */

/*
 * A structure pointed by g_pDisplay holds all information
 * regarding the display, screen, root window etc.
 */
SFG_Display fgDisplay;

/*
 * The settings for the current freeglut session
 */
SFG_State fgState;


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

/*
 * A call to this function should initialize all the display stuff...
 */
void fgInitialize( const char* displayName )
{
#if TARGET_HOST_UNIX_X11
    /*
     * Have the display created
     */
    fgDisplay.Display = XOpenDisplay( displayName );

    if( fgDisplay.Display == NULL )
    {
        /*
         * Failed to open a display. That's no good.
         */
        fgError( "failed to open display '%s'", XDisplayName( displayName ) );
    }

    /*
     * Check for the OpenGL GLX extension availability:
     */
    if( !glXQueryExtension( fgDisplay.Display, NULL, NULL ) )
    {
        /*
         * GLX extensions have not been found...
         */
        fgError( "OpenGL GLX extension not supported by display '%s'", XDisplayName( displayName ) );
    }

    /*
     * Grab the default screen for the display we have just opened
     */
    fgDisplay.Screen = DefaultScreen( fgDisplay.Display );

    /*
     * The same applying to the root window
     */
    fgDisplay.RootWindow = RootWindow(
        fgDisplay.Display,
        fgDisplay.Screen
    );

    /*
     * Grab the logical screen's geometry
     */
    fgDisplay.ScreenWidth  = DisplayWidth(
        fgDisplay.Display,
        fgDisplay.Screen
    );

    fgDisplay.ScreenHeight = DisplayHeight(
        fgDisplay.Display,
        fgDisplay.Screen
    );

    /*
     * Grab the physical screen's geometry
     */
    fgDisplay.ScreenWidthMM = DisplayWidthMM(
        fgDisplay.Display,
        fgDisplay.Screen
    );

    fgDisplay.ScreenHeightMM = DisplayHeightMM(
        fgDisplay.Display,
        fgDisplay.Screen
    );

    /*
     * The display's connection number
     */
    fgDisplay.Connection = ConnectionNumber( fgDisplay.Display );

    /*
     * Create the window deletion atom
     */
    fgDisplay.DeleteWindow = XInternAtom(
        fgDisplay.Display,
        "WM_DELETE_WINDOW",
        FALSE
    );

#elif TARGET_HOST_WIN32

    WNDCLASS wc;
    ATOM atom;

    /*
     * What we need to do is to initialize the fgDisplay global structure here...
     */
    fgDisplay.Instance = GetModuleHandle( NULL );

    /*
     * Check if the freeglut window class has been registered before...
     */
    atom = GetClassInfo( fgDisplay.Instance, "FREEGLUT", &wc );

    /*
     * ...nope, it has not, and we have to do it right now:
     */
    if( atom == 0 )
    {
        GLboolean retval;

        /*
         * Make sure the unitialized fields are reset to zero
         */
        ZeroMemory( &wc, sizeof(WNDCLASS) );

        /*
         * Each of the windows should have it's own device context...
         */
        wc.style          = CS_OWNDC;
        wc.lpfnWndProc    = fgWindowProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = 0;
        wc.hInstance      = fgDisplay.Instance;
        wc.hIcon          = LoadIcon( NULL, IDI_WINLOGO );
        wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground  = NULL;
        wc.lpszMenuName   = NULL;
        wc.lpszClassName  = "FREEGLUT";

        /*
         * Register the window class
         */
        retval = RegisterClass( &wc );
        g_assert( retval != FALSE );
    }

    /*
     * The screen dimensions can be obtained via GetSystemMetrics() calls
     */
    fgDisplay.ScreenWidth  = GetSystemMetrics( SM_CXSCREEN );
    fgDisplay.ScreenHeight = GetSystemMetrics( SM_CYSCREEN );

    {
        /*
         * Checking the display's size in millimeters isn't too hard, too:
         */
        HWND desktop = GetDesktopWindow();
        HDC  context = GetDC( desktop );

        /*
         * Grab the appropriate values now (HORZSIZE and VERTSIZE respectably):
         */
        fgDisplay.ScreenWidthMM  = GetDeviceCaps( context, HORZSIZE );
        fgDisplay.ScreenHeightMM = GetDeviceCaps( context, VERTSIZE );

        /*
         * Whoops, forgot to release the device context :)
         */
        ReleaseDC( desktop, context );
    }

#endif

    /*
     * Have the joystick device initialized now
     */
    fgJoystickInit( 0 );
}

/*
 * Perform the freeglut deinitialization...
 */
void fgDeinitialize( void )
{
    SFG_Timer *timer;

    /*
     * Check if initialization has been performed before
     */
    if( !fgState.Time.Set )
    {
        fgWarning( "fgDeinitialize(): fgState.Timer is null => no valid initialization has been performed" );
        return;
    }

    /*
     * Perform the freeglut structure deinitialization
     */
    fgDestroyStructure();

    /*
     * Delete all the timers and their storage list
     */
    while ( (timer = fgState.Timers.First) != NULL )
    {
	fgListRemove(&fgState.Timers, &timer->Node);
	free(timer);
    }

    /*
     * Deinitialize the joystick device
     */
    fgJoystickClose();

#if TARGET_HOST_UNIX_X11

    /*
     * Make sure all X-client data we have created will be destroyed on display closing
     */
    XSetCloseDownMode( fgDisplay.Display, DestroyAll );

    /*
     * Close the display connection, destroying all windows we have created so far
     */
    XCloseDisplay( fgDisplay.Display );

#endif
}


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Perform initialization. This usually happens on the program startup
 * and restarting after glutMainLoop termination...
 */
void FGAPIENTRY glutInit( int* pargc, char** argv )
{
    char* geometrySettings = NULL;
    char* displayName = NULL;
    int i, j, argc = *pargc;

    /*
     * Do not allow multiple initialization of the library
     */
    if( fgState.Time.Set )
    {
        /*
         * We can't have multiple initialization performed
         */
        fgError( "illegal glutInit() reinitialization attemp" );
    }

    /*
     * Have the internal freeglut structure initialized now
     */
    fgCreateStructure();

    /*
     * Fill in the default values that have not been passed in yet.
     */
    if( fgState.Position.Use == FALSE )
    {
        fgState.Position.X = -1;
        fgState.Position.Y = -1;
    }

    if( fgState.Size.Use == FALSE )
    {
        fgState.Size.X   =  300;
        fgState.Size.Y   =  300;
        fgState.Size.Use = TRUE;
    }

    /*
     * Some more settings to come
     */
    fgState.ForceDirectContext = FALSE;
    fgState.TryDirectContext   = TRUE;
    fgState.ForceIconic        = FALSE;
    fgState.GLDebugSwitch      = FALSE;
    fgState.XSyncSwitch        = FALSE;

    /*
     * Assume we want to ignore the automatic key repeat
     */
    fgState.IgnoreKeyRepeat = TRUE;

    /*
     * The default display mode to be used
     */
    fgState.DisplayMode = GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH;

    /*
     * Set the default game mode settings
     */
    fgState.GameModeSize.X  = 640;
    fgState.GameModeSize.Y  = 480;
    fgState.GameModeDepth   =  16;
    fgState.GameModeRefresh =  72;

    /*
     * Remember the function's call time
     */
#ifndef WIN32
    gettimeofday(&fgState.Time.Value, NULL);
#else
    fgState.Time.Value = timeGetTime();
#endif
    fgState.Time.Set = TRUE;

    /*
     * Grab the environment variable indicating the X display to use.
     * This is harmless under Win32, so let's let it stay here...
     */
    displayName = strdup( getenv( "DISPLAY" ) );

    /*
     * Have the program arguments parsed.
     */
    for( i=1; i<argc; i++ )
    {
        /*
         * The X display name settings
         */
        if( strcmp( argv[ i ], "-display" ) == 0 )
        {
            /*
             * Check for possible lack of the next argument
             */
            if( ++i >= argc )
                fgError( "-display parameter must be followed by display name" );

            /*
             * Release the previous display name (the one from app's environment)
             */
            free( displayName );

            /*
             * Make a working copy of the name for us to use
             */
            displayName = strdup( argv[ i ] );

            /*
             * Have both arguments removed
             */
            argv[ i - 1 ] = NULL;
            argv[   i   ] = NULL;
            (* pargc) -= 2;
        }

        /*
         * The geometry settings
         */
        else if( strcmp( argv[ i ], "-geometry" ) == 0 )
        {
            /*
             * Again, check if there is at least one more argument
             */
            if( ++i >= argc )
                fgError( "-geometry parameter must be followed by window geometry settings" );

            /*
             * Otherwise make a duplicate of the geometry settings...
             */
            geometrySettings = strdup( argv[ i ] );

            /*
             * Have both arguments removed
             */
            argv[ i - 1 ] = NULL;
            argv[   i   ] = NULL;
            (* pargc) -= 2;
        }

        /*
         * The direct/indirect OpenGL contexts settings
         */
        else if( strcmp( argv[ i ], "-direct" ) == 0)
        {
            /*
             * We try to force direct rendering...
             */
            if( fgState.TryDirectContext == FALSE )
                fgError( "parameters ambiguity, -direct and -indirect cannot be both specified" );

            fgState.ForceDirectContext = TRUE;
            argv[ i ] = NULL;
            (* pargc)--;
        }
        else if( strcmp( argv[ i ], "-indirect" ) == 0 )
        {
            /*
             * We try to force indirect rendering...
             */
            if( fgState.ForceDirectContext == TRUE )
                fgError( "parameters ambiguity, -direct and -indirect cannot be both specified" );

            fgState.TryDirectContext = FALSE;
            argv[ i ] = NULL;
            (* pargc)--;
        }

        /*
         * The '-iconic' parameter makes all new top-level
         * windows created in iconified state...
         */
        else if( strcmp( argv[ i ], "-iconic" ) == 0 )
        {
            fgState.ForceIconic = TRUE;
            argv[ i ] = NULL;
            (* pargc)--;
        }

        /*
         * The '-gldebug' option activates some OpenGL state debugging features
         */
        else if( strcmp( argv[ i ], "-gldebug" ) == 0 )
        {
            fgState.GLDebugSwitch = TRUE;
            argv[ i ] = NULL;
            (* pargc)--;
        }

        /*
         * The '-sync' option activates X protocol synchronization (for debugging purposes)
         */
        else if( strcmp( argv[ i ], "-sync" ) == 0 )
        {
            fgState.XSyncSwitch = TRUE;
            argv[ i ] = NULL;
            (* pargc)--;
        }
    }

    /*
     * Have the arguments list compacted now
     */
    for( i=1; i<argc; i++ )
    {
        if( argv[ i ] == NULL )
        {
            for( j=i; j<argc; j++ )
            {
                if( argv[ j ] != NULL )
                {
                    argv[ i ] = argv[ j ];
                    argv[ j ] = NULL;
                }
            }
        }
    }

    /*
     * Have the display created now. As I am too lazy to implement
     * the program arguments parsing, we will have the DISPLAY
     * environment variable used for opening the X display:
     */
    fgInitialize( displayName );

#if TARGET_HOST_UNIX_X11
    /*
     * We can process the default window geometry settings safely now
     *
     * WARNING: have this rewritten to be portable. That won't be hard.
     */
    if( geometrySettings != NULL )
    {
        int result, x, y;
	unsigned int w, h;

        /*
         * Have the settings parsed now. This is easy.
         * We will use the XParseGeometry function.
         */
        result = XParseGeometry( geometrySettings, &x, &y, &w, &h );

        /*
         * Check what we have been supplied with...
         */
        if( (result & WidthValue) && (w >= 0) )
            fgState.Size.X = w;

        if( (result & HeightValue) && (h >= 0) )
            fgState.Size.Y = h;

        if( result & XValue )
        {
            if( result & XNegative )
                fgState.Position.X = fgDisplay.ScreenWidth + x - fgState.Size.X;
            else
                fgState.Position.X = x;
        }

        if( result & YValue )
        {
            if( result & YNegative )
                fgState.Position.Y = fgDisplay.ScreenHeight + y - fgState.Size.Y;
            else
                fgState.Position.Y = y;
        }

        /*
         * Free the geometry settings string
         */
        free( geometrySettings );
    }
#endif

    /*
     * Check for the minus one settings for both position and size...
     */
    if( fgState.Position.X < 0 || fgState.Position.Y < 0 )
        fgState.Position.Use = FALSE;

    if( fgState.Size.X < 0 || fgState.Size.Y < 0 )
        fgState.Size.Use = FALSE;

    /*
     * Do not forget about releasing the display name string
     */
    free( displayName );
}

/*
 * Sets the default initial window position for new windows
 */
void FGAPIENTRY glutInitWindowPosition( int x, int y )
{
    /*
     * The settings can be disables when both coordinates are negative
     */
    if( (x >= 0) && (y >= 0) )
    {
        /*
         * We want to specify the initial position of each of the windows
         */
        fgState.Position.X   =    x;
        fgState.Position.Y   =    y;
        fgState.Position.Use = TRUE;
    }
    else
    {
        /*
         * The initial position of each of the windows is specified by the wm
         */
        fgState.Position.X   =    -1;
        fgState.Position.Y   =    -1;
        fgState.Position.Use = FALSE;
    }
}

/*
 * Sets the default initial window size for new windows
 */
void FGAPIENTRY glutInitWindowSize( int width, int height )
{
    /*
     * The settings can be disables when both values are negative
     */
    if( (width >= 0) && (height >= 0) )
    {
        /*
         * We want to specify the initial size of each of the windows
         */
        fgState.Size.X   =  width;
        fgState.Size.Y   = height;
        fgState.Size.Use =   TRUE;
    }
    else
    {
        /*
         * The initial size of each of the windows is specified by the wm
         */
        fgState.Size.X   =    -1;
        fgState.Size.Y   =    -1;
        fgState.Size.Use = FALSE;
    }
}

/*
 * Sets the default display mode for all new windows
 */
void FGAPIENTRY glutInitDisplayMode( int displayMode )
{
    /*
     * We will make use of this value when creating a new OpenGL context...
     */
    fgState.DisplayMode = displayMode;
}


/* -- INIT DISPLAY STRING PARSING ------------------------------------------ */

#if 0 /* FIXME: CJP */
/*
 * There is a discrete number of comparison operators we can encounter:
 *
 *     comparison ::= "=" | "!=" | "<" | ">" | "<=" | ">=" | "~"
 */
#define  FG_NONE           0x0000
#define  FG_EQUAL          0x0001
#define  FG_NOT_EQUAL      0x0002
#define  FG_LESS           0x0003
#define  FG_MORE           0x0004
#define  FG_LESS_OR_EQUAL  0x0005
#define  FG_MORE_OR_EQUAL  0x0006
#define  FG_CLOSEST        0x0007

/*
 * The caller can feed us with a number of capability tokens:
 *
 * capability ::= "alpha" | "acca" | "acc" | "blue" | "buffer" | "conformant" | "depth" | "double" |
 *                "green" | "index" | "num" | "red" | "rgba" | "rgb" | "luminance" | "stencil" |
 *                "single" | "stereo" | "samples" | "slow" | "win32pdf" | "xvisual" | "xstaticgray" |
 *                "xgrayscale" | "xstaticcolor" | "xpseudocolor" | "xtruecolor" | "xdirectcolor"
 */
static gchar* g_Tokens[] =
{
    "none", "alpha", "acca", "acc", "blue", "buffer", "conformant", "depth", "double", "green",
    "index", "num", "red", "rgba", "rgb", "luminance", "stencil", "single", "stereo", "samples",
    "slow", "win32pdf", "xvisual", "xstaticgray", "xgrayscale", "xstaticcolor", "xpseudocolor",
    "xtruecolor", "xdirectcolor", NULL
};

/*
 * The structure to hold the parsed display string tokens
 */
typedef struct tagSFG_Capability SFG_Capability;
struct tagSFG_Capability
{
    gint capability;        /* the capability token enumerator */
    gint comparison;        /* the comparison operator used    */
    gint value;             /* the value we're comparing to    */
};

/*
 * The scanner configuration for the init display string
 */
static GScannerConfig fgInitDisplayStringScannerConfig =
{
    ( " \t\r\n" )               /* cset_skip_characters     */,
    (
        G_CSET_a_2_z
        "_"
        G_CSET_A_2_Z
    )			                /* cset_identifier_first    */,
    (
        G_CSET_a_2_z
        "_0123456789"
        G_CSET_A_2_Z
        G_CSET_LATINS
        G_CSET_LATINC
        "<>!=~"
    )			                /* cset_identifier_nth      */,
    ( "#\n" )		            /* cpair_comment_single     */,
    FALSE			            /* case_sensitive           */,
    TRUE			            /* skip_comment_multi       */,
    TRUE			            /* skip_comment_single      */,
    TRUE			            /* scan_comment_multi       */,
    TRUE			            /* scan_identifier          */,
    FALSE			            /* scan_identifier_1char    */,
    FALSE			            /* scan_identifier_NULL     */,
    TRUE			            /* scan_symbols             */,
    FALSE			            /* scan_binary              */,
    TRUE			            /* scan_octal               */,
    TRUE			            /* scan_float               */,
    TRUE			            /* scan_hex                 */,
    FALSE			            /* scan_hex_dollar          */,
    TRUE			            /* scan_string_sq           */,
    TRUE			            /* scan_string_dq           */,
    TRUE			            /* numbers_2_int            */,
    FALSE			            /* int_2_float              */,
    FALSE			            /* identifier_2_string      */,
    TRUE			            /* char_2_token             */,
    FALSE			            /* symbol_2_token           */,
    FALSE			            /* scope_0_fallback         */,
};

/*
 * Sets the default display mode for all new windows using a string
 */
void FGAPIENTRY glutInitDisplayString( char* displayMode )
{
    /*
     * display_string ::= (switch)
     * switch         ::= capability [comparison value]
     * comparison     ::= "=" | "!=" | "<" | ">" | "<=" | ">=" | "~"
     * capability     ::= "alpha" | "acca" | "acc" | "blue" | "buffer" | "conformant" |
     *                    "depth" | "double" | "green" | "index" | "num" | "red" | "rgba" |
     *                    "rgb" | "luminance" | "stencil" | "single" | "stereo" |
     *                    "samples" | "slow" | "win32pdf" | "xvisual" | "xstaticgray" |
     *                    "xgrayscale" | "xstaticcolor" | "xpseudocolor" |
     *                    "xtruecolor" | "xdirectcolor"
     * value          ::= 0..9 [value]
     *
     * The display string grammar. This should be EBNF, but I couldn't find the definitions so, to
     * clarify: (expr) means 0 or more times the expression, [expr] means 0 or 1 times expr.
     *
     * Create a new GLib lexical analyzer to process the display mode string
     */
    GScanner* scanner = g_scanner_new( &fgInitDisplayStringScannerConfig );
    GList* caps = NULL;
    gint i;

    /*
     * Fail if the display mode string is empty or the scanner failed to initialize
     */
    freeglut_return_if_fail( (scanner != NULL) && (strlen( displayMode ) > 0) );

    /*
     * Set the scanner's input name (for debugging)
     */
    scanner->input_name = "glutInitDisplayString";

    /*
     * Start the lexical analysis of the extensions string
     */
    g_scanner_input_text( scanner, displayMode, strlen( displayMode ) );

    /*
     * While there are any more tokens to be checked...
     */
    while( !g_scanner_eof( scanner ) )
    {
        /*
         * Actually we're expecting only string tokens
         */
        GTokenType tokenType = g_scanner_get_next_token( scanner );

        /*
         * We are looking for identifiers
         */
        if( tokenType == G_TOKEN_IDENTIFIER )
        {
            gchar* capability  = NULL;  /* the capability identifier string (always present) */
            gint   capID       =    0;  /* the capability identifier value (from g_Tokens)   */
            gint   comparison  =    0;  /* the comparison operator value, see definitions    */
            gchar* valueString = NULL;  /* if the previous one is present, this is needed    */
            gint   value       =    0;  /* the integer value converted using a strtol call   */
            SFG_Capability* capStruct;  /* the capability description structure              */

            /*
             * OK. The general rule of thumb that we always should be getting a capability identifier
             * string (see the grammar description). If it is followed by a comparison identifier, then
             * there must follow an integer value we're comparing the capability to...
             *
             * Have the current token analyzed with that in mind...
             */
            for( i=0; i<(gint) strlen( scanner->value.v_identifier ); i++ )
            {
                gchar c = scanner->value.v_identifier[ i ];

                if( (c == '=') || (c == '!') || (c == '<') || (c == '>') || (c == '~') )
                    break;
            }

            /*
             * Here we go with the length of the capability identifier string.
             * In the worst of cases, it is as long as the token identifier.
             */
            capability = g_strndup( scanner->value.v_identifier, i );

            /*
             * OK. Is there a chance for comparison and value identifiers to follow?
             * Note: checking against i+1 as this handles two cases: single character
             * comparison operator and first of value's digits, which must always be
             * there, or the two-character comparison operators.
             */
            if( (i + 1) < (gint) strlen( scanner->value.v_identifier ) )
            {
                /*
                 * Yeah, indeed, it is the i-th character to start the identifier, then.
                 */
                gchar c1 = scanner->value.v_identifier[ i + 0 ];
                gchar c2 = scanner->value.v_identifier[ i + 1 ];

                if( (c1 == '=')                ) { i += 1; comparison = FG_EQUAL;         } else
                if( (c1 == '!') && (c2 == '=') ) { i += 2; comparison = FG_NOT_EQUAL;     } else
                if( (c1 == '<') && (c2 == '=') ) { i += 2; comparison = FG_LESS_OR_EQUAL; } else
                if( (c1 == '>') && (c2 == '=') ) { i += 2; comparison = FG_MORE_OR_EQUAL; } else
                if( (c1 == '<')                ) { i += 1; comparison = FG_LESS;          } else
                if( (c1 == '>')                ) { i += 1; comparison = FG_MORE;          } else
                if( (c1 == '~')                ) { i += 1; comparison = FG_CLOSEST;       } else
                g_warning( "invalid comparison operator in token `%s'", scanner->value.v_identifier );
            }

            /*
             * Grab the value string that must follow the comparison operator...
             */
            if( comparison != FG_NONE && i < (gint) strlen( scanner->value.v_identifier ) )
                valueString = g_strdup( scanner->value.v_identifier + i );

            /*
             * If there was a value string, convert it to integer...
             */
            if( comparison != FG_NONE && strlen( valueString ) > 0 )
                value = strtol( valueString, NULL, 0 );

            /*
             * Now we need to match the capability string and it's ID
             */
            for( i=0; g_Tokens[ i ]!=NULL; i++ )
            {
                if( strcmp( capability, g_Tokens[ i ] ) == 0 )
                {
                    /*
                     * Looks like we've found the one we were looking for
                     */
                    capID = i;
                    break;
                }
            }

            /*
             * Create a new capability description structure
             */
            capStruct = g_new0( SFG_Capability, 1 );

            /*
             * Fill in the cap's values, as we have parsed it:
             */
            capStruct->capability =      capID;
            capStruct->comparison = comparison;
            capStruct->value      =      value;

            /*
             * Add the new capabaility to the caps list
             */
            caps = g_list_append( caps, capStruct );

            /*
             * Clean up the local mess and keep rolling on
             */
            g_free( valueString );
            g_free( capability );
        }
    }

    /*
     * Now that we have converted the string into somewhat more machine-friendly
     * form, proceed with matching the frame buffer configuration...
     *
     * The caps list could be passed to a function that would try finding the closest 
     * matching pixel format, visual, frame buffer configuration or whatever. It would 
     * be good to convert the glutInitDisplayMode() to use the same method.
     */
#if 0
    g_message( "found %i capability preferences", g_list_length( caps ) );

    g_message( "token `%s': cap: %i, com: %i, val: %i",
        scanner->value.v_identifier,
        capStruct->capability,
        capStruct->comparison,
        capStruct->value
    );
#endif

    /*
     * Free the capabilities we have parsed
     */
    for( i=0; i<(gint) g_list_length( caps ); i++ )
        g_free( g_list_nth( caps, i )->data );

    /*
     * Destroy the capabilities list itself
     */
    g_list_free( caps );

    /*
     * Free the lexical scanner now...
     */
    g_scanner_destroy( scanner );
}
#endif

void FGAPIENTRY glutInitDisplayString( char* displayMode )
{
}

/*** END OF FILE ***/
