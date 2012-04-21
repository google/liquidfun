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

#ifndef  FG_GL2_H
#define  FG_GL2_H

#include <GL/freeglut.h>
#include "fg_internal.h"

#ifdef GL_ES_VERSION_2_0
/* Use existing functions on GLES 2.0 */

#define FGH_ARRAY_BUFFER GL_ARRAY_BUFFER
#define FGH_STATIC_DRAW GL_STATIC_DRAW
#define FGH_ELEMENT_ARRAY_BUFFER GL_ELEMENT_ARRAY_BUFFER

#define fghGenBuffers glGenBuffers
#define fghDeleteBuffers glDeleteBuffers
#define fghBindBuffer glBindBuffer
#define fghBufferData glBufferData
#define fghEnableVertexAttribArray glEnableVertexAttribArray
#define fghDisableVertexAttribArray glDisableVertexAttribArray
#define fghVertexAttribPointer glVertexAttribPointer

#else
/* Load functions dynamically, they are not defined in e.g. win32's
   OpenGL headers */

#    ifndef APIENTRY
#        define APIENTRY
#    endif

/* extension #defines, types and entries, avoiding a dependency on additional
   libraries like GLEW or the GL/glext.h header */
#define FGH_ARRAY_BUFFER 0x8892
#define FGH_STATIC_DRAW 0x88E4
#define FGH_ELEMENT_ARRAY_BUFFER 0x8893

typedef int fghGLsizeiptr;
typedef void (APIENTRY *FGH_PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY *FGH_PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY *FGH_PFNGLBUFFERDATAPROC) (GLenum target, fghGLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (APIENTRY *FGH_PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint* buffers);
typedef void (APIENTRY *FGH_PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY *FGH_PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint);
typedef void (APIENTRY *FGH_PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);

FGH_PFNGLGENBUFFERSPROC fghGenBuffers;
FGH_PFNGLDELETEBUFFERSPROC fghDeleteBuffers;
FGH_PFNGLBINDBUFFERPROC fghBindBuffer;
FGH_PFNGLBUFFERDATAPROC fghBufferData;
FGH_PFNGLENABLEVERTEXATTRIBARRAYPROC fghEnableVertexAttribArray;
FGH_PFNGLDISABLEVERTEXATTRIBARRAYPROC fghDisableVertexAttribArray;
FGH_PFNGLVERTEXATTRIBPOINTERPROC fghVertexAttribPointer;

#    endif

extern void fgInitGL2();

#endif
