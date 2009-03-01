/*
 * Lorenz Strange Attractor
 *
 * Written by John F. Fay in honor of the "freeglut" 2.0.0 release in July 2003
 *
 * What it does:
 *  This program starts with two particles right next to each other.  The particles
 *  move through a three-dimensional phase space governed by the following equations:
 *       dx/dt = sigma * ( y - x )
 *       dy/dt = r * x - y + x * z
 *       dz/dt = x * y + b * z
 *  These are the Lorenz equations and define the "Lorenz Attractor."  Any two particles
 *  arbitrarily close together will move apart as time increases, but their tracks are
 *  confined within a region of the space.
 *
 * Commands:
 *  Arrow keys:  Rotate the view
 *  PgUp, PgDn:  Zoom in and out
 *  Mouse click:  Center on the nearest point on a particle trajectory
 *
 *  'r'/'R':  Reset the simulation
 *  'm'/'M':  Modify the Lorenz parameters (in the text window)
 *  's'/'S':  Stop (the advancement in time)
 *  'g'/'G':  Go
 *  <spacebar>:  Single-step
 *  <Escape>:  Quit
 */

/* Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <GL/freeglut.h>
#ifdef _MSC_VER
/* DUMP MEMORY LEAKS */
#include <crtdbg.h>
#endif


/************************************** Defined Constants ***************************************/
/* Number of points to draw in the curves */
#define NUM_POINTS    512

/* Angle to rotate when the user presses an arrow key */
#define ROTATION_ANGLE  5.0

/* Amount to scale bu when the user presses PgUp or PgDn */
#define SCALE_FACTOR     0.8


/*************************************** Global Variables ***************************************/
/* Lorenz Attractor variables */
double s0 = 10.0, r0 = 28.0, b0 = 8.0/3.0 ;   /* Default Lorenz attactor parameters */
double time_step = 0.03 ;                     /* Time step in the simulation */
double sigma = 10.0, r = 28.0, b = 8.0/3.0 ;  /* Lorenz attactor parameters */
double red_position[NUM_POINTS][3] ;          /* Path of the red point */
double grn_position[NUM_POINTS][3] ;          /* Path of the green point */
int array_index ;                             /* Position in *_position arrays of most recent point */
double distance = 0.0 ;                       /* Distance between the two points */

/* GLUT variables */
double yaw = 0.0, pit = 0.0 ;                 /* Euler angles of the viewing rotation */
double scale = 1.0 ;                          /* Scale factor */
double xcen = 0.0, ycen = 0.0, zcen = 0.0 ;   /* Coordinates of the point looked at */

int animate = 1 ;                             /* 0 - stop, 1 = go, 2 = single-step */


/******************************************* Functions ******************************************/

/* The Lorenz Attractor */
void calc_deriv ( double position[3], double deriv[3] )
{
  /* Calculate the Lorenz attractor derivatives */
  deriv[0] = sigma * ( position[1] - position[0] ) ;
  deriv[1] = ( r + position[2] ) * position[0] - position[1] ;
  deriv[2] = -position[0] * position[1] - b * position[2] ;
}

void advance_in_time ( double time_step, double position[3], double new_position[3] )
{
  /* Move a point along the Lorenz attractor */
  double deriv0[3], deriv1[3], deriv2[3], deriv3[3] ;
  int i ;
  memcpy ( new_position, position, 3 * sizeof(double) ) ;  /* Save the present values */

  /* First pass in a Fourth-Order Runge-Kutta integration method */
  calc_deriv ( position, deriv0 ) ;
  for ( i = 0; i < 3; i++ )
    new_position[i] = position[i] + 0.5 * time_step * deriv0[i] ;

  /* Second pass */
  calc_deriv ( new_position, deriv1 ) ;
  for ( i = 0; i < 3; i++ )
    new_position[i] = position[i] + 0.5 * time_step * deriv1[i] ;

  /* Third pass */
  calc_deriv ( position, deriv2 ) ;
  for ( i = 0; i < 3; i++ )
    new_position[i] = position[i] + time_step * deriv2[i] ;

  /* Second pass */
  calc_deriv ( new_position, deriv3 ) ;
  for ( i = 0; i < 3; i++ )
    new_position[i] = position[i] + 0.1666666666666666667 * time_step *
                      ( deriv0[i] + 2.0 * ( deriv1[i] + deriv2[i] ) + deriv3[i] ) ;
}

static void
checkedFGets ( char *s, int size, FILE *stream )
{
  if ( fgets ( s, size, stream ) == NULL ) {
    fprintf ( stderr, "fgets failed\n");
    exit ( EXIT_FAILURE );
  }
}


/* GLUT callbacks */

#define INPUT_LINE_LENGTH 80

void key_cb ( unsigned char key, int x, int y )
{
  int i ;
  char inputline [ INPUT_LINE_LENGTH ] ;

  switch ( key )
  {
  case 'r' :  case 'R' :  /* Reset the simulation */
    /* Reset the Lorenz parameters */
    sigma = s0 ;
    b = b0 ;
    r = r0 ;
    /* Set an initial position */
    red_position[0][0] = (double)rand() / (double)RAND_MAX ;
    red_position[0][1] = (double)rand() / (double)RAND_MAX ;
    red_position[0][2] = (double)rand() / (double)RAND_MAX ;
    grn_position[0][0] = (double)rand() / (double)RAND_MAX ;
    grn_position[0][1] = (double)rand() / (double)RAND_MAX ;
    grn_position[0][2] = (double)rand() / (double)RAND_MAX ;
    array_index = 0 ;
    /* Initialize the arrays */
    for ( i = 1; i < NUM_POINTS; i++ )
    {
      memcpy ( red_position[i], red_position[0], 3 * sizeof(double) ) ;
      memcpy ( grn_position[i], grn_position[0], 3 * sizeof(double) ) ;
    }

    break ;

  case 'm' :  case 'M' :  /* Modify the Lorenz parameters */
    printf ( "Please enter new value for <sigma> (default %f, currently %f): ", s0, sigma ) ;
    checkedFGets ( inputline, sizeof ( inputline ), stdin ) ;
    sscanf ( inputline, "%lf", &sigma ) ;

    printf ( "Please enter new value for <b> (default %f, currently %f): ", b0, b ) ;
    checkedFGets ( inputline, sizeof ( inputline ), stdin ) ;
    sscanf ( inputline, "%lf", &b ) ;

    printf ( "Please enter new value for <r> (default %f, currently %f): ", r0, r ) ;
    checkedFGets ( inputline, sizeof ( inputline ), stdin ) ;
    sscanf ( inputline, "%lf", &r ) ;

    break ;

  case 's' :  case 'S' :  /* Stop the animation */
    animate = 0 ;
    break ;

  case 'g' :  case 'G' :  /* Start the animation */
    animate = 1 ;
    break ;

  case ' ' :  /* Spacebar:  Single step */
    animate = 2 ;
    break ;

  case 27 :  /* Escape key */
    glutLeaveMainLoop () ;
    break ;
  }
}

void special_cb ( int key, int x, int y )
{
  switch ( key )
  {
  case GLUT_KEY_UP :  /* Rotate up a little */
    glRotated ( ROTATION_ANGLE, 0.0, 1.0, 0.0 ) ;
    break ;

  case GLUT_KEY_DOWN :  /* Rotate down a little */
    glRotated ( -ROTATION_ANGLE, 0.0, 1.0, 0.0 ) ;
    break ;

  case GLUT_KEY_LEFT :  /* Rotate left a little */
    glRotated ( ROTATION_ANGLE, 0.0, 0.0, 1.0 ) ;
    break ;

  case GLUT_KEY_RIGHT :  /* Rotate right a little */
    glRotated ( -ROTATION_ANGLE, 0.0, 0.0, 1.0 ) ;
    break ;

  case GLUT_KEY_PAGE_UP :  /* Zoom in a little */
    glScaled ( 1.0 / SCALE_FACTOR, 1.0 / SCALE_FACTOR, 1.0 / SCALE_FACTOR ) ;
    break ;

  case GLUT_KEY_PAGE_DOWN :  /* Zoom out a little */
    glScaled ( SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR ) ;
    break ;
  }

  glutPostRedisplay () ;
}

void mouse_cb ( int button, int updown, int x, int y )
{
  if ( updown == GLUT_DOWN )
  {
    double dist = 1.0e20 ;  /* A very large number */
    dist = 0.0 ;  /* so we don't get "unused variable" compiler warning */
    /* The idea here is that we go into "pick" mode and pick the nearest point
       to the mouse click position.  Unfortunately I don't have the time to implement
       it at the moment. */
  }
}

void draw_curve ( int index, double position [ NUM_POINTS ][3] )
{
  int i = index ;

  glBegin ( GL_LINE_STRIP ) ;
  do
  {
    i = ( i == NUM_POINTS-1 ) ? 0 : i + 1 ;
    glVertex3dv ( position[i] ) ;
  }
  while ( i != index ) ;

  glEnd () ;
}

void bitmapPrintf (const char *fmt, ...)
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

void display_cb ( void )
{
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;

  glColor3d ( 1.0, 1.0, 1.0 ) ;  /* White */
  /* Draw some axes */
  glBegin ( GL_LINES ) ;
  glVertex3d ( 0.0, 0.0, 0.0 ) ;
  glVertex3d ( 2.0, 0.0, 0.0 ) ;
  glVertex3d ( 0.0, 0.0, 0.0 ) ;
  glVertex3d ( 0.0, 1.0, 0.0 ) ;
  glVertex3d ( 0.0, 0.0, 0.0 ) ;
  glVertex3d ( 0.0, 0.0, 1.0 ) ;
  glEnd () ;

  glColor3d ( 1.0, 0.0, 0.0 ) ;  /* Red */
  draw_curve ( array_index, red_position ) ;

  glColor3d ( 0.0, 1.0, 0.0 ) ;  /* Green */
  draw_curve ( array_index, grn_position ) ;

  /* Print the distance between the two points */
  glColor3d ( 1.0, 1.0, 1.0 ) ;  /* White */
  glRasterPos2i ( 1, 1 ) ;
  bitmapPrintf ( "Distance: %10.6f", distance ) ;

  glutSwapBuffers();
}

void reshape_cb ( int width, int height )
{
  float ar;
  glViewport ( 0, 0, width, height ) ;
  glMatrixMode ( GL_PROJECTION ) ;
  glLoadIdentity () ;
  ar = (float) width / (float) height ;
  glFrustum ( -ar, ar, -1.0, 1.0, 10.0, 100.0 ) ;
  glMatrixMode ( GL_MODELVIEW ) ;
  glLoadIdentity () ;
  xcen = 0.0 ;
  ycen = 0.0 ;
  zcen = 0.0 ;
  glTranslated ( xcen, ycen, zcen - 50.0 ) ;
}


void timer_cb ( int value )
{
  /* Function called at regular intervals to update the positions of the points */
  double deltax, deltay, deltaz ;
  int new_index = array_index + 1 ;

  /* Set the next timed callback */
  glutTimerFunc ( 30, timer_cb, 0 ) ;

  if ( animate > 0 )
  {
    if ( new_index == NUM_POINTS ) new_index = 0 ;
    advance_in_time ( time_step, red_position[array_index], red_position[new_index] ) ;
    advance_in_time ( time_step, grn_position[array_index], grn_position[new_index] ) ;
    array_index = new_index ;

    deltax = red_position[array_index][0] - grn_position[array_index][0] ;
    deltay = red_position[array_index][1] - grn_position[array_index][1] ;
    deltaz = red_position[array_index][2] - grn_position[array_index][2] ;
    distance = sqrt ( deltax * deltax + deltay * deltay + deltaz * deltaz ) ;

    if ( animate == 2 ) animate = 0 ;
  }

  glutPostRedisplay () ;
}



/* The Main Program */

int main ( int argc, char *argv[] )
{
  int pargc = argc ;

  /* Initialize the random number generator */
  srand ( 1023 ) ;

  /* Set up the OpenGL parameters */
  glEnable ( GL_DEPTH_TEST ) ;
  glClearColor ( 0.0, 0.0, 0.0, 0.0 ) ;
  glClearDepth ( 1.0 ) ;

  /* Initialize GLUT */
  glutInitWindowSize ( 600, 600 ) ;
  glutInit ( &pargc, argv ) ;
  glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH ) ;

  /* Create the window */
  glutCreateWindow ( "Lorenz Attractor" ) ;
  glutKeyboardFunc ( key_cb ) ;
  glutMouseFunc ( mouse_cb ) ;
  glutSpecialFunc ( special_cb ) ;
  glutDisplayFunc ( display_cb ) ;
  glutReshapeFunc ( reshape_cb ) ;
  glutTimerFunc ( 30, timer_cb, 0 ) ;

  /* Initialize the attractor:  The easiest way is to call the keyboard callback with an
   * argument of 'r' for Reset.
   */
  key_cb ( 'r', 0, 0 ) ;

  /* Enter the GLUT main loop */
  glutMainLoop () ;

#ifdef _MSC_VER
  /* DUMP MEMORY LEAK INFORMATION */
  _CrtDumpMemoryLeaks () ;
#endif

  return 0 ;
}

