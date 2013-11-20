#ifndef ANDROID_MAIN_WRAPPER_H
#define ANDROID_MAIN_WRAPPER_H

// This includes functionality to calls a standard C main() from the
// Android's NativeActivity entry point android_main().
//
// For example:
//
// #include "AndroidMainWrapper.h"
//
// int main(int argc, char **argv) {
//   ... do stuff ...
//   return 0;
// }
//
// Will "do stuff" and exit the NativeActivity on return from main().
// For more information see ndk/sources/android/native_app_glue.

#if defined(ANDROID) || defined(__ANDROID__)
#include <android_native_app_glue.h>

#if defined(__cplusplus)
extern "C" {
#endif  // defined(__cplusplus)

// This should be implemented by the application including  this header.
extern int main(int argc, char** argv);

// Call application's main().
void CallMain(struct android_app* state);

#if defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#if !defined(ANDROID_MAIN_WRAPPER_DISABLE_ANDROID_MAIN)
// Entry point for the android native activity.
void android_main(struct android_app* state) {
  // Make sure android native glue isn't stripped.
  app_dummy();
  CallMain(state);
}
#endif  // !defined(ANDROID_MAIN_WRAPPER_DISABLE_ANDROID_MAIN)

#endif // defined(ANDROID) || defined(__ANDROID__)
#endif // ANDROID_MAIN_WRAPPER_H
