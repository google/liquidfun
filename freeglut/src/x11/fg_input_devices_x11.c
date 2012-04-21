/*
 * freeglut_input_devices_x11.c
 *
 * Handles miscellaneous input devices via direct serial-port access.
 * Proper X11 XInput device support is not yet supported.
 * Also lacks Mac support.
 *
 * Written by Joe Krahn <krahn@niehs.nih.gov> 2005
 *
 * Copyright (c) 2005 Stephen J. Baker. All Rights Reserved.
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
 * PAWEL W. OLSZTA OR STEPHEN J. BAKER BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include <GL/freeglut.h>
#include "../fg_internal.h"

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

struct _serialport {
   int fd;
   struct termios termio, termio_save;
};

typedef struct _serialport SERIALPORT;

void serial_flush ( SERIALPORT *port );

/*****************************************************************/

/*
 * Try initializing the input device(s)
 */
void fgPlatformRegisterDialDevice ( const char *dial_device )
{
}

SERIALPORT *serial_open ( const char *device )
{
    int fd;
    struct termios termio;
    SERIALPORT *port;

    fd = open(device, O_RDWR | O_NONBLOCK );
    if (fd <0) {
        perror(device);
        return NULL;
    }

    port = malloc(sizeof(SERIALPORT));
    memset(port, 0, sizeof(SERIALPORT));
    port->fd = fd;

    /* save current port settings */
    tcgetattr(fd,&port->termio_save);

    memset(&termio, 0, sizeof(termio));
    termio.c_cflag = CS8 | CREAD | HUPCL ;
    termio.c_iflag = IGNPAR | IGNBRK ;
    termio.c_cc[VTIME]    = 0;   /* inter-character timer */
    termio.c_cc[VMIN]     = 1;   /* block read until 1 chars received, when blocking I/O */

    cfsetispeed(&termio, B9600);
    cfsetospeed(&termio, B9600);
    tcsetattr(fd,TCSANOW,&termio);

    serial_flush(port);
    return port;
}

void serial_close(SERIALPORT *port)
{
    if (port)
    {
        /* restore old port settings */
        tcsetattr(port->fd,TCSANOW,&port->termio_save);
        close(port->fd);
        free(port);
    }
}

int serial_getchar(SERIALPORT *port)
{
    unsigned char ch;
    if (!port) return EOF;
    if (read(port->fd,&ch,1)) return ch;
    return EOF;
}

int serial_putchar(SERIALPORT *port, unsigned char ch)
{
    if (!port) return 0;
    return write(port->fd,&ch,1);
}

void serial_flush ( SERIALPORT *port )
{
    tcflush ( port->fd, TCIOFLUSH );
}
