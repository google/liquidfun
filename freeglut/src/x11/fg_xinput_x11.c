/* Written for XI1 by Nikolas Doerfler <doerflen@in.tum.de> (c) 2008 *
 * Rewritten for XI2 by Florian Echtler <echtler@in.tum.de> (c) 2009 */

#include <GL/freeglut.h>

#include "../fg_internal.h"

#ifdef HAVE_X11_EXTENSIONS_XINPUT2_H

#include <errno.h>
#include <stdarg.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

/* convert the XInput button state mask to the regular X mouse event button mask */
#define BUTTON_MASK(xistate)	((xistate) << 8)

/* import function from freeglut_main.c */
extern int fgPlatformGetModifiers( int state );

/* extension opcode for XInput */
int xi_opcode = -1;

/**
 * \brief Sets window up for XI2 events.
 */
void fgRegisterDevices( Display* dpy, Window* win ) {

	XIEventMask mask;
	unsigned char flags[2] = { 0, 0 };
	int event, error;

	/*Display* dpy = fgDisplay.pDisplay.Display;
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
void fgHandleExtensionEvents( XEvent* base_ev )
{
    XEvent std_ev;    /* standard single-pointer event to be added to the event queue */
    int i, button = 0;
    XGenericEventCookie* cookie = (XGenericEventCookie*)&(base_ev->xcookie);

    /* initialize the generic fields from base_ev */
    std_ev.xany = base_ev->xany;

    if ( XGetEventData( fgDisplay.pDisplay.Display, cookie ) && (cookie->type == GenericEvent) && (cookie->extension == xi_opcode) ) {

        XIDeviceEvent* event = (XIDeviceEvent*)(cookie->data);
        XIEnterEvent *evcross;
        /*printf("XI2 event type: %d - %d\n", cookie->evtype, event->type );*/

        SFG_Window* window = fgWindowByHandle( event->event );
        if (!window) return;

        switch (cookie->evtype) {
        case XI_Enter:
        case XI_Leave:
            evcross = (XIEnterEvent*)event;

            fgState.Modifiers = fgPlatformGetModifiers( evcross->mods.base );
            INVOKE_WCB( *window, MultiEntry, (
                event->deviceid,
                (event->evtype == XI_Enter ? GLUT_ENTERED : GLUT_LEFT)
            ));
            #if _DEBUG
            fgPrintXILeaveEvent((XILeaveEvent*)event);
            #endif

            /* Also process the standard crossing event */
            std_ev.type = evcross->evtype == XI_Enter ? EnterNotify : LeaveNotify;
            std_ev.xcrossing.window = evcross->event;
            std_ev.xcrossing.root = evcross->root;
            std_ev.xcrossing.subwindow = evcross->child;
            std_ev.xcrossing.x = evcross->event_x;
            std_ev.xcrossing.y = evcross->event_y;
            std_ev.xcrossing.x_root = evcross->root_x;
            std_ev.xcrossing.y_root = evcross->root_y;
            std_ev.xcrossing.mode = evcross->mode;
            std_ev.xcrossing.detail = evcross->detail;
            std_ev.xcrossing.same_screen = evcross->same_screen;
            std_ev.xcrossing.focus = evcross->focus;
            std_ev.xcrossing.state = BUTTON_MASK(*(unsigned int*)evcross->buttons.mask);

            XPutBackEvent(fgDisplay.pDisplay.Display, &std_ev);
            break;

        case XI_ButtonPress:
        case XI_ButtonRelease:
            fgState.Modifiers = fgPlatformGetModifiers( event->mods.base );
            INVOKE_WCB( *window, MultiButton, (
                event->deviceid,
                event->event_x,
                event->event_y,
                event->detail-1,
                (event->evtype == XI_ButtonPress ? GLUT_DOWN : GLUT_UP)
            ));

            /* Also process the standard button event */
            std_ev.type = event->evtype == XI_ButtonPress ? ButtonPress : ButtonRelease;
            std_ev.xbutton.window = event->event;
            std_ev.xbutton.root = event->root;
            std_ev.xbutton.subwindow = event->child;
            std_ev.xbutton.x = event->event_x;
            std_ev.xbutton.y = event->event_y;
            std_ev.xbutton.x_root = event->root_x;
            std_ev.xbutton.y_root = event->root_y;
            std_ev.xbutton.state = event->mods.base;
            std_ev.xbutton.button = event->detail;

            XPutBackEvent(fgDisplay.pDisplay.Display, &std_ev);
            break;

        case XI_Motion:
            fgState.Modifiers = fgPlatformGetModifiers( event->mods.base );
            for (i = 0; i < event->buttons.mask_len; i++) {
                if (event->buttons.mask[i]) {
                    button = 1;
                }
            }
            if (button) {
                INVOKE_WCB( *window, MultiMotion,  ( event->deviceid, event->event_x, event->event_y ) );
            } else {
                INVOKE_WCB( *window, MultiPassive, ( event->deviceid, event->event_x, event->event_y ) );
            }
            #if _DEBUG
            fgPrintXIDeviceEvent(event);
            #endif

            /* Also process the standard motion event */
            std_ev.type = MotionNotify;
            std_ev.xmotion.window = event->event;
            std_ev.xmotion.root = event->root;
            std_ev.xmotion.subwindow = event->child;
            std_ev.xmotion.time = event->time;
            std_ev.xmotion.x = event->event_x;
            std_ev.xmotion.y = event->event_y;
            std_ev.xmotion.x_root = event->root_x;
            std_ev.xmotion.y_root = event->root_y;
            std_ev.xmotion.state = BUTTON_MASK(*(unsigned int*)event->buttons.mask);
            std_ev.xmotion.is_hint = NotifyNormal;

            XPutBackEvent(fgDisplay.pDisplay.Display, &std_ev);
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
    XFreeEventData( fgDisplay.pDisplay.Display, cookie );
}

#endif
