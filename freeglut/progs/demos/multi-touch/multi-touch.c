/**
 * Sample multi-touch program that displays a square where a cursor
 * clicks, with a different color for each cursor.
 *
 * Copyright (C) 2012  Sylvain Beucler
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/freeglut.h>
#include <GL/gl.h>

#define NUM_DEVICES 16
#define NUM_CURSORS 64
typedef struct cursor {
    char on;
    float x;
    float y;
} *Cursor;
struct cursor cursors[NUM_DEVICES][NUM_CURSORS];


static float square[] = {
        -.5, -.5,
         .5, -.5,
        -.5,  .5,
         .5,  .5,
    };

void onDisplay(void) {
    int d;
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, square);
    for (d = 0; d < NUM_DEVICES; d++) {
        int c;
        for (c = 0; d < NUM_DEVICES; d++) {
            Cursor C = &cursors[d][c];
            if (C->on) {
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glTranslatef(C->x, C->y, 0);
                glScalef(30, 30, 1);
                
                switch(c) {
                case 0:
                    glColor4f(0,0,1,1);
                    break;
                case 1:
                    glColor4f(0,1,0,1);
                    break;
                case 2:
                    glColor4f(1,0,0,1);
                    break;
                case 3:
                    glColor4f(1,1,1,1);
                    break;
                default:
                    glColor4d(.5,.5,.5,1);
                    break;
                }
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
    }
    glDisableClientState(GL_VERTEX_ARRAY);

    glutSwapBuffers();
}

void onMouse(int button, int state, int x, int y) {
    if (button == 0) {
        cursors[0][0].on = (state == GLUT_DOWN);
        cursors[0][0].x = (float)x;
        cursors[0][0].y = (float)y;
        printf("normal click\n");
    }
}

void onMotion(int x, int y) {
    cursors[0][0].x = (float)x;
    cursors[0][0].y = (float)y;
}

/* Using FG2.8 (reversed) prototype for now */
/* void onMultiButton(int cursor_id, int button, int state, int x, int y) { */
void onMultiButton(int cursor_id, int x, int y, int button, int state) {
    if (cursor_id > NUM_CURSORS) {
        fprintf(stderr, "cursor_id(%d) > NUM_CURSORS(%d)\n", cursor_id, NUM_CURSORS);
        return;
    }
    if (button == 0) {
        cursors[0][cursor_id].on = (state == GLUT_DOWN);
        cursors[0][cursor_id].x = (float)x;
        cursors[0][cursor_id].y = (float)y;
        printf("multi-touch %d click\n", cursor_id);
    }
}

void onMultiMotion(int cursor_id, int x, int y) {
    if (cursor_id > NUM_CURSORS) {
        fprintf(stderr, "cursor_id(%d) > NUM_CURSORS(%d)\n", cursor_id, NUM_CURSORS);
        return;
    }
    cursors[0][cursor_id].x = (float)x;
    cursors[0][cursor_id].y = (float)y;
}

void onReshape(int width, int height) {
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, width, height, 0, -1, 1);
}

void onIdle(void) {
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    memset(cursors, 0, sizeof(cursors));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Multi-touch test");

    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutIdleFunc(onIdle);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMotion);
    glutMultiButtonFunc(onMultiButton);
    glutMultiMotionFunc(onMultiMotion);

    glutMainLoop();

    return EXIT_SUCCESS;
}
