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
#include "freeglut_internal.h"

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
  const char *extensions;
  const char *ptr;
  int len = strlen ( extension ) ;

  /*
   * Make sure there is a current window, and thus -- a current context available
   */
  freeglut_assert_ready;
  freeglut_return_val_if_fail( fgStructure.Window != NULL, 0 );

  /*
   * Note it is safe to query the extensions
   */
  extensions = glGetString(GL_EXTENSIONS);

  freeglut_return_val_if_fail( extensions != NULL, 0 );

  /*
   * Check if the extension itself looks valid
   */
  if ( strchr ( extension, ' ' ) != NULL )
    return( 0 );

  /*
   * Look for our extension
   */
  for (ptr = extensions; *ptr;)
  {
    /*
     * Is it the current extension?
     */
    if ( strncmp ( extension, extensions, len ) == 0 )
      return 1 ;

    /*
     * No, go find the next extension.  They are separated from each other by one or more blank spaces.
     */
    ptr = strchr ( ptr + len, ' ' ) ;

    /*
     * If we ran off the end of the "extensions" character string, we didn't find it.  Return failure.
     */
    if ( !ptr ) return 0 ;

    while ( *ptr == ' ' )
      ptr++ ;
  }

  return 0 ;
}

/*
 * This function reports all the OpenGL errors that happened till now
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

        fgWarning( "GL error: %s", gluErrorString( error ) );

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
void FGAPIENTRY glutIgnoreKeyRepeat( int ignore )  /* DEPRECATED 11/4/02 - Do not use */
{
    /*
     * This is simple and not damaging...
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
        break;
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
