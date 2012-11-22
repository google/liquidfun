/* CallbackMaker.c */
/*
 * Program to invoke all the callbacks that "freeglut" supports
 */


#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static int sequence_number = 0 ;

int windows[2] = {0};

/* define status vars showing whether given callback has been called for given window */
#define CALLBACK_CALLED_VAR(name) int name##_called[2] = {0};
#define CALLBACK_1_VAR(name,field) int name##_##field[2], name##_seq[2] = {-1};
#define CALLBACK_2_VARS(name,field1,field2) int name##_##field1[2], name##_##field2[2], name##_seq[2] = {-1};
#define CALLBACK_3_VARS(name,field1,field2,field3) int name##_##field1[2], name##_##field2[2], name##_##field3[2], name##_seq[2] = {-1};
#define CALLBACK_4_VARS(name,field1,field2,field3,field4) int name##_##field1[2], name##_##field2[2], name##_##field3[2], name##_##field4[2], name##_seq[2] = {-1};
CALLBACK_CALLED_VAR(reshape);
CALLBACK_2_VARS(reshape,width,height);
CALLBACK_CALLED_VAR(key);
CALLBACK_3_VARS(key,key,x,y);
CALLBACK_CALLED_VAR(keyup);
CALLBACK_3_VARS(keyup,key,x,y);
CALLBACK_CALLED_VAR(special);
CALLBACK_3_VARS(special,key,x,y);
CALLBACK_CALLED_VAR(specialup);
CALLBACK_3_VARS(specialup,key,x,y);
CALLBACK_CALLED_VAR(visibility);
CALLBACK_1_VAR(visibility,vis);
CALLBACK_CALLED_VAR(joystick);
CALLBACK_4_VARS(joystick,a,b,c,d);
CALLBACK_CALLED_VAR(mouse);
CALLBACK_4_VARS(mouse,button,updown,x,y);
CALLBACK_CALLED_VAR(mousewheel);
CALLBACK_4_VARS(mousewheel,number,direction,x,y);
CALLBACK_CALLED_VAR(motion);
CALLBACK_2_VARS(motion,x,y);
CALLBACK_CALLED_VAR(passivemotion);
CALLBACK_2_VARS(passivemotion,x,y);
CALLBACK_CALLED_VAR(entry);
CALLBACK_CALLED_VAR(close);
/*CALLBACK_CALLED_VAR(overlaydisplay);
CALLBACK_CALLED_VAR(windowstatus);
CALLBACK_CALLED_VAR(spacemotion);
CALLBACK_CALLED_VAR(spacerotation);
CALLBACK_CALLED_VAR(spacebutton);
CALLBACK_CALLED_VAR(buttonbox);
CALLBACK_CALLED_VAR(dials);
CALLBACK_CALLED_VAR(tabletmotion);
CALLBACK_CALLED_VAR(tabletbutton);
CALLBACK_CALLED_VAR(menustatus);*/
int menudestroy_called = 0 ;    /* menudestroy is registered on menu, so don't use above */

#define STRING_LENGTH   10

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

static int
getWindowAndIdx(int *winIdx)
{
    int window = glutGetWindow();

    if (winIdx)
        (*winIdx) = window==windows[0]?0:1;

    return window;
}

static void 
Display(void)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
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
  glRasterPos2i ( 10, glutGet ( GLUT_WINDOW_HEIGHT ) - 20 );	/* 10pt margin above 10pt letters */

  if ( reshape_called[winIdx] )
  {
    bitmapPrintf ( "Reshape %d:  %d %d\n", reshape_seq[winIdx], reshape_width[winIdx], reshape_height[winIdx] );
  }

  if ( key_called[winIdx] )
  {
    bitmapPrintf ( "Key %d:  %d(%c) %d %d\n", key_seq[winIdx], key_key[winIdx], key_key[winIdx], key_x[winIdx], key_y[winIdx] );
  }

  if ( special_called[winIdx] )
  {
    bitmapPrintf ( "Special %d:  %d(%c) %d %d\n", special_seq[winIdx], special_key[winIdx], special_key[winIdx], special_x[winIdx], special_y[winIdx] );
  }

  if ( visibility_called[winIdx] )
  {
    bitmapPrintf ( "Visibility %d:  %d\n", visibility_seq[winIdx], visibility_vis[winIdx] );
  }

  if ( keyup_called[winIdx] )
  {
    bitmapPrintf ( "Key Up %d:  %d(%c) %d %d\n", keyup_seq[winIdx], keyup_key[winIdx], keyup_key[winIdx], keyup_x[winIdx], keyup_y[winIdx] );
  }

  if ( specialup_called[winIdx] )
  {
    bitmapPrintf ( "Special Up %d:  %d(%c) %d %d\n", specialup_seq[winIdx], specialup_key[winIdx], specialup_key[winIdx], specialup_x[winIdx], specialup_y[winIdx] );
  }

  if ( joystick_called[winIdx] )
  {
    bitmapPrintf ( "Joystick %d:  %d %d %d %d\n", joystick_seq[winIdx], joystick_a[winIdx], joystick_b[winIdx], joystick_c[winIdx], joystick_d[winIdx] );
  }

  if ( mouse_called[winIdx] )
  {
    bitmapPrintf ( "Mouse %d:  %d %d %d %d\n", mouse_seq[winIdx], mouse_button[winIdx], mouse_updown[winIdx], mouse_x[winIdx], mouse_y[winIdx] );
  }

  if ( mousewheel_called[winIdx] )
  {
    bitmapPrintf ( "Mouse Wheel %d:  %d %d %d %d\n", mousewheel_seq[winIdx], mousewheel_number[winIdx], mousewheel_direction[winIdx], mousewheel_x[winIdx], mousewheel_y[winIdx] );
  }

  if ( motion_called[winIdx] )
  {
    bitmapPrintf ( "Motion %d:  %d %d\n", motion_seq[winIdx], motion_x[winIdx], motion_y[winIdx] );
  }

  if ( passivemotion_called[winIdx] )
  {
    bitmapPrintf ( "Passive Motion %d:  %d %d\n", passivemotion_seq[winIdx], passivemotion_x[winIdx], passivemotion_y[winIdx] );
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
Warning(const char *fmt, va_list ap)
{
    printf("%6d Warning callback:\n");

    /* print warning message */
    vprintf(fmt, ap);
}

static void
Error(const char *fmt, va_list ap)
{
    char dummy_string[STRING_LENGTH];
    printf("%6d Error callback:\n");

    /* print warning message */
    vprintf(fmt, ap);

    /* terminate program, after pause for input so user can see */
    printf ( "Please enter something to exit: " );
    fgets ( dummy_string, STRING_LENGTH, stdin );

    /* Call exit directly as freeglut is messed
     * up internally when an error is called. 
     */
    exit(1);
}

static void 
Reshape(int width, int height)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Reshape Callback:  %d %d\n",
            ++sequence_number, window, width, height ) ;
  reshape_called[winIdx] = 1 ;
  reshape_width[winIdx] = width ;
  reshape_height[winIdx] = height ;
  reshape_seq[winIdx] = sequence_number ;
  glViewport(0,0,width,height);
  glutPostRedisplay () ;
}

static void 
Key(unsigned char key, int x, int y)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Keyboard Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
  key_called[winIdx] = 1 ;
  key_key[winIdx] = key ;
  key_x[winIdx] = x ;
  key_y[winIdx] = y ;
  key_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Special(int key, int x, int y)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Special Key Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
  special_called[winIdx] = 1 ;
  special_key[winIdx] = key ;
  special_x[winIdx] = x ;
  special_y[winIdx] = y ;
  special_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Visibility(int vis)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Visibility Callback:  %d\n",
            ++sequence_number, window, vis ) ;
  visibility_called[winIdx] = 1 ;
  visibility_vis[winIdx] = vis ;
  visibility_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
KeyUp(unsigned char key, int x, int y)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Key Release Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
  keyup_called[winIdx] = 1 ;
  keyup_key[winIdx] = key ;
  keyup_x[winIdx] = x ;
  keyup_y[winIdx] = y ;
  keyup_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
SpecialUp(int key, int x, int y)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Special Key Release Callback:  %d %d %d\n",
            ++sequence_number, window, key, x, y ) ;
  specialup_called[winIdx] = 1 ;
  specialup_key[winIdx] = key ;
  specialup_x[winIdx] = x ;
  specialup_y[winIdx] = y ;
  specialup_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Joystick( unsigned int a, int b, int c, int d)  /* Need meaningful names */
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Joystick Callback:  %d %d %d %d\n",
            ++sequence_number, window, a, b, c, d ) ;
  joystick_called[winIdx] = 1 ;
  joystick_a[winIdx] = a ;
  joystick_b[winIdx] = b ;
  joystick_c[winIdx] = c ;
  joystick_d[winIdx] = d ;
  joystick_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Mouse(int button, int updown, int x, int y)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Mouse Click Callback:  %d %d %d %d\n",
            ++sequence_number, window, button, updown, x, y ) ;
  mouse_called[winIdx] = 1 ;
  mouse_button[winIdx] = button ;
  mouse_updown[winIdx] = updown ;
  mouse_x[winIdx] = x ;
  mouse_y[winIdx] = y ;
  mouse_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
MouseWheel(int wheel_number, int direction, int x, int y)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Mouse Wheel Callback:  %d %d %d %d\n",
            ++sequence_number, window, wheel_number, direction, x, y ) ;
  mousewheel_called[winIdx] = 1 ;
  mousewheel_number[winIdx] = wheel_number ;
  mousewheel_direction[winIdx] = direction ;
  mousewheel_x[winIdx] = x ;
  mousewheel_y[winIdx] = y ;
  mousewheel_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Motion(int x, int y)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Mouse Motion Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
  motion_called[winIdx] = 1 ;
  motion_x[winIdx] = x ;
  motion_y[winIdx] = y ;
  motion_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
PassiveMotion(int x, int y)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Mouse Passive Motion Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
  passivemotion_called[winIdx] = 1 ;
  passivemotion_x[winIdx] = x ;
  passivemotion_y[winIdx] = y ;
  passivemotion_seq[winIdx] = sequence_number ;
  glutPostRedisplay () ;
}

static void 
Entry(int state)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d Entry Callback:  %d\n",
            ++sequence_number, window, state ) ;
  glutPostRedisplay () ;
}

static void 
Close(void)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d Close Callback\n",
            ++sequence_number, window ) ;
}

static void 
OverlayDisplay(void)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d OverlayDisplay Callback\n",
            ++sequence_number, window ) ;
  glutPostRedisplay () ;
}

static void 
WindowStatus(int state)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d WindowStatus Callback:  %d\n",
            ++sequence_number, window, state ) ;
  glutPostRedisplay () ;
}

static void 
SpaceMotion(int x, int y, int z)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d SpaceMotion Callback:  %d %d %d\n",
            ++sequence_number, window, x, y, z ) ;
  glutPostRedisplay () ;
}

static void 
SpaceRotation(int x, int y, int z)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d SpaceRotation Callback:  %d %d %d\n",
            ++sequence_number, window, x, y, z ) ;
  glutPostRedisplay () ;
}

static void 
SpaceButton(int button, int updown)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d SpaceButton Callback:  %d %d\n",
            ++sequence_number, window, button, updown ) ;
  glutPostRedisplay () ;
}

static void 
Dials(int x, int y)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d Dials Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
  glutPostRedisplay () ;
}

static void 
ButtonBox(int button, int updown)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d ButtonBox Callback:  %d %d\n",
            ++sequence_number, window, button, updown ) ;
  glutPostRedisplay () ;
}

static void 
TabletMotion(int x, int y)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d TabletMotion Callback:  %d %d\n",
            ++sequence_number, window, x, y ) ;
  glutPostRedisplay () ;
}

static void 
TabletButton(int button, int updown, int x, int y)
{
  int window = getWindowAndIdx(NULL);
  printf ( "%6d Window %d TabletButton Callback:  %d %d %d %d\n",
            ++sequence_number, window, button, updown, x, y ) ;
  glutPostRedisplay () ;
}

static void
MenuCallback ( int menuID )
{
  int window = getWindowAndIdx(NULL);
  printf( "%6d Window %d MenuCallback - menuID is %d\n",
          ++sequence_number, window, menuID );
}

static void 
MenuDestroy( void )
{
  int window = getWindowAndIdx(NULL);
  menudestroy_called = 1 ;
  printf ( "%6d Window %d MenuDestroy Callback\n",
            ++sequence_number, window ) ;

  if (window)   /* When destroyed when shutting down, not always a window defined... */
    glutPostRedisplay () ;
}

static void 
MenuStatus( int status, int x, int y )
{
  int window = getWindowAndIdx(NULL);
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
  char dummy_string[STRING_LENGTH];

  int menuID, subMenuA, subMenuB;

  glutInitWarningFunc(Warning);
  glutInitErrorFunc(Error);
  glutInitWindowSize(500, 250);
  glutInitWindowPosition ( 140, 140 );
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE );
  glutInit(&argc, argv);
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION);

  windows[0] = glutCreateWindow( "Callback Demo" );
  printf ( "Creating window %d as 'Callback Demo'\n", windows[0] ) ;

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
  glutMenuStatusFunc ( MenuStatus );
  glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF) ;

  subMenuA = glutCreateMenu( MenuCallback );
  glutAddMenuEntry( "Sub menu A1 (01)", 1 );
  glutAddMenuEntry( "Sub menu A2 (02)", 2 );
  glutAddMenuEntry( "Sub menu A3 (03)", 3 );
  glutMenuDestroyFunc ( MenuDestroy );

  subMenuB = glutCreateMenu( MenuCallback );
  glutAddMenuEntry( "Sub menu B1 (04)", 4 );
  glutAddMenuEntry( "Sub menu B2 (05)", 5 );
  glutAddMenuEntry( "Sub menu B3 (06)", 6 );
  glutAddSubMenu( "Going to sub menu A", subMenuA );
  glutMenuDestroyFunc ( MenuDestroy );

  menuID = glutCreateMenu( MenuCallback );
  glutAddMenuEntry( "Entry one",   1 );
  glutAddMenuEntry( "Entry two",   2 );
  glutAddMenuEntry( "Entry three", 3 );
  glutAddMenuEntry( "Entry four",  4 );
  glutAddMenuEntry( "Entry five",  5 );
  glutAddSubMenu( "Enter sub menu A", subMenuA );
  glutAddSubMenu( "Enter sub menu B", subMenuB );
  glutMenuDestroyFunc ( MenuDestroy );

  glutAttachMenu( GLUT_LEFT_BUTTON );

  glutInitWindowPosition ( 140+500+2*glutGet(GLUT_WINDOW_BORDER_WIDTH), 140 );
  /* Position second window right next to the first */
  windows[1] = glutCreateWindow( "Second Window" );
  printf ( "Creating window %d as 'Second Window'\n", windows[1] ) ;

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
