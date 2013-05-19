/*
 * fg_runtime_android.c
 *
 * Android runtime
 *
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

/* Parts taken from Android NDK's 'native-activity' sample: */
/*
 * Copyright (C) 2010 The Android Open Source Project
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/native_window.h>
#include "android/native_app_glue/android_native_app_glue.h"
#include "android/fg_main_android.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "FreeGLUT-jnicb", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "FreeGLUT-jnicb", __VA_ARGS__))

/* Cf. freeglut_main_android.c */
extern int32_t handle_input(struct android_app* app, AInputEvent* event);
extern void handle_cmd(struct android_app* app, int32_t cmd);

extern int main(int argc, char* argv[]);

/** NativeActivity Callbacks **/
/* Caution: they are called in the native_activity thread, not the
   FreeGLUT thread. Use android_app_write_cmd. */

/* Could be used instead of onNativeWindowRedrawNeeded */
/* Deals with status bar presence */
static void onContentRectChanged(ANativeActivity* activity, const ARect* rect) {
  LOGI("onContentRectChanged: l=%d,t=%d,r=%d,b=%d", rect->left, rect->top, rect->right, rect->bottom);
}

/* Bug: not called during a resize in android-9, only once on startup :/ */
static void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window) {
  LOGI("onNativeWindowResized: %p\n", (void*)activity);
}

/* Called after a resize, compensate broken onNativeWindowResized */
static void onNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window) {
  LOGI("onNativeWindowRedrawNeeded: %p\n", (void*)activity);
  struct android_app* app = (struct android_app*)activity->instance;
  android_app_write_cmd(app, APP_CMD_WINDOW_RESIZED);
}

/**
 * Extract all .apk assets to the application directory so they can be
 * accessed using accessed.
 * TODO: parse directories recursively
 */
static void extract_assets(struct android_app* app) {
  /* Get usable JNI context */
  JNIEnv* env = app->activity->env;
  JavaVM* vm = app->activity->vm;
  (*vm)->AttachCurrentThread(vm, &env, NULL);
  
  {
    /* Get a handle on our calling NativeActivity class */
    jclass activityClass = (*env)->GetObjectClass(env, app->activity->clazz);
    
    /* Get path to cache dir (/data/data/org.myapp/cache) */
    jmethodID getCacheDir = (*env)->GetMethodID(env, activityClass, "getCacheDir", "()Ljava/io/File;");
    jobject file = (*env)->CallObjectMethod(env, app->activity->clazz, getCacheDir);
    jclass fileClass = (*env)->FindClass(env, "java/io/File");
    jmethodID getAbsolutePath = (*env)->GetMethodID(env, fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring jpath = (jstring)(*env)->CallObjectMethod(env, file, getAbsolutePath);
    const char* app_dir = (*env)->GetStringUTFChars(env, jpath, NULL);
    
    /* chdir in the application cache directory */
    LOGI("app_dir: %s", app_dir);
    chdir(app_dir);
    (*env)->ReleaseStringUTFChars(env, jpath, app_dir);
    
    /* Pre-extract assets, to avoid Android-specific file opening */
    {
      AAssetManager* mgr = app->activity->assetManager;
      AAssetDir* assetDir = AAssetManager_openDir(mgr, "");
      const char* filename = (const char*)NULL;
      while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
	AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
	char buf[BUFSIZ];
	int nb_read = 0;
	FILE* out = fopen(filename, "w");
	while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
	  fwrite(buf, nb_read, 1, out);
	fclose(out);
	AAsset_close(asset);
      }
      AAssetDir_close(assetDir);
    }
  }

  (*vm)->DetachCurrentThread(vm);
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
  LOGI("android_main savedState=%p", app->savedState);

  /* Register window resize callback */
  app->activity->callbacks->onNativeWindowResized = onNativeWindowResized;
  app->activity->callbacks->onContentRectChanged = onContentRectChanged;
  app->activity->callbacks->onNativeWindowRedrawNeeded = onNativeWindowRedrawNeeded;
  
  app->onAppCmd = handle_cmd;
  app->onInputEvent = handle_input;

  extract_assets(app);

  /* Call user's main */
  {
    char progname[5] = "self";
    char* argv[] = {progname, NULL};
    fgDisplay.pDisplay.app = app;
    main(1, argv);
    /* FreeGLUT will exit() by itself if
       GLUT_ACTION_ON_WINDOW_CLOSE == GLUT_ACTION_EXIT */
  }

  LOGI("android_main: end");

  /* Let NativeActivity restart us */
  /* Users may want to forcibly exit() in their main() anyway because
     NativeActivity doesn't dlclose() us, so all statically-assigned
     variables keep their old values on restart.. */
}
