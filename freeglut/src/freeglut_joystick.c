/*
 * freeglut_joystick.c
 *
 * Joystick handling code
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Steve Baker, <sjbaker1@airmail.net>
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

/*
 * PWO: This is not exactly what Steve Baker has done for PLIB, as I had to
 *      convert it from C++ to C. And I've also reformatted it a bit (that's
 *      my little personal deviation :]) I don't really know if it is still
 *      portable...
 *      Steve: could you please add some comments to the code? :)
 *
 * FreeBSD port by Stephen Montgomery-Smith <stephen@math.missouri.edu>
 */

#if defined( __FreeBSD__ ) || defined( __NetBSD__ )
#include <sys/param.h>
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../include/GL/freeglut.h"
#include "freeglut_internal.h"

/*
 * PWO: I don't like it at all. It's a mess. Could it be cleared?
 */
#ifdef WIN32
#   include <windows.h>
#   if defined( __CYGWIN32__ ) || defined( __CYGWIN__ )
#       define NEAR /* */
#       define FAR  /* */
#   endif
#   include <mmsystem.h>
#   include <string.h>
#else
#   include <unistd.h>
#   include <fcntl.h>
#   if defined(__FreeBSD__) || defined(__NetBSD__)
#   if __FreeBSD_version >= 500000
#       include <sys/joystick.h>
#   else
#       include <machine/joystick.h>
#   endif
#       define JS_DATA_TYPE joystick
#       define JS_RETURN (sizeof(struct JS_DATA_TYPE))
#   elif defined(__linux__)
#       include <sys/ioctl.h>
#       include <linux/joystick.h>
#       include <errno.h>

        /*
         * Check the joystick driver version
         */
#       ifdef JS_VERSION
#           if JS_VERSION >= 0x010000
#               define JS_NEW
#           endif
#       endif
#   else
#       ifndef JS_DATA_TYPE

            /*
             * Not Windoze and no (known) joystick driver...
             *
             * Well - we'll put these values in and that should
             * allow the code to at least compile. The JS open
             * routine should error out and shut off all the code
             * downstream anyway
             */
            struct JS_DATA_TYPE
            {
                int buttons;
                int x;
                int y;
            };

#           define JS_RETURN (sizeof(struct JS_DATA_TYPE))
#       endif
#   endif
#endif

#ifdef WIN32
#   define _JS_MAX_AXES 6
#else
#   ifdef __FreeBSD__
#       define _JS_MAX_AXES 2
#   else
#       define _JS_MAX_AXES 6
#   endif
#endif

typedef struct tagSFG_Joystick SFG_Joystick;
struct tagSFG_Joystick
{
/*
 * XXX All BSDs might share this?
 */
#ifdef __FreeBSD__
    int         id;
#endif

#ifdef WIN32
    JOYINFOEX   js;
    UINT        js_id;
#else
#   ifdef JS_NEW
        struct js_event js;
        int         tmp_buttons;
        float       tmp_axes[ _JS_MAX_AXES ];
#   else
        struct JS_DATA_TYPE js;
#   endif

    char fname[ 128 ];
    int  fd;
#endif

    GLboolean error;
    int       num_axes;
    int       num_buttons;

    float dead_band[ _JS_MAX_AXES ];
    float saturate [ _JS_MAX_AXES ];
    float center   [ _JS_MAX_AXES ];
    float max      [ _JS_MAX_AXES ];
    float min      [ _JS_MAX_AXES ];
};

/*
 * The static joystick structure pointer
 */
static SFG_Joystick* fgJoystick = NULL;

/*
 * Read the raw joystick data
 */
static void fghJoystickRawRead( SFG_Joystick* joy, int* buttons, float* axes )
{
#ifdef WIN32
    MMRESULT status;
#else
    int status;
#endif

    int i;

    if( joy->error )
    {
        if( buttons )
            *buttons = 0;

        if( axes )
            for( i=0; i<joy->num_axes; i++ )
                axes[ i ] = 1500.0f;

        return;
    }

#ifdef WIN32
    status = joyGetPosEx( joy->js_id, &joy->js );

    if( status != JOYERR_NOERROR )
    {
        joy->error = GL_TRUE;
        return;
    }

    if( buttons )
        *buttons = joy->js.dwButtons;

    if( axes )
    {
        /*
         * WARNING - Fall through case clauses!!
         */
        switch( joy->num_axes )
        {
        case 6: axes[5] = (float) joy->js.dwVpos;
        case 5: axes[4] = (float) joy->js.dwUpos;
        case 4: axes[3] = (float) joy->js.dwRpos;
        case 3: axes[2] = (float) joy->js.dwZpos;
        case 2: axes[1] = (float) joy->js.dwYpos;
        case 1: axes[0] = (float) joy->js.dwXpos;
        }
    }
#else
#   ifdef JS_NEW

    while( 1 )
    {
        status = read( joy->fd, &joy->js, sizeof(struct js_event) );

        if( status != sizeof( struct js_event ) )
        {
            if( errno == EAGAIN )
            {
                /*
                 * Use the old values
                 */
                if( buttons )
                    *buttons = joy->tmp_buttons;
                if( axes )
                    memcpy( axes, joy->tmp_axes,
                            sizeof( float ) * joy->num_axes );
                return;
            }

            fgWarning( "%s", joy->fname );
            joy->error = GL_TRUE;
            return;
        }

        switch( joy->js.type & ~JS_EVENT_INIT )
        {
        case JS_EVENT_BUTTON:
            if( joy->js.value == 0 ) /* clear the flag */
                joy->tmp_buttons &= ~( 1 << joy->js.number );
            else
                joy->tmp_buttons |= ( 1 << joy->js.number );
            break;

        case JS_EVENT_AXIS:
            joy->tmp_axes[ joy->js.number ] = ( float )joy->js.value;
            
            if( axes )
                memcpy( axes, joy->tmp_axes, sizeof(float) * joy->num_axes );
            break;
        }

        if( buttons )
            *buttons = joy->tmp_buttons;
    }
#   else

    status = read( joy->fd, &joy->js, JS_RETURN );

    if( status != JS_RETURN )
    {
        fgWarning( "%s", joy->fname );
        joy->error = GL_TRUE;
        return;
    }

    if( buttons )
#       if defined( __FreeBSD__ ) || defined( __NetBSD__ )
        *buttons = ( joy->js.b1 ? 1 : 0 ) | ( joy->js.b2 ? 2 : 0 );
#       else
        *buttons = joy->js.buttons;
#       endif

    if( axes )
    {
        axes[ 0 ] = (float) joy->js.x;
        axes[ 1 ] = (float) joy->js.y;
    }
#   endif
#endif
}

/*
 * Correct the joystick axis data
 */
static float fghJoystickFudgeAxis( SFG_Joystick* joy, float value, int axis )
{
    if( value < joy->center[ axis ] )
    {
        float xx = ( value - joy->center[ axis ] ) / ( joy->center[ axis ] -
                                                       joy->min[ axis ] );

        if( xx < -joy->saturate[ axis ] )
            return -1.0f;

        if( xx > -joy->dead_band [ axis ] )
            return 0.0f;

        xx = ( xx + joy->dead_band[ axis ] ) / ( joy->saturate[ axis ] -
                                                 joy->dead_band[ axis ] );

        return ( xx < -1.0f ) ? -1.0f : xx;
    }
    else
    {
        float xx = ( value - joy->center [ axis ] ) / ( joy->max[ axis ] -
                                                        joy->center[ axis ] );

        if( xx > joy->saturate[ axis ] )
            return 1.0f;

        if( xx < joy->dead_band[ axis ] )
            return 0.0f;

        xx = ( xx - joy->dead_band[ axis ] ) / ( joy->saturate[ axis ] -
                                                 joy->dead_band[ axis ] );

        return ( xx > 1.0f ) ? 1.0f : xx;
    }
}

/*
 * Read the corrected joystick data
 */
static void fghJoystickRead( SFG_Joystick* joy, int* buttons, float* axes )
{
    float raw_axes[ _JS_MAX_AXES ];
    int  i;

    if( joy->error )
    {
        if( buttons )
            *buttons = 0;

        if( axes )
            for ( i=0; i<joy->num_axes ; i++ )
                axes[ i ] = 0.0f ;
    }

    fghJoystickRawRead( joy, buttons, raw_axes );

    if( axes )
        for( i=0 ; i<joy->num_axes ; i++ )
            axes[ i ] = fghJoystickFudgeAxis( joy, raw_axes[ i ], i );
}

/*
 * Happy happy happy joy joy joy (happy new year toudi :D)
 */
static void fghJoystickOpen( SFG_Joystick* joy )
{
#ifdef WIN32
    JOYCAPS jsCaps;
    int     i;

    joy->js.dwFlags = JOY_RETURNALL;
    joy->js.dwSize  = sizeof( joy->js );

    memset( &jsCaps, 0, sizeof( jsCaps ) );

    joy->error =
        ( joyGetDevCaps( joy->js_id, &jsCaps, sizeof( jsCaps ) ) !=
          JOYERR_NOERROR );
    joy->num_axes =
        ( jsCaps.wNumAxes < _JS_MAX_AXES ) ? jsCaps.wNumAxes : _JS_MAX_AXES;

    /*
     * WARNING - Fall through case clauses!!
     */
    switch( joy->num_axes )
    {
    case 6:
        joy->min[ 5 ] = (float) jsCaps.wVmin;
        joy->max[ 5 ] = (float) jsCaps.wVmax;
    case 5:
        joy->min[ 4 ] = (float) jsCaps.wUmin;
        joy->max[ 4 ] = (float) jsCaps.wUmax;
    case 4:
        joy->min[ 3 ] = (float) jsCaps.wRmin;
        joy->max[ 3 ] = (float) jsCaps.wRmax;
    case 3:
        joy->min[ 2 ] = (float) jsCaps.wZmin;
        joy->max[ 2 ] = (float) jsCaps.wZmax;
    case 2:
        joy->min[ 1 ] = (float) jsCaps.wYmin;
        joy->max[ 1 ] = (float) jsCaps.wYmax;
    case 1:
        joy->min[ 0 ] = (float) jsCaps.wXmin;
        joy->max[ 0 ] = (float) jsCaps.wXmax;
        break;

    /*
     * I guess we have no axes at all
     */
    default:
        joy->error = GL_TRUE;
        break;
    }

    /*
     * Guess all the rest judging on the axes extremals
     */
    for( i=0 ; i<joy->num_axes ; i++ )
    {
        joy->center   [ i ] = (joy->max[i] + joy->min[i]) * 0.5f;
        joy->dead_band[ i ] = 0.0f;
        joy->saturate [ i ] = 1.0f;
    }

#else
#   ifdef __FreeBSD__
    int   buttons[ _JS_MAX_AXES ];
    float axes[ _JS_MAX_AXES ];
    int   noargs, in_no_axes;
    char  joyfname[ 1024 ];
    FILE* joyfile;
#   else
#       ifndef JS_NEW
    int counter;
#       endif
#   endif
    int i;

    /*
     * Default for older Linux systems.
     */
    joy->num_axes    =  2;
    joy->num_buttons = 32;

#   ifdef JS_NEW
    for( i=0 ; i<_JS_MAX_AXES ; i++ )
        joy->tmp_axes[ i ] = 0.0f ;

    joy->tmp_buttons = 0 ;
#   endif

    joy->fd = open( joy->fname, O_RDONLY );

    joy->error = (joy->fd < 0);

    if( joy->error )
        return;
/*
 * XXX All BSDs should share this?
 */
#   ifdef __FreeBSD__
    fghJoystickRawRead(joy, buttons, axes );
    joy->error = axes[ 0 ] < -1000000000.0f;
    if( joy->error )
        return;

    sprintf( joyfname, "%s/.joy%drc", getenv( "HOME" ), joy->id );

    joyfile = fopen( joyfname, "r" );
    joy->error =( joyfile == NULL );
    if( joy->error )
      return;

    noargs = fscanf(
        joyfile, "%d%f%f%f%f%f%f",
        &in_no_axes,
        &joy->min[ 0 ], &joy->center[ 0 ], &joy->max[ 0 ],
        &joy->min[ 1 ], &joy->center[ 1 ], &joy->max[ 1 ]
    );

    joy->error =( noargs != 7 ) || ( in_no_axes != _JS_MAX_AXES );
    fclose( joyfile );
    if( joy->error )
        return;

    for( i = 0; i < _JS_MAX_AXES; i++ )
    {
        joy->dead_band[ i ] = 0.0f;
        joy->saturate [ i ] = 1.0f;
    }
#   else

    /*
     * Set the correct number of axes for the linux driver
     */
#       ifdef JS_NEW

    ioctl( joy->fd, JSIOCGAXES   , &joy->num_axes    );
    ioctl( joy->fd, JSIOCGBUTTONS, &joy->num_buttons );
    fcntl( joy->fd, F_SETFL, O_NONBLOCK );

#       endif

    /*
     * The Linux driver seems to return 512 for all axes
     * when no stick is present - but there is a chance
     * that could happen by accident - so it's gotta happen
     * on both axes for at least 100 attempts.
     *
     * PWO: shouldn't be that done somehow wiser on the kernel level?
     */
#       ifndef JS_NEW
    counter = 0;

    do
    { 
        fghJoystickRawRead( joy, NULL, joy->center );
        counter++;
    } while( !joy->error &&
             counter < 100 &&
             joy->center[ 0 ] == 512.0f &&
             joy->center[ 1 ] == 512.0f );
   
    if( counter >= 100 )
        joy->error = GL_TRUE;
#       endif

    for( i = 0; i < _JS_MAX_AXES; i++ )
    {
#       ifdef JS_NEW
        joy->max   [ i ] =  32767.0f;
        joy->center[ i ] =      0.0f;
        joy->min   [ i ] = -32767.0f;
#       else
        joy->max[ i ] = joy->center[ i ] * 2.0f;
        joy->min[ i ] = 0.0f;
#       endif
        joy->dead_band[ i ] = 0.0f ;
        joy->saturate [ i ] = 1.0f ;
    }
#   endif
#endif
}

/*
 *
 */
void fgJoystickInit( int ident )
{
    if( fgJoystick )
        fgError( "illegal attemp to initialize joystick device" );

    fgJoystick = ( SFG_Joystick * )calloc( sizeof( SFG_Joystick ), 1 );

#ifdef WIN32
    switch( ident )
    {
    case 0:
        fgJoystick->js_id = JOYSTICKID1;
        fghJoystickOpen( fgJoystick );
        break;
    case 1:
        fgJoystick->js_id = JOYSTICKID2;
        fghJoystickOpen( fgJoystick );
        break;
    default:
        fgJoystick->num_axes = 0;
        fgJoystick->error = GL_TRUE;
        break;
    }
#else

/*
 * XXX All BSDs should share this code?
 */
#   ifdef __FreeBSD__
    fgJoystick->id = ident;
    sprintf( fgJoystick->fname, "/dev/joy%d", ident );
#   else
    sprintf( fgJoystick->fname, "/dev/js%d", ident );
#   endif

    fghJoystickOpen( fgJoystick );
#endif
}

/*
 *
 */
void fgJoystickClose( void )
{
    if( !fgJoystick )
        fgError( "illegal attempt to deinitialize joystick device" );

#ifndef WIN32
    if( ! fgJoystick->error )
        close( fgJoystick->fd );
#endif

    free( fgJoystick );
    fgJoystick = NULL;  /* show joystick has been deinitialized */
}

/*
 * Polls the joystick and executes the joystick callback hooked to the
 * window specified in the function's parameter:
 */
void fgJoystickPollWindow( SFG_Window* window )
{
    float axes[ _JS_MAX_AXES ];
    int buttons;

    freeglut_return_if_fail( fgJoystick );
    freeglut_return_if_fail( window );
    freeglut_return_if_fail( FETCH_WCB( *window, Joystick ) );

    fghJoystickRead( fgJoystick, &buttons, axes );

    INVOKE_WCB( *window, Joystick,
                ( buttons,
                  (int) (axes[ 0 ] * 1000.0f ),
                  (int) (axes[ 1 ] * 1000.0f ),
                  (int) (axes[ 2 ] * 1000.0f ) )
    );
    
    /*
     * fgSetWindow (window);
     * window->Callbacks.Joystick(
     *   buttons,
     *   (int) (axes[ 0 ] * 1000.0f),
     *   (int) (axes[ 1 ] * 1000.0f),
     *   (int) (axes[ 2 ] * 1000.0f)
     * );
     */
}

/*
 * PWO: These jsJoystick class methods have not been implemented.
 *      We might consider adding such functions to freeglut-2.0.
 */
#if 0
int  getNumAxes ()
    { return num_axes; }
int  notWorking ()
    { return error; }

float getDeadBand ( int axis )
    { return dead_band [ axis ]; }
void  setDeadBand ( int axis, float db )
    { dead_band [ axis ] = db; }

float getSaturation ( int axis )
    { return saturate [ axis ]; }
void  setSaturation ( int axis, float st )
    { saturate [ axis ] = st; }

void setMinRange ( float *axes )
    { memcpy ( min   , axes, num_axes * sizeof(float) ); }
void setMaxRange ( float *axes )
    { memcpy ( max   , axes, num_axes * sizeof(float) ); }
void setCenter   ( float *axes )
    { memcpy ( center, axes, num_axes * sizeof(float) ); }

void getMinRange ( float *axes )
    { memcpy ( axes, min   , num_axes * sizeof(float) ); }
void getMaxRange ( float *axes )
    { memcpy ( axes, max   , num_axes * sizeof(float) ); }
void getCenter   ( float *axes )
    { memcpy ( axes, center, num_axes * sizeof(float) ); }
#endif

/*** END OF FILE ***/
