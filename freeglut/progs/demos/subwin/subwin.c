/*! \file    subwin.c
    \ingroup demos

    This program is a test harness for the subwindows
    in OpenGLUT.  Based Originally on shape.c demo.
 
    \author  Written by Evan Felix February 2011

    \author  Portions Copyright (C) 2004, the OpenGLUT project contributors. <br>
             OpenGLUT branched from freeglut in February, 2004.
 
    \image   html openglut_subwin.png OpenGLUT Sub Window Demonstration
    \include demos/subwin/subwin.c
*/

#include <GL/freeglut.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
/* DUMP MEMORY LEAKS */
#include <crtdbg.h>
#endif

#define MAXSTR 16
char **strings;
int mainwin;


/*!
    Does printf()-like work using freeglut/OpenGLUT
    glutBitmapString().  Uses a fixed font.  Prints
    at the indicated row/column position.

    Limitation: Cannot address pixels.
    Limitation: Renders in screen coords, not model coords.
*/
static void shapesPrintf (int row, int col, const char *fmt, ...)
{
    static char buf[256];
    int viewport[4];
    void *font = GLUT_BITMAP_9_BY_15;
    va_list args;

    va_start(args, fmt);
#if defined(WIN32) && !defined(__CYGWIN__)
    (void) _vsnprintf (buf, sizeof(buf), fmt, args);
#else
    (void) vsnprintf (buf, sizeof(buf), fmt, args);
#endif
    va_end(args);

    glGetIntegerv(GL_VIEWPORT,viewport);

    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

        glOrtho(0,viewport[2],0,viewport[3],-1,1);

        glRasterPos2i
        (
              glutBitmapWidth(font, ' ') * col,
            - glutBitmapHeight(font) * (row+2) + viewport[3]
        );
        glutBitmapString (font, (unsigned char*)buf);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/* GLUT callback Handlers */

static void
resize(int width, int height)
{

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	/*gluOrtho2D(0, width, 0, height);*/

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{

	int win = glutGetWindow();
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3d(1,0,0);

    glDisable(GL_LIGHTING);
    glColor3d(0.1,0.1,0.4);

	if (win == mainwin) {
	    shapesPrintf (2, 3, "Move The mouse into different windows");
	    shapesPrintf (3, 3, "pressing keys will add to the string");
    }
    shapesPrintf (5, 3, "Window: %d", win);
    shapesPrintf (6, 3, "String: %s", strings[win]);

    glutSwapBuffers();
}


static void
key(unsigned char key, int x, int y)
{
	char *s,str[2];
	int win = glutGetWindow();
	
    switch (key)
    {
    case 27 :
    case 'Q':
    case 'q': glutLeaveMainLoop () ;      break;

    default:
    	s=strings[win];
        if (strlen(s)+1>MAXSTR) {
        	s[0]=0;
        }
        str[0]=key;
        str[1]=0;
        strcat(s,str);
        break;
    }

    glutPostRedisplay();
}

static void special (int key, int x, int y)
{
    switch (key)
    {
    default:
        break;
    }
    glutPostRedisplay();
}


static void
entry(int state)
{
    int win = glutGetWindow();
    printf("Win: %d, state: %d\n",win,state);
}

/* Program entry point */

int
main(int argc, char *argv[])
{
	int winmax,sw1,sw2,i;
	
    glutInitWindowSize(640,480);
    glutInitWindowPosition(40,40);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("FreeGLUT Sub Windows");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutEntryFunc(entry);

    glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION ) ;

    glClearColor(1,1,1,1);

	mainwin = glutGetWindow();
	winmax=mainwin;
	
	sw1=glutCreateSubWindow(mainwin,4,240,314,236);
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutEntryFunc(entry);
    glClearColor(0.7f,0.7f,0.7f,1);
	winmax = sw1 > winmax ? sw1 : winmax;

	sw2=glutCreateSubWindow(mainwin,322,240,314,236);
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutEntryFunc(entry);
    glClearColor(0.7f,0.7f,0.7f,1);
	winmax = sw2 > winmax ? sw2 : winmax;

	strings = malloc(sizeof(char *)*(winmax+1));
	for (i=0;i<winmax+1;i++) {
		strings[i] = malloc(sizeof(char)*MAXSTR+1);
		strings[i][0]=0;
	}

    glutMainLoop();

#ifdef _MSC_VER
    /* DUMP MEMORY LEAK INFORMATION */
    _CrtDumpMemoryLeaks () ;
#endif

    return EXIT_SUCCESS;
}
