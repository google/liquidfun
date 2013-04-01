/* fractals.c */
/*
 * Program to draw a fractal by Michael Barnsley's deterministic algorithm.
 * Algorithm:
 *  (1) Define the affine transformations (of the form r(i+1) = A r(i) + b )
 *  (2) Find the stationary point for each transformation
 *  (3) To draw:
 *        - If you are at the lowest level, draw lines connecting all the stationary points
 *        - If not, call the draw function recursively with each affine transformation applied
 */

/*
 * User Commands:
 *  +,- - increment/decrement number of levels
 *  PgUp, PgDn - increase/decrease scaling
 *  Arrow keys - translate viewing section
 *  r - reset view
 *  Escape - quit
 */

#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define FGH_PI       3.14159265358979323846

typedef struct
{
  double a00, a01, a10, a11 ;   /* Transformation matrix */
  double b0, b1 ;               /* Constant vector added on */
  double statx, staty ;         /* Coordinates of the stationary point */
}
AffineTrans ;

/* Number of levels to draw the fractal */
static int num_levels = 4 ;

/* The definition of the fractal */
static int num_trans ;
static AffineTrans *affine ;

/* Flag telling us to keep executing the main loop */
static int continue_in_main_loop = 1;

/* the window title */
char window_title [ 80 ] ;

/* The amount the view is translated and scaled */
double xwin = 0.0, ywin = 0.0 ;
double scale_factor = 1.0 ;

static void draw_level ( int num, double m00, double m01, double m10, double m11, double n0, double n1 )
{
  /* Draw a fractal transformed by "M", "N" as passed in */
  int i ;

  if ( num == 0 )
  {
    double x0 = m00 * affine[0].statx + m01 * affine[0].staty + n0 ;
    double y0 = m10 * affine[0].statx + m11 * affine[0].staty + n1 ;

    for ( i = 1; i < num_trans; i++ )
    {
      double x1 = m00 * affine[i].statx + m01 * affine[i].staty + n0 ;
      double y1 = m10 * affine[i].statx + m11 * affine[i].staty + n1 ;

      glVertex2d ( x0, y0 ) ;
      glVertex2d ( x1, y1 ) ;

      x0 = x1 ;
      y0 = y1 ;
    }
  }
  else
  {
    /* Map each affine transformation in the fractal through the one passed in and call "draw_level" */

    for ( i = 0; i < num_trans; i++ )
    {
      draw_level ( num-1, m00*affine[i].a00+m01*affine[i].a10,     m00*affine[i].a01+m01*affine[i].a11,
                          m10*affine[i].a00+m11*affine[i].a10,     m10*affine[i].a01+m11*affine[i].a11,
                          m00*affine[i].b0 +m01*affine[i].b1 + n0, m10*affine[i].b0 +m11*affine[i].b1  + n1 ) ;
    }
  }
}

static void 
Display(void)
{
  glClear( GL_COLOR_BUFFER_BIT );

  /* the curve */
  glPushMatrix();
  glScalef(2.5, 2.5, 2.5);

  glColor4f(0.0, 0.0, 0.0, 1.0);
  glBegin ( GL_LINES ) ;
  draw_level ( num_levels, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 );
  glEnd () ;

  glPopMatrix();
  glutSwapBuffers();
}

static void 
Reshape(int width, int height)
{
  float ar;
  glViewport ( 0, 0, width, height ) ;
  glMatrixMode ( GL_PROJECTION ) ;
  glLoadIdentity();
  ar = (float) width / (float) height ;
  if( ar > 1 )
      glFrustum ( -ar, ar, -1.0, 1.0, 2.0, 100.0 ) ;
  else
      glFrustum ( -1.0, 1.0, -1/ar, 1/ar, 2.0, 100.0 );
  glMatrixMode ( GL_MODELVIEW ) ;
  glLoadIdentity () ;
  xwin = -1.0 ;
  ywin =  0.0 ;
  glTranslated ( xwin, ywin, -5.0 ) ;
}

static void 
Key(unsigned char key, int x, int y)
{
  int need_redisplay = 1;
  
  switch (key) {
  case 27:  /* Escape key */
    continue_in_main_loop = 0 ;
    break;

  case '+' :
    ++num_levels ;
    break ;

  case '-' :
    if ( num_levels > 0 )
      --num_levels ;
    break ;

  case 'r' :  case 'R' :
    glMatrixMode ( GL_MODELVIEW ) ;
    glLoadIdentity();
    xwin = -1.0 ;
    ywin = 0.0 ;
    glTranslated ( xwin, ywin, -5.0 ) ;
    break ;

  default:
    need_redisplay = 0;
    break;
  }
  if (need_redisplay)
    glutPostRedisplay();
}

static void 
Special(int key, int x, int y)
{
  int need_redisplay = 1;

  switch (key) {
  case GLUT_KEY_UP :
    glMatrixMode ( GL_MODELVIEW ) ;
    ywin += 0.1 * scale_factor ;
    glTranslated ( 0.0, 0.1 * scale_factor, 0.0 ) ;
    break ;

  case GLUT_KEY_DOWN :
    glMatrixMode ( GL_MODELVIEW ) ;
    ywin -= 0.1 * scale_factor ;
    glTranslated ( 0.0, -0.1 * scale_factor, 0.0 ) ;
    break ;

  case GLUT_KEY_LEFT :
    glMatrixMode ( GL_MODELVIEW ) ;
    xwin -= 0.1 * scale_factor ;
    glTranslated ( -0.1 * scale_factor, 0.0, 0.0 ) ;
    break ;

  case GLUT_KEY_RIGHT :
    glMatrixMode ( GL_MODELVIEW ) ;
    xwin += 0.1 * scale_factor ;
    glTranslated ( 0.1 * scale_factor, 0.0, 0.0 ) ;
    break ;

  case GLUT_KEY_PAGE_UP :
    glMatrixMode ( GL_MODELVIEW ) ;
    glTranslated ( -xwin, -ywin, 0.0 ) ;
    glScaled ( 1.25, 1.25, 1.25 ) ;
    glTranslated ( xwin, ywin, 0.0 ) ;
    scale_factor *= 0.8 ;
    break ;

  case GLUT_KEY_PAGE_DOWN :
    glMatrixMode ( GL_MODELVIEW ) ;
    glTranslated ( -xwin, -ywin, 0.0 ) ;
    glScaled ( 0.8, 0.8, 0.8 ) ;
    glTranslated ( xwin, ywin, 0.0 ) ;
    scale_factor *= 1.25 ;
    break ;

  default:
    need_redisplay = 0;
    break;
  }
  if (need_redisplay)
    glutPostRedisplay();
}


static void
checkedFGets ( char *s, int size, FILE *stream )
{
  if ( fgets ( s, size, stream ) == NULL ) {
    fprintf ( stderr, "fgets failed\n");
    exit ( EXIT_FAILURE );
  }
}


void readConfigFile ( char *fnme )
{
  FILE *fptr = fopen ( fnme, "rt" ) ;
  int i ;
  char inputline [ 256 ] ;

  if ( fptr )
  {
    /* Read a header line */
    checkedFGets ( inputline, sizeof ( inputline ), fptr ) ;

    /* Read a comment line */
    checkedFGets ( inputline, sizeof ( inputline ), fptr ) ;

    /* Read the window title */
    checkedFGets ( inputline, sizeof ( inputline ), fptr ) ;
    /* We assume here that this line will not exceed 79 characters plus a 
       newline (window_title is 80 characters long). That'll cause a buffer 
       overflow. For a simple program like  this, though, we're letting it 
       slide! 
    */
    sscanf ( inputline, "%[a-zA-Z0-9!@#$%^&*()+=/\\_-\" ]", window_title ) ; 

    /* Read a comment line */
    checkedFGets ( inputline, sizeof ( inputline ), fptr ) ;

    /* Read the number of affine transformations */
    checkedFGets ( inputline, sizeof ( inputline ), fptr ) ;
    sscanf ( inputline, "%d", &num_trans ) ;

    affine = (AffineTrans *)malloc ( num_trans * sizeof(AffineTrans) ) ;

    /* Read a comment line */
    checkedFGets ( inputline, sizeof ( inputline ), fptr ) ;

    for ( i = 0; i < num_trans; i++ )
    {
      /* Read an affine transformation definition */
      checkedFGets ( inputline, sizeof ( inputline ), fptr ) ;
      sscanf ( inputline, "%lf %lf %lf %lf %lf %lf", &affine[i].a00, &affine[i].a01,
                       &affine[i].a10, &affine[i].a11, &affine[i].b0, &affine[i].b1 ) ;
    }
  }
  else  /* No data file, set a default */
  {
    printf ( "ERROR opening file <%s>\n", fnme ) ;
    strcpy ( window_title, "Koch Snowflake" ) ;
    num_trans = 4 ;
    affine = (AffineTrans *)malloc ( num_trans * sizeof(AffineTrans) ) ;
    affine[0].a00 = 1/3. ;  affine[0].a01 = 0.00 ;  affine[0].a10 = 0.00 ;  affine[0].a11 = 1/3. ;
    affine[0].b0 = 0.0 ;    affine[0].b1 = 0.0 ;

    affine[1].a00 = 1/6. ;  affine[1].a01 = -1/3.*sin(FGH_PI/3.) ;   affine[1].a10 = 1/3.*sin(FGH_PI/3.) ;   affine[1].a11 = 1/6. ;
    affine[1].b0 = 1/3. ;   affine[1].b1 = 0.0 ;

    affine[2].a00 = 1/6. ;  affine[2].a01 = -1/3.*sin(-FGH_PI/3.) ;  affine[2].a10 = 1/3.*sin(-FGH_PI/3.) ;  affine[2].a11 = 1/6. ;
    affine[2].b0 = 0.5 ;    affine[2].b1 = sqrt(3)/6. ;

    affine[3].a00 = 1/3. ;  affine[3].a01 = 0.00 ;  affine[3].a10 = 0.00 ;  affine[3].a11 = 1/3. ;
    affine[3].b0 = 2/3. ;   affine[3].b1 = 0.0 ;
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
  glutInitWindowSize(500, 250);
  glutInitWindowPosition ( 140, 140 );
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE );
  glutInit(&argc, argv);

  if ( argc > 1 )
    readConfigFile ( argv[1] ) ;
  else
    readConfigFile ( "fractals.dat" ) ;

  glutCreateWindow( window_title );

  glClearColor(1.0, 1.0, 1.0, 1.0);

  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Key);
  glutSpecialFunc(Special);
  glutDisplayFunc(Display);

#ifdef WIN32
#endif

  while ( continue_in_main_loop )
    glutMainLoopEvent();

  printf ( "Back from the 'freeglut' main loop\n" ) ;

  return 0;             /* ANSI C requires main to return int. */
}
