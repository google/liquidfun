/*
 * freeglut_misc.c
 *
 * Functions that didn't fit anywhere else...
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Thu Dec 9 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define  G_LOG_DOMAIN  "freeglut-misc"

#include "../include/GL/freeglut.h"
#include "../include/GL/freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  glutSetColor()     --
 *  glutGetColor()     --
 *  glutCopyColormap() --
 *  glutSetKeyRepeat() -- this is evil and should be removed from API
 */

/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * This functions checks if an OpenGL extension is supported or not
 */
int FGAPIENTRY glutExtensionSupported( const char* extension )
{
    /*
     * Grab the current context's OpenGL extensions
     * and create a new GLib lexical analyzer...
     */
    gchar *glExtensions = (gchar *) glGetString( GL_EXTENSIONS );
    GScanner* scanner = g_scanner_new( NULL );
    gint i;

    /*
     * Make sure there is a current window, and thus -- a current context available
     */
    freeglut_assert_ready; freeglut_return_val_if_fail( fgStructure.Window != NULL, 0 );

    /*
     * Fail if there is no extension, extensions or scanner available
     */
    freeglut_return_val_if_fail( (scanner != NULL) && (strlen( extension ) > 0)
                                          && (strlen( glExtensions ) > 0), 0 );

    /*
     * Check if the extension itself looks valid
     */
    for( i=0; i<(gint) strlen( extension ); i++ )
        if( extension[ i ] == ' ' )
            return( 0 );

    /*
     * Set the scanner's input name (for debugging)
     */
    scanner->input_name = "glutExtensionSupported()";

    /*
     * Start the lexical analysis of the extensions string
     */
    g_scanner_input_text( scanner, glExtensions, strlen( glExtensions ) );

    /*
     * While there are any more tokens to be checked...
     */
    while( !g_scanner_eof( scanner ) )
    {
        /*
         * Actually we're expecting only string tokens
         */
        GTokenType tokenType = g_scanner_get_next_token( scanner );

        /*
         * We are looking for identifiers
         */
        if( tokenType == G_TOKEN_IDENTIFIER )
        {
            /*
             * Compare the token and the extension string
             */
            if( strcmp( scanner->value.v_identifier, extension ) == 0 )
            {
                /*
                 * OKi, we have found the extension string we've been looking for
                 */
                g_scanner_destroy( scanner );
                return( 1 );
            }
        }
    }

    /*
     * Well, looks like we have failed to find the extension string
     */
    g_scanner_destroy( scanner );
    return( 0 );
}

/*
 * This function reports all the errors that happened till now
 */
void FGAPIENTRY glutReportErrors( void )
{
    GLenum error = glGetError();

    /*
     * Keep reporting errors as long as there are any...
     */
    while( error != GL_NO_ERROR )
    {
        /*
         * Print the current error
         */
#       undef  G_LOG_DOMAIN
#       define G_LOG_DOMAIN ((gchar *) 0)

        g_warning( "GL error: %s", gluErrorString( error ) );

#       undef   G_LOG_DOMAIN
#       define  G_LOG_DOMAIN  "freeglut_misc.c"

        /*
         * Grab the next error value
         */
        error = glGetError();
    };
}

/*
 * Turns the ignore key auto repeat feature on and off
 */
void FGAPIENTRY glutIgnoreKeyRepeat( int ignore )
{
    /*
     * This is simple and not demanging...
     */
    fgState.IgnoreKeyRepeat = ignore ? TRUE : FALSE;
}

/*
 * Hints the window system whether to generate key auto repeat, or not. This is evil.
 */
void FGAPIENTRY glutSetKeyRepeat( int repeatMode )
{
#if TARGET_HOST_UNIX_X11

    freeglut_assert_ready;

    /*
     * This is really evil, but let's have this done.
     */
    switch( repeatMode )
    {
    case GLUT_KEY_REPEAT_OFF:   XAutoRepeatOff( fgDisplay.Display ); break;
    case GLUT_KEY_REPEAT_ON:    XAutoRepeatOn( fgDisplay.Display );  break;
    case GLUT_KEY_REPEAT_DEFAULT:
        {
            XKeyboardState keyboardState;

            /*
             * Query the current keyboard state
             */
            XGetKeyboardControl( fgDisplay.Display, &keyboardState );

            /*
             * Set the auto key repeat basing on the global settings
             */
            glutSetKeyRepeat(
                keyboardState.global_auto_repeat == AutoRepeatModeOn ?
                GLUT_KEY_REPEAT_ON : GLUT_KEY_REPEAT_OFF
            );
        }
        break;

    default:
        /*
         * Whoops, this was not expected at all
         */
        g_assert_not_reached();
    }

#endif
}

/*
 * Forces the joystick callback to be executed
 */
void FGAPIENTRY glutForceJoystickFunc( void )
{
    freeglut_assert_ready;

    /*
     * Is there a current window selected?
     */
    freeglut_return_if_fail( fgStructure.Window != NULL );

    /*
     * Check if there is a joystick callback hooked to the current window
     */
    freeglut_return_if_fail( fgStructure.Window->Callbacks.Joystick != NULL );

    /*
     * Poll the joystick now, using the current window's joystick callback
     */
    fgJoystickPollWindow( fgStructure.Window );
}

/*
 *
 */
void FGAPIENTRY glutSetColor( int nColor, GLfloat red, GLfloat green, GLfloat blue )
{
    /*
     *
     */
}

/*
 *
 */
GLfloat FGAPIENTRY glutGetColor( int color, int component )
{
    /*
     *
     */
    return( 0.0f );
}

/*
 *
 */
void FGAPIENTRY glutCopyColormap( int window )
{
    /*
     *
     */
}

/*** END OF FILE ***/
