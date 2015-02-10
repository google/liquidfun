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
#ifndef ANDROID_LOG_PRINT_H
#define ANDROID_LOG_PRINT_H

// This header redefines printf to send data to Android's log stream and
// provides an implementation of std::ostream which prints to the Android
// log output.
//
// For example:
//
// #include "AndroidUtil/AndroidLogPrint.h"
// #include "AndroidUtil/AndroidMainWrapper.h"
//
// int main(int argc, char **argv) {
//   gAndroidLogPrintfTag = "my_application";
//   printf("Print to the log from printf\n");
//   AndroidLogPrintStreamBuf streambuf;
//   std::ostream stream(&streambuf);
//   stream << "Print to the log from a stream\n");
//   return 0;
// }
//
// will print "Print to the log from printf" and
// "Print to the log from a stream" to Android's log stream which can be viewed
// using "adb logcat".  All log messages from the application will be prefixed
// with the tag, "my_application" so they can optionally extracted from
// "adb logcat" output.

#if defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#include <stdarg.h>

// If a log tag isn't specified, default to "main".
//
// Log tag can optionally be set when building this library by adding
// -DANDROID_LOG_PRINT_TAG="mytag" to the ANDROIDUTIL_ADDITIONAL_CFLAGS
// variable before importing this module into a NDK project.
//
// For example, an Android.mk to add the tag "myproject" to the log output
// would look be:
//
// LOCAL_MODULE:=MyProject
// LOCAL_SRC_FILES:=MyProject.cpp
// LOCAL_STATIC_LIBRARIES:=android_native_app_glue libandroidutil_static
// ANDROIDUTIL_ADDITIONAL_CFLAGS:=-DANDROID_LOG_PRINT_TAG="myproject"
// include $(BUILD_SHARED_LIBRARY)
// $(call import-module,AndroidUtil/jni)
// $(call import-module,android/native_app_glue)
//
#if defined(ANDROID_LOG_PRINT_TAG)
// Macros to expand ANDROID_LOG_PRINT_TAG as a string.
#define ANDROID_LOG_PRINT_TAG_AS_STRING2(x) #x
#define ANDROID_LOG_PRINT_TAG_AS_STRING(x) ANDROID_LOG_PRINT_TAG_AS_STRING2(x)
#define ANDROID_LOG_PRINT_TAG_STRING \
  ANDROID_LOG_PRINT_TAG_AS_STRING(ANDROID_LOG_PRINT_TAG)
#else
#define ANDROID_LOG_PRINT_TAG "main"
#define ANDROID_LOG_PRINT_TAG_STRING ANDROID_LOG_PRINT_TAG
#endif  // !defined(ANDROID_LOG_PRINT_TAG)

// The priority to use when printing to the Android log.  If this isn't
// specified it defaults to ANDROID_LOG_INFO.  See android/log.h for more
// information.
//
// For example:
// #define ANDROID_LOG_PRINT_PRIORITY ANDROID_LOG_DEBUG
// #include "android_log_print.h"
#if !defined(ANDROID_LOG_PRINT_PRIORITY)
#define ANDROID_LOG_PRINT_PRIORITY ANDROID_LOG_INFO
#endif  // !defined(ANDROID_LOG_PRINT_PRIORITY)

// Enable / disable buffering of data written to the Android log.
// NOTE: Log buffering in this module is not thread safe.
//
// For example:
// #define ANDROID_LOG_PRINT_BUFFER 0
// #include "android_log_print.h"
#if !defined(ANDROID_LOG_PRINT_BUFFER)
#define ANDROID_LOG_PRINT_BUFFER 1
#endif  // !defined(ANDROID_LOG_PRINT_BUFFER)

// Whether to override printf() and vprintf() to redirect output to the android
// log.
#if !defined(ANDROID_LOG_OVERRIDE_PRINTF)
#define ANDROID_LOG_OVERRIDE_PRINTF 1
#endif  // !defined(ANDROID_LOG_OVERRIDE_PRINTF)

// Redefine printf() and vprintf() to print to the Android log.
#if ANDROID_LOG_OVERRIDE_PRINTF
#if ANDROID_LOG_PRINT_BUFFER
#define printf(...) AndroidPrintf(__VA_ARGS__)
#define vprintf(format_, arg_) AndroidVPrintf(format_, arg_)
#else
#define printf(...) \
  ((void)__android_log_print(ANDROID_LOG_PRINT_PRIORITY, \
                             gAndroidLogPrintfTag, __VA_ARGS__))
#define vprintf(format_, arg_) \
  ((void)__android_log_vprint(ANDROID_LOG_PRINT_PRIORITY, \
                              gAndroidLogPrintfTag, format_, arg_))
#endif  // ANDROID_LOG_PRINT_BUFFER
#endif  // ANDROID_LOG_OVERRIDE_PRINTF

#if defined(__cplusplus)
extern "C" {
#endif  // defined(__cplusplus)

// Global tag used for log output.  This can used when filtering the output of
// "adb logcat" to distinguish the log messages from this application vs.
// other applications and the rest of the system.
extern const char *gAndroidLogPrintfTag;

// Print to the log via a temporary buffer to workaround __android_log_print()
// adding a newline to the end of each printed string.
void AndroidVPrintf(const char *format, va_list arg);
// See android_vprintf().
void AndroidPrintf(const char *format, ...);

// Wrappers around AndroidVPrintf() and AndroidLogPrint() which ignore the
// prio and tag arguments, instead using ANDROID_LOG_PRINT_PRIORITY and
// ANDROID_LOG_PRINT_TAG.  These can be used to replace __android_log_print
// or __android_log_vprint in other modules.
int AndroidLogVPrint(int prio, const char *tag, const char *fmt, va_list ap);
int AndroidLogPrint(int prio, const char *tag,  const char *fmt, ...);

#if defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#if defined(__cplusplus)
#include <ostream>
#include <streambuf>

// Can be used to print to the Android log from a stream.
// See the header comment for an example of how this class is used.
class AndroidLogPrintStreamBuf : public std::streambuf {
 public:
  AndroidLogPrintStreamBuf() { }

 protected:
  // Print characters that are placed in the streambuf.
  virtual std::streamsize xsputn(const char *s, std::streamsize n) {
    for (std::streamsize i = 0; i < n; ++i) {
      printf("%c", s[i]);
    }
    return n;
  }
};
#endif  // defined(__cplusplus)
#endif  // defined(ANDROID) || defined(__ANDROID__)
#endif  // ANDROID_LOG_PRINT_H
