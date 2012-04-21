/*
 * freeglut_joystick_x11.c
 *
 * Joystick handling code
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Steve Baker, <sjbaker1@airmail.net>
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

/*
 * FreeBSD port by Stephen Montgomery-Smith <stephen@math.missouri.edu>
 *
 * Redone by John Fay 2/4/04 with another look from the PLIB "js" library.
 *  Many thanks for Steve Baker for permission to pull from that library.
 */

#include <GL/freeglut.h>
#include "../fg_internal.h"
#ifdef HAVE_SYS_PARAM_H
#    include <sys/param.h>
#endif


/*this should be defined in a header file */
#define MAX_NUM_JOYSTICKS  2   
extern SFG_Joystick *fgJoystick [ MAX_NUM_JOYSTICKS ];

void fgPlatformJoystickRawRead( SFG_Joystick* joy, int* buttons, float* axes )
{
    int status;

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
    int len;

    if ( joy->pJoystick.os->is_analog )
    {
        int status = read ( joy->pJoystick.os->fd, &joy->pJoystick.os->ajs, sizeof(joy->pJoystick.os->ajs) );
        if ( status != sizeof(joy->pJoystick.os->ajs) ) {
            perror ( joy->pJoystick.os->fname );
            joy->error = GL_TRUE;
            return;
        }
        if ( buttons != NULL )
            *buttons = ( joy->pJoystick.os->ajs.b1 ? 1 : 0 ) | ( joy->pJoystick.os->ajs.b2 ? 2 : 0 );

        if ( axes != NULL )
        {
            axes[0] = (float) joy->pJoystick.os->ajs.x;
            axes[1] = (float) joy->pJoystick.os->ajs.y;
        }

        return;
    }

#  ifdef HAVE_USB_JS
    while ( ( len = read ( joy->pJoystick.os->fd, joy->pJoystick.os->hid_data_buf, joy->pJoystick.os->hid_dlen ) ) == joy->pJoystick.os->hid_dlen )
    {
        struct hid_item *h;

        for  ( h = joy->pJoystick.os->hids; h; h = h->next )
        {
            int d = hid_get_data ( joy->pJoystick.os->hid_data_buf, h );

            int page = HID_PAGE ( h->usage );
            int usage = HID_USAGE ( h->usage );

            if ( page == HUP_GENERIC_DESKTOP )
            {
                int i;
                for ( i = 0; i < joy->num_axes; i++ )
                    if (joy->pJoystick.os->axes_usage[i] == usage)
                    {
                        if (usage == HUG_HAT_SWITCH)
                        {
                            if (d < 0 || d > 8)
                                d = 0;  /* safety */
                            joy->pJoystick.os->cache_axes[i] = (float)hatmap_x[d];
                            joy->pJoystick.os->cache_axes[i + 1] = (float)hatmap_y[d];
                        }
                        else
                        {
                            joy->pJoystick.os->cache_axes[i] = (float)d;
                        }
                        break;
                    }
            }
            else if (page == HUP_BUTTON)
            {
               if (usage > 0 && usage < _JS_MAX_BUTTONS + 1)
               {
                   if (d)
                       joy->pJoystick.os->cache_buttons |=  (1 << ( usage - 1 ));
                   else
                       joy->pJoystick.os->cache_buttons &= ~(1 << ( usage - 1 ));
               }
            }
        }
    }
#    ifdef HAVE_ERRNO_H
    if ( len < 0 && errno != EAGAIN )
#    else
    if ( len < 0 )
#    endif
    {
        perror( joy->pJoystick.os->fname );
        joy->error = 1;
    }
    if ( buttons != NULL ) *buttons = joy->pJoystick.os->cache_buttons;
    if ( axes    != NULL )
        memcpy ( axes, joy->pJoystick.os->cache_axes, sizeof(float) * joy->num_axes );
#  endif
#endif

#ifdef JS_NEW

    while ( 1 )
    {
        status = read ( joy->pJoystick.fd, &joy->pJoystick.js, sizeof(struct js_event) );

        if ( status != sizeof( struct js_event ) )
        {
#  ifdef HAVE_ERRNO_H
            if ( errno == EAGAIN )
            {
                /* Use the old values */
                if ( buttons )
                    *buttons = joy->pJoystick.tmp_buttons;
                if ( axes )
                    memcpy( axes, joy->pJoystick.tmp_axes,
                            sizeof( float ) * joy->num_axes );
                return;
            }
#  endif

            fgWarning ( "%s", joy->pJoystick.fname );
            joy->error = GL_TRUE;
            return;
        }

        switch ( joy->pJoystick.js.type & ~JS_EVENT_INIT )
        {
        case JS_EVENT_BUTTON:
            if( joy->pJoystick.js.value == 0 ) /* clear the flag */
                joy->pJoystick.tmp_buttons &= ~( 1 << joy->pJoystick.js.number );
            else
                joy->pJoystick.tmp_buttons |= ( 1 << joy->pJoystick.js.number );
            break;

        case JS_EVENT_AXIS:
            if ( joy->pJoystick.js.number < joy->num_axes )
            {
                joy->pJoystick.tmp_axes[ joy->pJoystick.js.number ] = ( float )joy->pJoystick.js.value;

                if( axes )
                    memcpy( axes, joy->pJoystick.tmp_axes, sizeof(float) * joy->num_axes );
            }
            break;

        default:
            fgWarning ( "PLIB_JS: Unrecognised /dev/js return!?!" );

            /* use the old values */

            if ( buttons != NULL ) *buttons = joy->pJoystick.tmp_buttons;
            if ( axes    != NULL )
                memcpy ( axes, joy->pJoystick.tmp_axes, sizeof(float) * joy->num_axes );

            return;
        }

        if( buttons )
            *buttons = joy->pJoystick.tmp_buttons;
    }
#else

    status = read( joy->pJoystick.fd, &joy->pJoystick.js, JS_RETURN );

    if ( status != JS_RETURN )
    {
        fgWarning( "%s", joy->pJoystick.fname );
        joy->error = GL_TRUE;
        return;
    }

    if ( buttons )
#    if defined( __FreeBSD__ ) || defined(__FreeBSD_kernel__) || defined( __NetBSD__ )
        *buttons = ( joy->pJoystick.js.b1 ? 1 : 0 ) | ( joy->pJoystick.js.b2 ? 2 : 0 );  /* XXX Should not be here -- BSD is handled earlier */
#    else
        *buttons = joy->pJoystick.js.buttons;
#    endif

    if ( axes )
    {
        axes[ 0 ] = (float) joy->pJoystick.js.x;
        axes[ 1 ] = (float) joy->pJoystick.js.y;
    }
#endif
}


void fgPlatformJoystickOpen( SFG_Joystick* joy )
{
#if defined( __FreeBSD__ ) || defined(__FreeBSD_kernel__) || defined( __NetBSD__ )
	int i = 0;
       char *cp;
#endif
#ifdef JS_NEW
       unsigned char u;
	int i=0;
#else
#  if defined( __linux__ ) || TARGET_HOST_SOLARIS
	int i = 0;
    int counter = 0;
#  endif
#endif

#if defined( __FreeBSD__ ) || defined(__FreeBSD_kernel__) || defined( __NetBSD__ )
    for( i = 0; i < _JS_MAX_AXES; i++ )
        joy->pJoystick.os->cache_axes[ i ] = 0.0f;

    joy->pJoystick.os->cache_buttons = 0;

    joy->pJoystick.os->fd = open( joy->pJoystick.os->fname, O_RDONLY | O_NONBLOCK);

#ifdef HAVE_ERRNO_H
    if( joy->pJoystick.os->fd < 0 && errno == EACCES )
        fgWarning ( "%s exists but is not readable by you", joy->pJoystick.os->fname );
#endif

    joy->error =( joy->pJoystick.os->fd < 0 );

    if( joy->error )
        return;

    joy->num_axes = 0;
    joy->num_buttons = 0;
    if( joy->pJoystick.os->is_analog )
    {
        FILE *joyfile;
        char joyfname[ 1024 ];
        int noargs, in_no_axes;

        float axes [ _JS_MAX_AXES ];
        int buttons[ _JS_MAX_AXES ];

        joy->num_axes    =  2;
        joy->num_buttons = 32;

        fghJoystickRawRead( joy, buttons, axes );
        joy->error = axes[ 0 ] < -1000000000.0f;
        if( joy->error )
            return;

        snprintf( joyfname, sizeof(joyfname), "%s/.joy%drc", getenv( "HOME" ), joy->id );

        joyfile = fopen( joyfname, "r" );
        joy->error =( joyfile == NULL );
        if( joy->error )
            return;

        noargs = fscanf( joyfile, "%d%f%f%f%f%f%f", &in_no_axes,
                         &joy->min[ 0 ], &joy->center[ 0 ], &joy->max[ 0 ],
                         &joy->min[ 1 ], &joy->center[ 1 ], &joy->max[ 1 ] );
        joy->error = noargs != 7 || in_no_axes != _JS_MAX_AXES;
        fclose( joyfile );
        if( joy->error )
            return;

        for( i = 0; i < _JS_MAX_AXES; i++ )
        {
            joy->dead_band[ i ] = 0.0f;
            joy->saturate [ i ] = 1.0f;
        }

        return;    /* End of analog code */
    }

#    ifdef HAVE_USB_JS
    if( ! fghJoystickInitializeHID( joy->pJoystick.os, &joy->num_axes,
                                    &joy->num_buttons ) )
    {
        close( joy->pJoystick.os->fd );
        joy->error = GL_TRUE;
        return;
    }

    cp = strrchr( joy->pJoystick.os->fname, '/' );
    if( cp )
    {
        if( fghJoystickFindUSBdev( &cp[1], joy->name, sizeof( joy->name ) ) ==
            0 )
            strcpy( joy->name, &cp[1] );
    }

    if( joy->num_axes > _JS_MAX_AXES )
        joy->num_axes = _JS_MAX_AXES;

    for( i = 0; i < _JS_MAX_AXES; i++ )
    {
        /* We really should get this from the HID, but that data seems
         * to be quite unreliable for analog-to-USB converters. Punt for
         * now.
         */
        if( joy->pJoystick.os->axes_usage[ i ] == HUG_HAT_SWITCH )
        {
            joy->max   [ i ] = 1.0f;
            joy->center[ i ] = 0.0f;
            joy->min   [ i ] = -1.0f;
        }
        else
        {
            joy->max   [ i ] = 255.0f;
            joy->center[ i ] = 127.0f;
            joy->min   [ i ] = 0.0f;
        }

        joy->dead_band[ i ] = 0.0f;
        joy->saturate[ i ] = 1.0f;
    }
#    endif
#endif

#if defined( __linux__ ) || TARGET_HOST_SOLARIS
    /* Default for older Linux systems. */
    joy->num_axes    =  2;
    joy->num_buttons = 32;

#    ifdef JS_NEW
    for( i = 0; i < _JS_MAX_AXES; i++ )
        joy->pJoystick.tmp_axes[ i ] = 0.0f;

    joy->pJoystick.tmp_buttons = 0;
#    endif

    joy->pJoystick.fd = open( joy->pJoystick.fname, O_RDONLY );

    joy->error =( joy->pJoystick.fd < 0 );

    if( joy->error )
        return;

    /* Set the correct number of axes for the linux driver */
#    ifdef JS_NEW
    /* Melchior Franz's fixes for big-endian Linuxes since writing
     *  to the upper byte of an uninitialized word doesn't work.
     *  9 April 2003
     */
    ioctl( joy->pJoystick.fd, JSIOCGAXES, &u );
    joy->num_axes = u;
    ioctl( joy->pJoystick.fd, JSIOCGBUTTONS, &u );
    joy->num_buttons = u;
    ioctl( joy->pJoystick.fd, JSIOCGNAME( sizeof( joy->name ) ), joy->name );
    fcntl( joy->pJoystick.fd, F_SETFL, O_NONBLOCK );
#    endif

    /*
     * The Linux driver seems to return 512 for all axes
     * when no stick is present - but there is a chance
     * that could happen by accident - so it's gotta happen
     * on both axes for at least 100 attempts.
     *
     * PWO: shouldn't be that done somehow wiser on the kernel level?
     */
#    ifndef JS_NEW
    counter = 0;

    do
    {
        fghJoystickRawRead( joy, NULL, joy->center );
        counter++;
    } while( !joy->error &&
             counter < 100 &&
             joy->center[ 0 ] == 512.0f &&
             joy->center[ 1 ] == 512.0f );

    if ( counter >= 100 )
        joy->error = GL_TRUE;
#    endif

    for( i = 0; i < _JS_MAX_AXES; i++ )
    {
#    ifdef JS_NEW
        joy->max   [ i ] =  32767.0f;
        joy->center[ i ] =      0.0f;
        joy->min   [ i ] = -32767.0f;
#    else
        joy->max[ i ] = joy->center[ i ] * 2.0f;
        joy->min[ i ] = 0.0f;
#    endif
        joy->dead_band[ i ] = 0.0f;
        joy->saturate [ i ] = 1.0f;
    }
#endif
}


void fgPlatformJoystickInit( SFG_Joystick *fgJoystick[], int ident )
{
#if defined( __FreeBSD__ ) || defined(__FreeBSD_kernel__) || defined( __NetBSD__ )
    fgJoystick[ ident ]->id = ident;
    fgJoystick[ ident ]->error = GL_FALSE;

    fgJoystick[ ident ]->pJoystick.os = calloc( 1, sizeof( struct os_specific_s ) );
    memset( fgJoystick[ ident ]->pJoystick.os, 0, sizeof( struct os_specific_s ) );
    if( ident < USB_IDENT_OFFSET )
        fgJoystick[ ident ]->pJoystick.os->is_analog = 1;
    if( fgJoystick[ ident ]->pJoystick.os->is_analog )
        snprintf( fgJoystick[ ident ]->pJoystick.os->fname, sizeof(fgJoystick[ ident ]->pJoystick.os->fname), "%s%d", AJSDEV, ident );
    else
        snprintf( fgJoystick[ ident ]->pJoystick.os->fname, sizeof(fgJoystick[ ident ]->pJoystick.os->fname), "%s%d", UHIDDEV,
                 ident - USB_IDENT_OFFSET );
#elif defined( __linux__ )
    fgJoystick[ ident ]->id = ident;
    fgJoystick[ ident ]->error = GL_FALSE;

    snprintf( fgJoystick[ident]->pJoystick.fname, sizeof(fgJoystick[ident]->pJoystick.fname), "/dev/input/js%d", ident );

    if( access( fgJoystick[ ident ]->pJoystick.fname, F_OK ) != 0 )
        snprintf( fgJoystick[ ident ]->pJoystick.fname, sizeof(fgJoystick[ ident ]->pJoystick.fname), "/dev/js%d", ident );
#endif
}


void fgPlatformJoystickClose ( int ident )
{
#if defined( __FreeBSD__ ) || defined(__FreeBSD_kernel__) || defined( __NetBSD__ )
    if( fgJoystick[ident]->pJoystick.os )
    {
        if( ! fgJoystick[ ident ]->error )
            close( fgJoystick[ ident ]->pJoystick.os->fd );
#ifdef HAVE_USB_JS
        if( fgJoystick[ ident ]->pJoystick.os->hids )
            free (fgJoystick[ ident ]->pJoystick.os->hids);
        if( fgJoystick[ ident ]->pJoystick.os->hid_data_buf )
            free( fgJoystick[ ident ]->pJoystick.os->hid_data_buf );
#endif
        free( fgJoystick[ident]->pJoystick.os );
	}
#endif

    if( ! fgJoystick[ident]->error )
         close( fgJoystick[ ident ]->pJoystick.fd );
}

