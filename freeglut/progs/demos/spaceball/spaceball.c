/* Spaceball demo
 *
 * Written by John Tsiombikas <nuclear@member.fsf.org>
 * (converted from the libspnav cube example)
 *
 * Use the spaceball to move and rotate the colored cube.
 * Pressing any button will reset the cube at its original location.
 *
 * Press escape or q to exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/freeglut.h>
#include "vmath.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950
#endif

void draw_cube(void);

/* callbacks */
void disp(void);
void reshape(int x, int y);
void keyb(unsigned char key, int x, int y);
void sbmot(int x, int y, int z);  /* spaceball translation */
void sbrot(int x, int y, int z);  /* spaceball rotation */
void sbbut(int bn, int state);    /* spaceball button */

vec3_t pos = {0, 0, -6};
quat_t rot = {0, 0, 0, 1};

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutCreateWindow("spaceball demo");

  glutDisplayFunc(disp);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyb);
  glutSpaceballMotionFunc(sbmot);
  glutSpaceballRotateFunc(sbrot);
  glutSpaceballButtonFunc(sbbut);

  glEnable(GL_CULL_FACE);

  glutMainLoop();
  return 0;
}

void disp(void)
{
  mat4_t xform;

  quat_to_mat(xform, rot);

  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(pos.x, pos.y, pos.z);
  glMultMatrixf((float*)xform);

  draw_cube();

  glutSwapBuffers();
}

void draw_cube(void)
{
  glBegin(GL_QUADS);
  /* face +Z */
  glNormal3f(0, 0, 1);
  glColor3f(1, 0, 0);
  glVertex3f(-1, -1, 1);
  glVertex3f(1, -1, 1);
  glVertex3f(1, 1, 1);
  glVertex3f(-1, 1, 1);
  /* face +X */
  glNormal3f(1, 0, 0);
  glColor3f(0, 1, 0);
  glVertex3f(1, -1, 1);
  glVertex3f(1, -1, -1);
  glVertex3f(1, 1, -1);
  glVertex3f(1, 1, 1);
  /* face -Z */
  glNormal3f(0, 0, -1);
  glColor3f(0, 0, 1);
  glVertex3f(1, -1, -1);
  glVertex3f(-1, -1, -1);
  glVertex3f(-1, 1, -1);
  glVertex3f(1, 1, -1);
  /* face -X */
  glNormal3f(-1, 0, 0);
  glColor3f(1, 1, 0);
  glVertex3f(-1, -1, -1);
  glVertex3f(-1, -1, 1);
  glVertex3f(-1, 1, 1);
  glVertex3f(-1, 1, -1);
  /* face +Y */
  glNormal3f(0, 1, 0);
  glColor3f(0, 1, 1);
  glVertex3f(-1, 1, 1);
  glVertex3f(1, 1, 1);
  glVertex3f(1, 1, -1);
  glVertex3f(-1, 1, -1);
  /* face -Y */
  glNormal3f(0, -1, 0);
  glColor3f(1, 0, 1);
  glVertex3f(-1, -1, -1);
  glVertex3f(1, -1, -1);
  glVertex3f(1, -1, 1);
  glVertex3f(-1, -1, 1);
  glEnd();
}

/* 45deg fov */
#define FOV    (M_PI / 4.0)

void reshape(int x, int y)
{
  float aspect = (float)x / (float)y;
  float halfy = (float)tan(FOV / 2.0);
  float halfx = halfy * aspect;

  glViewport(0, 0, x, y);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-halfx, halfx, -halfy, halfy, 1.0, 1000.0);
}

void keyb(unsigned char key, int x, int y)
{
  switch(key) {
  case 'q':
  case 'Q':
  case 27:
    exit(0);

  case ' ':
    /* reset initial view */
    pos = v3_cons(0, 0, -6);
    rot = quat_cons(1, 0, 0, 0);
    glutPostRedisplay();

  default:
    break;
  }
}

void sbmot(int x, int y, int z)
{
  pos.x += x * 0.001f;
  pos.y += y * 0.001f;
  pos.z -= z * 0.001f;
  glutPostRedisplay();
}

void sbrot(int x, int y, int z)
{
  float axis_len = (float)sqrt(x * x + y * y + z * z);
  rot = quat_rotate(rot, axis_len * 0.001f, -x / axis_len, -y / axis_len, z / axis_len);
  glutPostRedisplay();
}

void sbbut(int bn, int state)
{
  if(state == GLUT_DOWN) {
    pos = v3_cons(0, 0, -6);
    rot = quat_cons(1, 0, 0, 0);
    glutPostRedisplay();
  }
}
