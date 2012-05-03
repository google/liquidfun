/*
 * fg_input_devices_android.c
 *
 * Handles miscellaneous input devices via direct serial-port access.
 *
 * Written by Joe Krahn <krahn@niehs.nih.gov> 2005
 * Copyright (c) 2005 Stephen J. Baker. All Rights Reserved.
 * Copied for Platform code by Evan Felix <karcaw at gmail.com>
 * Copyright 2012 (C)  Sylvain Beucler
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
 * PAWEL W. OLSZTA OR STEPHEN J. BAKER BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <GL/freeglut.h>
#include "fg_internal.h"
typedef struct _serialport SERIALPORT;
#include <stdio.h>

/*
 * This is only used if the user calls:
 * glutDeviceGet(GLUT_HAS_DIAL_AND_BUTTON_BOX)
 * and has old hardware called 'dials&buttons box'.
 * http://www.reputable.com/sgipix/sgi-dialnbutton1.jpg
 * 
 * Not implemented on Android :)
 * http://sourceforge.net/mailarchive/message.php?msg_id=29209505
 */
void fgPlatformRegisterDialDevice ( const char *dial_device ) {
    fgWarning("GLUT_HAS_DIAL_AND_BUTTON_BOX: not implemented");
}
SERIALPORT *serial_open ( const char *device ) { return NULL; }
void serial_close(SERIALPORT *port) {}
int serial_getchar(SERIALPORT *port) { return EOF; }
int serial_putchar(SERIALPORT *port, unsigned char ch) { return 0; }
void serial_flush ( SERIALPORT *port ) {}
