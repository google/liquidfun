/* CallbackMaker.c */
/*
 * Program to invoke all the callbacks that "freeglut" supports
 */


#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

static int sequence_number = 0 ;

static void 
Display(void)
{
  int window = glutGetWindow () ;
  glClear( GL_COLOR_BUFFER_BIT );

  printf ( "%6d Window %d Display Callback\n",
            ++sequence_number, window ) ;
  glutSwapBuffers();
}

static void 
Reshape(int width, int height)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Reshape Callback:  %d %d\n",
            ++sequence_number, window, width, height ) ;
}

static void 
Key(unsigned char key, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Keyboard Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
}

static void 
Special(int key, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Special Key Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
}

static void 
Visibility(int vis)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Visibility Callback:  %d\n",
            ++sequence_number, window, vis ) ;
}

static void 
KeyUp(unsigned char key, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Key Release Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
}

static void 
SpecialUp(int key, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Special Key Release Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
}

static void 
Joystick( unsigned int a, int b, int c, int d)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Joystick Callback:  %d %d %d %d\n",
            ++sequence_number, window, a, b, c, d ) ;
}

static void 
Mouse(int button, int updown, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Mouse Click Callback:  %d %d %d %d\n",
            ++sequence_number, window, button, updown, x, y ) ;
}

static void 
MouseWheel(int wheel_number, int direction, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Mouse Wheel Callback:  %d %d %d %d\n",
            ++sequence_number, window, wheel_number, direction, x, y ) ;
}

static void 
Motion(int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Mouse Motion Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
}

static void 
PassiveMotion(int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Mouse Passive Motion Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
}

static void 
Entry(int state)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Entry Callback:  %d\n",
            ++sequence_number, window, state ) ;
}

static void 
Close(void)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Close Callback\n",
            ++sequence_number, window ) ;
}



int 
main(int argc, char *argv[])
{
  int freeglut_window, aux_window ;

  glutInitWindowSize(500, 250);
  glutInitWindowPosition ( 140, 140 );
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE );
  glutInit(&argc, argv);

  freeglut_window = glutCreateWindow( "Callback Demo" );
  printf ( "Creating window %d as 'Callback Demo'\n", freeglut_window ) ;

  glClearColor(1.0, 1.0, 1.0, 1.0);

  glutDisplayFunc( Display );
  glutReshapeFunc( Reshape );
  glutKeyboardFunc( Key );
  glutSpecialFunc( Special );
  glutVisibilityFunc( Visibility );
  glutKeyboardUpFunc( KeyUp );
  glutSpecialUpFunc( SpecialUp );
//  glutJoystickFunc( Joystick, 10 );
  glutMouseFunc ( Mouse ) ;
  glutMouseWheelFunc ( MouseWheel ) ;
  glutMotionFunc ( Motion ) ;
  glutPassiveMotionFunc ( PassiveMotion ) ;
  glutEntryFunc ( Entry ) ;
  glutCloseFunc ( Close ) ;

  aux_window = glutCreateWindow( "Second Window" );
  printf ( "Creating window %d as 'Second Window'\n", aux_window ) ;

  glClearColor(1.0, 1.0, 1.0, 1.0);

  glutDisplayFunc( Display );
  glutReshapeFunc( Reshape );
  glutKeyboardFunc( Key );
  glutSpecialFunc( Special );
  glutVisibilityFunc( Visibility );
  glutKeyboardUpFunc( KeyUp );
  glutSpecialUpFunc( SpecialUp );
//  glutJoystickFunc( Joystick, 10 );
  glutMouseFunc ( Mouse ) ;
  glutMouseWheelFunc ( MouseWheel ) ;
  glutMotionFunc ( Motion ) ;
  glutPassiveMotionFunc ( PassiveMotion ) ;
  glutEntryFunc ( Entry ) ;
  glutCloseFunc ( Close ) ;

  glutMainLoop();

  printf ( "Back from the 'freeglut' main loop\n" ) ;

  return 0;             /* ANSI C requires main to return int. */
}
