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

// This sample is meant to show a way of rendering a liquid (simulated by
// LiquidFun) in a "pretty" way, though not necessarily a very physically
// accurate way (which is hard to do in an entire 2d scene).
// Instead, it uses basic phong with spec, fresnel and some form of refraction
// tweaked in interesting ways to work well on a mobile device with limited
// resources (no floating point buffers).
//
// To understand how it works, there is plenty of description in the main
// shaders, point.glslf (that computes the FBO) and fullscreen.glslf
// (the final effect).
//
// The C++ code below is mostly just setup work for those shaders.



#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include <sys/system_properties.h>

#include <stdlib.h>
#include <assert.h>

#include <vector>

#include <Box2D/Box2D.h>

#define  LOG_TAG    "EyeCandy"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)


// GLES2 utility functions: shader loading
GLuint LoadShader(GLenum shader_type, const char* source, int len) {
  assert(source);
  assert(len);

  GLuint shader = glCreateShader(shader_type);
  if (shader) {
    glShaderSource(shader, 1, &source, &len);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      GLint info_len = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
      if (info_len) {
        auto buf = new char[info_len];
        glGetShaderInfoLog(shader, info_len, NULL, buf);
        LOGE("Could not compile shader %d:\n%s\n", shader_type, buf);
        delete[] buf;
        glDeleteShader(shader);
        shader = 0;
      }
    }
  }
  return shader;
}

GLuint CreateProgram(const char* vertex_source, const char* fragment_source,
                     int vs_len, int ps_len) {
  GLuint vertex_shader = LoadShader(GL_VERTEX_SHADER, vertex_source, vs_len);
  if (!vertex_shader) {
    return 0;
  }

  GLuint pixel_shader = LoadShader(GL_FRAGMENT_SHADER,
                                   fragment_source,
                                   ps_len);
  if (!pixel_shader) {
    glDeleteShader(vertex_shader);
    return 0;
  }

  GLuint program = glCreateProgram();
  if (program) {
    glAttachShader(program, vertex_shader);
    glAttachShader(program, pixel_shader);
    glLinkProgram(program);
    GLint link_status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
      GLint buf_length = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_length);
      if (buf_length) {
        auto buf = new char[buf_length];
        glGetProgramInfoLog(program, buf_length, NULL, buf);
        LOGE("Could not link program:\n%s\n", buf);
        delete[] buf;
      }
      glDeleteShader(vertex_shader);
      glDeleteShader(pixel_shader);
      glDeleteProgram(program);
      program = 0;
    }
  }
  return program;
}

// represents any of the shaders loaded for this sample,
// looks up standard uniforms etc.

struct EyeCandyShader {
  EyeCandyShader()
    : program(0),
      position_handle(0),
      particlesize_handle(0),
      extents_handle(0),
      scale_handle(0)
  {}

  void SetWorld(float scale, float width, float height) {
    glUseProgram(program);
    glUniform2f(extents_handle, width / scale, height / scale);
    glUniform1f(scale_handle, scale);
  }

  GLint Handle(const char *name) {
    GLint handle = glGetUniformLocation(program, name);
    assert(handle >= 0);
    return handle;
  }

  void Set1f(const char *name, float f) {
    glUniform1f(Handle(name), f);
  }

  void Set2f(const char *name, const b2Vec2 &v) {
    glUniform2fv(Handle(name), 1, &v.x);
  }

  void Set3f(const char *name, const b2Vec3 &v) {
    glUniform3fv(Handle(name), 1, &v.x);
  }

  int Create(const char *vs_source, const char *ps_source,
             int vs_len, int ps_len) {
    program = CreateProgram(vs_source, ps_source, vs_len, ps_len);
    if (!program) {
      LOGE("Could not create program.");
      return -1;
    }
    glUseProgram(program);

    particlesize_handle = glGetAttribLocation(program, "particlesize");
    position_handle = glGetAttribLocation(program, "position");

    extents_handle = glGetUniformLocation(program, "extents");
    scale_handle = glGetUniformLocation(program, "scale");

    char texname[] = "tex0";
    for (int i = 0; i < 8; i++) {
      texname[3] = '0' + i;
      GLint texloc = glGetUniformLocation(program, texname);
      if (texloc >= 0) glUniform1i(texloc, i);
    }

    return 0;
  }

  GLuint program;
  GLint position_handle;
  GLint particlesize_handle;
  GLint extents_handle;
  GLint scale_handle;
};

// loading / creating textures

GLuint CreateTexture(int width,
                     int height,
                     const unsigned char *rgba_buf,
                     bool clamp,
                     bool nearestfiltering,
                     bool generatemipmaps) {
  GLuint id;
  glGenTextures(1, &id);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, rgba_buf);
  GLenum clval = clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clval);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clval);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  nearestfiltering ? GL_NEAREST : GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nearestfiltering
    ? (generatemipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST)
    : (generatemipmaps ? GL_LINEAR_MIPMAP_LINEAR   : GL_LINEAR));
  if (generatemipmaps) glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  return id;
}

GLuint CreateTextureFromTGAMemory(const void *tga_buf) {
  struct TGA {
    unsigned char id_len, color_map_type, image_type, color_map_data[5];
    unsigned short x_origin, y_origin, width, height;
    unsigned char bpp, image_descriptor;
  };
  static_assert(sizeof(TGA) == 18,
    "Members of struct TGA need to be packed with no padding.");
  int little_endian = 1;
  if (!*reinterpret_cast<char *>(&little_endian)) {
    return 0; // TODO: endian swap the shorts instead
  }
  auto header = reinterpret_cast<const TGA *>(tga_buf);
  if (header->color_map_type != 0 // no color map
   || header->image_type != 2 // RGB or RGBA only
   || (header->bpp != 32 && header->bpp != 24)
   || header->image_descriptor != 0x20) // Y flipped only
    return 0;
  auto pixels = reinterpret_cast<const unsigned char *>(header + 1);
  pixels += header->id_len;
  int size = header->width * header->height;
  auto rgba = new unsigned char[size * 4];
  for (int y = header->height - 1; y >= 0; y--) {  // TGA's default Y-flipped
    for (int x = 0; x < header->width; x++) {
      auto p = rgba + (y * header->width + x) * 4;
      p[2] = *pixels++;    // BGR -> RGB
      p[1] = *pixels++;
      p[0] = *pixels++;
      p[3] = header->bpp == 32 ? *pixels++ : 255;
    }
  }
  auto id = CreateTexture(header->width, header->height, rgba,
                          false, false, true);
  delete[] rgba;
  return id;
}

AAsset *OpenAsset(android_app *state, const char *name) {
  AAsset *file = AAssetManager_open(state->activity->assetManager, name,
                                    AASSET_MODE_BUFFER);
  if (!file) LOGE("failed to open asset: %s\n", name);
  return file;
}

GLuint LoadTextureFromTGAAsset(android_app *state, const char *name) {
  AAsset *file = OpenAsset(state, name);
  if (!file)
    return 0;
  GLuint tex = CreateTextureFromTGAMemory(AAsset_getBuffer(file));
  AAsset_close(file);
  return tex;
}

bool CreateShaderFromAssets(android_app *state, EyeCandyShader &sh,
                            const char *vs_name, const char *ps_name) {
  AAsset *vs_file = OpenAsset(state, vs_name);
  if (!vs_file)
    return false;
  AAsset *ps_file = OpenAsset(state, ps_name);
  if (!ps_file) {
    AAsset_close(vs_file);
    return false;
  }
  int ret = sh.Create(
            reinterpret_cast<const char *>(AAsset_getBuffer(vs_file)),
            reinterpret_cast<const char *>(AAsset_getBuffer(ps_file)),
            AAsset_getLength(vs_file),
            AAsset_getLength(ps_file));
  AAsset_close(vs_file);
  AAsset_close(ps_file);
  return ret >= 0;
}


// Our saved state data.
struct SavedState {
  // add stuff here
};

class DrawScene;

/**
 * Engine encapsulates all state for a running sample, which involves
 * graphics context, resources, and the physics world
 * it gets recreated depending on lifecycle events.
 */

class Engine {
public:
  Engine(android_app* a) : app(a),
    sensor_manager(nullptr),
    accelerometer_sensor(nullptr),
    sensor_event_queue(nullptr),
    animating(0),
    display(0), surface(0), context(0),
    width(0), height(0), scale(1),
    fbo(0), fbo_tex(0), background_tex(0),
    world(nullptr), joint(nullptr), mover(nullptr)
  {}

  // helper class to draw all rigid bodies in the scene
  class DrawScene : public b2Draw
  {
  public:
    DrawScene(Engine *e) : engine(e) {}

    virtual void DrawPolygon(const b2Vec2* vertices,
                             int32 vertex_count,
                             const b2Color& color) {
      engine->sh_color.SetWorld(engine->scale, engine->width, engine->height);
      glVertexAttribPointer(engine->sh_color.position_handle, 2, GL_FLOAT,
                            GL_FALSE, 0, vertices);
      glEnableVertexAttribArray(engine->sh_color.position_handle);
      glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_count);
      glDisableVertexAttribArray(engine->sh_color.position_handle);
    };

    virtual void DrawSolidPolygon(const b2Vec2* vertices,
                                  int32 vertexCount,
                                  const b2Color& color) {
      DrawPolygon(vertices, vertexCount, color);
    };

    virtual void DrawCircle(const b2Vec2& center,
                            float32 radius,
                            const b2Color& color) {
    };

    virtual void DrawSolidCircle(const b2Vec2& center, float32 radius,
                                 const b2Vec2& axis, const b2Color& color) {
      DrawCircle(center, radius, color);
    };

    virtual void DrawParticles(const b2Vec2 *centers, float32 radius,
                               const b2ParticleColor *colors, int32 count) {
    };

    virtual void DrawSegment(const b2Vec2& p1,
                             const b2Vec2& p2,
                             const b2Color& color) {
    };

    virtual void DrawTransform(const b2Transform& xf) {
    };

  private:
    Engine *engine;
  };

  /**
   * Initialize an EGL context, load resources etc.
   */
  bool Init(android_app *app) {
    // initialize OpenGL ES and EGL

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
    EGLint dummy, format;
    EGLint num_configs;
    EGLConfig config;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    #define EGLCHECK(c) { EGLBoolean ok = (c); assert(ok); }

    EGLCHECK(eglInitialize(display, nullptr, nullptr));

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */

    EGLCHECK(eglChooseConfig(display, attribs, &config, 1, &num_configs));

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    EGLCHECK(eglGetConfigAttrib(display,
                                config,
                                EGL_NATIVE_VISUAL_ID,
                                &format));

    int ret = ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);
    assert(ret >= 0);

    surface = eglCreateWindowSurface(display, config, app->window, NULL);

    static const EGLint context_attrs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
    };

    context = eglCreateContext(display, config, NULL, context_attrs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
      LOGW("Unable to eglMakeCurrent");
      return false;
    }

    EGLCHECK(eglQuerySurface(display, surface, EGL_WIDTH, &width));
    EGLCHECK(eglQuerySurface(display, surface, EGL_HEIGHT, &height));

    #undef EGLCHECK

    LOGI("GL Version = %s\n", glGetString(GL_VERSION));
    LOGI("GL Vendor = %s\n", glGetString(GL_VENDOR));
    LOGI("GL Renderer = %s\n", glGetString(GL_RENDERER));
    LOGI("GL Extensions = %s\n", glGetString(GL_EXTENSIONS));

    LOGI("setup graphics: (%d, %d)", width, height);

    if (!CreateShaderFromAssets(app, sh_color, "color.glslv", "color.glslf"))
      return false;
    if (!CreateShaderFromAssets(app, sh_point, "point.glslv", "point.glslf"))
      return false;
    if (!CreateShaderFromAssets(app, sh_fulls, "fullscreen.glslv",
                                               "fullscreen.glslf"))
      return false;

    glViewport(0, 0, width, height);

    glGenFramebuffers(1, &fbo);
    fbo_tex = CreateTexture(width, height, nullptr,
                                   true, true, false);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           fbo_tex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    background_tex = LoadTextureFromTGAAsset(app, "background_s.tga");
    if (!background_tex)
      return false;

    InitPhysics();

    clock_gettime(CLOCK_REALTIME, &start_time);
    prev_time = -0.01f;

    return true;
  }

  bool InitPhysics() {
    world = new b2World(b2Vec2(0, -10));
    world->SetParticleGravityScale(0.4f);
    world->SetParticleDensity(1.2f);
    world->SetParticleRadius(0.5f);
    world->SetParticleDamping(0.2f);

    b2BodyDef bdg;
    b2Body* ground = world->CreateBody(&bdg);

    b2BodyDef bd;
    bd.type = b2_staticBody; //b2_dynamicBody;
    bd.allowSleep = false;
    bd.position.Set(0.0f, 0.0f);
    b2Body* body = world->CreateBody(&bd);

    b2PolygonShape shape;
    shape.SetAsBox(0.5f, 10.0f, b2Vec2( 20.0f, 0.0f), 0.0);
    body->CreateFixture(&shape, 5.0f);
    shape.SetAsBox(0.5f, 10.0f, b2Vec2(-20.0f, 0.0f), 0.0);
    body->CreateFixture(&shape, 5.0f);
    shape.SetAsBox(20.0f, 0.5f, b2Vec2(0.0f, 10.0f), 0.0);
    body->CreateFixture(&shape, 5.0f);
    shape.SetAsBox(20.0f, 0.5f, b2Vec2(0.0f, -10.0f), 0.0);
    body->CreateFixture(&shape, 5.0f);

    bd.type = b2_dynamicBody;
    bd.position.Set(0.0f, 0.0f);
    mover = world->CreateBody(&bd);
    shape.SetAsBox(1.0f, 6.0f, b2Vec2(0.0f, 2.0f), 0.0);
    mover->CreateFixture(&shape, 5.0f);

    b2RevoluteJointDef jd;
    jd.bodyA = ground;
    jd.bodyB = mover;
    jd.localAnchorA.Set(0.0f, 0.0f);
    jd.localAnchorB.Set(0.0f, 5.0f);
    jd.referenceAngle = 0.0f;
    jd.motorSpeed = 0;
    jd.maxMotorTorque = 1e7f;
    jd.enableMotor = true;
    joint = (b2RevoluteJoint*)world->CreateJoint(&jd);

    b2ParticleGroupDef pd;
    pd.flags = b2_waterParticle;

    b2PolygonShape shape2;
    shape2.SetAsBox(9.0f, 9.0f, b2Vec2(0.0f, 0.0f), 0.0);

    pd.shape = &shape2;
    world->CreateParticleGroup(pd);
  }


  /**
   * Just the current frame in the display.
   */
  void DrawFrame() {
    if (display == NULL || !animating) {
      return;
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    float time = static_cast<float>((ts.tv_sec - start_time.tv_sec) +
                                   ((ts.tv_nsec - start_time.tv_nsec) /
                                     1000000000.0));
    float framedelta = time - prev_time;
    prev_time = time;


    scale = height / 12.0f;

    joint->SetMotorSpeed(0.5f * cos(time));
    world->Step(framedelta, 8, 3);

    // Make sure we always have enough sizes in our array, if particles
    // were added dynamically.
    // note: will shift sizes if particles are removed, which will look
    // weird graphically.
    auto count = world->GetParticleCount();
    while (particle_sizes.size() < count) {
      particle_sizes.push_back((rand() / static_cast<float>(RAND_MAX)
                                       * 0.8f + 1.5f)
                                      * world->GetParticleRadius());
    }

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    // first pass: render particles to fbo, according to point.ps
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    sh_point.SetWorld(scale, width, height);

    glVertexAttribPointer(sh_point.position_handle, 2, GL_FLOAT,
                    GL_FALSE, 0, &world->GetParticlePositionBuffer()->x);
    glEnableVertexAttribArray(sh_point.position_handle);

    glVertexAttribPointer(sh_point.particlesize_handle, 1, GL_FLOAT,
                    GL_FALSE, 0, &particle_sizes[0]);
    glEnableVertexAttribArray(sh_point.particlesize_handle);

    glDrawArrays(GL_POINTS, 0, count);

    glDisableVertexAttribArray(sh_point.position_handle);
    glDisableVertexAttribArray(sh_point.particlesize_handle);

    glDisable(GL_BLEND);

    // second pass: render fbo as one quad to screen, apply final graphical
    // effects (see fulls.ps)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    static const float fullscreenquad[] = {
      -1,  1,
      -1, -1,
       1, -1,
       1,  1
    };
    sh_fulls.SetWorld(1, 1, 1);
    float angle = sinf(time) - M_PI / 2;
    b2Vec3 lightdir(cosf(angle), sinf(angle), 1.0f);
    lightdir.Normalize();
    sh_fulls.Set3f("lightdir", lightdir);
    glBindTexture(GL_TEXTURE_2D, fbo_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, background_tex);
    glActiveTexture(GL_TEXTURE0);
    glVertexAttribPointer(sh_color.position_handle, 2, GL_FLOAT,
                          GL_FALSE, 0, fullscreenquad);
    glEnableVertexAttribArray(sh_color.position_handle);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(sh_color.position_handle);
    glBindTexture(GL_TEXTURE_2D, 0);

    DrawScene ds(this);
    ds.SetFlags(b2Draw::e_shapeBit);
    world->SetDebugDraw(&ds);
    world->DrawDebugData();

    eglSwapBuffers(display, surface);
  }

  // Tear down the EGL context currently associated with the display.
  void Terminate() {
    // The window is being hidden or closed, clean it up.
    delete world;
    world = NULL;

    if (display != EGL_NO_DISPLAY) {
      eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                     EGL_NO_CONTEXT);
      if (context != EGL_NO_CONTEXT) {
        eglDestroyContext(display, context);
      }
      if (surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
      }
      eglTerminate(display);
    }
    animating = 0;
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
  }

  void GainedFocus() {
    // When our app gains focus, we start monitoring the accelerometer.
    if (accelerometer_sensor != NULL) {
      ASensorEventQueue_enableSensor(sensor_event_queue, accelerometer_sensor);
      // We'd like to get 60 events per second (in us).
      ASensorEventQueue_setEventRate(sensor_event_queue,
          accelerometer_sensor, (1000L/60)*1000);
    }
    animating = 1;
  }

  void LostFocus() {
    // When our app loses focus, we stop monitoring the accelerometer.
    // This is to avoid consuming battery while not being used.
    if (accelerometer_sensor != NULL) {
      ASensorEventQueue_disableSensor(sensor_event_queue,
                                      accelerometer_sensor);
    }
    // Also stop animating.
    animating = 0;
    DrawFrame();
  }

  void SaveState() {
    // The system has asked us to save our current state.  Do so.
    app->savedState = malloc(sizeof(SavedState));
    *((struct SavedState*)app->savedState) = state;
    app->savedStateSize = sizeof(SavedState);
  }

  void InitWindow() {
    // The window is being shown, get it ready.
    if (app->window != NULL) {
      if (!Init(app)) {
        assert(0);
        ANativeActivity_finish(app->activity);
      }
      DrawFrame();
    }
  }

  void InitApp(android_app *app) {
    // determine if we should assume the default is landscape mode
    char model_string[PROP_VALUE_MAX+1];
    __system_property_get("ro.product.model", model_string);
    LOGI("model = %s", model_string);
    // FIXME: instead, call Display.getRotation() (java)
    // https://googleplex-android.googlesource.com/platform/vendor/
    //   unbundled_google/libs/liquidfun/+/ub-games-master/Box2D/AndroidUtil/
    //   AndroidMainWrapper.c#
    landscape_device = strcmp(model_string, "SHIELD") == 0;

    // Prepare to monitor accelerometer
    sensor_manager = ASensorManager_getInstance();
    #define ASENSOR_TYPE_GRAVITY 9 // some SDK versions don't have it defined
    accelerometer_sensor = ASensorManager_getDefaultSensor(
        sensor_manager, ASENSOR_TYPE_GRAVITY);
    sensor_event_queue = ASensorManager_createEventQueue(
        sensor_manager, app->looper, LOOPER_ID_USER, NULL, NULL);

    if (app->savedState != NULL) {
      // We are starting with a previous saved state; restore from it.
      state = *(struct SavedState*)app->savedState;
    }
  }

  void PollEvents(android_app *app) {
    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source* source;

    // If not animating, we will block forever waiting for events.
    // If animating, we loop until all events are read, then continue
    // to draw the next frame of animation.
    while ((ident = ALooper_pollAll(animating ? 0 : -1, NULL, &events,
        (void**)&source)) >= 0) {

      // Process this event.
      if (source != NULL) {
        source->process(app, source);
      }

      // If a sensor has data, process it now.
      if (ident == LOOPER_ID_USER) {
        if (accelerometer_sensor != NULL) {
          ASensorEvent event;
          while (ASensorEventQueue_getEvents(sensor_event_queue,
              &event, 1) > 0) {
            if (world) {
              world->SetGravity(landscape_device
                ? b2Vec2(-event.vector.x, -event.vector.y)
                : b2Vec2( event.vector.y, -event.vector.x));
            }
          }
        }
      }

      // Check if we are exiting.
      if (app->destroyRequested != 0) {
        Terminate();
        return;
      }
    }
  }

private:
  android_app *app;

  ASensorManager *sensor_manager;
  const ASensor *accelerometer_sensor;
  ASensorEventQueue *sensor_event_queue;

  int animating;
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  int32_t width;
  int32_t height;
  float scale;
  SavedState state;
  bool landscape_device;

  EyeCandyShader sh_color; // simple single color polygons
  EyeCandyShader sh_point; // render particles into FBO
  EyeCandyShader sh_fulls; // use FBO to render water effect

  GLuint fbo, fbo_tex;
  GLuint background_tex;

  b2World *world;
  b2RevoluteJoint *joint;
  b2Body *mover;

  timespec start_time;
  float prev_time;

  std::vector<float> particle_sizes;
};

// Process the next input event.
static int32_t HandleInput(android_app *app, AInputEvent* event) {
  Engine *engine = (Engine *)app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    //engine->animating = 1;
    return 1;
  }
  return 0;
}

// Process the next main command.
static void HandleCmd(android_app *app, int32_t cmd) {
  Engine *engine = (Engine *)app->userData;
  switch (cmd) {
    case APP_CMD_SAVE_STATE:
      engine->SaveState();
      break;
    case APP_CMD_INIT_WINDOW:
      engine->InitWindow();
      break;
    case APP_CMD_TERM_WINDOW:
      engine->Terminate();
      break;
    case APP_CMD_GAINED_FOCUS:
      engine->GainedFocus();
      break;
    case APP_CMD_LOST_FOCUS:
      engine->LostFocus();
      break;
  }
}

// This is the main entry point of a native application that is using
// android_native_app_glue.  It runs in its own thread, with its own
// event loop for receiving input events and doing other things.
void android_main(android_app *app) {
  // Make sure glue isn't stripped.
  app_dummy();

  Engine engine(app);

  app->userData = &engine;
  app->onAppCmd = HandleCmd;
  app->onInputEvent = HandleInput;

  engine.InitApp(app);

  // loop waiting for stuff to do.
  for (;;) {
    engine.PollEvents(app);
    engine.DrawFrame();
  }
}

