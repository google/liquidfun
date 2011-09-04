#include <stdio.h>

#include <GL/freeglut.h>

int nWindow;
int nLoopMain = 0;

int nPosX,  nPosY;
int nWidth, nHeight;

void SampleKeyboard( unsigned char cChar, int nMouseX, int nMouseY );
void Redisplay();





void DrawQuad()
{
    glBegin(GL_QUADS);
        glVertex2d(nWidth*.25, nHeight*.75);
        glVertex2d(nWidth*.75, nHeight*.75);
        glVertex2d(nWidth*.75, nHeight*.25);
        glVertex2d(nWidth*.25, nHeight*.25);
    glEnd();
}

void SampleKeyboard( unsigned char cChar, int nMouseX, int nMouseY )
{
    if (cChar == 27)
        glutLeaveMainLoop();

    else if (cChar=='f')
    {
        printf("main window toggle fullscreen\n");

        glutFullScreenToggle();
    }
    else if (cChar=='r')
    {
        printf("main window resize\n");

        if (nWidth<400)
            glutReshapeWindow(600,300);
        else
            glutReshapeWindow(300,300);
    }
    else if (cChar=='m')
    {
        printf("main window position\n");

        /* The window position you request is the outer top-left of the window,
         * the client area is at a different position if the window has borders
         * and/or a title bar.
         */
        if (nPosX<400)
            glutPositionWindow(600,300);
        else
            glutPositionWindow(300,300);
    }
}

void Idle(void)
{
    glutPostRedisplay();
}

void Reshape(int x, int y)
{
    nWidth  = glutGet(GLUT_WINDOW_WIDTH);
    nHeight = glutGet(GLUT_WINDOW_HEIGHT);

    glViewport(0,0,nWidth,nHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,nWidth,0,nHeight);
}

void Redisplay(void)
{
    if (nLoopMain++%6==0)
    {
        int border, caption;

        nPosX   = glutGet(GLUT_WINDOW_X);
        nPosY   = glutGet(GLUT_WINDOW_Y);
        nWidth  = glutGet(GLUT_WINDOW_WIDTH);
        nHeight = glutGet(GLUT_WINDOW_HEIGHT);
        border  = glutGet(GLUT_WINDOW_BORDER_WIDTH);
        caption = glutGet(GLUT_WINDOW_HEADER_HEIGHT);
        /* returned position is top-left of client area, to get top-left of
         * of window you'll need to add the size of the border and caption
         * of the current window (can be 0).
         * Note that the window position is not necessarily positive (e.g.
         * when the window is on a monitor to the left of the primary monitor
         * or simply when maximized--try pressing the maximize button).
         * the returned size is the size of the client area
         */
        printf("window now %dx%d, top-left of client at: (%d,%d), of window at: (%d,%d)\n",
            nWidth, nHeight,
            nPosX ,nPosY,
            nPosX-border,
            nPosY-border-caption);
    }

    glClearColor(.2f,0.f,0.f,0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,1,1);
    DrawQuad();

    glutSwapBuffers();
    glutPostRedisplay();
}


int main(int argc, char* argv[])
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE /*| GLUT_BORDERLESS*/); // do try as well with GLUT_BORDERLESS and GLUT_CAPTIONLESS
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    /* The window position you request is the outer top-left of the window,
     * the client area is at a different position if the window has borders
     * and/or a title bar.
     */
    glutInitWindowPosition(150,250);
    glutInitWindowSize(200,200);

    nWindow = glutCreateWindow("test");
    printf("main window id: %d\n", nWindow);

    glutKeyboardFunc( SampleKeyboard );
    glutDisplayFunc( Redisplay );
    glutReshapeFunc( Reshape );

    glutMainLoop();
    printf("glutMainLoop returned\n");

    return 1;
}