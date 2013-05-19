/*
 * fg_main_android.c
 *
 * The Android-specific windows message processing methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Copied for Platform code by Evan Felix <karcaw at gmail.com>
 * Copyright (C) 2012  Sylvain Beucler
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
#include "egl/fg_window_egl.h"

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "FreeGLUT", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "FreeGLUT", __VA_ARGS__))
#include <android/native_app_glue/android_native_app_glue.h>
#include <android/keycodes.h>

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

static struct touchscreen touchscreen;
static unsigned char key_a2fg[256];

/* Cf. http://developer.android.com/reference/android/view/KeyEvent.html */
/* These codes are missing in <android/keycodes.h> */
/* Don't convert to enum, since it may conflict with future version of
   that <android/keycodes.h> */
#define AKEYCODE_FORWARD_DEL 112
#define AKEYCODE_CTRL_LEFT 113
#define AKEYCODE_CTRL_RIGHT 114
#define AKEYCODE_MOVE_HOME 122
#define AKEYCODE_MOVE_END 123
#define AKEYCODE_INSERT 124
#define AKEYCODE_ESCAPE 127
#define AKEYCODE_F1 131
#define AKEYCODE_F2 132
#define AKEYCODE_F3 133
#define AKEYCODE_F4 134
#define AKEYCODE_F5 135
#define AKEYCODE_F6 136
#define AKEYCODE_F7 137
#define AKEYCODE_F8 138
#define AKEYCODE_F9 139
#define AKEYCODE_F10 140
#define AKEYCODE_F11 141
#define AKEYCODE_F12 142

#define EVENT_HANDLED 1
#define EVENT_NOT_HANDLED 0

/**
 * Initialize Android keycode to GLUT keycode mapping
 */
static void key_init() {
  memset(key_a2fg, 0, sizeof(key_a2fg));

  key_a2fg[AKEYCODE_F1]  = GLUT_KEY_F1;
  key_a2fg[AKEYCODE_F2]  = GLUT_KEY_F2;
  key_a2fg[AKEYCODE_F3]  = GLUT_KEY_F3;
  key_a2fg[AKEYCODE_F4]  = GLUT_KEY_F4;
  key_a2fg[AKEYCODE_F5]  = GLUT_KEY_F5;
  key_a2fg[AKEYCODE_F6]  = GLUT_KEY_F6;
  key_a2fg[AKEYCODE_F7]  = GLUT_KEY_F7;
  key_a2fg[AKEYCODE_F8]  = GLUT_KEY_F8;
  key_a2fg[AKEYCODE_F9]  = GLUT_KEY_F9;
  key_a2fg[AKEYCODE_F10] = GLUT_KEY_F10;
  key_a2fg[AKEYCODE_F11] = GLUT_KEY_F11;
  key_a2fg[AKEYCODE_F12] = GLUT_KEY_F12;

  key_a2fg[AKEYCODE_PAGE_UP]   = GLUT_KEY_PAGE_UP;
  key_a2fg[AKEYCODE_PAGE_DOWN] = GLUT_KEY_PAGE_DOWN;
  key_a2fg[AKEYCODE_MOVE_HOME] = GLUT_KEY_HOME;
  key_a2fg[AKEYCODE_MOVE_END]  = GLUT_KEY_END;
  key_a2fg[AKEYCODE_INSERT]    = GLUT_KEY_INSERT;

  key_a2fg[AKEYCODE_DPAD_UP]    = GLUT_KEY_UP;
  key_a2fg[AKEYCODE_DPAD_DOWN]  = GLUT_KEY_DOWN;
  key_a2fg[AKEYCODE_DPAD_LEFT]  = GLUT_KEY_LEFT;
  key_a2fg[AKEYCODE_DPAD_RIGHT] = GLUT_KEY_RIGHT;

  key_a2fg[AKEYCODE_ALT_LEFT]    = GLUT_KEY_ALT_L;
  key_a2fg[AKEYCODE_ALT_RIGHT]   = GLUT_KEY_ALT_R;
  key_a2fg[AKEYCODE_SHIFT_LEFT]  = GLUT_KEY_SHIFT_L;
  key_a2fg[AKEYCODE_SHIFT_RIGHT] = GLUT_KEY_SHIFT_R;
  key_a2fg[AKEYCODE_CTRL_LEFT]   = GLUT_KEY_CTRL_L;
  key_a2fg[AKEYCODE_CTRL_RIGHT]  = GLUT_KEY_CTRL_R;
}

/**
 * Convert an Android key event to ASCII.
 */
static unsigned char key_ascii(struct android_app* app, AInputEvent* event) {
  int32_t code = AKeyEvent_getKeyCode(event);

  /* Handle a few special cases: */
  switch (code) {
  case AKEYCODE_DEL:
    return 8;
  case AKEYCODE_FORWARD_DEL:
    return 127;
  case AKEYCODE_ESCAPE:
    return 27;
  }

  /* Get usable JNI context */
  JNIEnv* env = app->activity->env;
  JavaVM* vm = app->activity->vm;
  (*vm)->AttachCurrentThread(vm, &env, NULL);

  jclass KeyEventClass = (*env)->FindClass(env, "android/view/KeyEvent");
  jmethodID KeyEventConstructor = (*env)->GetMethodID(env, KeyEventClass, "<init>", "(II)V");
  jobject keyEvent = (*env)->NewObject(env, KeyEventClass, KeyEventConstructor,
				       AKeyEvent_getAction(event), AKeyEvent_getKeyCode(event));
  jmethodID KeyEvent_getUnicodeChar = (*env)->GetMethodID(env, KeyEventClass, "getUnicodeChar", "(I)I");
  int ascii = (*env)->CallIntMethod(env, keyEvent, KeyEvent_getUnicodeChar, AKeyEvent_getMetaState(event));

  /* LOGI("getUnicodeChar(%d) = %d ('%c')", AKeyEvent_getKeyCode(event), ascii, ascii); */
  (*vm)->DetachCurrentThread(vm);

  return ascii;
}

unsigned long fgPlatformSystemTime ( void )
{
  struct timeval now;
  gettimeofday( &now, NULL );
  return now.tv_usec/1000 + now.tv_sec*1000;
}

/*
 * Does the magic required to relinquish the CPU until something interesting
 * happens.
 */
void fgPlatformSleepForEvents( long msec )
{
    /* Android's NativeActivity relies on a Looper/ALooper object to
       notify about events.  The Looper object is plugged on two
       internal pipe(2)s to detect system and input events.  Sadly you
       can only ask the Looper for an event, not just ask whether
       there is a pending event (and process it later).  Consequently,
       short of redesigning NativeActivity, we cannot
       SleepForEvents. */
}

/**
 * Process the next input event.
 */
int32_t handle_input(struct android_app* app, AInputEvent* event) {
  SFG_Window* window = fgWindowByHandle(app->window);
  if (window == NULL)
    return EVENT_NOT_HANDLED;

  /* FIXME: in Android, when a key is repeated, down
     and up events happen most often at the exact same time.  This
     makes it impossible to animate based on key press time. */
  /* e.g. down/up/wait/down/up rather than down/wait/down/wait/up */ 
  /* This looks like a bug in the Android virtual keyboard system :/
     Real buttons such as the Back button appear to work correctly
     (series of down events with proper getRepeatCount value). */
  
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
    /* LOGI("action: %d", AKeyEvent_getAction(event)); */
    /* LOGI("keycode: %d", code); */
    int32_t code = AKeyEvent_getKeyCode(event);

    if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN) {
      int32_t keypress = 0;
      unsigned char ascii = 0;
      if ((keypress = key_a2fg[code]) && FETCH_WCB(*window, Special)) {
	INVOKE_WCB(*window, Special, (keypress, window->State.MouseX, window->State.MouseY));
	return EVENT_HANDLED;
      } else if ((ascii = key_ascii(app, event)) && FETCH_WCB(*window, Keyboard)) {
	INVOKE_WCB(*window, Keyboard, (ascii, window->State.MouseX, window->State.MouseY));
	return EVENT_HANDLED;
      }
    }
    else if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_UP) {
      int32_t keypress = 0;
      unsigned char ascii = 0;
      if ((keypress = key_a2fg[code]) && FETCH_WCB(*window, Special)) {
	INVOKE_WCB(*window, SpecialUp, (keypress, window->State.MouseX, window->State.MouseY));
	return EVENT_HANDLED;
      } else if ((ascii = key_ascii(app, event)) && FETCH_WCB(*window, Keyboard)) {
	INVOKE_WCB(*window, KeyboardUp, (ascii, window->State.MouseX, window->State.MouseY));
	return EVENT_HANDLED;
      }
    }
  }

  int32_t source = AInputEvent_getSource(event);
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION
      && source == AINPUT_SOURCE_TOUCHSCREEN) {
    int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
    /* Pointer ID for clicks */
    int32_t pidx = AMotionEvent_getAction(event) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    /* TODO: Handle multi-touch; also handle multiple sources/devices */
    /* cf. http://sourceforge.net/mailarchive/forum.php?thread_name=20120518071314.GA28061%40perso.beuc.net&forum_name=freeglut-developer */
    if (0) {
      LOGI("motion action=%d index=%d source=%d", action, pidx, source);
      int count = AMotionEvent_getPointerCount(event);
      int i;
      for (i = 0; i < count; i++) {
        LOGI("multi(%d): %.01f,%.01f",
             AMotionEvent_getPointerId(event, i),
             AMotionEvent_getX(event, i), AMotionEvent_getY(event, i));
      }
    }
    float x = AMotionEvent_getX(event, 0);
    float y = AMotionEvent_getY(event, 0);

    /* Virtual arrows PAD */
    /* Don't interfere with existing mouse move event */
    if (!touchscreen.in_mmotion) {
      struct vpad_state prev_vpad = touchscreen.vpad;
      touchscreen.vpad.left = touchscreen.vpad.right
	= touchscreen.vpad.up = touchscreen.vpad.down = false;

      /* int32_t width = ANativeWindow_getWidth(window->Window.Handle); */
      int32_t height = ANativeWindow_getHeight(window->Window.Handle);
      if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_MOVE) {
	if ((x > 0 && x < 100) && (y > (height - 100) && y < height))
	  touchscreen.vpad.left = true;
	if ((x > 200 && x < 300) && (y > (height - 100) && y < height))
	  touchscreen.vpad.right = true;
	if ((x > 100 && x < 200) && (y > (height - 100) && y < height))
	  touchscreen.vpad.down = true;
	if ((x > 100 && x < 200) && (y > (height - 200) && y < (height - 100)))
	  touchscreen.vpad.up = true;
      }
      if (action == AMOTION_EVENT_ACTION_DOWN && 
	  (touchscreen.vpad.left || touchscreen.vpad.right || touchscreen.vpad.down || touchscreen.vpad.up))
	touchscreen.vpad.on = true;
      if (action == AMOTION_EVENT_ACTION_UP)
	touchscreen.vpad.on = false;
      if (prev_vpad.left != touchscreen.vpad.left
	  || prev_vpad.right != touchscreen.vpad.right
	  || prev_vpad.up != touchscreen.vpad.up
	  || prev_vpad.down != touchscreen.vpad.down
	  || prev_vpad.on != touchscreen.vpad.on) {
	if (FETCH_WCB(*window, Special)) {
	  if (prev_vpad.left == false && touchscreen.vpad.left == true)
	    INVOKE_WCB(*window, Special, (GLUT_KEY_LEFT, x, y));
	  else if (prev_vpad.right == false && touchscreen.vpad.right == true)
	    INVOKE_WCB(*window, Special, (GLUT_KEY_RIGHT, x, y));
	  else if (prev_vpad.up == false && touchscreen.vpad.up == true)
	    INVOKE_WCB(*window, Special, (GLUT_KEY_UP, x, y));
	  else if (prev_vpad.down == false && touchscreen.vpad.down == true)
	    INVOKE_WCB(*window, Special, (GLUT_KEY_DOWN, x, y));
	}
	if (FETCH_WCB(*window, SpecialUp)) {
	  if (prev_vpad.left == true && touchscreen.vpad.left == false)
	    INVOKE_WCB(*window, SpecialUp, (GLUT_KEY_LEFT, x, y));
	  if (prev_vpad.right == true && touchscreen.vpad.right == false)
	    INVOKE_WCB(*window, SpecialUp, (GLUT_KEY_RIGHT, x, y));
	  if (prev_vpad.up == true && touchscreen.vpad.up == false)
	    INVOKE_WCB(*window, SpecialUp, (GLUT_KEY_UP, x, y));
	  if (prev_vpad.down == true && touchscreen.vpad.down == false)
	    INVOKE_WCB(*window, SpecialUp, (GLUT_KEY_DOWN, x, y));
	}
	return EVENT_HANDLED;
      }
    }
    
    /* Normal mouse events */
    if (!touchscreen.vpad.on) {
      window->State.MouseX = x;
      window->State.MouseY = y;
      if (action == AMOTION_EVENT_ACTION_DOWN && FETCH_WCB(*window, Mouse)) {
	touchscreen.in_mmotion = true;
	INVOKE_WCB(*window, Mouse, (GLUT_LEFT_BUTTON, GLUT_DOWN, x, y));
      } else if (action == AMOTION_EVENT_ACTION_UP && FETCH_WCB(*window, Mouse)) {
	touchscreen.in_mmotion = false;
	INVOKE_WCB(*window, Mouse, (GLUT_LEFT_BUTTON, GLUT_UP, x, y));
      } else if (action == AMOTION_EVENT_ACTION_MOVE && FETCH_WCB(*window, Motion)) {
	INVOKE_WCB(*window, Motion, (x, y));
      }
    }
    
    return EVENT_HANDLED;
  }

  /* Let Android handle other events (e.g. Back and Menu buttons) */
  return EVENT_NOT_HANDLED;
}

/**
 * Process the next main command.
 */
void handle_cmd(struct android_app* app, int32_t cmd) {
  SFG_Window* window = fgWindowByHandle(app->window);  /* may be NULL */
  switch (cmd) {
  /* App life cycle, in that order: */
  case APP_CMD_START:
    LOGI("handle_cmd: APP_CMD_START");
    break;
  case APP_CMD_RESUME:
    LOGI("handle_cmd: APP_CMD_RESUME");
    /* Cf. fgPlatformProcessSingleEvent */
    break;
  case APP_CMD_INIT_WINDOW: /* surfaceCreated */
    /* The window is being shown, get it ready. */
    LOGI("handle_cmd: APP_CMD_INIT_WINDOW %p", app->window);
    fgDisplay.pDisplay.single_native_window = app->window;
    /* start|resume: glPlatformOpenWindow was waiting for Handle to be
       defined and will now continue processing */
    break;
  case APP_CMD_GAINED_FOCUS:
    LOGI("handle_cmd: APP_CMD_GAINED_FOCUS");
    break;
  case APP_CMD_WINDOW_RESIZED:
    LOGI("handle_cmd: APP_CMD_WINDOW_RESIZED");
    if (window->Window.pContext.egl.Surface != EGL_NO_SURFACE)
      /* Make ProcessSingleEvent detect the new size, only available
	 after the next SwapBuffer */
      glutPostRedisplay();
    break;

  case APP_CMD_SAVE_STATE: /* onSaveInstanceState */
    /* The system has asked us to save our current state, when it
       pauses the application without destroying it right after. */
    app->savedState = strdup("Detect me as non-NULL on next android_main");
    app->savedStateSize = strlen(app->savedState) + 1;
    LOGI("handle_cmd: APP_CMD_SAVE_STATE");
    break;
  case APP_CMD_PAUSE:
    LOGI("handle_cmd: APP_CMD_PAUSE");
    /* Cf. fgPlatformProcessSingleEvent */
    break;
  case APP_CMD_LOST_FOCUS:
    LOGI("handle_cmd: APP_CMD_LOST_FOCUS");
    break;
  case APP_CMD_TERM_WINDOW: /* surfaceDestroyed */
    /* The application is being hidden, but may be restored */
    LOGI("handle_cmd: APP_CMD_TERM_WINDOW");
    fghPlatformCloseWindowEGL(window);
    fgDisplay.pDisplay.single_native_window = NULL;
    break;
  case APP_CMD_STOP:
    LOGI("handle_cmd: APP_CMD_STOP");
    break;
  case APP_CMD_DESTROY: /* Activity.onDestroy */
    LOGI("handle_cmd: APP_CMD_DESTROY");
    /* User closed the application for good, let's kill the window */
    {
      /* Can't use fgWindowByHandle as app->window is NULL */
      SFG_Window* window = fgStructure.CurrentWindow;
      if (window != NULL) {
        fgDestroyWindow(window);
      } else {
        LOGI("APP_CMD_DESTROY: No current window");
      }
    }
    /* glue has already set android_app->destroyRequested=1 */
    break;

  case APP_CMD_CONFIG_CHANGED:
    /* Handle rotation / orientation change */
    LOGI("handle_cmd: APP_CMD_CONFIG_CHANGED");
    break;
  case APP_CMD_LOW_MEMORY:
    LOGI("handle_cmd: APP_CMD_LOW_MEMORY");
    break;
  default:
    LOGI("handle_cmd: unhandled cmd=%d", cmd);
  }
}

void fgPlatformOpenWindow( SFG_Window* window, const char* title,
                           GLboolean positionUse, int x, int y,
                           GLboolean sizeUse, int w, int h,
                           GLboolean gameMode, GLboolean isSubWindow );

void fgPlatformProcessSingleEvent ( void )
{
  /* When the screen is resized, the window handle still points to the
     old window until the next SwapBuffer, while it's crucial to set
     the size (onShape) correctly before the next onDisplay callback.
     Plus we don't know if the next SwapBuffer already occurred at the
     time we process the event (e.g. during onDisplay). */
  /* So we do the check each time rather than on event. */
  /* Interestingly, on a Samsung Galaxy S/PowerVR SGX540 GPU/Android
     2.3, that next SwapBuffer is fake (but still necessary to get the
     new size). */
  SFG_Window* window = fgStructure.CurrentWindow;
  if (window != NULL && window->Window.Handle != NULL) {
    int32_t width = ANativeWindow_getWidth(window->Window.Handle);
    int32_t height = ANativeWindow_getHeight(window->Window.Handle);
    fghOnReshapeNotify(window,width,height,GL_FALSE);
  }

  /* Read pending event. */
  int ident;
  int events;
  struct android_poll_source* source;
  /* This is called "ProcessSingleEvent" but this means we'd only
     process ~60 (screen Hz) mouse events per second, plus other ports
     are processing all events already.  So let's process all pending
     events. */
  /* if ((ident=ALooper_pollOnce(0, NULL, &events, (void**)&source)) >= 0) { */
  while ((ident=ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
    /* Process this event. */
    if (source != NULL) {
      source->process(source->app, source);
    }
  }

  /* If we're not in RESUME state, Android paused us, so wait */
  struct android_app* app = fgDisplay.pDisplay.app;
  if (app->destroyRequested != 1 && app->activityState != APP_CMD_RESUME) {
      INVOKE_WCB(*window, AppStatus, (GLUT_APPSTATUS_PAUSE));

    int FOREVER = -1;
    while (app->destroyRequested != 1 && (app->activityState != APP_CMD_RESUME)) {
      if ((ident=ALooper_pollOnce(FOREVER, NULL, &events, (void**)&source)) >= 0) {
        /* Process this event. */
        if (source != NULL) {
          source->process(source->app, source);
        }
      }
    }
    /* Coming back from a pause: */
    /* - Recreate window context and surface */
    /* - Call user-defined hook to restore resources (textures...) */
    /* - Exit pause loop */
    if (app->destroyRequested != 1) {
      /* Android is full-screen only, simplified call.. */
      /* Ideally we'd have a fgPlatformReopenWindow() */
      /* If we're hidden by a non-fullscreen or translucent activity,
         we'll be paused but not stopped, and keep the current
         surface; in which case fgPlatformOpenWindow will no-op. */
      fgPlatformOpenWindow(window, "", GL_FALSE, 0, 0, GL_FALSE, 0, 0, GL_FALSE, GL_FALSE);

      /* TODO: should there be a whole GLUT_INIT_WORK forced? probably...
       * Could queue that up in APP_CMD_TERM_WINDOW handler, but it'll
       * be not possible to ensure InitContext CB gets called before
       * Resume CB like that.. so maybe just force calling initContext CB
       * here is best. Or we could force work on the window in question..
       * 1) save old work mask, 2) set mask to init only, 3) call fgProcessWork directly
       * 4) set work mask back to the one saved in step 1.
       */
      if (!FETCH_WCB(*window, InitContext))
          fgWarning("Resuming application, but no callback to reload context resources (glutInitContextFunc)");
    }

    INVOKE_WCB(*window, AppStatus, (GLUT_APPSTATUS_RESUME));
  }
}

void fgPlatformMainLoopPreliminaryWork ( void )
{
  LOGI("fgPlatformMainLoopPreliminaryWork\n");

  key_init();

  /* Make sure glue isn't stripped. */
  /* JNI entry points need to be bundled even when linking statically */
  app_dummy();
}


/* deal with work list items */
void fgPlatformInitWork(SFG_Window* window)
{
    /* notify windowStatus/visibility */
    INVOKE_WCB( *window, WindowStatus, ( GLUT_FULLY_RETAINED ) );

    /* Position callback, always at 0,0 */
    fghOnPositionNotify(window, 0, 0, GL_TRUE);

    /* Size gets notified on window creation with size detection in mainloop above
     * XXX CHECK: does this messages happen too early like on windows,
     * so client code cannot have registered a callback yet and the message
     * is thus never received by client?
     */
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
    /* Visibility status of window should get updated in the window message handlers
     * For now, none of these functions called below do anything, so don't worry
     * about it
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

