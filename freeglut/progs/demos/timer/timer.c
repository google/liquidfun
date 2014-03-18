/* Timer demo
 *
 * Written by John Tsiombikas <nuclear@member.fsf.org>
 *
 * Demonstrate the use of glutTimerFunc, by changing the color of the
 * framebuffer every (approximately) 1 sec.
 */
#include <stdio.h>
#include <GL/glut.h>

void disp(void);
void timer_func(int unused);

/* color index will be advanced every time the timer expires */
int cidx = 0;
int pcidx = 2;
float color[][3] = {
	{1, 0, 0},
	{0, 1, 0},
	{0, 0, 1},
	{1, 1, 0},
	{0, 1, 1},
	{1, 0, 1}
};

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(128, 128);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("timer test");

	glutDisplayFunc(disp);

    /* get timer started, its reset in the timer function itself */
    glutTimerFunc(1000, timer_func, 1);
    glutTimerFunc(500, timer_func, 2);

	glutMainLoop();
	return 0;
}

void disp(void)
{
	glClearColor(color[cidx][0], color[cidx][1], color[cidx][2], 1);
	glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(10.f);
    glColor3f(color[pcidx][0], color[pcidx][1], color[pcidx][2]);
    glBegin(GL_POINTS);
        glVertex2i(0,0);
    glEnd();

	glutSwapBuffers();
}

void timer_func(int which)
{
	/* advance the color index and trigger a redisplay */
    switch (which)
    {
    case 1:
        cidx = (cidx + 1) % (sizeof color / sizeof *color);
        break;
    case 2:
        pcidx = (pcidx + 1) % (sizeof color / sizeof *color);
        break;
    }
    
	glutPostRedisplay();

	/* (re)set the timer callback and ask glut to call it in 1 second */
	glutTimerFunc(1000, timer_func, which);
}
