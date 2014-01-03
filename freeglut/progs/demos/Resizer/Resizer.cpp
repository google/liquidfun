#include <stdio.h>

#include <GL/freeglut.h>

int nWindow, nChildWindow = -1;
int nLoopMain = 0;
GLboolean bChildPosDone = GL_FALSE, bChildSizeDone = GL_FALSE;

void SampleKeyboard( unsigned char cChar, int nMouseX, int nMouseY );
void Redisplay();
void Reshape(int width, int height);
void Position(int x, int y);
void WindowStatus(int state);




void DrawQuad()
{
    int width  = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    glBegin(GL_QUADS);
        glVertex2d(width*.25, height*.75);
        glVertex2d(width*.75, height*.75);
        glVertex2d(width*.75, height*.25);
        glVertex2d(width*.25, height*.25);
    glEnd();
}

void UnhideTimer(int window)
{
    glutSetWindow(window);
    glutShowWindow();
}

void ChangeTitleTimer(int unused)
{
    glutSetIconTitle("new icon title");
    glutSetWindowTitle("new test title");
}

void SampleKeyboard( unsigned char cChar, int nMouseX, int nMouseY )
{
    switch (cChar)
    {
    case 27:
        glutLeaveMainLoop();

        break;


    case 'f':
    case 'F':
        printf("main window toggle fullscreen\n");
        glutFullScreenToggle();

        break;


    case 'r':
    case 'R':
        if (nChildWindow!=-1 && cChar=='r') /* Capital R always resizes the main window*/
        {
            glutSetWindow(nChildWindow);
            printf("child window resize\n");
            if (!bChildSizeDone)
                glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH)+50,glutGet(GLUT_WINDOW_HEIGHT)+50);
            else
                glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH)-50,glutGet(GLUT_WINDOW_HEIGHT)-50);
            bChildSizeDone = !bChildSizeDone;
        }
        else
        {
            glutSetWindow(nWindow);
            printf("main window resize\n");
            if (glutGet(GLUT_WINDOW_WIDTH)<400)
                glutReshapeWindow(600,300);
            else
                glutReshapeWindow(300,300);
        }

        break;


    case 'm':
    case 'M':
        if (nChildWindow!=-1 && cChar=='m') /* Capital M always moves the main window*/
        {
            glutSetWindow(nChildWindow);
            /* The window position you request is relative to the top-left
             * corner of the client area of the parent window.
             */
            if (!bChildPosDone)
                glutPositionWindow(glutGet(GLUT_WINDOW_X)+50,glutGet(GLUT_WINDOW_Y)+50);
            else
                glutPositionWindow(glutGet(GLUT_WINDOW_X)-50,glutGet(GLUT_WINDOW_Y)-50);
            bChildPosDone = !bChildPosDone;
        }
        else
        {
            glutSetWindow(nWindow);
            printf("main window position\n");
            /* The window position you request is the outer top-left of the window,
             * the client area is at a different position if the window has borders
             * and/or a title bar.
             */
            if (glutGet(GLUT_WINDOW_X)<400)
                glutPositionWindow(600,300);
            else
                glutPositionWindow(300,300);
        }

        break;


    case 'd':
    case 'D':
        if (nChildWindow!=-1 && cChar=='d') /* Capital D always moves+resizes the main window*/
        {
            glutSetWindow(nChildWindow);
            if (!bChildPosDone)
                glutPositionWindow(glutGet(GLUT_WINDOW_X)+50,glutGet(GLUT_WINDOW_Y)+50);
            else
                glutPositionWindow(glutGet(GLUT_WINDOW_X)-50,glutGet(GLUT_WINDOW_Y)-50);
            bChildPosDone = !bChildPosDone;
            if (!bChildSizeDone)
                glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH)+50,glutGet(GLUT_WINDOW_HEIGHT)+50);
            else
                glutReshapeWindow(glutGet(GLUT_WINDOW_WIDTH)-50,glutGet(GLUT_WINDOW_HEIGHT)-50);
            bChildSizeDone = !bChildSizeDone;
        }
        else
        {
            if (glutGet(GLUT_WINDOW_X)<400)
                glutPositionWindow(600,300);
            else
                glutPositionWindow(300,300);
            if (glutGet(GLUT_WINDOW_WIDTH)<400)
                glutReshapeWindow(600,300);
            else
                glutReshapeWindow(300,300);
        }
        break;


    case 'c':
    case 'C':
        if (nChildWindow==-1)
        {
            int width  = glutGet(GLUT_WINDOW_WIDTH);
            int height = glutGet(GLUT_WINDOW_HEIGHT);

            /* open child window */
            printf("open child window\n");

            nChildWindow = glutCreateSubWindow(nWindow,(int)(width*.35),(int)(height*.35),(int)(width*.3),(int)(height*.3));
            glutKeyboardFunc( SampleKeyboard );
            glutDisplayFunc( Redisplay );
            glutReshapeFunc( Reshape );
            glutPositionFunc( Position );
            glutWindowStatusFunc( WindowStatus );
        }
        else
        {
            /* close child window */
            printf("close child window\n");
            glutSetWindow(nWindow);
            glutDestroyWindow(nChildWindow);
            nChildWindow = -1;
            bChildSizeDone = GL_FALSE;
            bChildPosDone  = GL_FALSE;
        }
        break;


    case 'i':
    case 'I':
        glutIconifyWindow();
        glutTimerFunc(1500, ChangeTitleTimer, 0);
        break;


    case 'h':
    case 'H':
        if (nChildWindow!=-1 && cChar=='h') /* Capital H always hides the main window*/
        {
            glutSetWindow(nChildWindow);
            glutTimerFunc(2000, UnhideTimer, nChildWindow);
        }
        else
        {
            glutSetWindow(nWindow);
            glutTimerFunc(2000, UnhideTimer, nWindow);
        }
        glutHideWindow();


    case 'p':
    case 'P':
        if (nChildWindow!=-1 && cChar=='p') /* Capital P always changes pointer for the main window*/
        {
            glutSetWindow(nChildWindow);
            if (glutGet(GLUT_WINDOW_CURSOR)==GLUT_CURSOR_TOP_SIDE)
                glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
            else
                glutSetCursor(GLUT_CURSOR_TOP_SIDE);
        }
        else
        {
            glutSetWindow(nWindow);
            if (glutGet(GLUT_WINDOW_CURSOR)==GLUT_CURSOR_CYCLE)
                glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
            else
                glutSetCursor(GLUT_CURSOR_CYCLE);
        }

    default:
        break;
    }
}

void Idle(void)
{
    glutPostRedisplay();
}

void Reshape(int width, int height)
{
    int win = glutGetWindow();

    printf("reshape %s, client area: %dx%d\n",win==nWindow?"main":"child",
        width, height);

    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,width,0,height);

    if (win==nWindow && nChildWindow!=-1)
    {
        /* Put child window in right place */
        int x = (int)(width*.35), y=(int)(height*.35), w=(int)(width*.3), h = (int)(height*.3);
        if (bChildPosDone)
        {
            x += 50;
            y += 50;
        }
        if (bChildSizeDone)
        {
            w += 50;
            h += 50;
        }
        glutSetWindow(nChildWindow);
        glutPositionWindow(x,y);
        glutReshapeWindow(w,h);
        glutSetWindow(nWindow);
    }
}

void Position(int x, int y)
{
    int win = glutGetWindow();

    printf("position, %s: (%d,%d)\n",win==nWindow?"top-left (non-client) of main":"top-left of child relative to parent",
        x, y);
}

void WindowStatus(int state)
{
    int win = glutGetWindow();
    printf("windowstatus (win %i): %i\n",win,state);
}

void Redisplay(void)
{
    int win = glutGetWindow();

    if (win==nWindow)
    {
        glClearColor(.2f,0.f,0.f,0.f);
        glColor3f(1,1,1);
    }
    else
    {
        /* child window */
        glClearColor(.0f,.2f,0.f,0.f);
        glColor3f(.5,.5,.5);
        glutPostWindowRedisplay(nWindow);
    }
    glClear(GL_COLOR_BUFFER_BIT);
    DrawQuad();

    glutSwapBuffers();
    glutPostWindowRedisplay(win);
}

void Timer(int unused)
{
    int win = glutGetWindow();
    int x, y;
    int width, height;
    int border, caption;

    x       = glutGet(GLUT_WINDOW_X);
    y       = glutGet(GLUT_WINDOW_Y);
    width   = glutGet(GLUT_WINDOW_WIDTH);
    height  = glutGet(GLUT_WINDOW_HEIGHT);
    border  = glutGet(GLUT_WINDOW_BORDER_WIDTH);
    caption = glutGet(GLUT_WINDOW_HEADER_HEIGHT);
    /* returned position is top-left of client area, to get top-left of
     * of window you'll need to add the size of the border and caption
     * of the current window (can be 0).
     * Note that the window position is not necessarily positive (e.g.
     * when the window is on a monitor to the left of the primary monitor
     * or simply when maximized--try pressing the maximize button).
     * the returned size is the size of the client area
     * Note that the top-left of a child window is relative to the
     * top-left of the client area of the parent.
     */
    /* printf("window border: %dpx, caption: %dpx\n",border,caption); */
    if (win==nWindow)
        printf("main  window %dx%d, top-left of client at: (%d,%d), of window at: (%d,%d)\n",
            width, height,
            x ,y,
            x-border,
            y-border-caption);
    else
        printf("child window %dx%d, top-left of client at: (%d,%d), relative to parent\n",
        width, height,
        x ,y);

    /* (re)set the timer callback and ask glut to call it in 500 ms */
    glutTimerFunc(500, Timer, 0);
}


int main(int argc, char* argv[])
{
    int border, caption;
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE /*| GLUT_BORDERLESS*/); // do try as well with GLUT_BORDERLESS and GLUT_CAPTIONLESS
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    
    /* Get border and caption size of default window style */
    border  = glutGet(GLUT_WINDOW_BORDER_WIDTH);
    caption = glutGet(GLUT_WINDOW_HEADER_HEIGHT);
    printf("default window style border: %dpx, caption: %dpx\n",border,caption);

    /* NB: The window position you request is the outer top-left of the
     * window, the client area is at a different position if the window has
     * borders and/or a title bar.
     */
    glutInitWindowPosition(150,250);
    glutInitWindowSize(200,200);

    nWindow = glutCreateWindow("test");
    glutSetIconTitle("test icon title");
    printf("main window id: %d\n", nWindow);

    glutKeyboardFunc( SampleKeyboard );
    glutDisplayFunc( Redisplay );
    glutReshapeFunc( Reshape );
    glutPositionFunc( Position );
    glutWindowStatusFunc( WindowStatus );

    glutTimerFunc(300, Timer, 0);

    glutMainLoop();
    printf("glutMainLoop returned\n");

    return EXIT_SUCCESS;
}