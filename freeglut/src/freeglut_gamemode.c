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
#include "freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  glutGameModeString()    -- missing
 *  glutEnterGameMode()     -- X11 version
 *  glutLeaveGameMode()     -- is that correct?
 *  glutGameModeGet()       -- is that correct?
 */


/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */

static int xrandr_resize(int xsz, int ysz, int just_checking)
{
    int res = -1;

#ifdef HAVE_X11_EXTENSIONS_XRANDR_H
    int event_base, error_base;
    Status st;
    XRRScreenConfiguration *xrr_config;
    XRRScreenSize *ssizes;
    Rotation rot;
    int i, ssizes_count, curr;
    Time timestamp, cfg_timestamp;

    /* must check at runtime for the availability of the extension */
    if(!XRRQueryExtension(fgDisplay.Display, &event_base, &error_base)) {
        return -1;
    }

    if(!(xrr_config = XRRGetScreenInfo(fgDisplay.Display, fgDisplay.RootWindow))) {
        fgWarning("XRRGetScreenInfo failed");
        return -1;
    }
    ssizes = XRRConfigSizes(xrr_config, &ssizes_count);
    curr = XRRConfigCurrentConfiguration(xrr_config, &rot);
    timestamp = XRRConfigTimes(xrr_config, &cfg_timestamp);

    if(xsz == ssizes[curr].width && ysz == ssizes[curr].height) {
        /* no need to switch, we're already in the requested mode */
        res = 0;
        goto done;
    }

    for(i=0; i<ssizes_count; i++) {
        if(ssizes[i].width == xsz && ssizes[i].height == ysz) {
            break;  /* found it */
        }
    }
    if(i == ssizes_count)
        goto done;

    if(just_checking) {
        res = 0;
        goto done;
    }

    if((st = XRRSetScreenConfig(fgDisplay.Display, xrr_config, fgDisplay.RootWindow,
                    i, rot, timestamp)) != 0) {
        fgWarning("XRRSetScreenConfig failed");
        goto done;
    }
    res = 0;

done:
    XRRFreeScreenConfigInfo(xrr_config);
#endif
    return res;
}


/*
 * Remembers the current visual settings, so that
 * we can change them and restore later...
 */
static void fghRememberState( void )
{
#if TARGET_HOST_POSIX_X11
    int event_base, error_base;

#   ifdef HAVE_X11_EXTENSIONS_XRANDR_H
    if(XRRQueryExtension(fgDisplay.Display, &event_base, &error_base)) {
        XRRScreenConfiguration *xrr_config;
        XRRScreenSize *ssizes;
        Rotation rot;
        int ssize_count, curr;

        if((xrr_config = XRRGetScreenInfo(fgDisplay.Display, fgDisplay.RootWindow))) {
            ssizes = XRRConfigSizes(xrr_config, &ssize_count);
            curr = XRRConfigCurrentConfiguration(xrr_config, &rot);

            fgDisplay.prev_xsz = ssizes[curr].width;
            fgDisplay.prev_ysz = ssizes[curr].height;
            fgDisplay.prev_size_valid = 1;
            XRRFreeScreenConfigInfo(xrr_config);
            return;
        }
    }
#   endif

    /*
     * This highly depends on the XFree86 extensions,
     * not approved as X Consortium standards
     */
#   ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
    if(!XF86VidModeQueryExtension(fgDisplay.Display, &event_base, &error_base)) {
        return;
    }

    /*
     * Remember the current ViewPort location of the screen to be able to
     * restore the ViewPort on LeaveGameMode():
     */
    if( !XF86VidModeGetViewPort(
             fgDisplay.Display,
             fgDisplay.Screen,
             &fgDisplay.DisplayViewPortX,
             &fgDisplay.DisplayViewPortY ) )
        fgWarning( "XF86VidModeGetViewPort failed" );

    /*
     * Remember the current pointer location before going fullscreen
     * for restoring it later:
     */
    {
        Window junk_window;
        unsigned int mask;

        XQueryPointer(
            fgDisplay.Display, fgDisplay.RootWindow,
            &junk_window, &junk_window,
            &fgDisplay.DisplayPointerX, &fgDisplay.DisplayPointerY,
            &fgDisplay.DisplayPointerX, &fgDisplay.DisplayPointerY, &mask
        );
    }

    /* Query the current display settings: */
    fgDisplay.DisplayModeValid =
      XF86VidModeGetModeLine(
        fgDisplay.Display,
        fgDisplay.Screen,
        &fgDisplay.DisplayModeClock,
        &fgDisplay.DisplayMode
    );

    if( !fgDisplay.DisplayModeValid )
            fgWarning( "XF86VidModeGetModeLine failed" );
#   endif

#elif TARGET_HOST_MS_WINDOWS

/*    DEVMODE devMode; */

    /* Grab the current desktop settings... */

/* hack to get around my stupid cross-gcc headers */
#define FREEGLUT_ENUM_CURRENT_SETTINGS -1

    EnumDisplaySettings( fgDisplay.DisplayName, FREEGLUT_ENUM_CURRENT_SETTINGS,
                         &fgDisplay.DisplayMode );

    /* Make sure we will be restoring all settings needed */
    fgDisplay.DisplayMode.dmFields |=
        DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

#endif
}

/*
 * Restores the previously remembered visual settings
 */
static void fghRestoreState( void )
{
#if TARGET_HOST_POSIX_X11

#   ifdef HAVE_X11_EXTENSIONS_XRANDR_H
    if(fgDisplay.prev_size_valid) {
        if(xrandr_resize(fgDisplay.prev_xsz, fgDisplay.prev_ysz, 0) != -1) {
            fgDisplay.prev_size_valid = 0;
            return;
        }
    }
#   endif


#   ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
    /* Restore the remembered pointer position: */
    XWarpPointer(
        fgDisplay.Display, None, fgDisplay.RootWindow, 0, 0, 0, 0,
        fgDisplay.DisplayPointerX, fgDisplay.DisplayPointerY
    );

    /*
     * This highly depends on the XFree86 extensions,
     * not approved as X Consortium standards
     */

    if( fgDisplay.DisplayModeValid )
    {
        XF86VidModeModeInfo** displayModes;
        int i, displayModesCount;

        if( !XF86VidModeGetAllModeLines(
                 fgDisplay.Display,
                 fgDisplay.Screen,
                 &displayModesCount,
                 &displayModes ) )
        {
            fgWarning( "XF86VidModeGetAllModeLines failed" );
            return;
        }


        /*
         * Check every of the modes looking for one that matches our demands.
         * If we find one, switch to it and restore the remembered viewport.
         */
        for( i = 0; i < displayModesCount; i++ )
        {
            if(displayModes[ i ]->hdisplay == fgDisplay.DisplayMode.hdisplay &&
               displayModes[ i ]->vdisplay == fgDisplay.DisplayMode.vdisplay &&
               displayModes[ i ]->dotclock == fgDisplay.DisplayModeClock )
            {
                if( !XF86VidModeSwitchToMode(
                         fgDisplay.Display,
                         fgDisplay.Screen,
                         displayModes[ i ] ) )
                {
                    fgWarning( "XF86VidModeSwitchToMode failed" );
                    break;
                }

                if( !XF86VidModeSetViewPort(
                         fgDisplay.Display,
                         fgDisplay.Screen,
                         fgDisplay.DisplayViewPortX,
                         fgDisplay.DisplayViewPortY ) )
                    fgWarning( "HAVE_X11_EXTENSIONS_XF86VMODE_H failed" );


                /*
                 * For the case this would be the last X11 call the application
                 * calls exit() we've to flush the X11 output queue to have the
                 * commands sent to the X server before the application exits.
                 */
                XFlush( fgDisplay.Display );

                break;
            }
        }
        XFree( displayModes );
    }

#   endif

#elif TARGET_HOST_MS_WINDOWS

    /* Restore the previously remembered desktop display settings */
    ChangeDisplaySettingsEx( fgDisplay.DisplayName,&fgDisplay.DisplayMode, 0,0,0 );

#endif
}

#if TARGET_HOST_POSIX_X11
#ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H

/*
 * Checks a single display mode settings against user's preferences.
 */
static GLboolean fghCheckDisplayMode( int width, int height, int depth, int refresh )
{
    /* The desired values should be stored in fgState structure... */
    return ( width == fgState.GameModeSize.X ) &&
           ( height == fgState.GameModeSize.Y ) &&
           ( depth == fgState.GameModeDepth ) &&
           ( refresh == fgState.GameModeRefresh );
}

/*
 * Checks all display modes settings against user's preferences.
 * Returns the mode number found or -1 if none could be found.
 */
static int fghCheckDisplayModes( GLboolean exactMatch, int displayModesCount, XF86VidModeModeInfo** displayModes )
{
    int i;
    for( i = 0; i < displayModesCount; i++ )
    {
        /* Compute the displays refresh rate, dotclock comes in kHz. */
        int refresh = ( displayModes[ i ]->dotclock * 1000 ) /
                      ( displayModes[ i ]->htotal * displayModes[ i ]->vtotal );

        if( fghCheckDisplayMode( displayModes[ i ]->hdisplay,
                                 displayModes[ i ]->vdisplay,
                                 fgState.GameModeDepth,
                                 ( exactMatch ? refresh : fgState.GameModeRefresh ) ) ) {
            if (!exactMatch)
            {
                /* Update the chosen refresh rate, otherwise a
                 * glutGameModeGet(GLUT_GAME_MODE_REFRESH_RATE) would not
                 * return the right values
                 */
                fgState.GameModeRefresh = refresh;
            }

            return i;
        }
    }
    return -1;
}

#endif
#endif

/*
 * Changes the current display mode to match user's settings
 */
static GLboolean fghChangeDisplayMode( GLboolean haveToTest )
{
    GLboolean success = GL_FALSE;
#if TARGET_HOST_POSIX_X11

    /* first try to use XRandR, then fallback to XF86VidMode */
#   ifdef HAVE_X11_EXTENSIONS_XRANDR_H
    if(xrandr_resize(fgState.GameModeSize.X, fgState.GameModeSize.Y, haveToTest) != -1) {
        return GL_TRUE;
    }
#   endif


    /*
     * This highly depends on the XFree86 extensions,
     * not approved as X Consortium standards
     */
#   ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H

    /*
     * This is also used by applications which check modes by calling
     * glutGameModeGet(GLUT_GAME_MODE_POSSIBLE), so allow the check:
     */
    if( haveToTest || fgDisplay.DisplayModeValid )
    {
        XF86VidModeModeInfo** displayModes;
        int i, displayModesCount;

        /* current display mode was queried in fghRememberState
         * set defaulted values to the current display mode's
         */
        if (fgState.GameModeSize.X == -1)
        {
            fgState.GameModeSize.X = fgDisplay.DisplayMode.hdisplay;
        }
        if (fgState.GameModeSize.Y == -1)
        {
            fgState.GameModeSize.Y = fgDisplay.DisplayMode.vdisplay;
        }
        if (fgState.GameModeDepth == -1)
        {
            /* can't get color depth from this, nor can we change it, do nothing
             * TODO: get with XGetVisualInfo()? but then how to set?
             */
        }
        if (fgState.GameModeRefresh != -1)
        {
            /* Compute the displays refresh rate, dotclock comes in kHz. */
            int refresh = ( fgDisplay.DisplayModeClock * 1000 ) /
                ( fgDisplay.DisplayMode.htotal * fgDisplay.DisplayMode.vtotal );

            fgState.GameModeRefresh = refresh;
        }

        /* query all possible display modes */
        if( !XF86VidModeGetAllModeLines(
                 fgDisplay.Display,
                 fgDisplay.Screen,
                 &displayModesCount,
                 &displayModes ) )
        {
            fgWarning( "XF86VidModeGetAllModeLines failed" );
            return success;
        }


        /*
         * Check every of the modes looking for one that matches our demands,
         * ignoring the refresh rate if no exact match could be found.
         */
        i = fghCheckDisplayModes( GL_TRUE, displayModesCount, displayModes );
        if( i < 0 ) {
            i = fghCheckDisplayModes( GL_FALSE, displayModesCount, displayModes );
        }
        success = ( i < 0 ) ? GL_FALSE : GL_TRUE;

        if( !haveToTest && success ) {
            if( !XF86VidModeSwitchToMode(
                     fgDisplay.Display,
                     fgDisplay.Screen,
                     displayModes[ i ] ) )
                fgWarning( "XF86VidModeSwitchToMode failed" );
        }

        XFree( displayModes );
    }

#   else

    /*
     * XXX warning fghChangeDisplayMode: missing XFree86 video mode extensions,
     * XXX game mode will not change screen resolution when activated
     */
    success = GL_TRUE;

#   endif

#elif TARGET_HOST_MS_WINDOWS

    DEVMODE  devMode;
    char *fggmstr = NULL;

    success = GL_FALSE;

    EnumDisplaySettings( fgDisplay.DisplayName, -1, &devMode ); 
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

    switch ( ChangeDisplaySettingsEx(fgDisplay.DisplayName, &devMode, NULL, haveToTest ? CDS_TEST : CDS_FULLSCREEN , NULL) )
    {
    case DISP_CHANGE_SUCCESSFUL:
        success = GL_TRUE;

        /* update vars in case if windows switched to proper mode */
        EnumDisplaySettings( fgDisplay.DisplayName, FREEGLUT_ENUM_CURRENT_SETTINGS, &devMode );
        fgState.GameModeSize.X  = devMode.dmPelsWidth;        
        fgState.GameModeSize.Y  = devMode.dmPelsHeight;
        fgState.GameModeDepth   = devMode.dmBitsPerPel;
        fgState.GameModeRefresh = devMode.dmDisplayFrequency;
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
        fgWarning(fggmstr); /* I'd rather get info whats going on in my program than wonder about */
                            /* magic happenings behind my back, its lib for devels at last ;) */
#endif

    return success;
}


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
     * fghChangeDisplayMode() above.
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
        fghRememberState( );

    if( ! fghChangeDisplayMode( GL_FALSE ) )
    {
        fgWarning( "failed to change screen settings" );
        return 0;
    }

    fgStructure.GameModeWindow = fgCreateWindow(
        NULL, "FREEGLUT", GL_TRUE, 0, 0,
        GL_TRUE, fgState.GameModeSize.X, fgState.GameModeSize.Y,
        GL_TRUE, GL_FALSE
    );

    fgStructure.GameModeWindow->State.Width  = fgState.GameModeSize.X;
    fgStructure.GameModeWindow->State.Height = fgState.GameModeSize.Y;
    fgStructure.GameModeWindow->State.NeedToResize = GL_TRUE;

#if TARGET_HOST_POSIX_X11

    /*
     * Sync needed to avoid a real race, the Xserver must have really created
     * the window before we can grab the pointer into it:
     */
    XSync( fgDisplay.Display, False );

    /*
     * Grab the pointer to confine it into the window after the calls to
     * XWrapPointer() which ensure that the pointer really enters the window.
     *
     * We also need to wait here until XGrabPointer() returns GrabSuccess,
     * otherwise the new window is not viewable yet and if the next function
     * (XSetInputFocus) is called with a not yet viewable window, it will exit
     * the application which we have to aviod, so wait until it's viewable:
     */
    while( GrabSuccess != XGrabPointer(
               fgDisplay.Display, fgStructure.GameModeWindow->Window.Handle,
               TRUE,
               ButtonPressMask | ButtonReleaseMask | ButtonMotionMask
               | PointerMotionMask,
               GrabModeAsync, GrabModeAsync,
               fgStructure.GameModeWindow->Window.Handle, None, CurrentTime) )
        usleep( 100 );

    /*
     * Change input focus to the new window. This will exit the application
     * if the new window is not viewable yet, see the XGrabPointer loop above.
     */
    XSetInputFocus(
        fgDisplay.Display,
        fgStructure.GameModeWindow->Window.Handle,
        RevertToNone,
        CurrentTime
    );

    /* Move the Pointer to the middle of the fullscreen window */
    XWarpPointer(
        fgDisplay.Display,
        None,
        fgDisplay.RootWindow,
        0, 0, 0, 0,
        fgState.GameModeSize.X/2, fgState.GameModeSize.Y/2
    );

#   ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H

    if( fgDisplay.DisplayModeValid )
    {
        int x, y;
        Window child;

        /* Change to viewport to the window topleft edge: */
        if( !XF86VidModeSetViewPort( fgDisplay.Display, fgDisplay.Screen, 0, 0 ) )
            fgWarning( "XF86VidModeSetViewPort failed" );

        /*
         * Final window repositioning: It could be avoided using an undecorated
         * window using override_redirect, but this * would possily require
         * more changes and investigation.
         */

        /* Get the current postion of the drawable area on screen */
        XTranslateCoordinates(
            fgDisplay.Display,
            fgStructure.CurrentWindow->Window.Handle,
            fgDisplay.RootWindow,
            0, 0, &x, &y,
            &child
        );

        /* Move the decorataions out of the topleft corner of the display */
        XMoveWindow( fgDisplay.Display, fgStructure.CurrentWindow->Window.Handle,
                     -x, -y);
    }

#endif

    /* Grab the keyboard, too */
    XGrabKeyboard(
        fgDisplay.Display,
        fgStructure.GameModeWindow->Window.Handle,
        FALSE,
        GrabModeAsync, GrabModeAsync,
        CurrentTime
    );

#endif

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

#if TARGET_HOST_POSIX_X11

    XUngrabPointer( fgDisplay.Display, CurrentTime );
    XUngrabKeyboard( fgDisplay.Display, CurrentTime );

#endif

    fghRestoreState();
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
        return fghChangeDisplayMode( GL_TRUE );

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
