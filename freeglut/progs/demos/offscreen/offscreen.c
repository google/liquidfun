/*
 * freeglut offscreen rendering demo.
 *
 * Usage:
 *
 *    offscreen [standard GLUT options] [filename]
 *
 *    The filename is used to say where to write a PNM image file
 *    that is the sole way to verify that this program is doing what
 *    it should (i.e., rendering offscreen).
 *
 *    You can view the PNM file in image viewers and editors such as
 *    the GIMP.  (Everyone doing any kind of graphics has at least one
 *    multi-format-reading image manipulation program, right?)
 *
 *    If no filename is specified, the chosen filename is "offscreen.pnm".
 *
 * Some suggested uses for GLUT_OFFSCREEN:
 *
 *  * Implementation of a graphics language that writes bitmapped image files.
 *  * Capture GLUT bitmapped fonts to a texture at runtime to make scalable
 *    bitmapped fonts.
 *  * Using OpenGL to create special-effect textures at run-time.
 *  * Making stencils.
 *  * Getting the depth-buffer for a height-field.
 *  * Using OpenGL to create a poster texture (e.g., for pasting onto a a
 *    video screen in a virtual world).
 *  * Using OpenGL to prescale an image to a power-of-2 dimension (including
 *    OpenGL antialiasing).  There's no reason that you couldn't do this
 *    yourself, but OpenGL can do it for you, so why not?
 *  * Rendering to a bitmap larger than the display has been suggested
 *    as well.  This is a cheap way to produce arbitrarily high resolution
 *    images for printing.
 *
 * In every case, there are at least cases where you would want to do this
 * without actually rendering to a visible window.
 *
 */
#include <stdio.h>
#include <GL/freeglut.h>

#define OFFSCREEN_W 500
#define OFFSCREEN_H 250


int offscreen_id;
int onscreen_id;
const char *save_file_name = "offscreen.pnm";


void cb_onscreen_display( void )
{
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
    glColor3d( 1.0, 0.0, 0.0 );
    glBegin( GL_LINE_LOOP );
    glVertex2d( 0.0, 0.0 );
    glVertex2d( 0.5, 0.0 );
    glVertex2d( 0.5, 0.5 );
    glVertex2d( 0.0, 0.5 );
    glVertex2d( 0.0, 0.0 );
    glEnd( );
    glColor3d( 1.0, 1.0, 1.0 );
    glRasterPos2d( -1.0, 0.8 );
    glutBitmapString(
        GLUT_BITMAP_TIMES_ROMAN_24, "Press `s' or `S' to Send to"
    );
    glRasterPos2d( -.8, 0.6 );
    glutBitmapString(
        GLUT_BITMAP_TIMES_ROMAN_24, "'offscreen.pnm'"
    );
    glRasterPos2d( -1.0, 0.3 );
    glutBitmapString(
        GLUT_BITMAP_TIMES_ROMAN_24, "Press `q', `Q', or Esc to quit."
    );
    
    glutSwapBuffers( );
}

void write_raw_pnm( char *fname, char *pixels, int w, int h)
{
    FILE *f;
    
    printf( "Trying to create: %s\n", fname );
    f = fopen( fname, "wb" );
    if( !f )
        printf( "Ouch!  Cannot create file.\n" );
    else
    {
        int row;

        printf( "Opened file.\n" );
        fprintf( f, "P6\n" );
        fprintf( f, "# CREATOR: offscreen freeglut demo\n" );
        fprintf( f, "%d %d\n", w, h );
        fprintf( f, "255\n" );

        /*
         * Write the rows in reverse order because OpenGL's 0th row
         * is at the bottom.
         */
        for( row = h; row; --row )
            fwrite( pixels + ((row - 1)*w*3), 1, 3 * w, f );
        
        fclose( f );
    }
}


void cb_offscreen_display( void )
{
    static char pixels [OFFSCREEN_W * OFFSCREEN_H * 3];
    int i;

    cb_onscreen_display( );
    glColor3d( 0.0, 1.0, 0.0 );
    glBegin( GL_LINES );
    for( i = 0; i < 100; ++i)
    {
        glVertex2d( +1 - (i/50.0), +1 );
        glVertex2d( -1 + (i/50.0), -1 );
    }
    glEnd( );
    glFinish( );

    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glReadPixels(
	0, 0, OFFSCREEN_W, OFFSCREEN_H,
	GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *)pixels
    );
    write_raw_pnm( save_file_name, pixels, OFFSCREEN_W, OFFSCREEN_H );
}


void cb_onscreen_keyboard( unsigned char key, int x, int y )
{
    switch( key )
    {
    case 's':
    case 'S':
        glutSetWindow( offscreen_id );
        glutPostRedisplay( );
        break;
        
    case '\x1b':
    case 'q':
    case 'Q':
        exit( 0 );
        break;
        
    default:
        break;
    }
}

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    if( argv[ 1 ] )
        save_file_name = argv[ 1 ];
    
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    onscreen_id = glutCreateWindow( "Offscreen demo" );
    glutDisplayFunc( cb_onscreen_display );
    glutKeyboardFunc( cb_onscreen_keyboard );

    glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE | GLUT_OFFSCREEN );
    glutInitWindowSize( OFFSCREEN_W, OFFSCREEN_H );
    offscreen_id = glutCreateWindow( "" );
    glutDisplayFunc( cb_offscreen_display );
    
    glutMainLoop( );
}
