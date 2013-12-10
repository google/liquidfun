/*
 * freeglut_main_x11.c
 *
 * The X11-specific windows message processing methods.
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

#include <GL/freeglut.h>
#include "../fg_internal.h"
#ifdef HAVE_ERRNO_H
#    include <errno.h>
#endif
#include <stdarg.h>
#ifdef  HAVE_VFPRINTF
#    define VFPRINTF(s,f,a) vfprintf((s),(f),(a))
#elif defined(HAVE__DOPRNT)
#    define VFPRINTF(s,f,a) _doprnt((f),(a),(s))
#else
#    define VFPRINTF(s,f,a)
#endif


/*
 * Try to get the maximum value allowed for ints, falling back to the minimum
 * guaranteed by ISO C99 if there is no suitable header.
 */
#ifdef HAVE_LIMITS_H
#    include <limits.h>
#endif
#ifndef INT_MAX
#    define INT_MAX 32767
#endif

#ifndef MIN
#    define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif

extern void fghOnReshapeNotify(SFG_Window *window, int width, int height, GLboolean forceNotify);
extern void fghOnPositionNotify(SFG_Window *window, int x, int y, GLboolean forceNotify);
extern void fgPlatformFullScreenToggle( SFG_Window *win );
extern void fgPlatformPositionWindow( SFG_Window *window, int x, int y );
extern void fgPlatformReshapeWindow ( SFG_Window *window, int width, int height );
extern void fgPlatformPushWindow( SFG_Window *window );
extern void fgPlatformPopWindow( SFG_Window *window );
extern void fgPlatformHideWindow( SFG_Window *window );
extern void fgPlatformIconifyWindow( SFG_Window *window );
extern void fgPlatformShowWindow( SFG_Window *window );

/* used in the event handling code to match and discard stale mouse motion events */
static Bool match_motion(Display *dpy, XEvent *xev, XPointer arg);

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 * There are some issues concerning window redrawing under X11, and maybe
 * some events are not handled.
 *
 * Need to investigate why the X11 version breaks out with an error when
 * closing a window (using the window manager, not glutDestroyWindow)...
 */
 
 

fg_time_t fgPlatformSystemTime ( void )
{
#ifdef CLOCK_MONOTONIC
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_nsec/1000000 + now.tv_sec*1000;
#elif defined(HAVE_GETTIMEOFDAY)
    struct timeval now;
    gettimeofday( &now, NULL );
    return now.tv_usec/1000 + now.tv_sec*1000;
#endif
}

/*
 * Does the magic required to relinquish the CPU until something interesting
 * happens.
 */

void fgPlatformSleepForEvents( fg_time_t msec )
{
    /*
     * Possibly due to aggressive use of XFlush() and friends,
     * it is possible to have our socket drained but still have
     * unprocessed events.  (Or, this may just be normal with
     * X, anyway?)  We do non-trivial processing of X events
     * after the event-reading loop, in any case, so we
     * need to allow that we may have an empty socket but non-
     * empty event queue.
     */
    if( ! XPending( fgDisplay.pDisplay.Display ) )
    {
        fd_set fdset;
        int err;
        int socket;
        struct timeval wait;

        socket = ConnectionNumber( fgDisplay.pDisplay.Display );
        FD_ZERO( &fdset );
        FD_SET( socket, &fdset );
        wait.tv_sec = msec / 1000;
        wait.tv_usec = (msec % 1000) * 1000;
        err = select( socket+1, &fdset, NULL, NULL, &wait );

#ifdef HAVE_ERRNO_H
        if( ( -1 == err ) && ( errno != EINTR ) )
            fgWarning ( "freeglut select() error: %d", errno );
#endif
    }
}


/*
 * Returns GLUT modifier mask for the state field of an X11 event.
 */
int fgPlatformGetModifiers( int state )
{
    int ret = 0;

    if( state & ( ShiftMask | LockMask ) )
        ret |= GLUT_ACTIVE_SHIFT;
    if( state & ControlMask )
        ret |= GLUT_ACTIVE_CTRL;
    if( state & Mod1Mask )
        ret |= GLUT_ACTIVE_ALT;

    return ret;
}

static const char* fghTypeToString( int type )
{
    switch( type ) {
    case KeyPress: return "KeyPress";
    case KeyRelease: return "KeyRelease";
    case ButtonPress: return "ButtonPress";
    case ButtonRelease: return "ButtonRelease";
    case MotionNotify: return "MotionNotify";
    case EnterNotify: return "EnterNotify";
    case LeaveNotify: return "LeaveNotify";
    case FocusIn: return "FocusIn";
    case FocusOut: return "FocusOut";
    case KeymapNotify: return "KeymapNotify";
    case Expose: return "Expose";
    case GraphicsExpose: return "GraphicsExpose";
    case NoExpose: return "NoExpose";
    case VisibilityNotify: return "VisibilityNotify";
    case CreateNotify: return "CreateNotify";
    case DestroyNotify: return "DestroyNotify";
    case UnmapNotify: return "UnmapNotify";
    case MapNotify: return "MapNotify";
    case MapRequest: return "MapRequest";
    case ReparentNotify: return "ReparentNotify";
    case ConfigureNotify: return "ConfigureNotify";
    case ConfigureRequest: return "ConfigureRequest";
    case GravityNotify: return "GravityNotify";
    case ResizeRequest: return "ResizeRequest";
    case CirculateNotify: return "CirculateNotify";
    case CirculateRequest: return "CirculateRequest";
    case PropertyNotify: return "PropertyNotify";
    case SelectionClear: return "SelectionClear";
    case SelectionRequest: return "SelectionRequest";
    case SelectionNotify: return "SelectionNotify";
    case ColormapNotify: return "ColormapNotify";
    case ClientMessage: return "ClientMessage";
    case MappingNotify: return "MappingNotify";
    default: return "UNKNOWN";
    }
}

static const char* fghBoolToString( Bool b )
{
    return b == False ? "False" : "True";
}

static const char* fghNotifyHintToString( char is_hint )
{
    switch( is_hint ) {
    case NotifyNormal: return "NotifyNormal";
    case NotifyHint: return "NotifyHint";
    default: return "UNKNOWN";
    }
}

static const char* fghNotifyModeToString( int mode )
{
    switch( mode ) {
    case NotifyNormal: return "NotifyNormal";
    case NotifyGrab: return "NotifyGrab";
    case NotifyUngrab: return "NotifyUngrab";
    case NotifyWhileGrabbed: return "NotifyWhileGrabbed";
    default: return "UNKNOWN";
    }
}

static const char* fghNotifyDetailToString( int detail )
{
    switch( detail ) {
    case NotifyAncestor: return "NotifyAncestor";
    case NotifyVirtual: return "NotifyVirtual";
    case NotifyInferior: return "NotifyInferior";
    case NotifyNonlinear: return "NotifyNonlinear";
    case NotifyNonlinearVirtual: return "NotifyNonlinearVirtual";
    case NotifyPointer: return "NotifyPointer";
    case NotifyPointerRoot: return "NotifyPointerRoot";
    case NotifyDetailNone: return "NotifyDetailNone";
    default: return "UNKNOWN";
    }
}

static const char* fghVisibilityToString( int state ) {
    switch( state ) {
    case VisibilityUnobscured: return "VisibilityUnobscured";
    case VisibilityPartiallyObscured: return "VisibilityPartiallyObscured";
    case VisibilityFullyObscured: return "VisibilityFullyObscured";
    default: return "UNKNOWN";
    }
}

static const char* fghConfigureDetailToString( int detail )
{
    switch( detail ) {
    case Above: return "Above";
    case Below: return "Below";
    case TopIf: return "TopIf";
    case BottomIf: return "BottomIf";
    case Opposite: return "Opposite";
    default: return "UNKNOWN";
    }
}

static const char* fghPlaceToString( int place )
{
    switch( place ) {
    case PlaceOnTop: return "PlaceOnTop";
    case PlaceOnBottom: return "PlaceOnBottom";
    default: return "UNKNOWN";
    }
}

static const char* fghMappingRequestToString( int request )
{
    switch( request ) {
    case MappingModifier: return "MappingModifier";
    case MappingKeyboard: return "MappingKeyboard";
    case MappingPointer: return "MappingPointer";
    default: return "UNKNOWN";
    }
}

static const char* fghPropertyStateToString( int state )
{
    switch( state ) {
    case PropertyNewValue: return "PropertyNewValue";
    case PropertyDelete: return "PropertyDelete";
    default: return "UNKNOWN";
    }
}

static const char* fghColormapStateToString( int state )
{
    switch( state ) {
    case ColormapUninstalled: return "ColormapUninstalled";
    case ColormapInstalled: return "ColormapInstalled";
    default: return "UNKNOWN";
    }
}

__fg_unused static void fghPrintEvent( XEvent *event )
{
    switch( event->type ) {

    case KeyPress:
    case KeyRelease: {
        XKeyEvent *e = &event->xkey;
        fgWarning( "%s: window=0x%x, root=0x%x, subwindow=0x%x, time=%lu, "
                   "(x,y)=(%d,%d), (x_root,y_root)=(%d,%d), state=0x%x, "
                   "keycode=%u, same_screen=%s", fghTypeToString( e->type ),
                   e->window, e->root, e->subwindow, (unsigned long)e->time,
                   e->x, e->y, e->x_root, e->y_root, e->state, e->keycode,
                   fghBoolToString( e->same_screen ) );
        break;
    }

    case ButtonPress:
    case ButtonRelease: {
        XButtonEvent *e = &event->xbutton;
        fgWarning( "%s: window=0x%x, root=0x%x, subwindow=0x%x, time=%lu, "
                   "(x,y)=(%d,%d), (x_root,y_root)=(%d,%d), state=0x%x, "
                   "button=%u, same_screen=%d", fghTypeToString( e->type ),
                   e->window, e->root, e->subwindow, (unsigned long)e->time,
                   e->x, e->y, e->x_root, e->y_root, e->state, e->button,
                   fghBoolToString( e->same_screen ) );
        break;
    }

    case MotionNotify: {
        XMotionEvent *e = &event->xmotion;
        fgWarning( "%s: window=0x%x, root=0x%x, subwindow=0x%x, time=%lu, "
                   "(x,y)=(%d,%d), (x_root,y_root)=(%d,%d), state=0x%x, "
                   "is_hint=%s, same_screen=%d", fghTypeToString( e->type ),
                   e->window, e->root, e->subwindow, (unsigned long)e->time,
                   e->x, e->y, e->x_root, e->y_root, e->state,
                   fghNotifyHintToString( e->is_hint ),
                   fghBoolToString( e->same_screen ) );
        break;
    }

    case EnterNotify:
    case LeaveNotify: {
        XCrossingEvent *e = &event->xcrossing;
        fgWarning( "%s: window=0x%x, root=0x%x, subwindow=0x%x, time=%lu, "
                   "(x,y)=(%d,%d), mode=%s, detail=%s, same_screen=%d, "
                   "focus=%d, state=0x%x", fghTypeToString( e->type ),
                   e->window, e->root, e->subwindow, (unsigned long)e->time,
                   e->x, e->y, fghNotifyModeToString( e->mode ),
                   fghNotifyDetailToString( e->detail ), (int)e->same_screen,
                   (int)e->focus, e->state );
        break;
    }

    case FocusIn:
    case FocusOut: {
        XFocusChangeEvent *e = &event->xfocus;
        fgWarning( "%s: window=0x%x, mode=%s, detail=%s",
                   fghTypeToString( e->type ), e->window,
                   fghNotifyModeToString( e->mode ),
                   fghNotifyDetailToString( e->detail ) );
        break;
    }

    case KeymapNotify: {
        XKeymapEvent *e = &event->xkeymap;
        char buf[32 * 2 + 1];
        int i;
        for ( i = 0; i < 32; i++ ) {
            snprintf( &buf[ i * 2 ], sizeof( buf ) - i * 2,
                      "%02x", e->key_vector[ i ] );
        }
        buf[ i ] = '\0';
        fgWarning( "%s: window=0x%x, %s", fghTypeToString( e->type ), e->window,
                   buf );
        break;
    }

    case Expose: {
        XExposeEvent *e = &event->xexpose;
        fgWarning( "%s: window=0x%x, (x,y)=(%d,%d), (width,height)=(%d,%d), "
                   "count=%d", fghTypeToString( e->type ), e->window, e->x,
                   e->y, e->width, e->height, e->count );
        break;
    }

    case GraphicsExpose: {
        XGraphicsExposeEvent *e = &event->xgraphicsexpose;
        fgWarning( "%s: drawable=0x%x, (x,y)=(%d,%d), (width,height)=(%d,%d), "
                   "count=%d, (major_code,minor_code)=(%d,%d)",
                   fghTypeToString( e->type ), e->drawable, e->x, e->y,
                   e->width, e->height, e->count, e->major_code,
                   e->minor_code );
        break;
    }

    case NoExpose: {
        XNoExposeEvent *e = &event->xnoexpose;
        fgWarning( "%s: drawable=0x%x, (major_code,minor_code)=(%d,%d)",
                   fghTypeToString( e->type ), e->drawable, e->major_code,
                   e->minor_code );
        break;
    }

    case VisibilityNotify: {
        XVisibilityEvent *e = &event->xvisibility;
        fgWarning( "%s: window=0x%x, state=%s", fghTypeToString( e->type ),
                   e->window, fghVisibilityToString( e->state) );
        break;
    }

    case CreateNotify: {
        XCreateWindowEvent *e = &event->xcreatewindow;
        fgWarning( "%s: (x,y)=(%d,%d), (width,height)=(%d,%d), border_width=%d, "
                   "window=0x%x, override_redirect=%s",
                   fghTypeToString( e->type ), e->x, e->y, e->width, e->height,
                   e->border_width, e->window,
                   fghBoolToString( e->override_redirect ) );
        break;
    }

    case DestroyNotify: {
        XDestroyWindowEvent *e = &event->xdestroywindow;
        fgWarning( "%s: event=0x%x, window=0x%x",
                   fghTypeToString( e->type ), e->event, e->window );
        break;
    }

    case UnmapNotify: {
        XUnmapEvent *e = &event->xunmap;
        fgWarning( "%s: event=0x%x, window=0x%x, from_configure=%s",
                   fghTypeToString( e->type ), e->event, e->window,
                   fghBoolToString( e->from_configure ) );
        break;
    }

    case MapNotify: {
        XMapEvent *e = &event->xmap;
        fgWarning( "%s: event=0x%x, window=0x%x, override_redirect=%s",
                   fghTypeToString( e->type ), e->event, e->window,
                   fghBoolToString( e->override_redirect ) );
        break;
    }

    case MapRequest: {
        XMapRequestEvent *e = &event->xmaprequest;
        fgWarning( "%s: parent=0x%x, window=0x%x",
                   fghTypeToString( event->type ), e->parent, e->window );
        break;
    }

    case ReparentNotify: {
        XReparentEvent *e = &event->xreparent;
        fgWarning( "%s: event=0x%x, window=0x%x, parent=0x%x, (x,y)=(%d,%d), "
                   "override_redirect=%s", fghTypeToString( e->type ),
                   e->event, e->window, e->parent, e->x, e->y,
                   fghBoolToString( e->override_redirect ) );
        break;
    }

    case ConfigureNotify: {
        XConfigureEvent *e = &event->xconfigure;
        fgWarning( "%s: event=0x%x, window=0x%x, (x,y)=(%d,%d), "
                   "(width,height)=(%d,%d), border_width=%d, above=0x%x, "
                   "override_redirect=%s", fghTypeToString( e->type ), e->event,
                   e->window, e->x, e->y, e->width, e->height, e->border_width,
                   e->above, fghBoolToString( e->override_redirect ) );
        break;
    }

    case ConfigureRequest: {
        XConfigureRequestEvent *e = &event->xconfigurerequest;
        fgWarning( "%s: parent=0x%x, window=0x%x, (x,y)=(%d,%d), "
                   "(width,height)=(%d,%d), border_width=%d, above=0x%x, "
                   "detail=%s, value_mask=%lx", fghTypeToString( e->type ),
                   e->parent, e->window, e->x, e->y, e->width, e->height,
                   e->border_width, e->above,
                   fghConfigureDetailToString( e->detail ), e->value_mask );
        break;
    }

    case GravityNotify: {
        XGravityEvent *e = &event->xgravity;
        fgWarning( "%s: event=0x%x, window=0x%x, (x,y)=(%d,%d)",
                   fghTypeToString( e->type ), e->event, e->window, e->x, e->y );
        break;
    }

    case ResizeRequest: {
        XResizeRequestEvent *e = &event->xresizerequest;
        fgWarning( "%s: window=0x%x, (width,height)=(%d,%d)",
                   fghTypeToString( e->type ), e->window, e->width, e->height );
        break;
    }

    case CirculateNotify: {
        XCirculateEvent *e = &event->xcirculate;
        fgWarning( "%s: event=0x%x, window=0x%x, place=%s",
                   fghTypeToString( e->type ), e->event, e->window,
                   fghPlaceToString( e->place ) );
        break;
    }

    case CirculateRequest: {
        XCirculateRequestEvent *e = &event->xcirculaterequest;
        fgWarning( "%s: parent=0x%x, window=0x%x, place=%s",
                   fghTypeToString( e->type ), e->parent, e->window,
                   fghPlaceToString( e->place ) );
        break;
    }

    case PropertyNotify: {
        XPropertyEvent *e = &event->xproperty;
        fgWarning( "%s: window=0x%x, atom=%lu, time=%lu, state=%s",
                   fghTypeToString( e->type ), e->window,
                   (unsigned long)e->atom, (unsigned long)e->time,
                   fghPropertyStateToString( e->state ) );
        break;
    }

    case SelectionClear: {
        XSelectionClearEvent *e = &event->xselectionclear;
        fgWarning( "%s: window=0x%x, selection=%lu, time=%lu",
                   fghTypeToString( e->type ), e->window,
                   (unsigned long)e->selection, (unsigned long)e->time );
        break;
    }

    case SelectionRequest: {
        XSelectionRequestEvent *e = &event->xselectionrequest;
        fgWarning( "%s: owner=0x%x, requestor=0x%x, selection=0x%x, "
                   "target=0x%x, property=%lu, time=%lu",
                   fghTypeToString( e->type ), e->owner, e->requestor,
                   (unsigned long)e->selection, (unsigned long)e->target,
                   (unsigned long)e->property, (unsigned long)e->time );
        break;
    }

    case SelectionNotify: {
        XSelectionEvent *e = &event->xselection;
        fgWarning( "%s: requestor=0x%x, selection=0x%x, target=0x%x, "
                   "property=%lu, time=%lu", fghTypeToString( e->type ),
                   e->requestor, (unsigned long)e->selection,
                   (unsigned long)e->target, (unsigned long)e->property,
                   (unsigned long)e->time );
        break;
    }

    case ColormapNotify: {
        XColormapEvent *e = &event->xcolormap;
        fgWarning( "%s: window=0x%x, colormap=%lu, new=%s, state=%s",
                   fghTypeToString( e->type ), e->window,
                   (unsigned long)e->colormap, fghBoolToString( e->new ),
                   fghColormapStateToString( e->state ) );
        break;
    }

    case ClientMessage: {
        XClientMessageEvent *e = &event->xclient;
        char buf[ 61 ];
        char* p = buf;
        char* end = buf + sizeof( buf );
        int i;
        switch( e->format ) {
        case 8:
          for ( i = 0; i < 20; i++, p += 3 ) {
                snprintf( p, end - p, " %02x", e->data.b[ i ] );
            }
            break;
        case 16:
            for ( i = 0; i < 10; i++, p += 5 ) {
                snprintf( p, end - p, " %04x", e->data.s[ i ] );
            }
            break;
        case 32:
            for ( i = 0; i < 5; i++, p += 9 ) {
                snprintf( p, end - p, " %08lx", e->data.l[ i ] );
            }
            break;
        }
        *p = '\0';
        fgWarning( "%s: window=0x%x, message_type=%lu, format=%d, data=(%s )",
                   fghTypeToString( e->type ), e->window,
                   (unsigned long)e->message_type, e->format, buf );
        break;
    }

    case MappingNotify: {
        XMappingEvent *e = &event->xmapping;
        fgWarning( "%s: window=0x%x, request=%s, first_keycode=%d, count=%d",
                   fghTypeToString( e->type ), e->window,
                   fghMappingRequestToString( e->request ), e->first_keycode,
                   e->count );
        break;
    }

    default: {
        fgWarning( "%s", fghTypeToString( event->type ) );
        break;
    }
    }
}


void fgPlatformProcessSingleEvent ( void )
{
    SFG_Window* window;
    XEvent event;

    /* This code was repeated constantly, so here it goes into a definition: */
#define GETWINDOW(a)                             \
    window = fgWindowByHandle( event.a.window ); \
    if( window == NULL )                         \
        break;

#define GETMOUSE(a)                              \
    window->State.MouseX = event.a.x;            \
    window->State.MouseY = event.a.y;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutMainLoopEvent" );

    while( XPending( fgDisplay.pDisplay.Display ) )
    {
        XNextEvent( fgDisplay.pDisplay.Display, &event );
#if _DEBUG
        fghPrintEvent( &event );
#endif

        switch( event.type )
        {
        case ClientMessage:
            if (fgStructure.CurrentWindow)
                if(fgIsSpaceballXEvent(&event)) {
                    fgSpaceballHandleXEvent(&event);
                    break;
                }
            /* Destroy the window when the WM_DELETE_WINDOW message arrives */
            if( (Atom) event.xclient.data.l[ 0 ] == fgDisplay.pDisplay.DeleteWindow )
            {
                GETWINDOW( xclient );

                fgDestroyWindow ( window );

                if( fgState.ActionOnWindowClose == GLUT_ACTION_EXIT )
                {
                    fgDeinitialize( );
                    exit( 0 );
                }
                else if( fgState.ActionOnWindowClose == GLUT_ACTION_GLUTMAINLOOP_RETURNS )
                    fgState.ExecState = GLUT_EXEC_STATE_STOP;

                return;
            }
            break;

            /*
             * CreateNotify causes a configure-event so that sub-windows are
             * handled compatibly with GLUT.  Otherwise, your sub-windows
             * (in freeglut only) will not get an initial reshape event,
             * which can break things.
             *
             * GLUT presumably does this because it generally tries to treat
             * sub-windows the same as windows.
             */
        case CreateNotify:
        case ConfigureNotify:
            {
                int width, height, x, y;
                if( event.type == CreateNotify ) {
                    GETWINDOW( xcreatewindow );
                    width = event.xcreatewindow.width;
                    height = event.xcreatewindow.height;
                    x = event.xcreatewindow.x;
                    y = event.xcreatewindow.y;
                } else {
                    GETWINDOW( xconfigure );
                    width = event.xconfigure.width;
                    height = event.xconfigure.height;
                    x = event.xconfigure.x;
                    y = event.xconfigure.y;
                }

                /* Update state and call callback, if there was a change */
                fghOnPositionNotify(window, x, y, GL_FALSE);
                /* Update state and call callback, if there was a change */
                fghOnReshapeNotify(window, width, height, GL_FALSE);
            }
            break;

        case DestroyNotify:
            /*
             * This is sent to confirm the XDestroyWindow call.
             *
             * XXX WHY is this commented out?  Should we re-enable it?
             */
            /* fgAddToWindowDestroyList ( window ); */
            break;

        case Expose:
            /*
             * We are too dumb to process partial exposes...
             *
             * XXX Well, we could do it.  However, it seems to only
             * XXX be potentially useful for single-buffered (since
             * XXX double-buffered does not respect viewport when we
             * XXX do a buffer-swap).
             *
             */
            if( event.xexpose.count == 0 )
            {
                GETWINDOW( xexpose );
                window->State.WorkMask |= GLUT_DISPLAY_WORK;
            }
            break;

        case MapNotify:
            break;

        case UnmapNotify:
            /* We get this when iconifying a window. */ 
            GETWINDOW( xunmap );
            INVOKE_WCB( *window, WindowStatus, ( GLUT_HIDDEN ) );
            window->State.Visible = GL_FALSE;
            break;

        case MappingNotify:
            /*
             * Have the client's keyboard knowledge updated (xlib.ps,
             * page 206, says that's a good thing to do)
             */
            XRefreshKeyboardMapping( (XMappingEvent *) &event );
            break;

        case VisibilityNotify:
        {
            /*
             * Sending this event, the X server can notify us that the window
             * has just acquired one of the three possible visibility states:
             * VisibilityUnobscured, VisibilityPartiallyObscured or
             * VisibilityFullyObscured. Note that we DO NOT receive a
             * VisibilityNotify event when iconifying a window, we only get an
             * UnmapNotify then.
             */
            GETWINDOW( xvisibility );
            switch( event.xvisibility.state )
            {
            case VisibilityUnobscured:
                INVOKE_WCB( *window, WindowStatus, ( GLUT_FULLY_RETAINED ) );
                window->State.Visible = GL_TRUE;
                break;

            case VisibilityPartiallyObscured:
                INVOKE_WCB( *window, WindowStatus,
                            ( GLUT_PARTIALLY_RETAINED ) );
                window->State.Visible = GL_TRUE;
                break;

            case VisibilityFullyObscured:
                INVOKE_WCB( *window, WindowStatus, ( GLUT_FULLY_COVERED ) );
                window->State.Visible = GL_FALSE;
                break;

            default:
                fgWarning( "Unknown X visibility state: %d",
                           event.xvisibility.state );
                break;
            }
        }
        break;

        case EnterNotify:
        case LeaveNotify:
            GETWINDOW( xcrossing );
            GETMOUSE( xcrossing );
            if( ( event.type == LeaveNotify ) && window->IsMenu &&
                window->ActiveMenu && window->ActiveMenu->IsActive )
                fgUpdateMenuHighlight( window->ActiveMenu );

            INVOKE_WCB( *window, Entry, ( ( EnterNotify == event.type ) ?
                                          GLUT_ENTERED :
                                          GLUT_LEFT ) );
            break;

        case MotionNotify:
        {
            /* if GLUT_SKIP_STALE_MOTION_EVENTS is true, then discard all but
             * the last motion event from the queue
             */
            if(fgState.SkipStaleMotion) {
                while(XCheckIfEvent(fgDisplay.pDisplay.Display, &event, match_motion, 0));
            }

            GETWINDOW( xmotion );
            GETMOUSE( xmotion );

            if( window->ActiveMenu )
            {
                if( window == window->ActiveMenu->ParentWindow )
                {
                    window->ActiveMenu->Window->State.MouseX =
                        event.xmotion.x_root - window->ActiveMenu->X;
                    window->ActiveMenu->Window->State.MouseY =
                        event.xmotion.y_root - window->ActiveMenu->Y;
                }

                fgUpdateMenuHighlight( window->ActiveMenu );

                break;
            }

            /*
             * XXX For more than 5 buttons, just check {event.xmotion.state},
             * XXX rather than a host of bit-masks?  Or maybe we need to
             * XXX track ButtonPress/ButtonRelease events in our own
             * XXX bit-mask?
             */
            fgState.Modifiers = fgPlatformGetModifiers( event.xmotion.state );
            if ( event.xmotion.state & ( Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask ) ) {
                INVOKE_WCB( *window, Motion, ( event.xmotion.x,
                                               event.xmotion.y ) );
            } else {
                INVOKE_WCB( *window, Passive, ( event.xmotion.x,
                                                event.xmotion.y ) );
            }
            fgState.Modifiers = INVALID_MODIFIERS;
        }
        break;

        case ButtonRelease:
        case ButtonPress:
        {
            GLboolean pressed = GL_TRUE;
            int button;

            if( event.type == ButtonRelease )
                pressed = GL_FALSE ;

            /*
             * A mouse button has been pressed or released. Traditionally,
             * break if the window was found within the freeglut structures.
             */
            GETWINDOW( xbutton );
            GETMOUSE( xbutton );

            /*
             * An X button (at least in XFree86) is numbered from 1.
             * A GLUT button is numbered from 0.
             * Old GLUT passed through buttons other than just the first
             * three, though it only gave symbolic names and official
             * support to the first three.
             */
            button = event.xbutton.button - 1;

            /*
             * Do not execute the application's mouse callback if a menu
             * is hooked to this button.  In that case an appropriate
             * private call should be generated.
             */
            if( fgCheckActiveMenu( window, button, pressed,
                                   event.xbutton.x, event.xbutton.y ) )
                break;

            /*
             * Check if there is a mouse or mouse wheel callback hooked to the
             * window
             */
            if( ! FETCH_WCB( *window, Mouse ) &&
                ! FETCH_WCB( *window, MouseWheel ) )
                break;

            fgState.Modifiers = fgPlatformGetModifiers( event.xbutton.state );

            /* Finally execute the mouse or mouse wheel callback */
            if( ( button < glutDeviceGet ( GLUT_NUM_MOUSE_BUTTONS ) ) || ( ! FETCH_WCB( *window, MouseWheel ) ) )
                INVOKE_WCB( *window, Mouse, ( button,
                                              pressed ? GLUT_DOWN : GLUT_UP,
                                              event.xbutton.x,
                                              event.xbutton.y )
                );
            else
            {
                /*
                 * Map 4 and 5 to wheel zero; EVEN to +1, ODD to -1
                 *  "  6 and 7 "    "   one; ...
                 *
                 * XXX This *should* be behind some variables/macros,
                 * XXX since the order and numbering isn't certain
                 * XXX See XFree86 configuration docs (even back in the
                 * XXX 3.x days, and especially with 4.x).
                 *
                 * XXX Note that {button} has already been decremented
                 * XXX in mapping from X button numbering to GLUT.
				 *
				 * XXX Should add support for partial wheel turns as Windows does -- 5/27/11
                 */
                int wheel_number = (button - glutDeviceGet ( GLUT_NUM_MOUSE_BUTTONS )) / 2;
                int direction = -1;
                if( button % 2 )
                    direction = 1;

                if( pressed )
                    INVOKE_WCB( *window, MouseWheel, ( wheel_number,
                                                       direction,
                                                       event.xbutton.x,
                                                       event.xbutton.y )
                    );
            }
            fgState.Modifiers = INVALID_MODIFIERS;
        }
        break;

        case KeyRelease:
        case KeyPress:
        {
            FGCBKeyboard keyboard_cb;
            FGCBSpecial special_cb;

            GETWINDOW( xkey );
            GETMOUSE( xkey );

            /* Detect auto repeated keys, if configured globally or per-window */

            if ( fgState.KeyRepeat==GLUT_KEY_REPEAT_OFF || window->State.IgnoreKeyRepeat==GL_TRUE )
            {
                if (event.type==KeyRelease)
                {
                    /*
                     * Look at X11 keystate to detect repeat mode.
                     * While X11 says the key is actually held down, we'll ignore KeyRelease/KeyPress pairs.
                     */

                    char keys[32];
                    XQueryKeymap( fgDisplay.pDisplay.Display, keys ); /* Look at X11 keystate to detect repeat mode */

                    if ( event.xkey.keycode<256 )            /* XQueryKeymap is limited to 256 keycodes    */
                    {
                        if ( keys[event.xkey.keycode>>3] & (1<<(event.xkey.keycode%8)) )
                            window->State.pWState.KeyRepeating = GL_TRUE;
                        else
                            window->State.pWState.KeyRepeating = GL_FALSE;
                    }
                }
            }
            else
                window->State.pWState.KeyRepeating = GL_FALSE;

            /* Cease processing this event if it is auto repeated */

            if (window->State.pWState.KeyRepeating)
            {
                if (event.type == KeyPress) window->State.pWState.KeyRepeating = GL_FALSE;
                break;
            }

            if( event.type == KeyPress )
            {
                keyboard_cb = (FGCBKeyboard)( FETCH_WCB( *window, Keyboard ));
                special_cb  = (FGCBSpecial) ( FETCH_WCB( *window, Special  ));
            }
            else
            {
                keyboard_cb = (FGCBKeyboard)( FETCH_WCB( *window, KeyboardUp ));
                special_cb  = (FGCBSpecial) ( FETCH_WCB( *window, SpecialUp  ));
            }

            /* Is there a keyboard/special callback hooked for this window? */
            if( keyboard_cb || special_cb )
            {
                XComposeStatus composeStatus;
                char asciiCode[ 32 ];
                KeySym keySym;
                int len;

                /* Check for the ASCII/KeySym codes associated with the event: */
                len = XLookupString( &event.xkey, asciiCode, sizeof(asciiCode),
                                     &keySym, &composeStatus
                );

                /* GLUT API tells us to have two separate callbacks... */
                if( len > 0 )
                {
                    /* ...one for the ASCII translateable keypresses... */
                    if( keyboard_cb )
                    {
                        fgSetWindow( window );
                        fgState.Modifiers = fgPlatformGetModifiers( event.xkey.state );
                        keyboard_cb( asciiCode[ 0 ],
                                     event.xkey.x, event.xkey.y
                        );
                        fgState.Modifiers = INVALID_MODIFIERS;
                    }
                }
                else
                {
                    int special = -1;

                    /*
                     * ...and one for all the others, which need to be
                     * translated to GLUT_KEY_Xs...
                     */
                    switch( keySym )
                    {
                    case XK_F1:     special = GLUT_KEY_F1;     break;
                    case XK_F2:     special = GLUT_KEY_F2;     break;
                    case XK_F3:     special = GLUT_KEY_F3;     break;
                    case XK_F4:     special = GLUT_KEY_F4;     break;
                    case XK_F5:     special = GLUT_KEY_F5;     break;
                    case XK_F6:     special = GLUT_KEY_F6;     break;
                    case XK_F7:     special = GLUT_KEY_F7;     break;
                    case XK_F8:     special = GLUT_KEY_F8;     break;
                    case XK_F9:     special = GLUT_KEY_F9;     break;
                    case XK_F10:    special = GLUT_KEY_F10;    break;
                    case XK_F11:    special = GLUT_KEY_F11;    break;
                    case XK_F12:    special = GLUT_KEY_F12;    break;

                    case XK_KP_Left:
                    case XK_Left:   special = GLUT_KEY_LEFT;   break;
                    case XK_KP_Right:
                    case XK_Right:  special = GLUT_KEY_RIGHT;  break;
                    case XK_KP_Up:
                    case XK_Up:     special = GLUT_KEY_UP;     break;
                    case XK_KP_Down:
                    case XK_Down:   special = GLUT_KEY_DOWN;   break;

                    case XK_KP_Prior:
                    case XK_Prior:  special = GLUT_KEY_PAGE_UP; break;
                    case XK_KP_Next:
                    case XK_Next:   special = GLUT_KEY_PAGE_DOWN; break;
                    case XK_KP_Home:
                    case XK_Home:   special = GLUT_KEY_HOME;   break;
                    case XK_KP_End:
                    case XK_End:    special = GLUT_KEY_END;    break;
                    case XK_KP_Insert:
                    case XK_Insert: special = GLUT_KEY_INSERT; break;

                    case XK_Num_Lock :  special = GLUT_KEY_NUM_LOCK;  break;
                    case XK_KP_Begin :  special = GLUT_KEY_BEGIN;     break;
                    case XK_KP_Delete:  special = GLUT_KEY_DELETE;    break;

                    case XK_Shift_L:   special = GLUT_KEY_SHIFT_L;    break;
                    case XK_Shift_R:   special = GLUT_KEY_SHIFT_R;    break;
                    case XK_Control_L: special = GLUT_KEY_CTRL_L;     break;
                    case XK_Control_R: special = GLUT_KEY_CTRL_R;     break;
                    case XK_Alt_L:     special = GLUT_KEY_ALT_L;      break;
                    case XK_Alt_R:     special = GLUT_KEY_ALT_R;      break;
                    }

                    /*
                     * Execute the callback (if one has been specified),
                     * given that the special code seems to be valid...
                     */
                    if( special_cb && (special != -1) )
                    {
                        fgSetWindow( window );
                        fgState.Modifiers = fgPlatformGetModifiers( event.xkey.state );
                        special_cb( special, event.xkey.x, event.xkey.y );
                        fgState.Modifiers = INVALID_MODIFIERS;
                    }
                }
            }
        }
        break;

        case ReparentNotify:
            break; /* XXX Should disable this event */

        /* Not handled */
        case GravityNotify:
            break;

        default:
            /* enter handling of Extension Events here */
            #ifdef HAVE_X11_EXTENSIONS_XINPUT2_H
                fgHandleExtensionEvents( &event );
            #endif
            break;
        }
    }
}


static Bool match_motion(Display *dpy, XEvent *xev, XPointer arg)
{
    return xev->type == MotionNotify;
}

void fgPlatformMainLoopPreliminaryWork ( void )
{
}


/* deal with work list items */
void fgPlatformInitWork(SFG_Window* window)
{
    /* Notify windowStatus/visibility, position and size get notified on window creation with message handlers above 
     * XXX CHECK: do the messages happen too early like on windows, so client code cannot have registered
     * a callback yet and the message is thus never received by client?
     * -> this is a no-op
     */
     return;
}

void fgPlatformPosResZordWork(SFG_Window* window, unsigned int workMask)
{
    if (workMask & GLUT_FULL_SCREEN_WORK)
        fgPlatformFullScreenToggle( window );
    if (workMask & GLUT_POSITION_WORK)
        fgPlatformPositionWindow( window, window->State.DesiredXpos, window->State.DesiredYpos );
    if (workMask & GLUT_SIZE_WORK)
        fgPlatformReshapeWindow ( window, window->State.DesiredWidth, window->State.DesiredHeight );
    if (workMask & GLUT_ZORDER_WORK)
    {
        if (window->State.DesiredZOrder < 0)
            fgPlatformPushWindow( window );
        else
            fgPlatformPopWindow( window );
    }
}

void fgPlatformVisibilityWork(SFG_Window* window)
{
    /* Visibility status of window gets updated in the window message handlers above 
     * XXX: is this really the case? check
     */
    SFG_Window *win = window;
    switch (window->State.DesiredVisibility)
    {
    case DesireHiddenState:
        fgPlatformHideWindow( window );
        break;
    case DesireIconicState:
        /* Call on top-level window */
        while (win->Parent)
            win = win->Parent;
        fgPlatformIconifyWindow( win );
        break;
    case DesireNormalState:
        fgPlatformShowWindow( window );
        break;
    }
}

