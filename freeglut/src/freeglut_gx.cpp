/*
 * freeglut_gx.cpp
 *
 * WindowsCE specific file
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "freeglut_internal.h"

#if TARGET_HOST_WINCE


#include <windows.h>
#include <gx.h>

#pragma comment( lib, "gx.lib" )

// Microsoft decided that gapi should only work in C++ (thanks...)
// so we need a c-wrapper for it...
//

extern "C"
{

void wince_GetDefaultKeys(void* nData, int iOptions)
{
    *(GXKeyList*)nData = GXGetDefaultKeys(iOptions);
};


void wince_OpenInput()
{
    GXOpenInput();
}

}


#endif
