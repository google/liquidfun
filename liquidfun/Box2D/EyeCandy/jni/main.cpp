/*
 * Copyright (c) 2013 Google, Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <EGL/egl.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <sys/system_properties.h>
#include <stdlib.h>
#include <assert.h>
#include <engine.h>

static int DeviceRotation(ANativeActivity *activity) {
  // Does the equivalent of:
  // WindowManager wm = Context.getSystemService(Context.WINDOW_SERVICE);
  // return = wm.getDefaultDisplay().getRotation();
  JNIEnv *env = activity->env;
  JavaVM *javaVm = activity->vm;
  javaVm->AttachCurrentThread(&env, NULL);
  jclass ContextClass = env->FindClass("android/content/Context");
  jmethodID getSystemServiceMethod = env->GetMethodID(ContextClass,
               "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
  jfieldID WindowServiceID = env->GetStaticFieldID(ContextClass,
                                     "WINDOW_SERVICE", "Ljava/lang/String;");
  jobject WindowServiceString = env->GetStaticObjectField(ContextClass,
                                                            WindowServiceID);
  jobject wmObject = env->CallObjectMethod(activity->clazz,
                                getSystemServiceMethod, WindowServiceString);
  jclass WindowManagerClass = env->FindClass("android/view/WindowManager");
  jmethodID getDefaultDisplay = env->GetMethodID(WindowManagerClass,
                            "getDefaultDisplay", "()Landroid/view/Display;");
  jobject ddObject = env->CallObjectMethod(wmObject, getDefaultDisplay);
  jclass DisplayClass = env->FindClass("android/view/Display");
  jmethodID getRotationMethod = env->GetMethodID(DisplayClass, "getRotation",
                                                                      "()I");
  int rotation = env->CallIntMethod(ddObject, getRotationMethod);
  javaVm->DetachCurrentThread();
  return rotation;
}

class UserData {
public:
  UserData(android_app *app) :
    engine_(new Engine(reinterpret_cast<SystemRef>(app))),
    display_(nullptr),
    surface_(nullptr),
    context_(nullptr),
    landscape_device_(false),
    sensor_manager_(nullptr),
    accelerometer_sensor_(nullptr),
    sensor_event_queue_(nullptr)
  {
    char model_string[PROP_VALUE_MAX+1];
    __system_property_get("ro.product.model", model_string);
    LOGI("model = %s", model_string);

    if (app->savedState != NULL) {
      // We are starting with a previous saved state; restore from it.
      engine_->SetState(*(struct SavedState *)app->savedState);
    }

    int rotation = DeviceRotation(app->activity);
    LOGI("rotation = %d", rotation);
    landscape_device_ = rotation == 0;

    // Prepare to monitor accelerometer
    sensor_manager_ = ASensorManager_getInstance();
    #define ASENSOR_TYPE_GRAVITY 9 // some SDK versions don't have it defined
    accelerometer_sensor_ = ASensorManager_getDefaultSensor(
        sensor_manager_, ASENSOR_TYPE_GRAVITY);
    sensor_event_queue_ = ASensorManager_createEventQueue(
        sensor_manager_, app->looper, LOOPER_ID_USER, NULL, NULL);
  }

  ~UserData()
  {
    delete engine_;
  }

  void SaveState(android_app *app) {
    // The system has asked us to save our current state.
    app->savedState = malloc(sizeof(SavedState));
    *((struct SavedState*)app->savedState) = engine_->GetState();
    app->savedStateSize = sizeof(SavedState);
  }

  /**
   * Initialize an EGL context_, load resources etc.
   */
  bool InitWindow(android_app *app) {
    // The window is being shown, get it ready.
    if (app->window == NULL) {
        assert(0);
        return false;
    }

    // initialize EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    static const EGLint attribs[] = {
        EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };
    EGLint format;
    EGLint num_configs;
    EGLConfig config;

    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#define EGLCHECK(c) { EGLBoolean ok = (c); assert(ok); B2_NOT_USED(ok); }

    EGLCHECK(eglInitialize(display_, nullptr, nullptr));

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */

    EGLCHECK(eglChooseConfig(display_, attribs, &config, 1, &num_configs));

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    EGLCHECK(eglGetConfigAttrib(display_,
                                config,
                                EGL_NATIVE_VISUAL_ID,
                                &format));

    int ret = ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);
    assert(ret >= 0);
    B2_NOT_USED(ret);

    surface_ = eglCreateWindowSurface(display_, config, app->window, NULL);

    static const EGLint context_attrs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
    };

    context_ = eglCreateContext(display_, config, NULL, context_attrs);

    if (eglMakeCurrent(display_, surface_, surface_, context_) == EGL_FALSE) {
      LOGW("Unable to eglMakeCurrent");
      return false;
    }

    EGLint width, height;
    EGLCHECK(eglQuerySurface(display_, surface_, EGL_WIDTH, &width));
    EGLCHECK(eglQuerySurface(display_, surface_, EGL_HEIGHT, &height));
    engine_->SetSize(width, height);

#undef EGLCHECK

    if (!engine_->Init()) {
       assert(0);
       ANativeActivity_finish(app->activity);
    }

    DrawFrame(app);

    return true;
  }

  // Tear down the EGL context_ currently associated with the display.
  void TermWindow(android_app *app) {
    if (display_ != EGL_NO_DISPLAY) {
      eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                     EGL_NO_CONTEXT);
      if (context_ != EGL_NO_CONTEXT) {
        eglDestroyContext(display_, context_);
      }
      if (surface_ != EGL_NO_SURFACE) {
        eglDestroySurface(display_, surface_);
      }
      eglTerminate(display_);
    }
    display_ = EGL_NO_DISPLAY;
    context_ = EGL_NO_CONTEXT;
    surface_ = EGL_NO_SURFACE;

    engine_->Terminate();
  }

  // When our app gains focus, we start monitoring the accelerometer.
  void GainedFocus(android_app *app) {
    if (accelerometer_sensor_ != NULL) {
      ASensorEventQueue_enableSensor(sensor_event_queue_, accelerometer_sensor_);
      // We'd like to get 60 events per second (in us).
      ASensorEventQueue_setEventRate(sensor_event_queue_,
                                     accelerometer_sensor_, (1000L/60)*1000);
    }
    engine_->SetAnimating(true);
  }

  // When our app loses focus, we stop monitoring the accelerometer.
  // This is to avoid consuming battery while not being used.
  void LostFocus(android_app *app) {
    if (accelerometer_sensor_ != NULL) {
      ASensorEventQueue_disableSensor(sensor_event_queue_,
                                      accelerometer_sensor_);
    }
    // Also stop animating_.
    engine_->SetAnimating(false);
    DrawFrame(app);
  }

  void ProcessEvent(android_app *app) {
    if (accelerometer_sensor_ != NULL) {
      ASensorEvent event;
      while (ASensorEventQueue_getEvents(sensor_event_queue_,
          &event, 1) > 0) {
        if (landscape_device_) {
          engine_->SetGravity(-event.vector.x, -event.vector.y, event.vector.z);
        } else {
          engine_->SetGravity( event.vector.y, -event.vector.x, event.vector.z);
        }
      }
    }
  }

  void DrawFrame(android_app *app) {
    if (display_ == NULL) {
      return;
    }

    engine_->DrawFrame();

    eglSwapBuffers(display_, surface_);
  }

  Engine *GetEngine()
  {
    return engine_;
  }

private:
  Engine *engine_;
  EGLDisplay display_;
  EGLSurface surface_;
  EGLContext context_;
  bool landscape_device_;
  ASensorManager *sensor_manager_;
  const ASensor *accelerometer_sensor_;
  ASensorEventQueue *sensor_event_queue_;
};

// Process the next input event.
static int32_t HandleInput(android_app *app, AInputEvent* event) {
  UserData *userData = (UserData *)app->userData;
  Engine *engine = userData->GetEngine();
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    auto action = AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
    float x = AMotionEvent_getX(event, 0);
    float y = AMotionEvent_getY(event, 0);
    switch (action) {
    case AMOTION_EVENT_ACTION_DOWN:
      engine->TouchDown(x, y, 0);
      break;
    case AMOTION_EVENT_ACTION_UP:
      engine->TouchUp(x, y, 0);
      break;
    case AMOTION_EVENT_ACTION_MOVE:
      engine->TouchMove(x, y, 0);
      break;
    case AMOTION_EVENT_ACTION_CANCEL:
      engine->TouchCancel(x, y, 0);
      break;
    }
    return 1;
  }
  return 0;
}

// Process the next main command.
static void HandleCmd(android_app *app, int32_t cmd) {
  UserData *userData = (UserData *)app->userData;
  switch (cmd) {
    case APP_CMD_SAVE_STATE:
      userData->SaveState(app);
      break;
    case APP_CMD_INIT_WINDOW:
      userData->InitWindow(app);
      break;
    case APP_CMD_TERM_WINDOW:
      userData->TermWindow(app);
      break;
    case APP_CMD_GAINED_FOCUS:
      userData->GainedFocus(app);
      break;
    case APP_CMD_LOST_FOCUS:
      userData->LostFocus(app);
      break;
  }
}

// This is the main entry point of a native application that is using
// android_native_app_glue.  It runs in its own thread, with its own
// event loop for receiving input events and doing other things.
void android_main(android_app *app) {
  // Make sure glue isn't stripped.
  app_dummy();

  UserData userData(app);

  app->userData = &userData;
  app->onAppCmd = HandleCmd;
  app->onInputEvent = HandleInput;

  Engine *engine = userData.GetEngine();

  // loop waiting for stuff to do.
  for (;;) {
    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source* source;

    // If not animating_, we will block forever waiting for events.
    // If animating_, we loop until all events are read, then continue
    // to draw the next frame of animation.
    while ((ident = ALooper_pollAll(engine->IsAnimating() ? 0 : -1, NULL,
                                    &events, (void**)&source)) >= 0) {

      // Process this event.
      if (source != NULL) {
        source->process(app, source);
      }

      // If a sensor has data, process it now.
      if (ident == LOOPER_ID_USER) {
        userData.ProcessEvent(app);
      }

      // Check if we are exiting.
      if (app->destroyRequested != 0) {
        userData.TermWindow(app);
        return;
      }
    }
    userData.DrawFrame(app);
  }
}
