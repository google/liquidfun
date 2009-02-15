/*
 * smooth_opengl3.c, based on smooth.c, which is (c) by SGI, see below.
 * This program demonstrates smooth shading in a way which is fully
 * OpenGL-3.0-compliant.
 * A smooth shaded polygon is drawn in a 2-D projection.
 */

/*
 * Original copyright notice from smooth.c:
 *
 * License Applicability. Except to the extent portions of this file are
 * made subject to an alternative license as permitted in the SGI Free
 * Software License B, Version 1.1 (the "License"), the contents of this
 * file are subject only to the provisions of the License. You may not use
 * this file except in compliance with the License. You may obtain a copy
 * of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
 * Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
 * 
 * http://oss.sgi.com/projects/FreeB
 * 
 * Note that, as provided in the License, the Software is distributed on an
 * "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
 * DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
 * CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
 * 
 * Original Code. The Original Code is: OpenGL Sample Implementation,
 * Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
 * Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
 * Copyright in any portions created by third parties is as indicated
 * elsewhere herein. All Rights Reserved.
 * 
 * Additional Notice Provisions: The application programming interfaces
 * established by SGI in conjunction with the Original Code are The
 * OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
 * April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
 * 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
 * Window System(R) (Version 1.3), released October 19, 1998. This software
 * was created using the OpenGL(R) version 1.2.1 Sample Implementation
 * published by SGI, but has not been independently verified as being
 * compliant with the OpenGL(R) version 1.2.1 Specification.
 *
 */

#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>

/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
   GLenum error;
   while (( error = glGetError() ) != GL_NO_ERROR) {
      fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
   }
}

/* extension types and entries, avoiding a dependency on additional libraries
   like GLEW or the GL/glext.h header */
typedef void (*PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
PFNGLGENBUFFERSPROC gl_GenBuffers;

typedef void (*PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
PFNGLBINDBUFFERPROC gl_BindBuffer;

typedef void (*PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size,
                                     const GLvoid *data, GLenum usage);
PFNGLBUFFERDATAPROC gl_BufferData;

void initExtensionEntries(void) 
{
   gl_GenBuffers = (PFNGLGENBUFFERSPROC) glutGetProcAddress ("glGenBuffers");
   gl_BindBuffer = (PFNGLBINDBUFFERPROC) glutGetProcAddress ("glBindBuffer");
   gl_BufferData = (PFNGLBUFFERDATAPROC) glutGetProcAddress ("glBufferData");
}

/* vertex array data for a colored 2D triangle, consisting of RGB color values
   and XY coordinates */
const GLfloat varray[] = {
   1.0f, 0.0f, 0.0f, /* red */
   5.0f, 5.0f,       /* lower left */

   0.0f, 1.0f, 0.0f, /* green */
   25.0f, 5.0f,      /* lower right */

   0.0f, 0.0f, 1.0f, /* blue */
   5.0f, 25.0f       /* upper left */
};

/* ISO C somehow enforces this silly use of 'enum' for compile-time constants */
enum {
  numColorComponents = 3,
  numVertexComponents = 2,
  stride = sizeof(GLfloat) * (numColorComponents + numVertexComponents),
  numElements = sizeof(varray) / stride
};

/* the name of the vertex buffer object */
GLuint vertexBufferName;

void initBuffer(void)
{
   gl_GenBuffers (1, &vertexBufferName);
   gl_BindBuffer (GL_ARRAY_BUFFER, vertexBufferName);
   gl_BufferData (GL_ARRAY_BUFFER, sizeof(varray), varray, GL_STATIC_DRAW);
   glEnableClientState (GL_COLOR_ARRAY);
   glEnableClientState (GL_VERTEX_ARRAY);
   checkError ("initBuffer");
}

void initRendering(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   checkError ("initRendering");
}

void init(void) 
{
   initExtensionEntries();
   initBuffer();
   initRendering();
}

const GLvoid *bufferObjectPtr (GLsizei index)
{
   return (const GLvoid *) (((char *) NULL) + index);
}

void triangle(void)
{
   gl_BindBuffer (GL_ARRAY_BUFFER, vertexBufferName);
   glColorPointer (numColorComponents, GL_FLOAT, stride, bufferObjectPtr (0));
   glVertexPointer(numVertexComponents, GL_FLOAT, stride,
                   bufferObjectPtr (sizeof(GLfloat) * numColorComponents));
   glDrawArrays(GL_TRIANGLES, 0, numElements);
   checkError ("triangle");
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   triangle ();
   glFlush ();
   checkError ("display");
}

void loadOrthof(GLfloat *m, GLfloat l, GLfloat r, GLfloat b, GLfloat t,
                GLfloat n, GLfloat f)
{
   m[ 0] = 2.0f / (r - l);
   m[ 1] = 0.0f;
   m[ 2] = 0.0f;
   m[ 3] = 0.0f;

   m[ 4] = 0.0f;
   m[ 5] = 2.0f / (t - b);
   m[ 6] = 0.0f;
   m[ 7] = 0.0f;

   m[ 8] = 0.0f;
   m[ 9] = 0.0f;
   m[10] = -2.0f / (f - n);
   m[11] = 0.0f;

   m[12] = -(r + l) / (r - l);
   m[13] = -(t + b) / (t - b);
   m[14] = -(f + n) / (f - n);
   m[15] = 1.0f;
}

void loadOrtho2Df(GLfloat *m, GLfloat l, GLfloat r, GLfloat b, GLfloat t)
{
   loadOrthof (m, l, r, b, t, -1.0f, 1.0f);
}

void reshape (int w, int h)
{
   GLfloat m[16];
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   if (w <= h) {
      loadOrtho2Df (m, 0.0, 30.0, 0.0, 30.0 * (GLfloat) h/(GLfloat) w);
   } else {
      loadOrtho2Df (m, 0.0, 30.0 * (GLfloat) w/(GLfloat) h, 0.0, 30.0);
   }
   glLoadMatrixf (m);
   glMatrixMode (GL_MODELVIEW);
   checkError ("reshape");
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   /* add command line argument "classic" for a pre-3.0 context */
   if ((argc != 2) || (strcmp (argv[1], "classic") != 0)) {
      glutInitContextVersion (3, 0);
      glutInitContextFlags (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
   }
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0;
}
