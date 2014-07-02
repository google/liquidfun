/*
* Copyright (c) 2013 Google, Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/
#if defined(ANDROID) || defined(__IOS__)

#include <GL/freeglut.h>
#include "gl_emu.h"

#include <assert.h>
#include <vector>

using namespace std;

struct vertex {
  float x, y, u, v;
};

vector<vertex> vertbuf;
vertex curvert = { 0 };

int curmode = GL_QUADS;

void glColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
  glColor4ub(r, g, b, 255);
}

void glColor3f(float r, float g, float b) { glColor3ub(r * 255, g * 255, b * 255); }
void glColor3fv(const float *color) { glColor3f(color[0], color[1], color[2]); }
void glColor3ubv(const unsigned char *color) { glColor3ub(color[0], color[1], color[2]); }
void glColor3b(char r, char g, char b) { glColor3ub(r, g, b); }
void glColor3d(double r, double g, double b) { glColor3f(r, g, b); }

void glTexCoord2f(float u, float v) {
  curvert.u = u;
  curvert.v = v;
}

void glVertex2f(float x, float y) {
  curvert.x = x;
  curvert.y = y;
  vertbuf.push_back(curvert);
}

void glVertex2i(int x, int y) { glVertex2f(x, y); }
void glVertex2fv(const float *vert) { glVertex2f(vert[0], vert[1]); }

void glBegin(int mode) {
  vertbuf.clear();
  curmode = mode;
}

void glEnd() {
  assert(vertbuf.size() >= 1);
  switch (curmode) {
    case GL_POINTS:
    case GL_LINE_STRIP:
    case GL_LINE_LOOP:
    case GL_LINES:
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
    case GL_TRIANGLES:
      break;

    default: // something unsupported in ES 1.1
      curmode = GL_TRIANGLE_STRIP;
      break;
  }

  float *buf = &vertbuf[0].x;
  glEnableClientState(GL_VERTEX_ARRAY);        glVertexPointer  (2, GL_FLOAT,         sizeof(vertex), buf + 0);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY); glTexCoordPointer(2, GL_FLOAT,         sizeof(vertex), buf + 2);

  glDrawArrays(curmode, 0, (GLsizei) vertbuf.size());

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void glColorMaterial(int face, int mode) {
  assert(0);
}

void glLineStipple(int factor, unsigned short pattern) {
  assert(0);
}

void glPolygonMode(int face, int mode) {
  assert(0);
}

void glRasterPos2i(int x, int y) {
  assert(0);
}

void glRasterPos2f(float x, float y) {
  assert(0);
}

void glRecti(int x1, int y1, int x2, int y2) {
  assert(0);
}

void glDrawBuffer(int mode) {
}

void glDrawPixels(int width, int height, int format, int type, const void* data) {
  assert(0);
}

int gluBuild2DMipmaps(int target, int internal, int width, int height, int format, int type, const void* data) {
  assert(0);
  return 0;
}

void glFrustum(double left, double right, double bottom, double top, double near, double far) { glFrustumf(left, right, bottom, top, near, far); }
void glOrtho(double left, double right, double bottom, double top, double near, double far) { glOrthof(left, right, bottom, top, near, far); }
void gluOrtho2D(double left, double right, double bottom, double top) { glOrthof(left, right, bottom, top, -1, 1); }

GLUquadric* gluNewQuadric() {
  assert(0);
  return (GLUquadric *)1;
}

void gluQuadricNormals(GLUquadric *quad, int normal) { assert(0); }
void gluQuadricTexture(GLUquadric *quad, int texture) { assert(0); }
void gluSphere(GLUquadric *quad, double radius, int slices, int stacks) { assert(0); }
void gluQuadricDrawStyle(GLUquadric *quad, int draw) { assert(0); }

#endif


