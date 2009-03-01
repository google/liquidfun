/* CallbackMaker.c */
/*
 * Program to invoke all the callbacks that "freeglut" supports
 */


#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static int sequence_number = 0 ;

int reshape_called = 0, key_called = 0, special_called = 0, visibility_called = 0,
    keyup_called = 0, specialup_called = 0, joystick_called = 0, mouse_called = 0,
    mousewheel_called = 0, motion_called = 0, passivemotion_called = 0, entry_called = 0,
    close_called = 0, overlaydisplay_called = 0, windowstatus_called = 0,
    spacemotion_called = 0, spacerotation_called = 0, spacebutton_called = 0,
    buttonbox_called = 0, dials_called = 0, tabletmotion_called = 0, tabletbutton_called = 0,
    menudestroy_called = 0, menustatus_called = 0 ;
int reshape_width = -1, reshape_height = -1, reshape_seq = -1 ;
int key_key = -1, key_x = -1, key_y = -1, key_seq = -1 ;
int special_key = -1, special_x = -1, special_y = -1, special_seq = -1 ;
int visibility_vis = -1, visibility_seq = -1 ;
int keyup_key = -1, keyup_x = -1, keyup_y = -1, keyup_seq = -1 ;
int specialup_key = -1, specialup_x = -1, specialup_y = -1, specialup_seq = -1 ;
int joystick_a = -1, joystick_b = -1, joystick_c = -1, joystick_d = -1, joystick_seq = -1 ;  /* Need meaningful names */
int mouse_button = -1, mouse_updown = -1, mouse_x = -1, mouse_y = -1, mouse_seq = -1 ;
int mousewheel_number = -1, mousewheel_direction = -1, mousewheel_x = -1, mousewheel_y = -1, mousewheel_seq = -1 ;
int motion_x = -1, motion_y = -1, motion_seq = -1 ;
int passivemotion_x = -1, passivemotion_y = -1, passivemotion_seq = -1 ;

static void
bitmapPrintf (const char *fmt, ...)
{
    static char buf[256];
    va_list args;

    va_start(args, fmt);
#if defined(WIN32) && !defined(__CYGWIN__)
    (void) _vsnprintf (buf, sizeof(buf), fmt, args);
#else
    (void) vsnprintf (buf, sizeof(buf), fmt, args);
#endif
    va_end(args);
    glutBitmapString ( GLUT_BITMAP_HELVETICA_12, (unsigned char*)buf ) ;
}


static void 
Display(void)
{
  int window = glutGetWindow () ;
  glClear ( GL_COLOR_BUFFER_BIT );

  glDisable ( GL_DEPTH_TEST );
  glMatrixMode ( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, glutGet ( GLUT_WINDOW_WIDTH ), 
          0, glutGet ( GLUT_WINDOW_HEIGHT ), -1, 1 );
  glMatrixMode ( GL_MODELVIEW );
  glPushMatrix ();
  glLoadIdentity ();
  glColor3ub ( 0, 0, 0 );
  glRasterPos2i ( 10, glutGet ( GLUT_WINDOW_HEIGHT ) - 10 );

  if ( reshape_called )
  {
    bitmapPrintf ( "Reshape %d:  %d %d\n", reshape_seq, reshape_width, reshape_height );
  }

  if ( key_called )
  {
    bitmapPrintf ( "Key %d:  %d(%c) %d %d\n", key_seq, key_key, key_key, key_x, key_y );
  }

  if ( special_called )
  {
    bitmapPrintf ( "Special %d:  %d(%c) %d %d\n", special_seq, special_key, special_key, special_x, special_y );
  }

  if ( visibility_called )
  {
    bitmapPrintf ( "Visibility %d:  %d\n", visibility_seq, visibility_vis );
  }

  if ( keyup_called )
  {
    bitmapPrintf ( "Key Up %d:  %d(%c) %d %d\n", keyup_seq, keyup_key, keyup_key, keyup_x, keyup_y );
  }

  if ( specialup_called )
  {
    bitmapPrintf ( "Special Up %d:  %d(%c) %d %d\n", specialup_seq, specialup_key, specialup_key, specialup_x, specialup_y );
  }

  if ( joystick_called )
  {
    bitmapPrintf ( "Joystick %d:  %d %d %d %d\n", joystick_seq, joystick_a, joystick_b, joystick_c, joystick_d );
  }

  if ( mouse_called )
  {
    bitmapPrintf ( "Mouse %d:  %d %d %d %d\n", mouse_seq, mouse_button, mouse_updown, mouse_x, mouse_y );
  }

  if ( mousewheel_called )
  {
    bitmapPrintf ( "Mouse Wheel %d:  %d %d %d %d\n", mousewheel_seq, mousewheel_number, mousewheel_direction, mousewheel_x, mousewheel_y );
  }

  if ( motion_called )
  {
    bitmapPrintf ( "Motion %d:  %d %d\n", motion_seq, motion_x, motion_y );
  }

  if ( passivemotion_called )
  {
    bitmapPrintf ( "Passive Motion %d:  %d %d\n", passivemotion_seq, passivemotion_x, passivemotion_y );
  }

  glMatrixMode ( GL_PROJECTION );
  glPopMatrix ();
  glMatrixMode ( GL_MODELVIEW );
  glPopMatrix ();
  glEnable ( GL_DEPTH_TEST );

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
  reshape_called = 1 ;
  reshape_width = width ;
  reshape_height = height ;
  reshape_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Key(unsigned char key, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Keyboard Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
  key_called = 1 ;
  key_key = key ;
  key_x = x ;
  key_y = y ;
  key_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Special(int key, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Special Key Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
  special_called = 1 ;
  special_key = key ;
  special_x = x ;
  special_y = y ;
  special_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Visibility(int vis)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Visibility Callback:  %d\n",
            ++sequence_number, window, vis ) ;
  visibility_called = 1 ;
  visibility_vis = vis ;
  visibility_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
KeyUp(unsigned char key, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Key Release Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
  keyup_called = 1 ;
  keyup_key = key ;
  keyup_x = x ;
  keyup_y = y ;
  keyup_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
SpecialUp(int key, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Special Key Release Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
  specialup_called = 1 ;
  specialup_key = key ;
  specialup_x = x ;
  specialup_y = y ;
  specialup_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Joystick( unsigned int a, int b, int c, int d)  /* Need meaningful names */
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Joystick Callback:  %d %d %d %d\n",
            ++sequence_number, window, a, b, c, d ) ;
  joystick_called = 1 ;
  joystick_a = a ;
  joystick_b = b ;
  joystick_c = c ;
  joystick_d = d ;
  joystick_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Mouse(int button, int updown, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Mouse Click Callback:  %d %d %d %d\n",
            ++sequence_number, window, button, updown, x, y ) ;
  mouse_called = 1 ;
  mouse_button = button ;
  mouse_updown = updown ;
  mouse_x = x ;
  mouse_y = y ;
  mouse_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
MouseWheel(int wheel_number, int direction, int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Mouse Wheel Callback:  %d %d %d %d\n",
            ++sequence_number, window, wheel_number, direction, x, y ) ;
  mousewheel_called = 1 ;
  mousewheel_number = wheel_number ;
  mousewheel_direction = direction ;
  mousewheel_x = x ;
  mousewheel_y = y ;
  mousewheel_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Motion(int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Mouse Motion Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
  motion_called = 1 ;
  motion_x = x ;
  motion_y = y ;
  motion_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
PassiveMotion(int x, int y)
{
  int window = glutGetWindow () ;
  printf ( "%6d Window %d Mouse Passive Motion Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
  passivemotion_called = 1 ;
  passivemotion_x = x ;
  passivemotion_y = y ;
  passivemotion_seq = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Entry(int state)
{
  int window = glutGetWindow () ;
  entry_called = 1 ;
  printf ( "%6d Window %d Entry Callback:  %d\n",
            ++sequence_number, window, state ) ;
  glutPostRedisplay () ;
}

static void 
Close(void)
{
  int window = glutGetWindow () ;
  close_called = 1 ;
  printf ( "%6d Window %d Close Callback\n",
            ++sequence_number, window ) ;
}

static void 
OverlayDisplay(void)
{
  int window = glutGetWindow () ;
  overlaydisplay_called = 1 ;
  printf ( "%6d Window %d OverlayDisplay Callback\n",
            ++sequence_number, window ) ;
  glutPostRedisplay () ;
}

static void 
WindowStatus(int state)
{
  int window = glutGetWindow () ;
  windowstatus_called = 1 ;
  printf ( "%6d Window %d WindowStatus Callback:  %d\n",
            ++sequence_number, window, state ) ;
  glutPostRedisplay () ;
}

static void 
SpaceMotion(int x, int y, int z)
{
  int window = glutGetWindow () ;
  spacemotion_called = 1 ;
  printf ( "%6d Window %d SpaceMotion Callback:  %d %d %d\n",
            ++sequence_number, window, x, y, z ) ;
  glutPostRedisplay () ;
}

static void 
SpaceRotation(int x, int y, int z)
{
  int window = glutGetWindow () ;
  spacerotation_called = 1 ;
  printf ( "%6d Window %d SpaceRotation Callback:  %d %d %d\n",
            ++sequence_number, window, x, y, z ) ;
  glutPostRedisplay () ;
}

static void 
SpaceButton(int button, int updown)
{
  int window = glutGetWindow () ;
  spacebutton_called = 1 ;
  printf ( "%6d Window %d SpaceButton Callback:  %d %d\n",
            ++sequence_number, window, button, updown ) ;
  glutPostRedisplay () ;
}

static void 
Dials(int x, int y)
{
  int window = glutGetWindow () ;
  dials_called = 1 ;
  printf ( "%6d Window %d Dials Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
  glutPostRedisplay () ;
}

static void 
ButtonBox(int button, int updown)
{
  int window = glutGetWindow () ;
  buttonbox_called = 1 ;
  printf ( "%6d Window %d ButtonBox Callback:  %d %d\n",
            ++sequence_number, window, button, updown ) ;
  glutPostRedisplay () ;
}

static void 
TabletMotion(int x, int y)
{
  int window = glutGetWindow () ;
  tabletmotion_called = 1 ;
  printf ( "%6d Window %d TabletMotion Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
  glutPostRedisplay () ;
}

static void 
TabletButton(int button, int updown, int x, int y)
{
  int window = glutGetWindow () ;
  tabletbutton_called = 1 ;
  printf ( "%6d Window %d TabletButton Callback:  %d %d %d %d\n",
            ++sequence_number, window, button, updown, x, y ) ;
  glutPostRedisplay () ;
}

static void
MenuCallback ( int menuID )
{
  int window = glutGetWindow () ;
  printf( "%6d Window %d MenuCallback - menuID is %d\n",
          ++sequence_number, window, menuID );
}

static void 
MenuDestroy( void )
{
  int window = glutGetWindow () ;
  menudestroy_called = 1 ;
  printf ( "%6d Window %d MenuDestroy Callback\n",
            ++sequence_number, window ) ;
  glutPostRedisplay () ;
}

static void 
MenuStatus( int status, int x, int y )
{
  int window = glutGetWindow () ;
  menudestroy_called = 1 ;
  printf ( "%6d Window %d MenuStatus Callback:  %d %d %d\n",
            ++sequence_number, window, status, x, y ) ;
  glutPostRedisplay () ;
}

static void Idle ( void )
{
  ++sequence_number ;
}

int 
main(int argc, char *argv[])
{
#define STRING_LENGTH   10
  int freeglut_window, aux_window ;
  char dummy_string[STRING_LENGTH];

  int menuID, subMenuA, subMenuB;

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
  glutJoystickFunc( Joystick, 100 );
  glutMouseFunc ( Mouse ) ;
  glutMouseWheelFunc ( MouseWheel ) ;
  glutMotionFunc ( Motion ) ;
  glutPassiveMotionFunc ( PassiveMotion ) ;
  glutEntryFunc ( Entry ) ;
  glutCloseFunc ( Close ) ;
  glutOverlayDisplayFunc ( OverlayDisplay ) ;
  glutWindowStatusFunc ( WindowStatus ) ;
  glutSpaceballMotionFunc ( SpaceMotion ) ;
  glutSpaceballRotateFunc ( SpaceRotation ) ;
  glutSpaceballButtonFunc ( SpaceButton ) ;
  glutButtonBoxFunc ( ButtonBox ) ;
  glutDialsFunc ( Dials ) ;
  glutTabletMotionFunc ( TabletMotion ) ;
  glutTabletButtonFunc ( TabletButton ) ;
  glutMenuDestroyFunc ( MenuDestroy );
  glutMenuStatusFunc ( MenuStatus );
  glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF) ;

  subMenuA = glutCreateMenu( MenuCallback );
  glutAddMenuEntry( "Sub menu A1 (01)", 1 );
  glutAddMenuEntry( "Sub menu A2 (02)", 2 );
  glutAddMenuEntry( "Sub menu A3 (03)", 3 );

  subMenuB = glutCreateMenu( MenuCallback );
  glutAddMenuEntry( "Sub menu B1 (04)", 4 );
  glutAddMenuEntry( "Sub menu B2 (05)", 5 );
  glutAddMenuEntry( "Sub menu B3 (06)", 6 );
  glutAddSubMenu( "Going to sub menu A", subMenuA );

  menuID = glutCreateMenu( MenuCallback );
  glutAddMenuEntry( "Entry one",   1 );
  glutAddMenuEntry( "Entry two",   2 );
  glutAddMenuEntry( "Entry three", 3 );
  glutAddMenuEntry( "Entry four",  4 );
  glutAddMenuEntry( "Entry five",  5 );
  glutAddSubMenu( "Enter sub menu A", subMenuA );
  glutAddSubMenu( "Enter sub menu B", subMenuB );

  glutAttachMenu( GLUT_LEFT_BUTTON );

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
  /*  glutJoystickFunc( Joystick, 100 ); */
  glutMouseFunc ( Mouse ) ;
  glutMouseWheelFunc ( MouseWheel ) ;
  glutMotionFunc ( Motion ) ;
  glutPassiveMotionFunc ( PassiveMotion ) ;
  glutEntryFunc ( Entry ) ;
  glutCloseFunc ( Close ) ;
  glutOverlayDisplayFunc ( OverlayDisplay ) ;
  glutWindowStatusFunc ( WindowStatus ) ;
  glutSpaceballMotionFunc ( SpaceMotion ) ;
  glutSpaceballRotateFunc ( SpaceRotation ) ;
  glutSpaceballButtonFunc ( SpaceButton ) ;
  glutButtonBoxFunc ( ButtonBox ) ;
  glutDialsFunc ( Dials ) ;
  glutTabletMotionFunc ( TabletMotion ) ;
  glutTabletButtonFunc ( TabletButton ) ;
  glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF) ;

  glutIdleFunc ( Idle );

  printf ( "Please enter something to continue: " );
  fgets ( dummy_string, STRING_LENGTH, stdin );

  glutMainLoop();

  printf ( "Back from the 'freeglut' main loop\n" ) ;

  return 0;             /* ANSI C requires main to return int. */
}
