#include <GL/freeglut.h>
#include <stdio.h>

static int sequence_number=0;

static void Display(void)
{
    int window=glutGetWindow();
    printf ("%6d Window %d Display Callback\n",
        ++sequence_number, window );
    glClear ( GL_COLOR_BUFFER_BIT );
    glutSwapBuffers();
}
static void Entry(int state)
{
    int window=glutGetWindow () ;
    printf ("%6d Window %d Entry Callback:  %d\n",
        ++sequence_number, window, state);
}
static void SampleMenu(int a)
{
    printf("Menu clicked %d\n", a);
}
int main(int argc, char *argv[])
{
    int freeglut_window, menuID;
    glutInitWindowSize(400, 250);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInit(&argc, argv);
    freeglut_window=glutCreateWindow("INVOKE_WCB Issue");
    printf("window created\n");
    glClearColor(1.0, .0, 1.0, 1.0);
    glutDisplayFunc(Display);
    glutEntryFunc(Entry) ;
    menuID=glutCreateMenu(SampleMenu);
    glutAddMenuEntry("Entry one", 1);
    glutAddMenuEntry("Entry two", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
    return 0;
}
