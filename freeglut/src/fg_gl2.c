/*
 * fg_gl2.c
 *
 * Load OpenGL (ES) 2.0 functions used by fg_geometry
 *
 * Copyright (C) 2012  Sylvain Beucler
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
#include "fg_internal.h"
#include "fg_gl2.h"

typedef void (APIENTRY *PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC) (GLenum target, fghGLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint* buffers);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY *PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);

PFNGLGENBUFFERSPROC fghGenBuffers = NULL;
PFNGLDELETEBUFFERSPROC fghDeleteBuffers = NULL;
PFNGLBINDBUFFERPROC fghBindBuffer = NULL;
PFNGLBUFFERDATAPROC fghBufferData = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC fghEnableVertexAttribArray = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC fghDisableVertexAttribArray = NULL;
PFNGLVERTEXATTRIBPOINTERPROC fghVertexAttribPointer = NULL;

void fgInitGL2() {
    fgState.HasOpenGL20 = 0;
#define CHECK(func, a) if ((a) == NULL) { fgWarning("fgInitGL2: " func " is NULL"); return; }
    CHECK("fghGenBuffers", fghGenBuffers = (PFNGLGENBUFFERSPROC) glutGetProcAddress ("glGenBuffers"));
    CHECK("fghDeleteBuffers", fghDeleteBuffers = (PFNGLDELETEBUFFERSPROC) glutGetProcAddress ("glDeleteBuffers"));
    CHECK("fghBindBuffer", fghBindBuffer = (PFNGLBINDBUFFERPROC) glutGetProcAddress ("glBindBuffer"));
    CHECK("fghBufferData", fghBufferData = (PFNGLBUFFERDATAPROC) glutGetProcAddress ("glBufferData"));
    CHECK("fghVertexAttribPointer", fghVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) glutGetProcAddress ("glVertexAttribPointer"));
    CHECK("fghEnableVertexAttribArray", fghEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) glutGetProcAddress ("glEnableVertexAttribArray"));
    CHECK("fghDisableVertexAttribArray", fghDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) glutGetProcAddress ("glDisnableVertexAttribArray"));
#undef CHECK
    fgState.HasOpenGL20 = 1;
}
