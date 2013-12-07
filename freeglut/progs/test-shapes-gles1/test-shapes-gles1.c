/*! \file    shapes.c
    \ingroup demos

    This program is a test harness for the various shapes
    in OpenGLUT.  It may also be useful to see which
    parameters control what behavior in the OpenGLUT
    objects.
 
    Spinning wireframe and solid-shaded shapes are
    displayed.  Some parameters can be adjusted.
 
   Keys:
      -    <tt>Esc &nbsp;</tt> Quit
      -    <tt>q Q &nbsp;</tt> Quit
      -    <tt>i I &nbsp;</tt> Show info
      -    <tt>p P &nbsp;</tt> Toggle perspective or orthographic projection
      -    <tt>= + &nbsp;</tt> Increase \a slices
      -    <tt>- _ &nbsp;</tt> Decreate \a slices
      -    <tt>, < &nbsp;</tt> Decreate \a stacks
      -    <tt>. > &nbsp;</tt> Increase \a stacks
      -    <tt>9 ( &nbsp;</tt> Decreate \a depth  (Sierpinski Sponge)
      -    <tt>0 ) &nbsp;</tt> Increase \a depth  (Sierpinski Sponge)
      -    <tt>up&nbsp; &nbsp;</tt> Increase "outer radius"
      -    <tt>down&nbsp;</tt> Decrease "outer radius"
      -    <tt>left&nbsp;</tt> Decrease "inner radius"
      -    <tt>right</tt> Increase "inner radius"
      -    <tt>PgUp&nbsp;</tt> Next shape-drawing function
      -    <tt>PgDn&nbsp;</tt> Prev shape-drawing function

    \author  Written by Nigel Stewart November 2003

    \author  Portions Copyright (C) 2004, the OpenGLUT project contributors. <br>
             OpenGLUT branched from freeglut in February, 2004.
 
    \image   html openglut_shapes.png OpenGLUT Geometric Shapes Demonstration
    \include demos/shapes/shapes.c
*/

#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
/* DUMP MEMORY LEAKS */
#include <crtdbg.h>
#endif

/*
 * This macro is only intended to be used on arrays, of course.
 */
#define NUMBEROF(x) ((sizeof(x))/(sizeof(x[0])))

#define glRotated glRotatef
#define glTranslated glTranslatef

/*
 * These global variables control which object is drawn,
 * and how it is drawn.  No object uses all of these
 * variables.
 */
static int function_index;
static int slices = 16;
static int stacks = 16;
static double irad = .25;
static double orad = 1.0;   /* doubles as size for objects other than Torus */
static int depth = 4;
static double offset[ 3 ] = { 0, 0, 0 };
static GLboolean show_info = GL_TRUE;
static float ar;
static GLboolean persProject = GL_TRUE;
static GLboolean animateXRot = GL_FALSE;

/*
 * These one-liners draw particular objects, fetching appropriate
 * information from the above globals.  They are just thin wrappers
 * for the FreeGLUT objects.
 */
static void drawSolidTetrahedron(void)         { glutSolidTetrahedron ();                        }
static void drawWireTetrahedron(void)          { glutWireTetrahedron ();                         }
static void drawSolidCube(void)                { glutSolidCube(orad);                            }  /* orad doubles as size input */
static void drawWireCube(void)                 { glutWireCube(orad);                             }  /* orad doubles as size input */
static void drawSolidOctahedron(void)          { glutSolidOctahedron ();                         }
static void drawWireOctahedron(void)           { glutWireOctahedron ();                          }
static void drawSolidDodecahedron(void)        { glutSolidDodecahedron ();                       }
static void drawWireDodecahedron(void)         { glutWireDodecahedron ();                        }
static void drawSolidRhombicDodecahedron(void) { glutSolidRhombicDodecahedron ();                }
static void drawWireRhombicDodecahedron(void)  { glutWireRhombicDodecahedron ();                 }
static void drawSolidIcosahedron(void)         { glutSolidIcosahedron ();                        }
static void drawWireIcosahedron(void)          { glutWireIcosahedron ();                         }
static void drawSolidSierpinskiSponge(void)    { glutSolidSierpinskiSponge (depth, offset, orad);}  /* orad doubles as size input */
static void drawWireSierpinskiSponge(void)     { glutWireSierpinskiSponge (depth, offset, orad); }  /* orad doubles as size input */
static void drawSolidTorus(void)               { glutSolidTorus(irad,orad,slices,stacks);        }
static void drawWireTorus(void)                { glutWireTorus (irad,orad,slices,stacks);        }
static void drawSolidSphere(void)              { glutSolidSphere(orad,slices,stacks);            }  /* orad doubles as size input */
static void drawWireSphere(void)               { glutWireSphere(orad,slices,stacks);             }  /* orad doubles as size input */
static void drawSolidCone(void)                { glutSolidCone(orad,orad,slices,stacks);         }  /* orad doubles as size input */
static void drawWireCone(void)                 { glutWireCone(orad,orad,slices,stacks);          }  /* orad doubles as size input */
static void drawSolidCylinder(void)            { glutSolidCylinder(orad,orad,slices,stacks);     }  /* orad doubles as size input */
static void drawWireCylinder(void)             { glutWireCylinder(orad,orad,slices,stacks);      }  /* orad doubles as size input */
static void drawSolidTeapot(void)
{   glFrontFace(GL_CW);    glutSolidTeapot(orad);   glFrontFace(GL_CCW);    /* orad doubles as size input */}
static void drawWireTeapot(void)
{   glFrontFace(GL_CW);    glutWireTeapot(orad);    glFrontFace(GL_CCW);    /* orad doubles as size input */}

/*
 * This structure defines an entry in our function-table.
 */
typedef struct
{
    const char * const name;
    void (*solid) (void);
    void (*wire)  (void);
} entry;

#define ENTRY(e) {#e, drawSolid##e, drawWire##e}
static const entry table [] =
{
    ENTRY (Tetrahedron),
    ENTRY (Cube),
    ENTRY (Octahedron),
    ENTRY (Dodecahedron),
    ENTRY (RhombicDodecahedron),
    ENTRY (Icosahedron),
    ENTRY (SierpinskiSponge),
    ENTRY (Teapot),
    ENTRY (Torus),
    ENTRY (Sphere),
    ENTRY (Cone),
    ENTRY (Cylinder),
    /* ENTRY (Cuboctahedron) */
};
#undef ENTRY

/*!
    Does printf()-like work using freeglut
    glutBitmapString().  Uses a fixed font.  Prints
    at the indicated row/column position.

    Limitation: Cannot address pixels.
    Limitation: Renders in screen coords, not model coords.
*/
static void shapesPrintf (int row, int col, const char *fmt, ...)
{
}

/* GLUT callback Handlers */

static void
resize(int width, int height)
{
    ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;
    const double b = (animateXRot?t:1)*60.0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumf(-ar, ar, -1.0, 1.0, 2.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_LIGHTING);

    glColor4f(1,0,0,1);

    glPushMatrix();
        glTranslated(0,1.2,-6);
        glRotated(b,1,0,0);
        glRotated(a,0,0,1);
        table [function_index].solid ();
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,-1.2,-6);
        glRotated(b,1,0,0);
        glRotated(a,0,0,1);
        table [function_index].wire ();
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor4f(0.1,0.1,0.4,1.0);

    glutSwapBuffers();
}


static void
key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'Q':
    case 'q': glutLeaveMainLoop () ;      break;

    case 'I':
    case 'i': show_info = ( show_info == GL_TRUE ) ? GL_FALSE : GL_TRUE; break;

    case '=':
    case '+': slices++; printf("%d,%d\n", slices, stacks); break;

    case '-':
    case '_': if( slices > -1 ) slices--; break;

    case ',':
    case '<': if( stacks > -1 ) stacks--; break;

    case '.':
    case '>': stacks++;                   break;

    case '9': 
    case '(': if( depth > -1 ) depth--;   break;

    case '0': 
    case ')': ++depth;                    break;

    case 'P':
    case 'p': persProject=!persProject;   break;

    case 'R':
    case 'r': animateXRot=!animateXRot;   break;

    default:
        break;
    }

    glutPostRedisplay();
}

static void special (int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_PAGE_UP:    ++function_index; break;
    case GLUT_KEY_PAGE_DOWN:  --function_index; break;
    case GLUT_KEY_UP:         orad *= 2;        break;
    case GLUT_KEY_DOWN:       orad /= 2;        break;

    case GLUT_KEY_RIGHT:      irad *= 2;        break;
    case GLUT_KEY_LEFT:       irad /= 2;        break;

    default:
        break;
    }

    if (0 > function_index)
        function_index = NUMBEROF (table) - 1;

    if (NUMBEROF (table) <= ( unsigned )function_index)
        function_index = 0;
}


static void
idle(void)
{
    glutPostRedisplay();
}

static void
onMouseClick(int button, int state, int x, int y) {
  if (state == GLUT_DOWN)
    special(GLUT_KEY_PAGE_UP, 0, 0);
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* Program entry point */

void init_context() {
    printf("init_context\n"); fflush(stdout);
    glClearColor(1,1,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
}

int
main(int argc, char *argv[])
{
    glutInitWindowSize(640,480);
    glutInitWindowPosition(40,40);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("FreeGLUT Shapes");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
    glutIdleFunc(idle);
    glutMouseFunc(onMouseClick);
    glutInitContextFunc(init_context);

    glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION ) ;

    glutMainLoop();

#ifdef _MSC_VER
    /* DUMP MEMORY LEAK INFORMATION */
    _CrtDumpMemoryLeaks () ;
#endif

    return EXIT_SUCCESS;
}
