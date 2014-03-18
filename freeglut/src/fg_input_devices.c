/*
 * freeglut_input_devices.c
 *
 * Handles miscellaneous input devices via direct serial-port access.
 * Proper X11 XInput device support is not yet supported.
 * Also lacks Mac support.
 *
 * Written by Joe Krahn <krahn@niehs.nih.gov> 2005
 *
 * Copyright (c) 2005 Stephen J. Baker. All Rights Reserved.
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
 * PAWEL W. OLSZTA OR STEPHEN J. BAKER BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include <GL/freeglut.h>
#include "fg_internal.h"

typedef struct _serialport SERIALPORT;


/********************* Dialbox definitions ***********************/

#define DIAL_NUM_VALUATORS 8

/* dial parser state machine states */
#define DIAL_NEW                (-1)
#define DIAL_WHICH_DEVICE       0
#define DIAL_VALUE_HIGH         1
#define DIAL_VALUE_LOW          2

/* dial/button box commands */
#define DIAL_INITIALIZE                 0x20
#define DIAL_SET_LEDS                   0x75
#define DIAL_SET_TEXT                   0x61
#define DIAL_SET_AUTO_DIALS             0x50
#define DIAL_SET_AUTO_DELTA_DIALS       0x51
#define DIAL_SET_FILTER                 0x53
#define DIAL_SET_BUTTONS_MOM_TYPE       0x71
#define DIAL_SET_AUTO_MOM_BUTTONS       0x73
#define DIAL_SET_ALL_LEDS               0x4b
#define DIAL_CLEAR_ALL_LEDS             0x4c

/* dial/button box replies and events */
#define DIAL_INITIALIZED        0x20
#define DIAL_BASE               0x30
#define DIAL_DELTA_BASE         0x40
#define DIAL_PRESS_BASE         0xc0
#define DIAL_RELEASE_BASE       0xe0

/* macros to determine reply type */
#define IS_DIAL_EVENT(ch)       (((ch)>=DIAL_BASE)&&((ch)<DIAL_BASE+DIAL_NUM_VALUATORS))
#define IS_KEY_PRESS(ch)        (((ch)>=DIAL_PRESS_BASE)&&((ch)<DIAL_PRESS_BASE+DIAL_NUM_BUTTONS))
#define IS_KEY_RELEASE(ch)      (((ch)>=DIAL_RELEASE_BASE)&&((ch)<DIAL_RELEASE_BASE+DIAL_NUM_BUTTONS))
#define IS_INIT_EVENT(ch)       ((ch)==DIAL_INITIALIZED)

/*****************************************************************/

extern SERIALPORT *serial_open ( const char *device );
extern void serial_close ( SERIALPORT *port );
extern int serial_getchar ( SERIALPORT *port );
extern int serial_putchar ( SERIALPORT *port, unsigned char ch );
extern void serial_flush ( SERIALPORT *port );

extern void fgPlatformRegisterDialDevice ( const char *dial_device );
static void send_dial_event(int dial, int value);
static void poll_dials(int id);

/* local variables */
static SERIALPORT *dialbox_port=NULL;

/*****************************************************************/

/*
 * Implementation for glutDeviceGet(GLUT_HAS_DIAL_AND_BUTTON_BOX)
 */
int fgInputDeviceDetect( void )
{
    fgInitialiseInputDevices ();

    if ( !dialbox_port )
        return 0;

    if ( !fgState.InputDevsInitialised )
        return 0;

    return 1;
}

/*
 * Try initializing the input device(s)
 */
void fgInitialiseInputDevices ( void )
{
    if( !fgState.InputDevsInitialised )
    {
        const char *dial_device=NULL;
        dial_device = getenv ( "GLUT_DIALS_SERIAL" );
		fgPlatformRegisterDialDevice ( dial_device );

        if ( !dial_device ) return;
        dialbox_port = serial_open ( dial_device );
        if ( !dialbox_port ) return;
        serial_putchar(dialbox_port,DIAL_INITIALIZE);
        glutTimerFunc ( 10, poll_dials, 0 );
        fgState.InputDevsInitialised = GL_TRUE;
    }
}

/*
 *
 */
void fgInputDeviceClose( void )
{
    if ( fgState.InputDevsInitialised )
    {
        serial_close ( dialbox_port );
        dialbox_port = NULL;
        fgState.InputDevsInitialised = GL_FALSE;
    }
}

/********************************************************************/

/* Check all windows for dialbox callbacks */
static void fghcbEnumDialCallbacks ( SFG_Window *window, SFG_Enumerator *enumerator )
{
    /* Built-in to INVOKE_WCB():  if window->Callbacks[CB_Dials] */
    INVOKE_WCB ( *window,Dials, ( ((int*)enumerator->data)[0], ((int*)enumerator->data)[1]) );
    fgEnumSubWindows ( window, fghcbEnumDialCallbacks, enumerator );
}

static void send_dial_event ( int num, int value )
{
    SFG_Enumerator enumerator;
    int data[2];
    data[0] = num;
    data[1] = value;
    enumerator.found = GL_FALSE;
    enumerator.data  =  data;
    fgEnumWindows ( fghcbEnumDialCallbacks, &enumerator );
}

/********************************************************************/
static void poll_dials ( int id )
{
    int data;
    static int dial_state = DIAL_NEW;
    static int dial_which;
    static int dial_value;
    (void)id;

    if ( !dialbox_port ) return;

    while ( (data=serial_getchar(dialbox_port)) != EOF )
    {
        if ( ( dial_state > DIAL_WHICH_DEVICE ) || IS_DIAL_EVENT ( data ) )
        {
            switch ( dial_state )
            {
            case DIAL_WHICH_DEVICE:
                dial_which = data - DIAL_BASE;
                dial_state++;
                break;
            case DIAL_VALUE_HIGH:
                dial_value = ( data << 8 );
                dial_state++;
                break;
            case DIAL_VALUE_LOW:
                dial_value |= data;
                if ( dial_value & 0x8000 ) dial_value -= 0x10000;
                send_dial_event ( dial_which + 1, dial_value * 360 / 256 );
                dial_state = DIAL_WHICH_DEVICE;
                break;
            default:
                /* error: Impossible state value! */
                break;
            }
        }
        else if ( data == DIAL_INITIALIZED )
        {
            fgState.InputDevsInitialised = GL_TRUE;
            dial_state = DIAL_WHICH_DEVICE;
            serial_putchar(dialbox_port,DIAL_SET_AUTO_DIALS);
            serial_putchar(dialbox_port,0xff);
            serial_putchar(dialbox_port,0xff);
        }
        else  /* Unknown data; try flushing. */
            serial_flush(dialbox_port);
    }

    glutTimerFunc ( 2, poll_dials, 0 );
}

