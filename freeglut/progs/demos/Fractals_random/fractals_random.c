/* fractals_random.c */
/* This demo shows a single-buffering "freeglut" example. */

/*
 * Program to draw a fractal by Michael Barnsley's stochastic algorithm.
 * Algorithm:
 *  (1) Define the affine transformations (of the form r(i+1) = A r(i) + b )
 *  (2) Find the stationary point for the first transformation
 *  (3) To draw:
 *        - Pick a random integer between 1 and the number of transformations (inclusive)
 *        - Send the current point through the transformation to create the new current point
 *        - Plot the new current point
 */

/*
 * User Commands:
 *  PgUp, PgDn - increase/decrease scaling
 *  Arrow keys - translate viewing section
 *  r - reset view
 *  Escape - quit
 */

#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef WIN32
#include <crtdbg.h>  // DUMP MEMORY LEAKS
#endif

typedef struct
{
  double a00, a01, a10, a11 ;   /* Transformation matrix */
  double b0, b1 ;               /* Constant vector added on */
  double statx, staty ;         /* Coordinates of the stationary point */
}
AffineTrans ;

/* Number of levels to draw the fractal */
static int num_levels = 0 ;

/* The definition of the fractal */
static int num_trans ;
static AffineTrans *affine ;

/* the window title */
char window_title [ 80 ] ;

/* The amount the view is translated */
double xwin = 0.0, ywin = 0.0 ;
double scale_factor = 1.0 ;

/* The current point */
double current_x = 0.0, current_y = 0.0 ;

/* Signals when a glClear is needed */
static GLboolean needClear = GL_TRUE;

static void draw_level ( int num, double m00, double m01, double m10, double m11, double n0, double n1 )
{
  /* Draw a fractal transformed by "M", "N" as passed in */
  int i ;

  for ( i = 0; i < 10; i++ )
  {
    int random = (rand() >> 10) % num_trans;
    double new_x = affine[random].a00 * current_x + affine[random].a01 * current_y + affine[random].b0 ;
    double new_y = affine[random].a10 * current_x + affine[random].a11 * current_y + affine[random].b1 ;

    glVertex2f ( new_x, new_y ) ;
    current_x = new_x ;
    current_y = new_y ;
  }
}

static void 
Display(void)
{
  if (needClear) {
    glClear(GL_COLOR_BUFFER_BIT);
    needClear = GL_FALSE;
  }

  /* the curve */
  glPushMatrix();
  glScaled(2.5, 2.5, 2.5);

  glColor4f(0.0, 0.0, 0.0, 1.0);
  glBegin ( GL_POINTS ) ;
  draw_level ( num_levels, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 );
  glEnd () ;

  glPopMatrix();

  glFlush();
  glutPostRedisplay();  /* Needed so that this function will be called again */
}

static void 
Reshape(int width, int height)
{
  float ar;
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  ar = (float) width / (float) height;
  if( ar > 1 )
      glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
  else
      glFrustum(-1.0, 1.0, -1/ar, 1/ar, 2.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  xwin = -1.0 ;
  ywin =  0.0 ;
  glTranslated(xwin, ywin, -5.0);
  needClear = GL_TRUE;
}

static void 
Key(unsigned char key, int x, int y)
{
  int changed_settings = 1;
  
  switch (key) {
  case 27:  /* Escape key */
    glutLeaveMainLoop ();
    break;

  case 'r' :  case 'R' :
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    xwin = -1.0 ;
    ywin = 0.0 ;
    glTranslated(xwin, ywin, -5.0);
    break ;

  default:
    changed_settings = 0;
    break;
  }
  if (changed_settings)
    needClear = GL_TRUE;
  glutPostRedisplay();
}

static void 
Special(int key, int x, int y)
{
  int changed_settings = 1;

  switch (key) {
  case GLUT_KEY_UP :
    glMatrixMode(GL_MODELVIEW);
    ywin += 0.1 * scale_factor ;
    glTranslated(0.0, 0.1 * scale_factor, 0.0);
    break ;

  case GLUT_KEY_DOWN :
    glMatrixMode(GL_MODELVIEW);
    ywin -= 0.1 * scale_factor ;
    glTranslated(0.0, -0.1 * scale_factor, 0.0);
    break ;

  case GLUT_KEY_LEFT :
    glMatrixMode(GL_MODELVIEW);
    xwin -= 0.1 * scale_factor ;
    glTranslated(-0.1 * scale_factor, 0.0, 0.0);
    break ;

  case GLUT_KEY_RIGHT :
    glMatrixMode(GL_MODELVIEW);
    xwin += 0.1 * scale_factor ;
    glTranslated(0.1 * scale_factor, 0.0, 0.0);
    break ;

  case GLUT_KEY_PAGE_UP :
    glMatrixMode(GL_MODELVIEW);
    glTranslated ( -xwin, -ywin, 0.0 ) ;
    glScaled(1.25, 1.25, 1.25);
    glTranslated ( xwin, ywin, 0.0 ) ;
    scale_factor *= 0.8 ;
    break ;

  case GLUT_KEY_PAGE_DOWN :
    glMatrixMode(GL_MODELVIEW);
    glTranslated ( -xwin, -ywin, 0.0 ) ;
    glScaled(0.8, 0.8, 0.8);
    glTranslated ( xwin, ywin, 0.0 ) ;
    scale_factor *= 1.25 ;
    break ;

  default:
    changed_settings = 0;
    break;
  }
  if (changed_settings)
    needClear = GL_TRUE;

  glutPostRedisplay();
}


void readConfigFile ( char *fnme )
{
  FILE *fptr = fopen ( fnme, "rt" ) ;
  int i ;
  char inputline [ 256 ] ;

  if ( fptr )
  {
    /* Read a header line */
    fgets ( inputline, 256, fptr ) ;

    /* Read a comment line */
    fgets ( inputline, 256, fptr ) ;

    /* Read the window title */
    fgets ( inputline, 256, fptr ) ;
    /* We assume here that this line will not exceed 79 characters plus a 
       newline (window_title is 80 characters long). That'll cause a buffer 
       overflow. For a simple program like  this, though, we're letting it 
       slide! 
    */
    sscanf ( inputline, "%[a-zA-Z0-9!@#$%^&*()+=/\\_-\" ]", window_title ) ; 

    /* Read a comment line */
    fgets ( inputline, 256, fptr ) ;

    /* Read the number of affine transformations */
    fgets ( inputline, 256, fptr ) ;
    sscanf ( inputline, "%d", &num_trans ) ;

    affine = (AffineTrans *)malloc ( num_trans * sizeof(AffineTrans) ) ;

    /* Read a comment line */
    fgets ( inputline, 256, fptr ) ;

    for ( i = 0; i < num_trans; i++ )
    {
      /* Read an affine transformation definition */
      fgets ( inputline, 256, fptr ) ;
      sscanf ( inputline, "%lf %lf %lf %lf %lf %lf", &affine[i].a00, &affine[i].a01,
                       &affine[i].a10, &affine[i].a11, &affine[i].b0, &affine[i].b1 ) ;
    }
  }
  else  /* No data file, set a default */
  {
    printf ( "ERROR opening file <%s>\n", fnme ) ;
    strcpy ( window_title, "Cantor Dust" ) ;
    num_trans = 2 ;
    affine = (AffineTrans *)malloc ( num_trans * sizeof(AffineTrans) ) ;
    affine[0].a00 = 0.25 ;  affine[0].a01 = 0.00 ;  affine[0].a10 = 0.00 ;  affine[0].a11 = 0.25 ;
    affine[0].b0 = 0.0 ;    affine[0].b1 = 0.0 ;
    affine[1].a00 = 0.25 ;  affine[1].a01 = 0.00 ;  affine[1].a10 = 0.00 ;  affine[1].a11 = 0.25 ;
    affine[1].b0 = 0.5 ;    affine[1].b1 = 0.0 ;
  }

  for ( i = 0; i < num_trans; i++ )
  {
    double m00, m01, m10, m11 ;  /* Matrix "I" minus "A" */
    double determ ;              /* Determinant of this matrix */

    /* Calculate the stationary point */

    m00 = 1.0 - affine[i].a00 ;
    m01 =     - affine[i].a01 ;
    m10 =     - affine[i].a10 ;
    m11 = 1.0 - affine[i].a11 ;

    determ = m00 * m11 - m01 * m10 ;

    if ( fabs ( determ ) > 1.e-6 )
    {
      affine[i].statx = (  m11 * affine[i].b0 - m01 * affine[i].b1 ) / determ ;
      affine[i].staty = ( -m10 * affine[i].b0 + m00 * affine[i].b1 ) / determ ;
    }
    else
      affine[i].statx = affine[i].staty = 0.0 ;
  }
}

int 
main(int argc, char *argv[])
{
  int fractal_window ;

  glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE );

  glutInitWindowSize(500, 250);
  glutInitWindowPosition ( 140, 140 ) ;
  glutInit(&argc, argv);

  if ( argc > 1 )
    readConfigFile ( argv[1] ) ;
  else
    readConfigFile ( "fractals.dat" ) ;

  fractal_window = glutCreateWindow( window_title );

  glClearColor(1.0, 1.0, 1.0, 1.0);

  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Key);
  glutSpecialFunc(Special);
  glutDisplayFunc(Display);

  glutMainLoop();

  printf ( "Back from the 'freeglut' main loop\n" ) ;

  free ( affine ) ;

#ifdef WIN32
  _CrtDumpMemoryLeaks () ;  // DUMP MEMORY LEAK INFORMATION
#endif

  return 0;             /* ANSI C requires main to return int. */
}
