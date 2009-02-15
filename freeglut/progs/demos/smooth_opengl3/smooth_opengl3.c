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
void reportErrors(const char *message)
{
   GLenum error;
   while (( error = glGetError() ) != GL_NO_ERROR) {
      fprintf (stderr, "GL error 0x%X %s\n", error, message);
   }
}

/* extension entries */
void (*gl_GenBuffers) (GLsizei n, GLuint *buffers);
void (*gl_BindBuffer) (GLenum target, GLuint buffer);
void (*gl_BufferData) (GLenum target, GLsizeiptr size, const GLvoid *data,
                       GLenum usage);

void initExtensionEntries(void) 
{
   gl_GenBuffers = glutGetProcAddress ("glGenBuffers");
   gl_BindBuffer = glutGetProcAddress ("glBindBuffer");
   gl_BufferData = glutGetProcAddress ("glBufferData");
}

/* vertex array data for a colored 2D triangle, consisting of RGB color values
   and XY coordinates */
const GLsizei numColorComponents = 3;
const GLsizei numVertexComponents = 2;

const GLfloat varray[] = {
   1.0f, 0.0f, 0.0f,
   5.0f, 5.0f,

   0.0f, 1.0f, 0.0f,
   25.0f, 5.0f,

   0.0f, 0.0f, 1.0f,
   5.0f, 25.0f
};

/* the name of the vertex buffer object */
GLuint bufferName;

void initBuffer(void)
{
   reportErrors ("at start of initBuffer");
   gl_GenBuffers (1, &bufferName);
   gl_BindBuffer (GL_ARRAY_BUFFER, bufferName);
   gl_BufferData (GL_ARRAY_BUFFER, sizeof(varray), varray, GL_STATIC_DRAW);
   reportErrors ("at end of initBuffer");
}

void initRendering(void)
{
   reportErrors ("at start of initRendering");
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   reportErrors ("at end of initRendering");
}

void init(void) 
{
   initExtensionEntries();
   initBuffer();
   initRendering();
}

void triangle(void)
{
   const char *base = NULL;
   const GLsizei stride = sizeof(GLfloat) * (numColorComponents + numVertexComponents);
   const GLsizei numElements = sizeof(varray) / stride;
   reportErrors ("at start of triangle");
   gl_BindBuffer (GL_ARRAY_BUFFER, bufferName);
   glColorPointer (numColorComponents, GL_FLOAT, stride, base);
   glVertexPointer(numVertexComponents, GL_FLOAT, stride,
                   base + sizeof(GLfloat) * numColorComponents);
   glEnableClientState (GL_COLOR_ARRAY);
   glEnableClientState (GL_VERTEX_ARRAY);
   glDrawArrays(GL_TRIANGLES, 0, numElements);
   reportErrors ("at end of triangle");
}

void display(void)
{
   reportErrors ("at start of display");
   glClear (GL_COLOR_BUFFER_BIT);
   triangle ();
   glFlush ();
   reportErrors ("at end of display");
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
   reportErrors ("at start of reshape");
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   if (w <= h) {
      loadOrtho2Df (m, 0.0, 30.0, 0.0, 30.0 * (GLfloat) h/(GLfloat) w);
   } else {
      loadOrtho2Df (m, 0.0, 30.0 * (GLfloat) w/(GLfloat) h, 0.0, 30.0);
   }
   glLoadMatrixf (m);
   glMatrixMode (GL_MODELVIEW);
   reportErrors ("at end of reshape");
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
   glutInitContextVersion(3, 0);
   /* glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG); */
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
