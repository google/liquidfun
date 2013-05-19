/*
 * freeglut_gamemode_x11.c
 *
 * The game mode handling code.
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

#include <GL/freeglut.h>
#include "../fg_internal.h"

#ifdef HAVE_X11_EXTENSIONS_XRANDR_H
static int xrandr_resize(int xsz, int ysz, int rate, int just_checking)
{
    int event_base, error_base, ver_major, ver_minor, use_rate;
    XRRScreenConfiguration *xrr_config = 0;
    Status result = -1;

    /* must check at runtime for the availability of the extension */
    if(!XRRQueryExtension(fgDisplay.pDisplay.Display, &event_base, &error_base)) {
        return -1;
    }

    XRRQueryVersion(fgDisplay.pDisplay.Display, &ver_major, &ver_minor);

    /* we only heed the rate if we CAN actually use it (Xrandr >= 1.1) and
     * the user actually cares about it (rate > 0)
     */
    use_rate = ( rate > 0 ) && ( ( ver_major > 1 ) ||
		                         ( ( ver_major == 1 ) && ( ver_minor >= 1 ) ) );

    /* this loop is only so that the whole thing will be repeated if someone
     * else changes video mode between our query of the current information and
     * the attempt to change it.
     */
    do {
        XRRScreenSize *ssizes;
        short *rates;
        Rotation rot;
        int i, ssizes_count, rates_count, curr, res_idx = -1;
        Time timestamp, cfg_timestamp;

        if(xrr_config) {
            XRRFreeScreenConfigInfo(xrr_config);
        }

        if(!(xrr_config = XRRGetScreenInfo(fgDisplay.pDisplay.Display, fgDisplay.pDisplay.RootWindow))) {
            fgWarning("XRRGetScreenInfo failed");
            break;
        }
        ssizes = XRRConfigSizes(xrr_config, &ssizes_count);
        curr = XRRConfigCurrentConfiguration(xrr_config, &rot);
        timestamp = XRRConfigTimes(xrr_config, &cfg_timestamp);

        /* if either of xsz or ysz are unspecified, use the current values */
        if(xsz <= 0)
            xsz = fgState.GameModeSize.X = ssizes[curr].width;
        if(ysz <= 0)
            ysz = fgState.GameModeSize.Y = ssizes[curr].height;


        if(xsz == ssizes[curr].width && ysz == ssizes[curr].height) {
            /* no need to switch, we're already in the requested resolution */
            res_idx = curr;
        } else {
            for(i=0; i<ssizes_count; i++) {
                if(ssizes[i].width == xsz && ssizes[i].height == ysz) {
                    res_idx = i;
                    break;  /* found it */
                }
            }
        }
        if(res_idx == -1)
            break;  /* no matching resolution */

#if ( RANDR_MAJOR > 1 ) || ( ( RANDR_MAJOR == 1 ) && ( RANDR_MINOR >= 1 ) )
        if(use_rate) {
            rate = fgState.GameModeRefresh;

            /* for the selected resolution, let's find out if there is
             * a matching refresh rate available.
             */
            rates = XRRConfigRates(xrr_config, res_idx, &rates_count);

            for(i=0; i<rates_count; i++) {
                if(rates[i] == rate) {
                    break;
                }
            }
            if(i == rates_count) {
                break; /* no matching rate */
            }
        }
#endif

        if(just_checking) {
            result = 0;
            break;
        }

#if ( RANDR_MAJOR > 1 ) || ( ( RANDR_MAJOR == 1 ) && ( RANDR_MINOR >= 1 ) )
        if(use_rate)
            result = XRRSetScreenConfigAndRate(fgDisplay.pDisplay.Display, xrr_config,
                    fgDisplay.pDisplay.RootWindow, res_idx, rot, rate, timestamp);
        else
#endif
            result = XRRSetScreenConfig(fgDisplay.pDisplay.Display, xrr_config,
                    fgDisplay.pDisplay.RootWindow, res_idx, rot, timestamp);

    } while(result == RRSetConfigInvalidTime);

    if(xrr_config) {
        XRRFreeScreenConfigInfo(xrr_config);
    }

    if(result == 0) {
        return 0;
    }

    return -1;
}
#endif  /* HAVE_X11_EXTENSIONS_XRANDR_H */

/*
 * Remembers the current visual settings, so that
 * we can change them and restore later...
 */
void fgPlatformRememberState( void )
{
#   if defined(HAVE_X11_EXTENSIONS_XRANDR_H) | defined(HAVE_X11_EXTENSIONS_XF86VMODE_H)
    int event_base, error_base;
#   endif

    /*
     * Remember the current pointer location before going fullscreen
     * for restoring it later:
     */
    Window junk_window;
    unsigned int junk_mask;

    XQueryPointer(fgDisplay.pDisplay.Display, fgDisplay.pDisplay.RootWindow,
            &junk_window, &junk_window,
            &fgDisplay.pDisplay.DisplayPointerX, &fgDisplay.pDisplay.DisplayPointerY,
            &fgDisplay.pDisplay.DisplayPointerX, &fgDisplay.pDisplay.DisplayPointerY, &junk_mask);

#   ifdef HAVE_X11_EXTENSIONS_XRANDR_H
    if(XRRQueryExtension(fgDisplay.pDisplay.Display, &event_base, &error_base)) {
        XRRScreenConfiguration *xrr_config;
        XRRScreenSize *ssizes;
        Rotation rot;
        int ssize_count, curr;

        if((xrr_config = XRRGetScreenInfo(fgDisplay.pDisplay.Display, fgDisplay.pDisplay.RootWindow))) {
            ssizes = XRRConfigSizes(xrr_config, &ssize_count);
            curr = XRRConfigCurrentConfiguration(xrr_config, &rot);

            fgDisplay.pDisplay.prev_xsz = ssizes[curr].width;
            fgDisplay.pDisplay.prev_ysz = ssizes[curr].height;
            fgDisplay.pDisplay.prev_refresh = -1;

#       if ( RANDR_MAJOR > 1 ) || ( ( RANDR_MAJOR == 1 ) && ( RANDR_MINOR >= 1 ) )
            if(fgState.GameModeRefresh != -1) {
                fgDisplay.pDisplay.prev_refresh = XRRConfigCurrentRate(xrr_config);
            }
#       endif

            fgDisplay.pDisplay.prev_size_valid = 1;

            XRRFreeScreenConfigInfo(xrr_config);
        }
    }
#   endif

    /*
     * This highly depends on the XFree86 extensions,
     * not approved as X Consortium standards
     */
#   ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
    if(!XF86VidModeQueryExtension(fgDisplay.pDisplay.Display, &event_base, &error_base)) {
        return;
    }

    /*
     * Remember the current ViewPort location of the screen to be able to
     * restore the ViewPort on LeaveGameMode():
     */
    if( !XF86VidModeGetViewPort(
             fgDisplay.pDisplay.Display,
             fgDisplay.pDisplay.Screen,
             &fgDisplay.pDisplay.DisplayViewPortX,
             &fgDisplay.pDisplay.DisplayViewPortY ) )
        fgWarning( "XF86VidModeGetViewPort failed" );


    /* Query the current display settings: */
    fgDisplay.pDisplay.DisplayModeValid =
      XF86VidModeGetModeLine(
        fgDisplay.pDisplay.Display,
        fgDisplay.pDisplay.Screen,
        &fgDisplay.pDisplay.DisplayModeClock,
        &fgDisplay.pDisplay.DisplayMode
    );

    if( !fgDisplay.pDisplay.DisplayModeValid )
        fgWarning( "XF86VidModeGetModeLine failed" );
#   endif

}

/*
 * Restores the previously remembered visual settings
 */
void fgPlatformRestoreState( void )
{
    /* Restore the remembered pointer position: */
    XWarpPointer(
        fgDisplay.pDisplay.Display, None, fgDisplay.pDisplay.RootWindow, 0, 0, 0, 0,
        fgDisplay.pDisplay.DisplayPointerX, fgDisplay.pDisplay.DisplayPointerY
    );


#   ifdef HAVE_X11_EXTENSIONS_XRANDR_H
    if(fgDisplay.pDisplay.prev_size_valid) {
        if(xrandr_resize(fgDisplay.pDisplay.prev_xsz, fgDisplay.pDisplay.prev_ysz, fgDisplay.pDisplay.prev_refresh, 0) != -1) {
            fgDisplay.pDisplay.prev_size_valid = 0;
#       ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
            fgDisplay.pDisplay.DisplayModeValid = 0;
#       endif
            return;
        }
    }
#   endif



#   ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
    /*
     * This highly depends on the XFree86 extensions,
     * not approved as X Consortium standards
     */

    if( fgDisplay.pDisplay.DisplayModeValid )
    {
        XF86VidModeModeInfo** displayModes;
        int i, displayModesCount;

        if( !XF86VidModeGetAllModeLines(
                 fgDisplay.pDisplay.Display,
                 fgDisplay.pDisplay.Screen,
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
            if(displayModes[ i ]->hdisplay == fgDisplay.pDisplay.DisplayMode.hdisplay &&
               displayModes[ i ]->vdisplay == fgDisplay.pDisplay.DisplayMode.vdisplay &&
               displayModes[ i ]->dotclock == fgDisplay.pDisplay.DisplayModeClock )
            {
                if( !XF86VidModeSwitchToMode(
                         fgDisplay.pDisplay.Display,
                         fgDisplay.pDisplay.Screen,
                         displayModes[ i ] ) )
                {
                    fgWarning( "XF86VidModeSwitchToMode failed" );
                    break;
                }

                if( !XF86VidModeSetViewPort(
                         fgDisplay.pDisplay.Display,
                         fgDisplay.pDisplay.Screen,
                         fgDisplay.pDisplay.DisplayViewPortX,
                         fgDisplay.pDisplay.DisplayViewPortY ) )
                    fgWarning( "XF86VidModeSetViewPort failed" );


                /*
                 * For the case this would be the last X11 call the application
                 * calls exit() we've to flush the X11 output queue to have the
                 * commands sent to the X server before the application exits.
                 */
                XFlush( fgDisplay.pDisplay.Display );

                fgDisplay.pDisplay.DisplayModeValid = 0;
#       ifdef HAVE_X11_EXTENSIONS_XRANDR_H
                fgDisplay.pDisplay.prev_size_valid = 0;
#       endif

                break;
            }
        }
        XFree( displayModes );
    }

#   endif

}

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

/*
 * Changes the current display mode to match user's settings
 */
GLboolean fgPlatformChangeDisplayMode( GLboolean haveToTest )
{
    GLboolean success = GL_FALSE;
    /* first try to use XRandR, then fallback to XF86VidMode */
#   ifdef HAVE_X11_EXTENSIONS_XRANDR_H
    if(xrandr_resize(fgState.GameModeSize.X, fgState.GameModeSize.Y,
                fgState.GameModeRefresh, haveToTest) != -1) {
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
    if( haveToTest || fgDisplay.pDisplay.DisplayModeValid )
    {
        XF86VidModeModeInfo** displayModes;
        int i, displayModesCount;

        /* If we don't have a valid modeline in the display structure, which
         * can happen if this is called from glutGameModeGet instead of
         * glutEnterGameMode, then we need to query the current mode, to make
         * unspecified settings to default to their current values.
         */
        if(!fgDisplay.pDisplay.DisplayModeValid) {
            if(!XF86VidModeGetModeLine(fgDisplay.pDisplay.Display, fgDisplay.pDisplay.Screen,
                    &fgDisplay.pDisplay.DisplayModeClock, &fgDisplay.pDisplay.DisplayMode)) {
                return success;
            }
        }

        if (fgState.GameModeSize.X == -1)
        {
            fgState.GameModeSize.X = fgDisplay.pDisplay.DisplayMode.hdisplay;
        }
        if (fgState.GameModeSize.Y == -1)
        {
            fgState.GameModeSize.Y = fgDisplay.pDisplay.DisplayMode.vdisplay;
        }
        if (fgState.GameModeDepth == -1)
        {
            /* can't get color depth from this, nor can we change it, do nothing
             * TODO: get with XGetVisualInfo()? but then how to set?
             */
        }
        if (fgState.GameModeRefresh == -1)
        {
            /* Compute the displays refresh rate, dotclock comes in kHz. */
            int refresh = ( fgDisplay.pDisplay.DisplayModeClock * 1000 ) /
                ( fgDisplay.pDisplay.DisplayMode.htotal * fgDisplay.pDisplay.DisplayMode.vtotal );

            fgState.GameModeRefresh = refresh;
        }

        /* query all possible display modes */
        if( !XF86VidModeGetAllModeLines(
                 fgDisplay.pDisplay.Display,
                 fgDisplay.pDisplay.Screen,
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
                     fgDisplay.pDisplay.Display,
                     fgDisplay.pDisplay.Screen,
                     displayModes[ i ] ) )
                fgWarning( "XF86VidModeSwitchToMode failed" );
        }

        XFree( displayModes );
    }

#   endif

    return success;
}


void fgPlatformEnterGameMode( void ) 
{

    /*
     * Sync needed to avoid a real race, the Xserver must have really created
     * the window before we can grab the pointer into it:
     */
    XSync( fgDisplay.pDisplay.Display, False );
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
               fgDisplay.pDisplay.Display, fgStructure.GameModeWindow->Window.Handle,
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
        fgDisplay.pDisplay.Display,
        fgStructure.GameModeWindow->Window.Handle,
        RevertToNone,
        CurrentTime
    );

    /* Move the Pointer to the middle of the fullscreen window */
    XWarpPointer(
        fgDisplay.pDisplay.Display,
        None,
        fgDisplay.pDisplay.RootWindow,
        0, 0, 0, 0,
        fgState.GameModeSize.X/2, fgState.GameModeSize.Y/2
    );

#   ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H

    if( fgDisplay.pDisplay.DisplayModeValid )
    {
        int x, y;
        Window child;

        /* Change to viewport to the window topleft edge: */
        if( !XF86VidModeSetViewPort( fgDisplay.pDisplay.Display, fgDisplay.pDisplay.Screen, 0, 0 ) )
            fgWarning( "XF86VidModeSetViewPort failed" );

        /*
         * Final window repositioning: It could be avoided using an undecorated
         * window using override_redirect, but this * would possily require
         * more changes and investigation.
         */

        /* Get the current postion of the drawable area on screen */
        XTranslateCoordinates(
            fgDisplay.pDisplay.Display,
            fgStructure.CurrentWindow->Window.Handle,
            fgDisplay.pDisplay.RootWindow,
            0, 0, &x, &y,
            &child
        );

        /* Move the decorataions out of the topleft corner of the display */
        XMoveWindow( fgDisplay.pDisplay.Display, fgStructure.CurrentWindow->Window.Handle,
                     -x, -y);
    }

#endif

    /* Grab the keyboard, too */
    XGrabKeyboard(
        fgDisplay.pDisplay.Display,
        fgStructure.GameModeWindow->Window.Handle,
        FALSE,
        GrabModeAsync, GrabModeAsync,
        CurrentTime
    );

}

void fgPlatformLeaveGameMode( void ) 
{
    XUngrabPointer( fgDisplay.pDisplay.Display, CurrentTime );
    XUngrabKeyboard( fgDisplay.pDisplay.Display, CurrentTime );
}

