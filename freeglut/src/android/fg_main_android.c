/*
 * freeglut_main_android.c
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

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "FreeGLUT", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "FreeGLUT", __VA_ARGS__))
#include <android/native_app_glue/android_native_app_glue.h>
#include <android/keycodes.h>

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

  return ascii;
}

/*
 * Handle a window configuration change. When no reshape
 * callback is hooked, the viewport size is updated to
 * match the new window size.
 */
void fgPlatformReshapeWindow ( SFG_Window *window, int width, int height )
{
  fprintf(stderr, "fgPlatformReshapeWindow: STUB\n");
}

/*
 * A static helper function to execute display callback for a window
 */
void fgPlatformDisplayWindow ( SFG_Window *window )
{
  fghRedrawWindow ( window ) ;
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
  /* fprintf(stderr, "fgPlatformSleepForEvents: STUB\n"); */
}

/**
 * Process the next input event.
 */
int32_t handle_input(struct android_app* app, AInputEvent* event) {
  SFG_Window* window = fgStructure.CurrentWindow;

  /* FIXME: in Android, when key is repeated, down and up events
     happen most often at the exact same time.  This makes it
     impossible to animate based on key press time. */
  /* e.g. down/up/wait/down/up rather than down/wait/down/wait/up */
  
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

  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    int32_t action = AMotionEvent_getAction(event);
    float x = AMotionEvent_getX(event, 0);
    float y = AMotionEvent_getY(event, 0);
    LOGI("motion %.01f,%.01f action=%d", x, y, AMotionEvent_getAction(event));
    
    /* Virtual arrows PAD */
    // Don't interfere with existing mouse move event
    if (!touchscreen.in_mmotion) {
      struct vpad_state prev_vpad = touchscreen.vpad;
      touchscreen.vpad.left = touchscreen.vpad.right
	= touchscreen.vpad.up = touchscreen.vpad.down = false;

      int32_t width = ANativeWindow_getWidth(window->Window.Handle);
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
      LOGI("Changed mouse position: %d,%d", x, y);
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
  switch (cmd) {
  case APP_CMD_SAVE_STATE:
    /* The system has asked us to save our current state.  Do so. */
    LOGI("handle_cmd: APP_CMD_SAVE_STATE");
    break;
  case APP_CMD_INIT_WINDOW:
    /* The window is being shown, get it ready. */
    LOGI("handle_cmd: APP_CMD_INIT_WINDOW");
    fgDisplay.pDisplay.single_window->Window.Handle = app->window;
    /* glPlatformOpenWindow was waiting for Handle to be defined and
       will now return from fgPlatformProcessSingleEvent() */
    break;
  case APP_CMD_TERM_WINDOW:
    /* The window is being hidden or closed, clean it up. */
    LOGI("handle_cmd: APP_CMD_TERM_WINDOW");
    fgDestroyWindow(fgDisplay.pDisplay.single_window);
    break;
  case APP_CMD_DESTROY:
    /* Not reached because GLUT exit()s when last window is closed */
    LOGI("handle_cmd: APP_CMD_DESTROY");
    break;
  case APP_CMD_GAINED_FOCUS:
    LOGI("handle_cmd: APP_CMD_GAINED_FOCUS");
    break;
  case APP_CMD_LOST_FOCUS:
    LOGI("handle_cmd: APP_CMD_LOST_FOCUS");
    break;
  case APP_CMD_CONFIG_CHANGED:
    /* Handle rotation / orientation change */
    LOGI("handle_cmd: APP_CMD_CONFIG_CHANGED");
    break;
  case APP_CMD_WINDOW_RESIZED:
    LOGI("handle_cmd: APP_CMD_WINDOW_RESIZED");
    if (fgDisplay.pDisplay.single_window->Window.pContext.eglSurface != EGL_NO_SURFACE)
      /* Make ProcessSingleEvent detect the new size, only available
	 after the next SwapBuffer */
      glutPostRedisplay();
    break;
  default:
    LOGI("handle_cmd: unhandled cmd=%d", cmd);
  }
}

void fgPlatformProcessSingleEvent ( void )
{
  static int32_t last_width = -1;
  static int32_t last_height = -1;

  /* When the screen is resized, the window handle still points to the
     old window until the next SwapBuffer, while it's crucial to set
     the size (onShape) correctly before the next onDisplay callback.
     Plus we don't know if the next SwapBuffer already occurred at the
     time we process the event (e.g. during onDisplay). */
  /* So we do the check each time rather than on event. */
  /* Interestingly, on a Samsung Galaxy S/PowerVR SGX540 GPU/Android
     2.3, that next SwapBuffer is fake (but still necessary to get the
     new size). */
  SFG_Window* window = fgDisplay.pDisplay.single_window;
  if (window != NULL && window->Window.Handle != NULL) {
    int32_t width = ANativeWindow_getWidth(window->Window.Handle);
    int32_t height = ANativeWindow_getHeight(window->Window.Handle);
    if (width != last_width || height != last_height) {
      last_width = width;
      last_height = height;
      LOGI("width=%d, height=%d", width, height);
      if( FETCH_WCB( *window, Reshape ) )
	INVOKE_WCB( *window, Reshape, ( width, height ) );
      else
	glViewport( 0, 0, width, height );
      glutPostRedisplay();
    }
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
}

void fgPlatformMainLoopPreliminaryWork ( void )
{
  printf("fgPlatformMainLoopPreliminaryWork\n");

  key_init();

  /* Make sure glue isn't stripped. */
  /* JNI entry points need to be bundled even when linking statically */
  app_dummy();
}

void fgPlatformDeinitialiseInputDevices ( void )
{
  fprintf(stderr, "fgPlatformDeinitialiseInputDevices: STUB\n");
}
