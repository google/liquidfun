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

#include <stdio.h>
typedef struct _serialport SERIALPORT;

/*
 * Try initializing the input device(s)
 */
void fgPlatformRegisterDialDevice ( const char *dial_device )
{
  fprintf(stderr, "fgPlatformRegisterDialDevice: STUB\n");
}

SERIALPORT *serial_open ( const char *device )
{
  fprintf(stderr, "serial_open: STUB\n");
  return NULL;
}

void serial_close(SERIALPORT *port)
{
  fprintf(stderr, "serial_close: STUB\n");
}

int serial_getchar(SERIALPORT *port)
{
  fprintf(stderr, "serial_getchar: STUB\n");
  return EOF;
}

int serial_putchar(SERIALPORT *port, unsigned char ch)
{
  fprintf(stderr, "serial_putchar: STUB\n");
  return 0;
}

void serial_flush ( SERIALPORT *port )
{
  fprintf(stderr, "serial_flush: STUB\n");
}
