/*
 * freeglut_state.c
 *
 * Freeglut state query methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Thu Dec 16 1999
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

#include <GL/freeglut.h>
#include "fg_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 *  glutGet()               -- X11 tests passed, but check if all enums
 *                             handled (what about Win32?)
 *  glutDeviceGet()         -- X11 tests passed, but check if all enums
 *                             handled (what about Win32?)
 *  glutGetModifiers()      -- OK, but could also remove the limitation
 *  glutLayerGet()          -- what about GLUT_NORMAL_DAMAGED?
 *
 * The fail-on-call policy will help adding the most needed things imho.
 */

extern int fgPlatformGlutGet ( GLenum eWhat );
extern int fgPlatformGlutDeviceGet ( GLenum eWhat );
extern int *fgPlatformGlutGetModeValues(GLenum eWhat, int *size);
extern SFG_Font* fghFontByID( void* font );


/* -- LOCAL DEFINITIONS ---------------------------------------------------- */

/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * General settings assignment method
 */
void FGAPIENTRY glutSetOption( GLenum eWhat, int value )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSetOption" );

    switch( eWhat )
    {
    case GLUT_INIT_WINDOW_X:
        fgState.Position.X = (GLint)value;
        break;

    case GLUT_INIT_WINDOW_Y:
        fgState.Position.Y = (GLint)value;
        break;

    case GLUT_INIT_WINDOW_WIDTH:
        fgState.Size.X = (GLint)value;
        break;

    case GLUT_INIT_WINDOW_HEIGHT:
        fgState.Size.Y = (GLint)value;
        break;

    case GLUT_INIT_DISPLAY_MODE:
        fgState.DisplayMode = (unsigned int)value;
        break;

    case GLUT_ACTION_ON_WINDOW_CLOSE:
        fgState.ActionOnWindowClose = value;
        break;

    case GLUT_RENDERING_CONTEXT:
        fgState.UseCurrentContext =
            ( value == GLUT_USE_CURRENT_CONTEXT ) ? GL_TRUE : GL_FALSE;
        break;

    case GLUT_DIRECT_RENDERING:
        fgState.DirectContext = value;
        break;

    case GLUT_WINDOW_CURSOR:
        if( fgStructure.CurrentWindow != NULL )
            fgStructure.CurrentWindow->State.Cursor = value;
        break;

    case GLUT_AUX:
      fgState.AuxiliaryBufferNumber = value;
      break;

    case GLUT_MULTISAMPLE:
      fgState.SampleNumber = value;
      break;

    case GLUT_SKIP_STALE_MOTION_EVENTS:
      fgState.SkipStaleMotion = (GLboolean)value;
      break;

    case GLUT_GEOMETRY_VISUALIZE_NORMALS:
      if( fgStructure.CurrentWindow != NULL )
        fgStructure.CurrentWindow->State.VisualizeNormals = (GLboolean)value;
      break;

    default:
        fgWarning( "glutSetOption(): missing enum handle %d", eWhat );
        break;
    }
}

/*
 * General settings query method
 */
int FGAPIENTRY glutGet( GLenum eWhat )
{
    switch (eWhat)
    {
    case GLUT_INIT_STATE:
        return fgState.Initialised;

    /* Although internally the time store is 64bits wide, the return value
     * here still wraps every 49.7 days. Integer overflows cancel however
     * when subtracting an initial start time, unless the total time exceeds
     * 32-bit, so you can still work with this.
     * XXX: a glutGet64 to return the time might be an idea...
     */
    case GLUT_ELAPSED_TIME:
        return (int) fgElapsedTime();
    }

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutGet" );

    switch( eWhat )
    {
    /* Following values are stored in fgState and fgDisplay global structures */
    case GLUT_SCREEN_WIDTH:         return fgDisplay.ScreenWidth   ;
    case GLUT_SCREEN_HEIGHT:        return fgDisplay.ScreenHeight  ;
    case GLUT_SCREEN_WIDTH_MM:      return fgDisplay.ScreenWidthMM ;
    case GLUT_SCREEN_HEIGHT_MM:     return fgDisplay.ScreenHeightMM;
    case GLUT_INIT_WINDOW_X:        return fgState.Position.Use ?
                                           fgState.Position.X : -1 ;
    case GLUT_INIT_WINDOW_Y:        return fgState.Position.Use ?
                                           fgState.Position.Y : -1 ;
    case GLUT_INIT_WINDOW_WIDTH:    return fgState.Size.Use ?
                                           fgState.Size.X : -1     ;
    case GLUT_INIT_WINDOW_HEIGHT:   return fgState.Size.Use ?
                                           fgState.Size.Y : -1     ;
    case GLUT_INIT_DISPLAY_MODE:    return fgState.DisplayMode     ;
    case GLUT_INIT_MAJOR_VERSION:   return fgState.MajorVersion    ;
    case GLUT_INIT_MINOR_VERSION:   return fgState.MinorVersion    ;
    case GLUT_INIT_FLAGS:           return fgState.ContextFlags    ;
    case GLUT_INIT_PROFILE:         return fgState.ContextProfile  ;

    /* The window structure queries */
    case GLUT_WINDOW_PARENT:
        if( fgStructure.CurrentWindow         == NULL ) return 0;
        if( fgStructure.CurrentWindow->Parent == NULL ) return 0;
        return fgStructure.CurrentWindow->Parent->ID;

    case GLUT_WINDOW_NUM_CHILDREN:
        if( fgStructure.CurrentWindow == NULL )
            return 0;
        return fgListLength( &fgStructure.CurrentWindow->Children );

    case GLUT_WINDOW_CURSOR:
        if( fgStructure.CurrentWindow == NULL )
            return 0;
        return fgStructure.CurrentWindow->State.Cursor;

    case GLUT_MENU_NUM_ITEMS:
        if( fgStructure.CurrentMenu == NULL )
            return 0;
        return fgListLength( &fgStructure.CurrentMenu->Entries );

    case GLUT_ACTION_ON_WINDOW_CLOSE:
        return fgState.ActionOnWindowClose;

    case GLUT_VERSION :
        return VERSION_MAJOR * 10000 + VERSION_MINOR * 100 + VERSION_PATCH;

    case GLUT_RENDERING_CONTEXT:
        return fgState.UseCurrentContext ? GLUT_USE_CURRENT_CONTEXT
                                         : GLUT_CREATE_NEW_CONTEXT;

    case GLUT_DIRECT_RENDERING:
        return fgState.DirectContext;

    case GLUT_FULL_SCREEN:
        return fgStructure.CurrentWindow->State.IsFullscreen;

    case GLUT_AUX:
      return fgState.AuxiliaryBufferNumber;

    case GLUT_MULTISAMPLE:
      return fgState.SampleNumber;

    case GLUT_SKIP_STALE_MOTION_EVENTS:
      return fgState.SkipStaleMotion;

    case GLUT_GEOMETRY_VISUALIZE_NORMALS:
      if( fgStructure.CurrentWindow == NULL )
        return GL_FALSE;
      return fgStructure.CurrentWindow->State.VisualizeNormals;

    default:
        return fgPlatformGlutGet ( eWhat );
    }
}

/*
 * Returns various device information.
 */
int FGAPIENTRY glutDeviceGet( GLenum eWhat )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutDeviceGet" );

    /* XXX WARNING: we are mostly lying in this function. */
    switch( eWhat )
    {
    case GLUT_HAS_JOYSTICK:
        return fgJoystickDetect ();

    case GLUT_OWNS_JOYSTICK:
        return fgState.JoysticksInitialised;

    case GLUT_JOYSTICK_POLL_RATE:
        return fgStructure.CurrentWindow ? fgStructure.CurrentWindow->State.JoystickPollRate : 0;

    /* XXX The following two are only for Joystick 0 but this is an improvement */
    case GLUT_JOYSTICK_BUTTONS:
        return glutJoystickGetNumButtons ( 0 );

    case GLUT_JOYSTICK_AXES:
        return glutJoystickGetNumAxes ( 0 );

    case GLUT_HAS_DIAL_AND_BUTTON_BOX:
        return fgInputDeviceDetect ();

    case GLUT_NUM_DIALS:
        if ( fgState.InputDevsInitialised ) return 8;
        return 0;
 
    case GLUT_NUM_BUTTON_BOX_BUTTONS:
        return 0;

    case GLUT_HAS_SPACEBALL:
        return fgHasSpaceball();

    case GLUT_HAS_TABLET:
        return 0;

    case GLUT_NUM_SPACEBALL_BUTTONS:
        return fgSpaceballNumButtons();

    case GLUT_NUM_TABLET_BUTTONS:
        return 0;

    case GLUT_DEVICE_IGNORE_KEY_REPEAT:
        return fgStructure.CurrentWindow ? fgStructure.CurrentWindow->State.IgnoreKeyRepeat : 0;

    case GLUT_DEVICE_KEY_REPEAT:
        return fgState.KeyRepeat;

    default:
		return fgPlatformGlutDeviceGet ( eWhat );
    }
}

/*
 * This should return the current state of ALT, SHIFT and CTRL keys.
 */
int FGAPIENTRY glutGetModifiers( void )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutGetModifiers" );
    if( fgState.Modifiers == INVALID_MODIFIERS )
    {
        fgWarning( "glutGetModifiers() called outside an input callback" );
        return 0;
    }

    return fgState.Modifiers;
}

/*
 * Return the state of the GLUT API overlay subsystem. A misery ;-)
 */
int FGAPIENTRY glutLayerGet( GLenum eWhat )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutLayerGet" );

    /*
     * This is easy as layers are not implemented and
     * overlay support is not planned. E.g. on Windows,
     * overlay requests in PFDs are ignored
     * (see iLayerType at http://msdn.microsoft.com/en-us/library/dd368826(v=vs.85).aspx)
     */
    switch( eWhat )
    {

    case GLUT_OVERLAY_POSSIBLE:
        return 0 ;

    case GLUT_LAYER_IN_USE:
        return GLUT_NORMAL;

    case GLUT_HAS_OVERLAY:
        return 0;

    case GLUT_TRANSPARENT_INDEX:
        /*
        * Return just anything, which is always defined as zero
        *
        * XXX HUH?
        */
        return 0;

    case GLUT_NORMAL_DAMAGED:
        /* XXX Actually I do not know. Maybe. */
        return 0;

    case GLUT_OVERLAY_DAMAGED:
        return -1;

    default:
        fgWarning( "glutLayerGet(): missing enum handle %d", eWhat );
        break;
    }

    /* And fail. That's good. Programs do love failing. */
    return -1;
}

int * FGAPIENTRY glutGetModeValues(GLenum eWhat, int *size)
{
  int *array;

  FREEGLUT_EXIT_IF_NOT_INITIALISED("glutGetModeValues");

  *size = 0;
  array = fgPlatformGlutGetModeValues ( eWhat, size );

  return array;
}

/*** END OF FILE ***/
