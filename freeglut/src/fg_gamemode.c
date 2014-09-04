/*
 * freeglut_gamemode.c
 *
 * The game mode handling code.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Thu Dec 16 1999
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
#include "fg_internal.h"


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */
extern void fgPlatformRememberState( void );
extern void fgPlatformRestoreState( void );
extern GLboolean fgPlatformChangeDisplayMode( GLboolean haveToTest );
extern void fgPlatformEnterGameMode( void );
extern void fgPlatformLeaveGameMode( void );


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Sets the game mode display string
 */
void FGAPIENTRY glutGameModeString( const char* string )
{
    int width = -1, height = -1, depth = -1, refresh = -1;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutGameModeString" );

    /*
     * This one seems a bit easier than glutInitDisplayString. The bad thing
     * about it that I was unable to find the game mode string definition, so
     * that I assumed it is: "[width]x[height]:[depth]@[refresh rate]", which
     * appears in all GLUT game mode programs I have seen to date.
     */
    if( sscanf( string, "%ix%i:%i@%i", &width, &height, &depth, &refresh ) !=
        4 )
        if( sscanf( string, "%ix%i:%i", &width, &height, &depth ) != 3 )
            if( sscanf( string, "%ix%i@%i", &width, &height, &refresh ) != 3 )
                if( sscanf( string, "%ix%i", &width, &height ) != 2 )
                    if( sscanf( string, ":%i@%i", &depth, &refresh ) != 2 )
                        if( sscanf( string, ":%i", &depth ) != 1 )
                            if( sscanf( string, "@%i", &refresh ) != 1 )
                                fgWarning(
                                    "unable to parse game mode string `%s'",
                                    string
                                );

    /* All values not specified are now set to -1, which means those
     * aspects of the current display mode are not changed in
     * fgPlatformChangeDisplayMode() above.
     */
    fgState.GameModeSize.X  = width;
    fgState.GameModeSize.Y  = height;
    fgState.GameModeDepth   = depth;
    fgState.GameModeRefresh = refresh;
}



/*
 * Enters the game mode
 */
int FGAPIENTRY glutEnterGameMode( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutEnterGameMode" );

    if( fgStructure.GameModeWindow )
        fgAddToWindowDestroyList( fgStructure.GameModeWindow );
    else
        fgPlatformRememberState( );

    if( ! fgPlatformChangeDisplayMode( GL_FALSE ) )
    {
        fgWarning( "failed to change screen settings" );
        return 0;
    }

    fgStructure.GameModeWindow = fgCreateWindow(
        NULL, "FREEGLUT", GL_TRUE, 0, 0,
        GL_TRUE, fgState.GameModeSize.X, fgState.GameModeSize.Y,
        GL_TRUE, GL_FALSE
    );

    glutFullScreen();

    fgPlatformEnterGameMode();

    return fgStructure.GameModeWindow->ID;
}

/*
 * Leaves the game mode
 */
void FGAPIENTRY glutLeaveGameMode( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutLeaveGameMode" );

    freeglut_return_if_fail( fgStructure.GameModeWindow );

    fgAddToWindowDestroyList( fgStructure.GameModeWindow );
    fgStructure.GameModeWindow = NULL;

    fgPlatformLeaveGameMode();

    fgPlatformRestoreState();
}

/*
 * Returns information concerning the freeglut game mode
 */
int FGAPIENTRY glutGameModeGet( GLenum eWhat )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutGameModeGet" );

    switch( eWhat )
    {
    case GLUT_GAME_MODE_ACTIVE:
        return !!fgStructure.GameModeWindow;

    case GLUT_GAME_MODE_POSSIBLE:
        return fgPlatformChangeDisplayMode( GL_TRUE );

    case GLUT_GAME_MODE_WIDTH:
        return fgState.GameModeSize.X;

    case GLUT_GAME_MODE_HEIGHT:
        return fgState.GameModeSize.Y;

    case GLUT_GAME_MODE_PIXEL_DEPTH:
        return fgState.GameModeDepth;

    case GLUT_GAME_MODE_REFRESH_RATE:
        return fgState.GameModeRefresh;

    case GLUT_GAME_MODE_DISPLAY_CHANGED:
        /*
         * This is true if the game mode has been activated successfully..
         */
        return !!fgStructure.GameModeWindow;
    }

    fgWarning( "Unknown gamemode get: %d", eWhat );
    return -1;
}

/*** END OF FILE ***/
