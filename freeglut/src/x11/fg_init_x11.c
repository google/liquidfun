/*
 * freeglut_init_x11.c
 *
 * Various freeglut initialization functions.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Copied for Platform code by Evan Felix <karcaw at gmail.com>
 * Creation date: Thur Feb 2 2012
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

#define FREEGLUT_BUILDING_LIB
#include <limits.h>  /* LONG_MAX */
#include <GL/freeglut.h>
#include "fg_internal.h"
#include "fg_init.h"
#include "egl/fg_init_egl.h"

/* Return the atom associated with "name". */
static Atom fghGetAtom(const char * name)
{
  return XInternAtom(fgDisplay.pDisplay.Display, name, False);
}

/*
 * Check if "property" is set on "window".  The property's values are returned
 * through "data".  If the property is set and is of type "type", return the
 * number of elements in "data".  Return zero otherwise.  In both cases, use
 * "Xfree()" to free "data".
 */
static int fghGetWindowProperty(Window window,
				Atom property,
				Atom type,
				unsigned char ** data)
{
  /*
   * Caller always has to use "Xfree()" to free "data", since
   * "XGetWindowProperty() always allocates one extra byte in prop_return
   * [i.e. "data"] (even if the property is zero length) [..]".
   */

  int status;  /*  Returned by "XGetWindowProperty". */

  Atom          type_returned;
  int           temp_format;             /*  Not used. */
  unsigned long number_of_elements;
  unsigned long temp_bytes_after;        /*  Not used. */


  status = XGetWindowProperty(fgDisplay.pDisplay.Display,
			      window,
			      property,
			      0,
			      LONG_MAX,
			      False,
			      type,
			      &type_returned,
			      &temp_format,
			      &number_of_elements,
			      &temp_bytes_after,
			      data);

  FREEGLUT_INTERNAL_ERROR_EXIT(status == Success,
			       "XGetWindowProperty failled",
			       "fghGetWindowProperty");

  if (type_returned != type)
  {
    number_of_elements = 0;
  }

  return number_of_elements;
}

/*  Check if the window manager is NET WM compliant. */
static int fghNetWMSupported(void)
{
  Atom wm_check;
  Window ** window_ptr_1;

  int number_of_windows;
  int net_wm_supported;


  net_wm_supported = 0;

  wm_check = fghGetAtom("_NET_SUPPORTING_WM_CHECK");
  window_ptr_1 = malloc(sizeof(Window *));

  /*
   * Check that the window manager has set this property on the root window.
   * The property must be the ID of a child window.
   */
  number_of_windows = fghGetWindowProperty(fgDisplay.pDisplay.RootWindow,
                                           wm_check,
                                           XA_WINDOW,
                                           (unsigned char **) window_ptr_1);
  if (number_of_windows == 1)
  {
    Window ** window_ptr_2;

    window_ptr_2 = malloc(sizeof(Window *));

    /* Check that the window has the same property set to the same value. */
    number_of_windows = fghGetWindowProperty(**window_ptr_1,
                                             wm_check,
                                             XA_WINDOW,
                                             (unsigned char **) window_ptr_2);
    if ((number_of_windows == 1) && (**window_ptr_1 == **window_ptr_2))
    {
      /* NET WM compliant */
      net_wm_supported = 1;
    }

    XFree(*window_ptr_2);
    free(window_ptr_2);
  }

  XFree(*window_ptr_1);
  free(window_ptr_1);

  return net_wm_supported;
}

/*  Check if "hint" is present in "property" for "window". */
int fgHintPresent(Window window, Atom property, Atom hint)
{
  Atom *atoms;
  int number_of_atoms;
  int supported;
  int i;

  supported = 0;

  number_of_atoms = fghGetWindowProperty(window,
					 property,
					 XA_ATOM,
					 (unsigned char **) &atoms);
  for (i = 0; i < number_of_atoms; i++)
  {
      if (atoms[i] == hint)
      {
          supported = 1;
          break;
      }
  }

  XFree(atoms);
  return supported;
}

/*
 * A call to this function should initialize all the display stuff...
 */
void fgPlatformInitialize( const char* displayName )
{
    fgDisplay.pDisplay.Display = XOpenDisplay( displayName );

    if( fgDisplay.pDisplay.Display == NULL )
        fgError( "failed to open display '%s'", XDisplayName( displayName ) );

#ifdef EGL_VERSION_1_0
    fghPlatformInitializeEGL();
#else
    if( !glXQueryExtension( fgDisplay.pDisplay.Display, NULL, NULL ) )
        fgError( "OpenGL GLX extension not supported by display '%s'",
            XDisplayName( displayName ) );
#endif

    fgDisplay.pDisplay.Screen = DefaultScreen( fgDisplay.pDisplay.Display );
    fgDisplay.pDisplay.RootWindow = RootWindow(
        fgDisplay.pDisplay.Display,
        fgDisplay.pDisplay.Screen
    );

    fgDisplay.ScreenWidth  = DisplayWidth(
        fgDisplay.pDisplay.Display,
        fgDisplay.pDisplay.Screen
    );
    fgDisplay.ScreenHeight = DisplayHeight(
        fgDisplay.pDisplay.Display,
        fgDisplay.pDisplay.Screen
    );

    fgDisplay.ScreenWidthMM = DisplayWidthMM(
        fgDisplay.pDisplay.Display,
        fgDisplay.pDisplay.Screen
    );
    fgDisplay.ScreenHeightMM = DisplayHeightMM(
        fgDisplay.pDisplay.Display,
        fgDisplay.pDisplay.Screen
    );

    fgDisplay.pDisplay.Connection = ConnectionNumber( fgDisplay.pDisplay.Display );

    /* Create the window deletion atom */
    fgDisplay.pDisplay.DeleteWindow = fghGetAtom("WM_DELETE_WINDOW");

    /* Create the state and full screen atoms */
    fgDisplay.pDisplay.State           = None;
    fgDisplay.pDisplay.StateFullScreen = None;
    fgDisplay.pDisplay.NetWMPid        = None;
    fgDisplay.pDisplay.ClientMachine   = None;

    fgDisplay.pDisplay.NetWMSupported = fghNetWMSupported();

    if (fgDisplay.pDisplay.NetWMSupported)
    {
      const Atom supported = fghGetAtom("_NET_SUPPORTED");
      const Atom state     = fghGetAtom("_NET_WM_STATE");
      
      /* Check if the state hint is supported. */
      if (fgHintPresent(fgDisplay.pDisplay.RootWindow, supported, state))
      {
        const Atom full_screen = fghGetAtom("_NET_WM_STATE_FULLSCREEN");
        
        fgDisplay.pDisplay.State = state;
        
        /* Check if the window manager supports full screen. */
        /**  Check "_NET_WM_ALLOWED_ACTIONS" on our window instead? **/
        if (fgHintPresent(fgDisplay.pDisplay.RootWindow, supported, full_screen))
        {
          fgDisplay.pDisplay.StateFullScreen = full_screen;
        }
      }

      fgDisplay.pDisplay.NetWMPid = fghGetAtom("_NET_WM_PID");
      fgDisplay.pDisplay.ClientMachine = fghGetAtom("WM_CLIENT_MACHINE");
    }

    /* Get start time */
    fgState.Time = fgSystemTime();
    

    fgState.Initialised = GL_TRUE;

    atexit(fgDeinitialize);

    /* InputDevice uses GlutTimerFunc(), so fgState.Initialised must be TRUE */
    fgInitialiseInputDevices();
}

void fgPlatformDeinitialiseInputDevices ( void )
{
	fghCloseInputDevices ();

    fgState.JoysticksInitialised = GL_FALSE;
    fgState.InputDevsInitialised = GL_FALSE;
}


void fgPlatformCloseDisplay ( void )
{
    /*
     * Make sure all X-client data we have created will be destroyed on
     * display closing
     */
    XSetCloseDownMode( fgDisplay.pDisplay.Display, DestroyAll );

    /*
     * Close the display connection, destroying all windows we have
     * created so far
     */
    XCloseDisplay( fgDisplay.pDisplay.Display );
}


#ifndef EGL_VERSION_1_0
void fgPlatformDestroyContext ( SFG_PlatformDisplay pDisplay, SFG_WindowContextType MContext )
{
    /* Note that the MVisualInfo is not owned by the MenuContext! */
    glXDestroyContext( pDisplay.Display, MContext );
}
#endif
