/*
 * one.c
 *
 * Hey! This was the original file where freeglut development started. Just
 * note what I have written here at the time. And see the creation date :)
 *
 * : This is a wrapper. I still have to figure out
 * : how to build shared libraries under *nix :)
 *
 * Copyright (c) 1999 by Pawel W. Olszta
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: czw gru  2 11:58:41 CET 1999
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <GL/freeglut.h>

int g_LeaveGameMode = 0;
int g_InGameMode = 0;
int g_mainwin1, g_mainwin2, g_sw1, g_sw2, g_gamemodewin;

/*
 * Call this function to have some text drawn at given coordinates
 */
void PrintText( int nX, int nY, char* pszText )
{
    int lines;
    char *p;

    /*
     * Prepare the OpenGL state
     */
    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    /*
     * Have an orthogonal projection matrix set
     */
    glOrtho( 0, glutGet( GLUT_WINDOW_WIDTH ),
             0, glutGet( GLUT_WINDOW_HEIGHT ),
             -1, +1
    );

    /*
     * Now the matrix mode
     */
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    /*
     * Now the main text
     */
    glColor3ub( 0, 0, 0 );
    glRasterPos2i( nX, nY );

    for( p=pszText, lines=0; *p; p++ )
    {
        if( *p == '\n' )
        {
            lines++;
            glRasterPos2i( nX, nY-(lines*18) );
        }

        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,  *p );
    }

    /*
     * Revert to the old matrix modes
     */
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();

    /*
     * Restore the old OpenGL states
     */
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LIGHTING );
}

/*
 * This is the display routine for our sample FreeGLUT windows
 */
void SampleDisplay( void )
{
    int win = glutGetWindow();

    if (g_InGameMode && win!=g_gamemodewin)
        /* Don't draw other windows when in gamemode, those aren't visible
         * anyway. Drawing them continuously nonetheless can cause flicker trouble
         * on my machine. This only seems to occur only when there are child windows
         * among the non-visible windows 
         */
        return;

    if (win==g_sw1)
    {
        /*
         * Clear the screen
         */
        glClearColor(0.7f,0.7f,0.7f,1);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glutPostWindowRedisplay(g_mainwin2);
    }
    else if (win==g_sw2)
    {
        /*
         * Clear the screen
         */
        glClearColor(0.3f,0.3f,0.3f,1);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glutPostWindowRedisplay(g_mainwin2);
    }
    else
    {
        const GLfloat time = glutGet(GLUT_ELAPSED_TIME) / 1000.f * 40;

        /*
         * Clear the screen
         */
        glClearColor( 0, 0.5, 1, 1 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        /*
         * Have the cube rotated
         */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();

        glRotatef( time, 0, 0, 1 );
        glRotatef( time, 0, 1, 0 );
        glRotatef( time, 1, 0, 0 );

        /*
         * And then drawn...
         */
        glColor3f( 1, 1, 0 );
        /* glutWireCube( 20.0 ); */
        glutWireTeapot( 20.0 );
        /* glutWireSphere( 15.0, 15, 15 ); */
        /* glColor3f( 0, 1, 0 ); */
        /* glutWireCube( 30.0 ); */
        /* glutSolidCone( 10, 20, 10, 2 ); */

        /*
         * Don't forget about the model-view matrix
         */
        glPopMatrix( );

        /*
         * Draw a silly text
         */
        if( g_InGameMode == 0 )
            PrintText( 20, 20, "Hello there cruel world!" );
        else
            PrintText( 20, 20, "Press ESC to leave the game mode!" );
    }

    /*
     * And swap this context's buffers
     */
    glutSwapBuffers( );
    glutPostWindowRedisplay(win);
}

/*
 * This is a sample idle function
 */
void SampleIdle( void )
{
    if( g_LeaveGameMode == 1 )
    {
        /* One could do all this just as well in SampleGameModeKeyboard... */
        printf("leaving gamemode...\n");
        glutLeaveGameMode( );
        g_LeaveGameMode = 0;
        g_InGameMode = 0;
        glutPostWindowRedisplay(g_mainwin1);
        glutPostWindowRedisplay(g_mainwin2);
        glutPostWindowRedisplay(g_sw1);
        glutPostWindowRedisplay(g_sw2);
    }
}

void SampleEntry(int state)
{
    int window = glutGetWindow () ;
    printf ( "Window %d Entry Callback: %d\n", window, state ) ;
}

/*
 * The reshape function
 */
void SampleReshape( int nWidth, int nHeight )
{
    GLfloat fAspect = (GLfloat) nHeight / (GLfloat) nWidth;
    GLfloat fPos[ 4 ] = { 0.0f, 0.0f, 10.0f, 0.0f };
    GLfloat fCol[ 4 ] = { 0.5f, 1.0f,  0.0f, 1.0f };

    /*
     * Update the viewport first
     */
    glViewport( 0, 0, nWidth, nHeight );

    /*
     * Then the projection matrix
     */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -fAspect, fAspect, 1.0, 80.0 );

    /*
     * Move back the camera a bit
     */
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    glTranslatef( 0.0, 0.0, -40.0f );

    /*
     * Enable some features...
     */
    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_NORMALIZE );

    /*
     * Set up some lighting
     */
    glLightfv( GL_LIGHT0, GL_POSITION, fPos );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    /*
     * Set up a sample material
     */
    glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fCol );
}

/*
 * A sample keyboard callback
 */
void SampleKeyboard( unsigned char cChar, int nMouseX, int nMouseY )
{
    printf( "SampleKeyboard(): keypress '%c' at (%i,%i)\n",
            cChar, nMouseX, nMouseY );
}

/*
 * A sample keyboard callback (for game mode window)
 */
void SampleGameModeKeyboard( unsigned char cChar, int nMouseX, int nMouseY )
{
    if( cChar == 27 )
        g_LeaveGameMode = 1;
}


/*
 * A sample special callback
 */
void SampleSpecial( int nSpecial, int nMouseX, int nMouseY )
{
    printf( "SampleSpecial(): special keypress %i at (%i,%i)\n",
            nSpecial, nMouseX, nMouseY );
}

/*
 * A sample menu callback
 */
void SampleMenu( int menuID )
{
    printf( "SampleMenu() callback executed, menuID is %i\n", menuID );
}

/*
 * A sample menu status callback
 */
void SampleMenuStatus( int status, int x, int y )
{
    printf ( "SampleMenu() callback executed, MenuStatus is %i at (%i,%i)\n", status, x, y );
}

/*
 * The sample's entry point
 */
int main( int argc, char** argv )
{
    int menuID, subMenuA, subMenuB;

    glutInitDisplayString( "stencil~2 rgb double depth>=16 samples" );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowPosition( 100, 100 );

    glutInit( &argc, argv );

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    subMenuA = glutCreateMenu( SampleMenu );
    glutAddMenuEntry( "Sub menu A1 (01)", 1 );
    glutAddMenuEntry( "Sub menu A2 (02)", 2 );
    glutAddMenuEntry( "Sub menu A3 (03)", 3 );

    subMenuB = glutCreateMenu( SampleMenu );
    glutAddMenuEntry( "Sub menu B1 (04)", 4 );
    glutAddMenuEntry( "Sub menu B2 (05)", 5 );
    glutAddMenuEntry( "Sub menu B3 (06)", 6 );
    glutAddSubMenu( "Going to sub menu A", subMenuA );

    menuID = glutCreateMenu( SampleMenu );
    glutAddMenuEntry( "Entry one",   1 );
    glutAddMenuEntry( "Entry two",   2 );
    glutAddMenuEntry( "Entry three", 3 );
    glutAddMenuEntry( "Entry four",  4 );
    glutAddMenuEntry( "Entry five",  5 );
    glutAddSubMenu( "Enter sub menu A", subMenuA );
    glutAddSubMenu( "Enter sub menu B", subMenuB );

    g_mainwin1 = glutCreateWindow( "Hello world!" );
    glutDisplayFunc( SampleDisplay );
    glutReshapeFunc( SampleReshape );
    glutKeyboardFunc( SampleKeyboard );
    glutSpecialFunc( SampleSpecial );
    glutIdleFunc( SampleIdle );
    glutEntryFunc( SampleEntry );
    glutMenuStatusFunc( SampleMenuStatus );
    glutAttachMenu( GLUT_LEFT_BUTTON );

    glutInitWindowPosition( 200, 200 );
    g_mainwin2 = glutCreateWindow( "I am not Jan B." );
    glutDisplayFunc( SampleDisplay );
    glutReshapeFunc( SampleReshape );
    glutKeyboardFunc( SampleKeyboard );
    glutSpecialFunc( SampleSpecial );
    glutIdleFunc( SampleIdle );
    glutEntryFunc( SampleEntry );
    glutMenuStatusFunc( SampleMenuStatus );
    glutAttachMenu( GLUT_LEFT_BUTTON );
    glutSetMenu(subMenuA);
    glutAttachMenu( GLUT_RIGHT_BUTTON );

    g_sw1=glutCreateSubWindow(g_mainwin2,200,0,100,100);
    glutDisplayFunc( SampleDisplay );
    glutSetMenu(subMenuB);
    glutAttachMenu( GLUT_LEFT_BUTTON );

    g_sw2=glutCreateSubWindow(g_sw1,50,0,50,50);
    glutDisplayFunc( SampleDisplay );
    glutSetMenu(menuID);
    glutAttachMenu( GLUT_RIGHT_BUTTON );

    printf( "Testing game mode string parsing, don't panic!\n" );
    glutGameModeString( "320x240:32@100" );
    glutGameModeString( "640x480:16@72" );
    glutGameModeString( "1024x768" );
    glutGameModeString( ":32@120" );
    glutGameModeString( "Toudi glupcze, Danwin bedzie moj!" );
    
    glutGameModeString( "640x480:37@300" );    /* this one should fail */
    glutEnterGameMode();

    glutGameModeString( "800x600" );    /* this one is likely to succeed */
    g_gamemodewin = glutEnterGameMode();

    if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE))
        g_InGameMode = 1;
    glutDisplayFunc( SampleDisplay );
    glutReshapeFunc( SampleReshape );
    glutKeyboardFunc( SampleGameModeKeyboard );
    glutIdleFunc( SampleIdle );
    glutEntryFunc( SampleEntry );
    glutMenuStatusFunc( SampleMenuStatus );
    glutSetMenu(menuID);
    glutAttachMenu( GLUT_LEFT_BUTTON );

    printf( "current window is %ix%i at (%i,%i)\n",
        glutGet( GLUT_WINDOW_WIDTH ), glutGet( GLUT_WINDOW_HEIGHT ),
        glutGet( GLUT_WINDOW_X ), glutGet( GLUT_WINDOW_Y )
    );

    /*
     * Enter the main FreeGLUT processing loop
     */
    glutMainLoop();

    /*
     * returned from mainloop after window closed
     * see glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS); above
     */
    printf( "glutMainLoop() termination works fine!\n" );

    return EXIT_SUCCESS;
}

/*** END OF FILE ***/
