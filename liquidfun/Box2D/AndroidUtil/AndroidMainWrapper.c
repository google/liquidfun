#include <AndroidUtil/AndroidMainWrapper.h>
#include <jni.h>
#include <AndroidUtil/AndroidLogPrint.h>

// See android.app.Activity.
enum AndroidAppActivityResults {
  ANDROID_APP_ACTIVITY_RESULT_CANCELED = 0,
  ANDROID_APP_ACTIVITY_RESULT_FIRST_USER = 1,
  ANDROID_APP_ACTIVITY_RESULT_OK = -1,
};

// Entry point for the android native activity.
void CallMain(struct android_app* state) {
  int result = main(0, NULL);
  ANativeActivity * const activity = state->activity;
  {
    jobject nativeActivityObject = activity->clazz;
    jclass nativeActivityClass;
    jmethodID setResult;
    JNIEnv *env = activity->env;
    JavaVM *javaVm = activity->vm;
    int returnResult = result == 0 ? ANDROID_APP_ACTIVITY_RESULT_OK :
      result > 0 ? result + ANDROID_APP_ACTIVITY_RESULT_FIRST_USER :
      ANDROID_APP_ACTIVITY_RESULT_CANCELED;
    (*javaVm)->AttachCurrentThread(javaVm, &env, NULL);
    nativeActivityClass = (*env)->GetObjectClass(env, nativeActivityObject);
    setResult = (*env)->GetMethodID(env, nativeActivityClass, "setResult",
                                    "(I)V");
    (*env)->CallVoidMethod(env, nativeActivityObject, setResult, returnResult);
    (*javaVm)->DetachCurrentThread(javaVm);
  }
  for ( ; ; ) {
    struct android_poll_source* source;
    int looperId;
    int events;
    // Signal app completion.
    ANativeActivity_finish(activity);
    // Pump the event loop.
    while ((looperId = ALooper_pollAll(-1, NULL, &events,
                                       (void**)&source)) >= 0) {
      switch (looperId) {
      case LOOPER_ID_MAIN:
        // drop through
      case LOOPER_ID_INPUT:
        if (source) {
          source->process(state, source);
        }
        break;
      default:
        // >= LOOPER_ID_USER so this is a user data source which this doesn't
        // know how to process.
        break;
      }
      // If the NativeActivity is waiting for the application thread to
      // complete, exit this function.
      if (state->destroyRequested) {
        break;
      }
    }
  }
}
