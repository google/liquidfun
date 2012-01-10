/* Written for XI1 by Nikolas Doerfler <doerflen@in.tum.de> (c) 2008 *
 * Rewritten for XI2 by Florian Echtler <echtler@in.tum.de> (c) 2009 */

#include <GL/freeglut.h>

#include "freeglut_internal.h"

#if TARGET_HOST_POSIX_X11 && HAVE_X11_EXTENSIONS_XINPUT2_H

#include <errno.h>
#include <stdarg.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

/* import function from freeglut_main.c */
int fghGetXModifiers( int state );

/* extension opcode for XInput */
int xi_opcode = -1;

/**
 * \brief Sets window up for XI2 events.
 */
void fgRegisterDevices( Display* dpy, Window* win ) {

	XIEventMask mask;
	unsigned char flags[2] = { 0, 0 };
	int event, error;

	/*Display* dpy = fgDisplay.Display;
	Window* win = glutGetXWindow();*/

	/* get XInput extension opcode */
	if (!XQueryExtension( dpy, "XInputExtension", &xi_opcode, &event, &error )) { xi_opcode = -1; }

	/* Select for motion events */
	mask.deviceid = XIAllMasterDevices;
	mask.mask_len = 2;
	mask.mask = flags;

	XISetMask(mask.mask, XI_Enter);
	XISetMask(mask.mask, XI_Motion);
	XISetMask(mask.mask, XI_ButtonPress);
	XISetMask(mask.mask, XI_ButtonRelease);
	XISetMask(mask.mask, XI_Leave);
	/*XISetMask(mask.mask, XI_KeyPress);
	XISetMask(mask.mask, XI_KeyRelease);
	XISetMask(mask.mask, XI_DeviceChanged);
	XISetMask(mask.mask, XI_RawEvent);
	XISetMask(mask.mask, XI_FocusIn);
	XISetMask(mask.mask, XI_FocusOut);
	XISetMask(mask.mask, XI_HierarchyChanged);*/

	XISelectEvents( dpy, *win, &mask, 1 );
}


void fgPrintXILeaveEvent(XILeaveEvent* event)
{
    char* mode = "";
		char* detail = "";
    int i;

    printf("    windows: root 0x%lx event 0x%lx child 0x%ld\n",
            event->root, event->event, event->child);
    switch(event->mode)
    {
        case NotifyNormal:       mode = "NotifyNormal"; break;
        case NotifyGrab:         mode = "NotifyGrab"; break;
        case NotifyUngrab:       mode = "NotifyUngrab"; break;
        case NotifyWhileGrabbed: mode = "NotifyWhileGrabbed"; break;
    }
    switch (event->detail)
    {
        case NotifyAncestor: detail = "NotifyAncestor"; break;
        case NotifyVirtual: detail = "NotifyVirtual"; break;
        case NotifyInferior: detail = "NotifyInferior"; break;
        case NotifyNonlinear: detail = "NotifyNonlinear"; break;
        case NotifyNonlinearVirtual: detail = "NotifyNonlinearVirtual"; break;
        case NotifyPointer: detail = "NotifyPointer"; break;
        case NotifyPointerRoot: detail = "NotifyPointerRoot"; break;
        case NotifyDetailNone: detail = "NotifyDetailNone"; break;
    }
    printf("    mode: %s (detail %s)\n", mode, detail);
    printf("    flags: %s %s\n", event->focus ? "[focus]" : "",
                                 event->same_screen ? "[same screen]" : "");
    printf("    buttons:");
    for (i = 0; i < event->buttons.mask_len * 8; i++)
        if (XIMaskIsSet(event->buttons.mask, i))
            printf(" %d", i);
    printf("\n");

    printf("    modifiers: locked 0x%x latched 0x%x base 0x%x\n",
            event->mods.locked, event->mods.latched,
            event->mods.base);
    printf("    group: locked 0x%x latched 0x%x base 0x%x\n",
            event->group.locked, event->group.latched,
            event->group.base);

    printf("    root x/y:  %.2f / %.2f\n", event->root_x, event->root_y);
    printf("    event x/y: %.2f / %.2f\n", event->event_x, event->event_y);

}


void fgPrintXIDeviceEvent(XIDeviceEvent* event)
{
    double *val;
    int i;

    printf("    device: %d (%d)\n", event->deviceid, event->sourceid);
    printf("    detail: %d\n", event->detail);
    printf("    buttons:");
 	  for (i = 0; i < event->buttons.mask_len * 8; i++)
   	    if (XIMaskIsSet(event->buttons.mask, i))
     	      printf(" %d", i);
    printf("\n");

    printf("    modifiers: locked 0x%x latched 0x%x base 0x%x\n",
            event->mods.locked, event->mods.latched,
            event->mods.base);
    printf("    group: locked 0x%x latched 0x%x base 0x%x\n",
            event->group.locked, event->group.latched,
            event->group.base);
    printf("    valuators:");

    val = event->valuators.values;
    for (i = 0; i < event->valuators.mask_len * 8; i++)
        if (XIMaskIsSet(event->valuators.mask, i))
            printf(" %d: %.2f", i, *val++);
    printf("\n");

    printf("    windows: root 0x%lx event 0x%lx child 0x%ld\n",
            event->root, event->event, event->child);
    printf("    root x/y:  %.2f / %.2f\n", event->root_x, event->root_y);
    printf("    event x/y: %.2f / %.2f\n", event->event_x, event->event_y);

}


/**
 * \brief This function is called when an Extension Event is received
 * and calls the corresponding callback functions for these events.
 */
void fgHandleExtensionEvents( XEvent* base_ev ) {

	int i, button = 0;
	XGenericEventCookie* cookie = (XGenericEventCookie*)&(base_ev->xcookie);

	if ( XGetEventData( fgDisplay.Display, cookie ) && (cookie->type == GenericEvent) && (cookie->extension == xi_opcode) ) {

		XIDeviceEvent* event = (XIDeviceEvent*)(cookie->data);
		/*printf("XI2 event type: %d - %d\n", cookie->evtype, event->type );*/

		SFG_Window* window = fgWindowByHandle( event->event );
		if (!window) return;

		switch (cookie->evtype) {

			case XI_Enter:
			case XI_Leave:
				fgState.Modifiers = fghGetXModifiers( ((XIEnterEvent*)event)->mods.base );
				INVOKE_WCB( *window, MultiEntry, (
					event->deviceid, 
					(event->evtype == XI_Enter ? GLUT_ENTERED : GLUT_LEFT)
				));
				#if _DEBUG
					fgPrintXILeaveEvent((XILeaveEvent*)event);
				#endif
				break;

			case XI_ButtonPress:
			case XI_ButtonRelease:
				fgState.Modifiers = fghGetXModifiers( event->mods.base );
				INVOKE_WCB( *window, MultiButton, (
					event->deviceid, 
					event->event_x,
					event->event_y,
					(event->detail)-1, 
					(event->evtype == XI_ButtonPress ? GLUT_DOWN : GLUT_UP)
				));
				INVOKE_WCB( *window, Mouse, (
					(event->detail)-1, 
					(event->evtype == XI_ButtonPress ? GLUT_DOWN : GLUT_UP),
					event->event_x,
					event->event_y
				));
				break;

			case XI_Motion:
				fgState.Modifiers = fghGetXModifiers( event->mods.base );
				for (i = 0; i < event->buttons.mask_len; i++) if (event->buttons.mask[i]) button = 1;
				if (button) {
					INVOKE_WCB( *window, MultiMotion,  ( event->deviceid, event->event_x, event->event_y ) );
					INVOKE_WCB( *window,           Motion,  (                  event->event_x, event->event_y ) );
				} else {
					INVOKE_WCB( *window, MultiPassive, ( event->deviceid, event->event_x, event->event_y ) );
					INVOKE_WCB( *window,           Passive, (                  event->event_x, event->event_y ) );
				}
				#if _DEBUG
					fgPrintXIDeviceEvent(event);
				#endif
				break;

			default:
				#if _DEBUG
					fgWarning( "Unknown XI2 device event:" );
					fgPrintXIDeviceEvent( event );
				#endif
				break;
		}
		fgState.Modifiers = INVALID_MODIFIERS;
	}
	XFreeEventData( fgDisplay.Display, cookie );
}

#endif

