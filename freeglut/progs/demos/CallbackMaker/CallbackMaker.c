/* CallbackMaker.c */
/*
 * Program to invoke all the callbacks that "freeglut" supports
 */


#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static int sequence_number = 0 ;

#define CALLBACKMAKER_N_WINDOWS 4
int windows[CALLBACKMAKER_N_WINDOWS] = {0};

/* define status vars showing whether given callback has been called for given window */
#define CALLBACK_CALLED_VAR(name)                            int name##_called[CALLBACKMAKER_N_WINDOWS]   = {0};
#define CALLBACK_0V(name)                                    int name##_seq[CALLBACKMAKER_N_WINDOWS]      = {-1}; CALLBACK_CALLED_VAR(name); 
#define CALLBACK_1V(name,field)                              int name##_##field[CALLBACKMAKER_N_WINDOWS]  = {-1}; CALLBACK_0V(name);
#define CALLBACK_2V(name,field1,field2)                      int name##_##field2[CALLBACKMAKER_N_WINDOWS] = {-1}; CALLBACK_1V(name,field1);
#define CALLBACK_3V(name,field1,field2,field3)               int name##_##field3[CALLBACKMAKER_N_WINDOWS] = {-1}; CALLBACK_2V(name,field1,field2);
#define CALLBACK_4V(name,field1,field2,field3,field4)        int name##_##field4[CALLBACKMAKER_N_WINDOWS] = {-1}; CALLBACK_3V(name,field1,field2,field3);
#define CALLBACK_5V(name,field1,field2,field3,field4,field5) int name##_##field5[CALLBACKMAKER_N_WINDOWS] = {-1}; CALLBACK_4V(name,field1,field2,field3,field4);
CALLBACK_2V(reshape,width,height);
CALLBACK_2V(position,top,left);
CALLBACK_1V(visibility,vis);
CALLBACK_1V(windowStatus,state);
CALLBACK_4V(key,key,x,y,mod);
CALLBACK_4V(keyup,key,x,y,mod);
CALLBACK_4V(special,key,x,y,mod);
CALLBACK_4V(specialup,key,x,y,mod);
CALLBACK_4V(joystick,a,b,c,d);
CALLBACK_5V(mouse,button,updown,x,y,mod);
CALLBACK_5V(mousewheel,number,direction,x,y,mod);
CALLBACK_3V(motion,x,y,mod);
CALLBACK_3V(passivemotion,x,y,mod);
CALLBACK_1V(entry,state);
CALLBACK_0V(close);
/* menudestroy is registered on each menu, not a window */
int menudestroy_called = 0 ;
/* menustatus and menustate are global callbacks, set for all menus at the same time */
int menustatus_called = 0;
int menustate_called = 0;

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
        (*winIdx) = window==windows[0] ? 0 :
                    window==windows[1] ? 1 :
                    window==windows[2] ? 2 : 3;

    return window;
}

static void
Mod2Text(int mods, char *text)
{
    if (mods&GLUT_ACTIVE_CTRL)
        strcat(text,"CTRL");
    if (mods&GLUT_ACTIVE_SHIFT)
    {
        if (text[0])
            strcat(text,"+SHIFT");
        else
            strcat(text,"SHIFT");
    }
    if (mods&GLUT_ACTIVE_ALT)
    {
        if (text[0])
            strcat(text,"+ALT");
        else
            strcat(text,"ALT");
    }

    if (!text[0])
        strcat(text,"none");
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

  if ( entry_called[winIdx] )
  {
    bitmapPrintf ( "Entry %d:  %d\n", entry_seq[winIdx], entry_state[winIdx] );
  }
  
  if ( visibility_called[winIdx] )
  {
    bitmapPrintf ( "Visibility %d:  %d\n", visibility_seq[winIdx], visibility_vis[winIdx] );
  }

  if ( windowStatus_called[winIdx] )
  {
    bitmapPrintf ( "WindowStatus %d:  %d\n", windowStatus_seq[winIdx], windowStatus_state[winIdx] );
  }

  if ( reshape_called[winIdx] )
  {
    bitmapPrintf ( "Reshape %d:  %d %d\n", reshape_seq[winIdx], reshape_width[winIdx], reshape_height[winIdx] );
  }

  if ( position_called[winIdx] )
  {
    bitmapPrintf ( "Position %d:  %d %d\n", position_seq[winIdx], position_left[winIdx], position_top[winIdx] );
  }

  if ( key_called[winIdx] )
  {
    char mods[50] = {0};
    Mod2Text(key_mod[winIdx],mods);
    bitmapPrintf ( "Key %d:  %d(%c) %d %d (mod: %s)\n", key_seq[winIdx], key_key[winIdx], key_key[winIdx], key_x[winIdx], key_y[winIdx], mods );
  }

  if ( keyup_called[winIdx] )
  {
    char mods[50] = {0};
    Mod2Text(keyup_mod[winIdx],mods);
    bitmapPrintf ( "Key Up %d:  %d(%c) %d %d (mod: %s)\n", keyup_seq[winIdx], keyup_key[winIdx], keyup_key[winIdx], keyup_x[winIdx], keyup_y[winIdx], mods );
  }

  if ( special_called[winIdx] )
  {
    char mods[50] = {0};
    Mod2Text(special_mod[winIdx],mods);
    bitmapPrintf ( "Special %d:  %d(%c) %d %d (mod: %s)\n", special_seq[winIdx], special_key[winIdx], special_key[winIdx], special_x[winIdx], special_y[winIdx], mods );
  }

  if ( specialup_called[winIdx] )
  {
    char mods[50] = {0};
    Mod2Text(specialup_mod[winIdx],mods);
    bitmapPrintf ( "Special Up %d:  %d(%c) %d %d (mod: %s)\n", specialup_seq[winIdx], specialup_key[winIdx], specialup_key[winIdx], specialup_x[winIdx], specialup_y[winIdx], mods );
  }

  if ( joystick_called[winIdx] )
  {
    bitmapPrintf ( "Joystick %d:  %d %d %d %d\n", joystick_seq[winIdx], joystick_a[winIdx], joystick_b[winIdx], joystick_c[winIdx], joystick_d[winIdx] );
  }

  if ( mouse_called[winIdx] )
  {
    char mods[50] = {0};
    Mod2Text(mouse_mod[winIdx],mods);
    bitmapPrintf ( "Mouse %d:  %d %d %d %d (mod: %s)\n", mouse_seq[winIdx], mouse_button[winIdx], mouse_updown[winIdx], mouse_x[winIdx], mouse_y[winIdx], mods );
  }

  if ( mousewheel_called[winIdx] )
  {
    char mods[50] = {0};
    Mod2Text(mousewheel_mod[winIdx],mods);
    bitmapPrintf ( "Mouse Wheel %d:  %d %d %d %d (mod: %s)\n", mousewheel_seq[winIdx], mousewheel_number[winIdx], mousewheel_direction[winIdx], mousewheel_x[winIdx], mousewheel_y[winIdx], mods );
  }

  if ( motion_called[winIdx] )
  {
    char mods[50] = {0};
    Mod2Text(motion_mod[winIdx],mods);
    bitmapPrintf ( "Motion %d:  %d %d (mod: %s)\n", motion_seq[winIdx], motion_x[winIdx], motion_y[winIdx], mods );
  }

  if ( passivemotion_called[winIdx] )
  {
    char mods[50] = {0};
    Mod2Text(passivemotion_mod[winIdx],mods);
    bitmapPrintf ( "Passive Motion %d:  %d %d (mod: %s)\n", passivemotion_seq[winIdx], passivemotion_x[winIdx], passivemotion_y[winIdx], mods );
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
    printf("%6d Warning callback:\n",++sequence_number);

    /* print warning message */
    vprintf(fmt, ap);
}

static void
Error(const char *fmt, va_list ap)
{
    char dummy_string[STRING_LENGTH];
    printf("%6d Error callback:\n",++sequence_number);

    /* print warning message */
    vprintf(fmt, ap);
    printf("\n");

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
Position(int left, int top)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Position Callback:  %d %d\n",
            ++sequence_number, window, left, top ) ;
  position_called[winIdx] = 1 ;
  position_left[winIdx] = left ;
  position_top[winIdx] = top ;
  position_seq[winIdx] = sequence_number ;
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
  key_mod[winIdx] = glutGetModifiers() ;
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
  keyup_mod[winIdx] = glutGetModifiers() ;
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
  special_mod[winIdx] = glutGetModifiers() ;
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
  specialup_mod[winIdx] = glutGetModifiers() ;
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
  mouse_mod[winIdx] = glutGetModifiers() ;
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
  mousewheel_mod[winIdx] = glutGetModifiers() ;
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
  motion_mod[winIdx] = glutGetModifiers() ;
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
  passivemotion_mod[winIdx] = glutGetModifiers() ;
  glutPostRedisplay () ;
}

static void 
Entry(int state)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d Entry Callback:  %d\n",
            ++sequence_number, window, state ) ;
  entry_called[winIdx] = 1 ;
  entry_seq[winIdx] = sequence_number;
  entry_state[winIdx] = state;
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
WindowStatus(int state)
{
  int winIdx;
  int window = getWindowAndIdx(&winIdx);
  printf ( "%6d Window %d WindowStatus Callback:  %d\n",
            ++sequence_number, window, state ) ;
  windowStatus_called[winIdx] = 1 ;
  windowStatus_state[winIdx] = state ;
  windowStatus_seq[winIdx] = sequence_number ;
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
MenuCallback ( int value )
{
  int menu = glutGetMenu();
  int window = getWindowAndIdx(NULL);
  printf( "%6d Menu %d MenuCallback for menu opened in Window %d - value is %d\n",
          ++sequence_number, menu, window, value );
}

static void 
MenuDestroy( void )
{
  int menu = glutGetMenu();
  menudestroy_called = 1 ;
  printf ( "%6d Menu %d MenuDestroy Callback\n",
            ++sequence_number, menu ) ;
}

static void 
MenuStatus( int status, int x, int y )
{
  /* Menu and window for which this event is triggered are current when the callback is called */
  int menu = glutGetMenu();
  int window = getWindowAndIdx(NULL);
  menustatus_called = 1 ;
  printf ( "%6d Menu %d MenuStatus Callback in Window %d:  %d %d %d\n",
            ++sequence_number, menu, window, status, x, y ) ;
  glutPostRedisplay () ;
}


static void 
MenuState( int status )
{
  /* Menu and window for which this event is triggered are current when the callback is called */
  int menu = glutGetMenu();
  int window = getWindowAndIdx(NULL);
  menustate_called = 1 ;
  printf ( "%6d Menu %d MenuState Callback in Window %d:  %d\n",
            ++sequence_number, menu, window, status) ;
  glutPostRedisplay () ;
}

static void Idle ( void )
{
  ++sequence_number ;
}

static void SetWindowCallbacks( int first )
{
    /* All these callbacks are set for only the current window */
    glutDisplayFunc( Display );
    glutReshapeFunc( Reshape );
    glutPositionFunc( Position );
    glutKeyboardFunc( Key );
    glutSpecialFunc( Special );
    glutKeyboardUpFunc( KeyUp );
    glutSpecialUpFunc( SpecialUp );
    if (first)
        glutJoystickFunc( Joystick, 100 );
    glutMouseFunc ( Mouse ) ;
    glutMouseWheelFunc ( MouseWheel ) ;
    glutMotionFunc ( Motion ) ;
    glutPassiveMotionFunc ( PassiveMotion ) ;
    glutEntryFunc ( Entry ) ;
    glutCloseFunc ( Close ) ;
    glutOverlayDisplayFunc ( OverlayDisplay ) ;
    glutSpaceballMotionFunc ( SpaceMotion ) ;
    glutSpaceballRotateFunc ( SpaceRotation ) ;
    glutSpaceballButtonFunc ( SpaceButton ) ;
    glutButtonBoxFunc ( ButtonBox ) ;
    glutDialsFunc ( Dials ) ;
    glutTabletMotionFunc ( TabletMotion ) ;
    glutTabletButtonFunc ( TabletButton ) ;
    /* glutVisibilityFunc is deprecated in favor of glutWindowStatusFunc, which provides more detail.
     * Setting one of these overwrites the other (see docs).
     */
    glutVisibilityFunc ( Visibility );  /* This will thus never be called, as glutWindowStatusFunc is set afterwards */
    glutWindowStatusFunc ( WindowStatus ) ;
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
  /* global setting: mainloop does not return when a window is closed, only returns when all windows are closed */
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION);
  /* global setting: repeated keys generating by keeping the key pressed down are passed on to the keyboard callback */
  /* There are two functions to control this behavior, glutSetKeyRepeat to set it globally, and glutIgnoreKeyRepeat to set it per window.
   * These two interact however. If key repeat is globally switched off (glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF)), it cannot be overridden
   * (switched back on) for a specific window with glutIgnoreKeyRepeat. However, if key repeat is globally switched on
   * (glutSetKeyRepeat(GLUT_KEY_REPEAT_ON)), it can be overridden (switched off) with glutIgnoreKeyRepeat on a per-window basis. That is
   * what we demonstrate here.
   */
  glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);

  /* Set other global callback (global as in not associated with any specific menu or window) */
  glutIdleFunc ( Idle );
  glutMenuStatusFunc ( MenuStatus );
  glutMenuStateFunc  ( MenuState ); /* Note that glutMenuStatusFunc is an enhanced version of the deprecated glutMenuStateFunc. */


  /* Open first window */
  windows[0] = glutCreateWindow( "Callback Demo" );
  printf ( "Creating window %d as 'Callback Demo'\n", windows[0] ) ;

  glClearColor(1.0, 1.0, 1.0, 1.0);

  /* callbacks, settings and menus for this window */
  SetWindowCallbacks( 1 );
  glutIgnoreKeyRepeat(GL_TRUE);
  glutSetIconTitle("Icon Test - Callback Demo");

  subMenuA = glutCreateMenu( MenuCallback );
  glutAddMenuEntry( "Sub menu A1 (01)", 11 );
  glutAddMenuEntry( "Sub menu A2 (02)", 12 );
  glutAddMenuEntry( "Sub menu A3 (03)", 13 );
  glutMenuDestroyFunc ( MenuDestroy );  /* callback specific to this menu */
  /* Change font for this menu */
  glutSetMenuFont(subMenuA, GLUT_BITMAP_HELVETICA_12);

  subMenuB = glutCreateMenu( MenuCallback );
  glutAddMenuEntry( "Sub menu B1 (04)", 14 );
  glutAddMenuEntry( "Sub menu B2 (05)", 15 );
  glutAddMenuEntry( "Sub menu B3 (06)", 16 );
  glutAddSubMenu( "Going to sub menu A", subMenuA );
  glutMenuDestroyFunc ( MenuDestroy );  /* callback specific to this menu */
  glutSetMenuFont(subMenuB, GLUT_BITMAP_9_BY_15);

  menuID = glutCreateMenu( MenuCallback );
  glutAddMenuEntry( "Entry one",   21 );
  glutAddMenuEntry( "Entry two",   22 );
  glutAddMenuEntry( "Entry three", 23 );
  glutAddMenuEntry( "Entry four",  24 );
  glutAddMenuEntry( "Entry five",  25 );
  glutAddSubMenu( "Enter sub menu A", subMenuA );
  glutAddSubMenu( "Enter sub menu B", subMenuB );
  glutMenuDestroyFunc ( MenuDestroy );  /* callback specific to this menu */

  glutAttachMenu( GLUT_LEFT_BUTTON );


  /* Position second window right next to the first */
  glutInitWindowPosition ( 140+500+2*glutGet(GLUT_WINDOW_BORDER_WIDTH), 140 );
  glutInitWindowSize(600, 600);
  windows[1] = glutCreateWindow( "Second Window" );
  printf ( "Creating window %d as 'Second Window'\n", windows[1] ) ;

  glClearColor(1.0, 1.0, 1.0, 1.0);

  /* callbacks, settings and menus for this window */
  SetWindowCallbacks( 0 );
  glutIgnoreKeyRepeat(GL_TRUE);

  glutSetMenu(subMenuB);
  glutAttachMenu( GLUT_RIGHT_BUTTON );


  /* position a third window as a subwindow of the second */
  windows[2] = glutCreateSubWindow(windows[1],0,300,600,300);
  printf ( "Creating window %d as subwindow to 'Second Window'\n", windows[2] ) ;

  glClearColor(0.7f, 0.7f, 0.7f, 1.0);

  /* callbacks, settings and menus for this window */
  SetWindowCallbacks( 0 );
  glutSetCursor(GLUT_CURSOR_CROSSHAIR); /* Cursors are per window */

  glutSetMenu(subMenuA);
  glutAttachMenu( GLUT_RIGHT_BUTTON );


  /* position a fourth window as a subsubwindow (grandchild) of the second */
  windows[3] = glutCreateSubWindow(windows[2],300,0,300,300);
  printf ( "Creating window %d as subsubwindow to 'Second Window'\n", windows[3] ) ;

  glClearColor(0.4f, 0.4f, 0.4f, 1.0);

  /* callbacks and menus for this window */
  SetWindowCallbacks( 0 );
  glutSetCursor(GLUT_CURSOR_INHERIT);   /* Inherit cursor look from parent (this is default on window creation) - comment the below to see in action */
  glutSetCursor(GLUT_CURSOR_CYCLE);


  printf ( "Please enter something to continue: " );
  fgets ( dummy_string, STRING_LENGTH, stdin );

  glutMainLoop();

  printf ( "Back from the 'freeglut' main loop\n" ) ;

  return EXIT_SUCCESS;
}
