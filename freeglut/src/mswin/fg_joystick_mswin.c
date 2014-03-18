/*
 * freeglut_joystick_mswin.c
 *
 * The Windows-specific mouse cursor related stuff.
 *
 * Copyright (c) 2012 Stephen J. Baker. All Rights Reserved.
 * Written by John F. Fay, <fayjf@sourceforge.net>
 * Creation date: Sat Jan 28, 2012
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


#if !defined(_WIN32_WCE)
#    include <windows.h>
#    include <mmsystem.h>
#    include <regstr.h>




void fgPlatformJoystickRawRead( SFG_Joystick* joy, int* buttons, float* axes )
{
    MMRESULT status;

    status = joyGetPosEx( joy->pJoystick.js_id, &joy->pJoystick.js );

    if ( status != JOYERR_NOERROR )
    {
        joy->error = GL_TRUE;
        return;
    }

    if ( buttons )
        *buttons = joy->pJoystick.js.dwButtons;

    if ( axes )
    {
        /*
         * WARNING - Fall through case clauses!!
         */
        switch ( joy->num_axes )
        {
        case 8:
            /* Generate two POV axes from the POV hat angle.
             * Low 16 bits of js.dwPOV gives heading (clockwise from ahead) in
             *   hundredths of a degree, or 0xFFFF when idle.
             */
            if ( ( joy->pJoystick.js.dwPOV & 0xFFFF ) == 0xFFFF )
            {
              axes [ 6 ] = 0.0;
              axes [ 7 ] = 0.0;
            }
            else
            {
              /* This is the contentious bit: how to convert angle to X/Y.
               *    wk: I know of no define for PI that we could use here:
               *    SG_PI would pull in sg, M_PI is undefined for MSVC
               * But the accuracy of the value of PI is very unimportant at
               * this point.
               */
              float s = (float) sin ( ( joy->pJoystick.js.dwPOV & 0xFFFF ) * ( 0.01 * 3.1415926535f / 180.0f ) );
              float c = (float) cos ( ( joy->pJoystick.js.dwPOV & 0xFFFF ) * ( 0.01 * 3.1415926535f / 180.0f ) );

              /* Convert to coordinates on a square so that North-East
               * is (1,1) not (.7,.7), etc.
               * s and c cannot both be zero so we won't divide by zero.
               */
              if ( fabs ( s ) < fabs ( c ) )
              {
                axes [ 6 ] = ( c < 0.0 ) ? -s/c  : s/c ;
                axes [ 7 ] = ( c < 0.0 ) ? -1.0f : 1.0f;
              }
              else
              {
                axes [ 6 ] = ( s < 0.0 ) ? -1.0f : 1.0f;
                axes [ 7 ] = ( s < 0.0 ) ? -c/s  : c/s ;
              }
            }

        case 6: axes[5] = (float) joy->pJoystick.js.dwVpos;
        case 5: axes[4] = (float) joy->pJoystick.js.dwUpos;
        case 4: axes[3] = (float) joy->pJoystick.js.dwRpos;
        case 3: axes[2] = (float) joy->pJoystick.js.dwZpos;
        case 2: axes[1] = (float) joy->pJoystick.js.dwYpos;
        case 1: axes[0] = (float) joy->pJoystick.js.dwXpos;
        }
    }
}



/* Inspired by
   http://msdn.microsoft.com/archive/en-us/dnargame/html/msdn_sidewind3d.asp
 */
#  if FREEGLUT_LIB_PRAGMAS
#      pragma comment (lib, "advapi32.lib")
#  endif

static int fghJoystickGetOEMProductName ( SFG_Joystick* joy, char *buf, int buf_sz )
{
    char buffer [ 256 ];

    char OEMKey [ 256 ];

    HKEY  hKey;
    DWORD dwcb;
    LONG  lr;

    if ( joy->error )
        return 0;

    /* Open .. MediaResources\CurrentJoystickSettings */
    _snprintf ( buffer, sizeof(buffer), "%s\\%s\\%s",
                REGSTR_PATH_JOYCONFIG, joy->pJoystick.jsCaps.szRegKey,
                REGSTR_KEY_JOYCURR );

    lr = RegOpenKeyEx ( HKEY_LOCAL_MACHINE, buffer, 0, KEY_QUERY_VALUE, &hKey);

    if ( lr != ERROR_SUCCESS ) return 0;

    /* Get OEM Key name */
    dwcb = sizeof(OEMKey);

    /* JOYSTICKID1-16 is zero-based; registry entries for VJOYD are 1-based. */
    _snprintf ( buffer, sizeof(buffer), "Joystick%d%s", joy->pJoystick.js_id + 1, REGSTR_VAL_JOYOEMNAME );

    lr = RegQueryValueEx ( hKey, buffer, 0, 0, (LPBYTE) OEMKey, &dwcb);
    RegCloseKey ( hKey );

    if ( lr != ERROR_SUCCESS ) return 0;

    /* Open OEM Key from ...MediaProperties */
    _snprintf ( buffer, sizeof(buffer), "%s\\%s", REGSTR_PATH_JOYOEM, OEMKey );

    lr = RegOpenKeyEx ( HKEY_LOCAL_MACHINE, buffer, 0, KEY_QUERY_VALUE, &hKey );

    if ( lr != ERROR_SUCCESS ) return 0;

    /* Get OEM Name */
    dwcb = buf_sz;

    lr = RegQueryValueEx ( hKey, REGSTR_VAL_JOYOEMNAME, 0, 0, (LPBYTE) buf,
                             &dwcb );
    RegCloseKey ( hKey );

    if ( lr != ERROR_SUCCESS ) return 0;

    return 1;
}


void fgPlatformJoystickOpen( SFG_Joystick* joy )
{
	int i = 0;

    joy->pJoystick.js.dwFlags = JOY_RETURNALL;
    joy->pJoystick.js.dwSize  = sizeof( joy->pJoystick.js );

    memset( &joy->pJoystick.jsCaps, 0, sizeof( joy->pJoystick.jsCaps ) );

    joy->error =
        ( joyGetDevCaps( joy->pJoystick.js_id, &joy->pJoystick.jsCaps, sizeof( joy->pJoystick.jsCaps ) ) !=
          JOYERR_NOERROR );

    if( joy->pJoystick.jsCaps.wNumAxes == 0 )
    {
        joy->num_axes = 0;
        joy->error = GL_TRUE;
    }
    else
    {
        /* Device name from jsCaps is often "Microsoft PC-joystick driver",
         * at least for USB.  Try to get the real name from the registry.
         */
        if ( ! fghJoystickGetOEMProductName( joy, joy->name,
                                             sizeof( joy->name ) ) )
        {
            fgWarning( "JS: Failed to read joystick name from registry" );
            strncpy( joy->name, joy->pJoystick.jsCaps.szPname, sizeof( joy->name ) );
        }

        /* Windows joystick drivers may provide any combination of
         * X,Y,Z,R,U,V,POV - not necessarily the first n of these.
         */
        if( joy->pJoystick.jsCaps.wCaps & JOYCAPS_HASPOV )
        {
            joy->num_axes = _JS_MAX_AXES;
            joy->min[ 7 ] = -1.0; joy->max[ 7 ] = 1.0;  /* POV Y */
            joy->min[ 6 ] = -1.0; joy->max[ 6 ] = 1.0;  /* POV X */
        }
        else
            joy->num_axes = 6;

        joy->min[ 5 ] = ( float )joy->pJoystick.jsCaps.wVmin;
        joy->max[ 5 ] = ( float )joy->pJoystick.jsCaps.wVmax;
        joy->min[ 4 ] = ( float )joy->pJoystick.jsCaps.wUmin;
        joy->max[ 4 ] = ( float )joy->pJoystick.jsCaps.wUmax;
        joy->min[ 3 ] = ( float )joy->pJoystick.jsCaps.wRmin;
        joy->max[ 3 ] = ( float )joy->pJoystick.jsCaps.wRmax;
        joy->min[ 2 ] = ( float )joy->pJoystick.jsCaps.wZmin;
        joy->max[ 2 ] = ( float )joy->pJoystick.jsCaps.wZmax;
        joy->min[ 1 ] = ( float )joy->pJoystick.jsCaps.wYmin;
        joy->max[ 1 ] = ( float )joy->pJoystick.jsCaps.wYmax;
        joy->min[ 0 ] = ( float )joy->pJoystick.jsCaps.wXmin;
        joy->max[ 0 ] = ( float )joy->pJoystick.jsCaps.wXmax;
    }

    /* Guess all the rest judging on the axes extremals */
    for( i = 0; i < joy->num_axes; i++ )
    {
        joy->center   [ i ] = ( joy->max[ i ] + joy->min[ i ] ) * 0.5f;
        joy->dead_band[ i ] = 0.0f;
        joy->saturate [ i ] = 1.0f;
    }
}



void fgPlatformJoystickInit( SFG_Joystick *fgJoystick[], int ident )
{
    switch( ident )
    {
    case 0:
        fgJoystick[ ident ]->pJoystick.js_id = JOYSTICKID1;
        fgJoystick[ ident ]->error = GL_FALSE;
        break;
    case 1:
        fgJoystick[ ident ]->pJoystick.js_id = JOYSTICKID2;
        fgJoystick[ ident ]->error = GL_FALSE;
        break;
    default:
        fgJoystick[ ident ]->num_axes = 0;
        fgJoystick[ ident ]->error = GL_TRUE;
        return;
    }
}



void fgPlatformJoystickClose ( int ident )
{
    /* Do nothing special */
    (void)ident;
}
#endif

