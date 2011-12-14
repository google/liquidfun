/*
 * smooth_opengl3.c, based on smooth.c, which is (c) by SGI, see below.
 * This program demonstrates smooth shading in a way which is fully
 * OpenGL-3.1-compliant.
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
#include <stddef.h>
#include <string.h>

/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
   GLenum error;
   while (( error = glGetError() ) != GL_NO_ERROR) {
      fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
   }
}

/* extension #defines, types and entries, avoiding a dependency on additional
   libraries like GLEW or the GL/glext.h header */
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif

#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif

#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif

#ifndef GL_SHADING_LANGUAGE_VERSION
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#endif

#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif

#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif

#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

typedef ptrdiff_t ourGLsizeiptr;
typedef char ourGLchar;

#ifndef APIENTRY
#define APIENTRY
#endif

typedef void (APIENTRY *PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC) (GLenum target, ourGLsizeiptr size, const GLvoid *data, GLenum usage);
typedef GLuint (APIENTRY *PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRY *PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const ourGLchar **string, const GLint *length);
typedef void (APIENTRY *PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRY *PFNGLCREATEPROGRAMPROC) (void);
typedef void (APIENTRY *PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY *PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY *PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, ourGLchar *infoLog);
typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, ourGLchar *infoLog);
typedef GLint (APIENTRY *PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const ourGLchar *name);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const ourGLchar *name);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

PFNGLGENBUFFERSPROC gl_GenBuffers;
PFNGLBINDBUFFERPROC gl_BindBuffer;
PFNGLBUFFERDATAPROC gl_BufferData;
PFNGLCREATESHADERPROC gl_CreateShader;
PFNGLSHADERSOURCEPROC gl_ShaderSource;
PFNGLCOMPILESHADERPROC gl_CompileShader;
PFNGLCREATEPROGRAMPROC gl_CreateProgram;
PFNGLATTACHSHADERPROC gl_AttachShader;
PFNGLLINKPROGRAMPROC gl_LinkProgram;
PFNGLUSEPROGRAMPROC gl_UseProgram;
PFNGLGETSHADERIVPROC gl_GetShaderiv;
PFNGLGETSHADERINFOLOGPROC gl_GetShaderInfoLog;
PFNGLGETPROGRAMIVPROC gl_GetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC gl_GetProgramInfoLog;
PFNGLGETATTRIBLOCATIONPROC gl_GetAttribLocation;
PFNGLVERTEXATTRIBPOINTERPROC gl_VertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC gl_EnableVertexAttribArray;
PFNGLGETUNIFORMLOCATIONPROC gl_GetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC gl_UniformMatrix4fv;

void initExtensionEntries(void) 
{
   gl_GenBuffers = (PFNGLGENBUFFERSPROC) glutGetProcAddress ("glGenBuffers");
   gl_BindBuffer = (PFNGLBINDBUFFERPROC) glutGetProcAddress ("glBindBuffer");
   gl_BufferData = (PFNGLBUFFERDATAPROC) glutGetProcAddress ("glBufferData");
   gl_CreateShader = (PFNGLCREATESHADERPROC) glutGetProcAddress ("glCreateShader");
   gl_ShaderSource = (PFNGLSHADERSOURCEPROC) glutGetProcAddress ("glShaderSource");
   gl_CompileShader = (PFNGLCOMPILESHADERPROC) glutGetProcAddress ("glCompileShader");
   gl_CreateProgram = (PFNGLCREATEPROGRAMPROC) glutGetProcAddress ("glCreateProgram");
   gl_AttachShader = (PFNGLATTACHSHADERPROC) glutGetProcAddress ("glAttachShader");
   gl_LinkProgram = (PFNGLLINKPROGRAMPROC) glutGetProcAddress ("glLinkProgram");
   gl_UseProgram = (PFNGLUSEPROGRAMPROC) glutGetProcAddress ("glUseProgram");
   gl_GetShaderiv = (PFNGLGETSHADERIVPROC) glutGetProcAddress ("glGetShaderiv");
   gl_GetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) glutGetProcAddress ("glGetShaderInfoLog");
   gl_GetProgramiv = (PFNGLGETPROGRAMIVPROC) glutGetProcAddress ("glGetProgramiv");
   gl_GetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) glutGetProcAddress ("glGetProgramInfoLog");
   gl_GetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) glutGetProcAddress ("glGetAttribLocation");
   gl_VertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) glutGetProcAddress ("glVertexAttribPointer");
   gl_EnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) glutGetProcAddress ("glEnableVertexAttribArray");
   gl_GetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) glutGetProcAddress ("glGetUniformLocation");
   gl_UniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) glutGetProcAddress ("glUniformMatrix4fv");
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
   checkError ("initBuffer");
}

const ourGLchar *vertexShaderSource[] = {
   "#version 140\n",
   "uniform mat4 fg_ProjectionMatrix;\n",
   "in vec4 fg_Color;\n",
   "in vec4 fg_Vertex;\n",
   "smooth out vec4 fg_SmoothColor;\n",
   "void main()\n",
   "{\n",
   "   fg_SmoothColor = fg_Color;\n",
   "   gl_Position = fg_ProjectionMatrix * fg_Vertex;\n",
   "}\n"
};

const ourGLchar *fragmentShaderSource[] = {
   "#version 140\n",
   "smooth in vec4 fg_SmoothColor;\n",
   "out vec4 fg_FragColor;\n",
   "void main(void)\n",
   "{\n",
   "   fg_FragColor = fg_SmoothColor;\n",
   "}\n"
};

void compileAndCheck(GLuint shader)
{
   GLint status;
   gl_CompileShader (shader);
   gl_GetShaderiv (shader, GL_COMPILE_STATUS, &status);
   if (status == GL_FALSE) {
     GLint infoLogLength;
     ourGLchar *infoLog;
     gl_GetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLength);
     infoLog = (ourGLchar*) malloc (infoLogLength);
     gl_GetShaderInfoLog (shader, infoLogLength, NULL, infoLog);
     fprintf (stderr, "compile log: %s\n", infoLog);
     free (infoLog);
   }
}

GLuint compileShaderSource(GLenum type, GLsizei count, const ourGLchar **string)
{
   GLuint shader = gl_CreateShader (type);
   gl_ShaderSource (shader, count, string, NULL);
   compileAndCheck (shader);
   return shader;
}

void linkAndCheck(GLuint program)
{
   GLint status;
   gl_LinkProgram (program);
   gl_GetProgramiv (program, GL_LINK_STATUS, &status);
   if (status == GL_FALSE) {
     GLint infoLogLength;
     ourGLchar *infoLog;
     gl_GetProgramiv (program, GL_INFO_LOG_LENGTH, &infoLogLength);
     infoLog = (ourGLchar*) malloc (infoLogLength);
     gl_GetProgramInfoLog (program, infoLogLength, NULL, infoLog);
     fprintf (stderr, "link log: %s\n", infoLog);
     free (infoLog);
   }
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader)
{
   GLuint program = gl_CreateProgram ();
   if (vertexShader != 0) {
      gl_AttachShader (program, vertexShader);
   }
   if (fragmentShader != 0) {
      gl_AttachShader (program, fragmentShader);
   }
   linkAndCheck (program);
   return program;
}

GLuint fgProjectionMatrixIndex;
GLuint fgColorIndex;
GLuint fgVertexIndex;

void initShader(void)
{
   const GLsizei vertexShaderLines = sizeof(vertexShaderSource) / sizeof(ourGLchar*);
   GLuint vertexShader =
     compileShaderSource (GL_VERTEX_SHADER, vertexShaderLines, vertexShaderSource);

   const GLsizei fragmentShaderLines = sizeof(fragmentShaderSource) / sizeof(ourGLchar*);
   GLuint fragmentShader =
     compileShaderSource (GL_FRAGMENT_SHADER, fragmentShaderLines, fragmentShaderSource);

   GLuint program = createProgram (vertexShader, fragmentShader);

   gl_UseProgram (program);

   fgProjectionMatrixIndex = gl_GetUniformLocation(program, "fg_ProjectionMatrix");

   fgColorIndex = gl_GetAttribLocation(program, "fg_Color");
   gl_EnableVertexAttribArray (fgColorIndex);

   fgVertexIndex = gl_GetAttribLocation(program, "fg_Vertex");
   gl_EnableVertexAttribArray (fgVertexIndex);

   checkError ("initShader");
}

void initRendering(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   checkError ("initRendering");
}

void init(void) 
{
   initExtensionEntries ();
   initBuffer ();
   initShader ();
   initRendering ();
}

void dumpInfo(void)
{
   printf ("Vendor: %s\n", glGetString (GL_VENDOR));
   printf ("Renderer: %s\n", glGetString (GL_RENDERER));
   printf ("Version: %s\n", glGetString (GL_VERSION));
   printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
   checkError ("dumpInfo");
}

const GLvoid *bufferObjectPtr (GLsizei index)
{
   return (const GLvoid *) (((char *) NULL) + index);
}

GLfloat projectionMatrix[16];

void triangle(void)
{
   gl_UniformMatrix4fv (fgProjectionMatrixIndex, 1, GL_FALSE, projectionMatrix);

   gl_BindBuffer (GL_ARRAY_BUFFER, vertexBufferName);
   gl_VertexAttribPointer (fgColorIndex, numColorComponents, GL_FLOAT, GL_FALSE,
                           stride, bufferObjectPtr (0));
   gl_VertexAttribPointer (fgVertexIndex, numVertexComponents, GL_FLOAT, GL_FALSE,
                           stride, bufferObjectPtr (sizeof(GLfloat) * numColorComponents));
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
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   if (w <= h) {
      loadOrtho2Df (projectionMatrix, 0.0f, 30.0f, 0.0f, 30.0f * (GLfloat) h/(GLfloat) w);
   } else {
      loadOrtho2Df (projectionMatrix, 0.0f, 30.0f * (GLfloat) w/(GLfloat) h, 0.0f, 30.0f);
   }
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
   /* add command line argument "classic" for a pre-3.x context */
   if ((argc != 2) || (strcmp (argv[1], "classic") != 0)) {
      glutInitContextVersion (3, 1);
      glutInitContextFlags (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
   }
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   dumpInfo ();
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0;
}
