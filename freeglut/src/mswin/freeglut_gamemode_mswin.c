/*
 * freeglut_gamemode_mswin.c
 *
 * The Windows-specific mouse cursor related stuff.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by John F. Fay, <fayjf@sourceforge.net>
 * Creation date: Thu Jan 19, 2012
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
#include "../Common/freeglut_internal.h"

/*
 * Remembers the current visual settings, so that
 * we can change them and restore later...
 */
void fgPlatformRememberState( void )
{
/*    DEVMODE devMode; */

    /* Grab the current desktop settings... */

/* hack to get around my stupid cross-gcc headers */
#define FREEGLUT_ENUM_CURRENT_SETTINGS -1

    EnumDisplaySettings( fgDisplay.pDisplay.DisplayName, FREEGLUT_ENUM_CURRENT_SETTINGS,
                         &fgDisplay.pDisplay.DisplayMode );

    /* Make sure we will be restoring all settings needed */
    fgDisplay.pDisplay.DisplayMode.dmFields |=
        DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

}

/*
 * Restores the previously remembered visual settings
 */
void fgPlatformRestoreState( void )
{
    /* Restore the previously remembered desktop display settings */
    ChangeDisplaySettingsEx( fgDisplay.pDisplay.DisplayName,&fgDisplay.pDisplay.DisplayMode, 0,0,0 );

}




/*
 * Changes the current display mode to match user's settings
 */
GLboolean fgPlatformChangeDisplayMode( GLboolean haveToTest )
{
    GLboolean success = GL_FALSE;
    DEVMODE  devMode;
    char *fggmstr = NULL;
    char displayMode[300];

    success = GL_FALSE;

    EnumDisplaySettings( fgDisplay.pDisplay.DisplayName, -1, &devMode ); 
    devMode.dmFields = 0;

    if (fgState.GameModeSize.X!=-1)
    {
        devMode.dmPelsWidth  = fgState.GameModeSize.X;
        devMode.dmFields |= DM_PELSWIDTH;
    }
    if (fgState.GameModeSize.Y!=-1)
    {
        devMode.dmPelsHeight  = fgState.GameModeSize.Y;
        devMode.dmFields |= DM_PELSHEIGHT;
    }
    if (fgState.GameModeDepth!=-1)
    {
        devMode.dmBitsPerPel  = fgState.GameModeDepth;
        devMode.dmFields |= DM_BITSPERPEL;
    }
    if (fgState.GameModeRefresh!=-1)
    {
        devMode.dmDisplayFrequency  = fgState.GameModeRefresh;
        devMode.dmFields |= DM_DISPLAYFREQUENCY;
    }

    switch ( ChangeDisplaySettingsEx(fgDisplay.pDisplay.DisplayName, &devMode, NULL, haveToTest ? CDS_TEST : CDS_FULLSCREEN , NULL) )
    {
    case DISP_CHANGE_SUCCESSFUL:
        success = GL_TRUE;

        if (!haveToTest)
        {
            /* update vars in case if windows switched to proper mode */
            EnumDisplaySettings( fgDisplay.pDisplay.DisplayName, FREEGLUT_ENUM_CURRENT_SETTINGS, &devMode );
            fgState.GameModeSize.X  = devMode.dmPelsWidth;        
            fgState.GameModeSize.Y  = devMode.dmPelsHeight;
            fgState.GameModeDepth   = devMode.dmBitsPerPel;
            fgState.GameModeRefresh = devMode.dmDisplayFrequency;
        }
		break;
    case DISP_CHANGE_RESTART:
        fggmstr = "The computer must be restarted for the graphics mode to work.";
        break;
    case DISP_CHANGE_BADFLAGS:
        fggmstr = "An invalid set of flags was passed in.";
        break;
    case DISP_CHANGE_BADPARAM:
        fggmstr = "An invalid parameter was passed in. This can include an invalid flag or combination of flags.";
        break;
    case DISP_CHANGE_FAILED:
        fggmstr = "The display driver failed the specified graphics mode.";
        break;
    case DISP_CHANGE_BADMODE:
        fggmstr = "The graphics mode is not supported.";
        break;
    default:
        fggmstr = "Unknown error in graphics mode???"; /* dunno if it is possible,MSDN does not mention any other error */
        break;
    }

    if ( !success )
    {
        /* I'd rather get info whats going on in my program than wonder about */
        /* magic happenings behind my back, its lib for devels at last ;) */
        
        /* append display mode to error to make things more informative */
        sprintf(displayMode,"%s Problem with requested mode: %ix%i:%i@%i", fggmstr, devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel, devMode.dmDisplayFrequency);
        fgWarning(displayMode);
    }

    return success;
}

void fgPlatformEnterGameMode( void ) 
{
}

void fgPlatformLeaveGameMode( void ) 
{
}

