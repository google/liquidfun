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
#include <algorithm>

#include <Box2D/Box2D.h>

using namespace std;

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
      scale_handle(0),
      tex_coord_handle(0)
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

  void Set4f(const char *name, const b2Vec4 &v) {
    glUniform4fv(Handle(name), 1, &v.x);
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

    tex_coord_handle = glGetAttribLocation(program, "tex_coord");

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
  GLint tex_coord_handle;
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

/**
 * Engine encapsulates all state for a running sample, which involves
 * graphics context_, resources, and the physics world_
 * it gets recreated depending on lifecycle events.
 */

class Engine {
public:
  Engine(android_app* a) : app_(a),
    sensor_manager_(nullptr),
    accelerometer_sensor_(nullptr),
    sensor_event_queue_(nullptr),
    animating_(0),
    display_(0), surface_(0), context_(0),
    width_(0), height_(0), scale_(1),
    fbo_(0), fbo_tex_(0), background_tex_(0),
    blob_normal_tex_(0), blob_temporal_tex_(0),
    world_(nullptr), particleSystem_(nullptr),
    joint_(nullptr), mover_(nullptr),
    which_effect_(0)
  {}

  // helper class to draw all rigid bodies in the scene
  class DrawScene : public b2Draw
  {
  public:
    DrawScene(Engine *e) : engine(e) {}

    virtual void DrawPolygon(const b2Vec2* vertices,
                             int32 vertex_count,
                             const b2Color& color) {
      assert(vertex_count == 4); // All our poly's are rectangles.

      float x_dist = vertices[1].x - vertices[0].x;
      float y_dist = vertices[2].y - vertices[1].y;
      if (fabsf(x_dist) < 0.01f) { // Are we horizontal?
        y_dist = vertices[2].x - vertices[1].x;
        x_dist = vertices[3].y - vertices[2].y;
      }
      float tc_ratio = y_dist / x_dist / 2;
      GLfloat texture_coordinates[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, tc_ratio,
        0.0f, tc_ratio,
      };

      engine->sh_texture_.SetWorld(engine->scale_,
                                 engine->width_,
                                 engine->height_);
      engine->sh_texture_.Set4f("color", b2Vec4(0.5f, 0.5f, 0.5f, 1.0f));
      glVertexAttribPointer(engine->sh_texture_.position_handle, 2, GL_FLOAT,
                            GL_FALSE, 0, vertices);
      glVertexAttribPointer(engine->sh_texture_.tex_coord_handle, 2, GL_FLOAT,
                            GL_FALSE, 0, texture_coordinates);
      glBindTexture(GL_TEXTURE_2D, engine->mover_tex_);
      glActiveTexture(GL_TEXTURE0);
      glEnableVertexAttribArray(engine->sh_texture_.position_handle);
      glEnableVertexAttribArray(engine->sh_texture_.tex_coord_handle);
      glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_count);
      glDisableVertexAttribArray(engine->sh_texture_.tex_coord_handle);
      glDisableVertexAttribArray(engine->sh_texture_.position_handle);
      glBindTexture(GL_TEXTURE_2D, 0);
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
   * Initialize an EGL context_, load resources etc.
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

    EGLCHECK(eglQuerySurface(display_, surface_, EGL_WIDTH, &width_));
    EGLCHECK(eglQuerySurface(display_, surface_, EGL_HEIGHT, &height_));

    #undef EGLCHECK

    LOGI("GL Version = %s\n", glGetString(GL_VERSION));
    LOGI("GL Vendor = %s\n", glGetString(GL_VENDOR));
    LOGI("GL Renderer = %s\n", glGetString(GL_RENDERER));
    LOGI("GL Extensions = %s\n", glGetString(GL_EXTENSIONS));

    LOGI("setup graphics: (%d, %d)", width_, height_);

    if (!CreateShaderFromAssets(app, sh_color_, "color.glslv",
                                               "color.glslf"))
      return false;
    if (!CreateShaderFromAssets(app, sh_texture_, "texture.glslv",
                                                  "texture.glslf"))
      return false;
    if (!CreateShaderFromAssets(app, sh_point_, "point.glslv",
                                               "point.glslf"))
      return false;
    if (!CreateShaderFromAssets(app, sh_fulls_, "fullscreen.glslv",
                                               "fullscreen.glslf"))
      return false;
    if (!CreateShaderFromAssets(app, sh_blob_, "blob.glslv",
                                              "blob.glslf"))
      return false;
    if (!CreateShaderFromAssets(app, sh_blobfs_, "blobfullscreen.glslv",
                                                "blobfullscreen.glslf"))
      return false;

    glViewport(0, 0, width_, height_);

    glGenFramebuffers(1, &fbo_);
    fbo_tex_ = CreateTexture(width_, height_, nullptr,
                                   true, true, false);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           fbo_tex_, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    background_tex_ = LoadTextureFromTGAAsset(app, "background_s.tga");
    if (!background_tex_)
      return false;

    mover_tex_ = LoadTextureFromTGAAsset(app, "mover_s.tga");
    if (!mover_tex_)
      return false;

    blob_normal_tex_ = PrecomputeBlobTexture(kEffectRefraction);
    blob_temporal_tex_ = PrecomputeBlobTexture(kEffectTemporalBlend);
    assert(blob_normal_tex_ && blob_temporal_tex_);

    InitPhysics();

    clock_gettime(CLOCK_REALTIME, &start_time_);
    prev_time_ = -0.01f;

    return true;
  }

  float saturate(float x) { return max(min(x, 1.0f), 0.0f); }
  float smoothstep(float x) { x = saturate(x); return x * x * (3 - 2 * x); }
  unsigned char quantize(float x) { return saturate(x) * 255.0f; }

  GLuint PrecomputeBlobTexture(int effect) {
    // This texture creates pre-computed parameters for each particle point
    // sprite to be blended into an FBO to later be used in the actual full
    // screen water shader.
    // We want to compute information that will allow us to have a blended
    // normal. This is tricky for a 2 reasons:
    // 1) Because we generally don't have floating point FBO's, we have to make
    //    sure to fit all the values within an 8-bit range, while taking into
    //    account up to 6 or so particles can blend for any pixel, which gives
    //    serious precision issues.
    // 2) To make the particles appear to smoothly transition into eachother
    //    as if they were a single liquid under additive blending we have to
    //    choose our math carefully.
    // For this reason, we cannot simply blend normals. Instead, we blend a
    // a directional vector and a fluid height each in their own way, and
    // reconstruct the normal later. This is also useful for the refraction
    // term.
    const int TSIZE = 64;  // Note: lower gives aliasing effects on high res
                           // screens, higher degrades performance because of
                           // texture cache.
    unsigned char tex[TSIZE][TSIZE][4];
    for (int y = 0; y < TSIZE; y++)
    {
      for (int x = 0; x < TSIZE; x++)
      {
        // define our cone
        auto xy = (b2Vec2(x, y) + 0.5f) / TSIZE * 2 - 1;
        float distsqr = xy.LengthSquared();
        float falloff = 1.0f - distsqr;
        float smooth = smoothstep(falloff); // outside circle drops to 0

        // the more we scale the distance for exp(), the more fluid the
        // transition looks, but also the more precision problems we cause on
        // the rim. 4 is a good tradeoff.
        // exp() works better than linear/smoothstep/hemisphere because it
        // makes the fluid transition nicer (less visible transition
        // boundaries).
        float waterheight = expf(distsqr * -4.0f);

        // this value represents the 0 point for the directional components
        // it needs to be fairly low to make sure we can fit many blended
        // samples (this depends on how "particlesize" for point.vs is
        // computed, the larger, the more overlapping particles, and we can't
        // allow them saturate to 1).
        // But, the lower, the more precision problems you get.
        const float bias = 0.075f;

        // the w component effectively holds the number of particles that were
        // blended to this pixel, i.e. the total bias.
        // xy is the directional vector. we reduce the magnitude of this vector
        // by a smooth version of the distance from the center to reduce its
        // contribution to the blend, this works well because vectors at the
        // edges tend to have opposed directions, this keeps it smooth and sort
        // of normalizes (since longer vectors get reduced more) at the cost of
        // precision at the far ends.
        // The z component is the fluid height, and unlike the xy is meant to
        // saturate under blending.
        const float falloff_min = 0.05f;
        b2Vec4 out;
        if (falloff > falloff_min) {
          if (effect == kEffectRefraction) {
            auto dxy = xy * 0.5f * smooth + bias;
            out = b2Vec4(dxy.x, dxy.y, waterheight, bias);
          } else {
            out = b2Vec4(0.05f * smooth,
                         0.08f * smooth,
                         0.30f * smooth, 1.0f);
          }
        } else {
          out = b2Vec4(0, 0, 0, 0);
        }
        tex[y][x][0] = quantize(out.x);
        tex[y][x][1] = quantize(out.y);
        tex[y][x][2] = quantize(out.z);
        tex[y][x][3] = quantize(out.w);
        // See fullscreen.glslf (starting at vec4 samp) for how these values
        // are used.
      }
    }

    return CreateTexture(TSIZE, TSIZE, &tex[0][0][0], true, false, false);
  }

  void InitPhysics() {
    world_ = new b2World(b2Vec2(0, -10));

    b2ParticleSystemDef particleSystemDef;
    particleSystemDef.dampingStrength = 0.2f;
    particleSystemDef.radius = 0.3f;
    particleSystem_ = world_->CreateParticleSystem(&particleSystemDef);
    particleSystem_->SetGravityScale(0.4f);
    particleSystem_->SetDensity(1.2f);

    b2BodyDef bdg;
    b2Body* ground = world_->CreateBody(&bdg);

    b2BodyDef bd;
    bd.type = b2_staticBody; //b2_dynamicBody;
    bd.allowSleep = false;
    bd.position.Set(0.0f, 0.0f);
    b2Body* body = world_->CreateBody(&bd);

    b2PolygonShape shape;
    shape.SetAsBox(0.5f, 10.0f, b2Vec2( 20.0f, 0.0f), 0.0f);
    body->CreateFixture(&shape, 5.0f);
    shape.SetAsBox(0.5f, 10.0f, b2Vec2(-20.0f, 0.0f), 0.0f);
    body->CreateFixture(&shape, 5.0f);
    shape.SetAsBox(0.5f, 20.0f, b2Vec2(0.0f, 10.0f), M_PI/2.0f);
    body->CreateFixture(&shape, 5.0f);
    shape.SetAsBox(0.5f, 20.0f, b2Vec2(0.0f, -10.0f), M_PI/2.0f);
    body->CreateFixture(&shape, 5.0f);

    bd.type = b2_dynamicBody;
    bd.position.Set(0.0f, 0.0f);
    mover_ = world_->CreateBody(&bd);
    shape.SetAsBox(1.0f, 5.0f, b2Vec2(0.0f, 2.0f), 0.0);
    mover_->CreateFixture(&shape, 5.0f);

    b2RevoluteJointDef jd;
    jd.bodyA = ground;
    jd.bodyB = mover_;
    jd.localAnchorA.Set(0.0f, 0.0f);
    jd.localAnchorB.Set(0.0f, 5.0f);
    jd.referenceAngle = 0.0f;
    jd.motorSpeed = 0;
    jd.maxMotorTorque = 1e7f;
    jd.enableMotor = true;
    joint_ = (b2RevoluteJoint*)world_->CreateJoint(&jd);

    b2ParticleGroupDef pd;
    pd.flags = b2_waterParticle;

    b2PolygonShape shape2;
    shape2.SetAsBox(9.0f, 9.0f, b2Vec2(0.0f, 0.0f), 0.0);

    pd.shape = &shape2;
    particleSystem_->CreateParticleGroup(pd);
  }

  enum { kEffectTemporalBlend, kEffectRefraction, kEffectMax };

  /**
   * Just the current frame in the display.
   */
  void DrawFrame() {
    if (display_ == NULL || !animating_) {
      return;
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    float time = static_cast<float>((ts.tv_sec - start_time_.tv_sec) +
                                   ((ts.tv_nsec - start_time_.tv_nsec) /
                                     1000000000.0));
    float framedelta = time - prev_time_;
    prev_time_ = time;
    static int frames = 0;
    if (!(frames++ & 16)) LOGI("framedelta = %f", framedelta);

    scale_ = height_ / 12.0f;

    joint_->SetMotorSpeed(0.7f * cos(time));

    world_->Step(framedelta, 8, 3); /*, b2CalculateParticleIterations(10,
                                   world_->GetRadius(), framedelta)); */

    // Make sure we always have enough sizes in our array, if particles
    // were added dynamically.
    // note: will shift sizes if particles are removed, which will look
    // weird graphically.
    while (static_cast<int>(particle_sizes_.size())
             < particleSystem_->GetParticleCount()) {
      particle_sizes_.push_back((rand() / static_cast<float>(RAND_MAX)
                                       * 0.8f + 1.5f)
                                      * particleSystem_->GetRadius());
    }

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    if (which_effect_ & 1) NormalsRefractEffect(time);
    else TemporalBlendEffect(framedelta);
    switch (which_effect_ % kEffectMax) {
      case kEffectRefraction:
        NormalsRefractEffect(time);
        break;
      case kEffectTemporalBlend:
        TemporalBlendEffect(framedelta);
        break;
      default:
        assert(!"Invalid effect selection");
        break;
    }

    DrawScene ds(this);
    ds.SetFlags(b2Draw::e_shapeBit);
    world_->SetDebugDraw(&ds);
    world_->DrawDebugData();

    eglSwapBuffers(display_, surface_);
  }

  void DrawUnitQuad(const EyeCandyShader &sh) {
    static const float unitquad[] = {
      -1,  1,
      -1, -1,
       1, -1,
       1,  1
    };
    glVertexAttribPointer(sh.position_handle, 2, GL_FLOAT,
                          GL_FALSE, 0, unitquad);
    glEnableVertexAttribArray(sh.position_handle);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(sh.position_handle);
  }

  void DrawParticleBuffers(const EyeCandyShader &sh) {
    glVertexAttribPointer(sh.position_handle, 2, GL_FLOAT,
                    GL_FALSE, 0,
                    &particleSystem_->GetPositionBuffer()->x);
    glEnableVertexAttribArray(sh.position_handle);

    glVertexAttribPointer(sh.particlesize_handle, 1, GL_FLOAT,
                    GL_FALSE, 0, &particle_sizes_[0]);
    glEnableVertexAttribArray(sh_blob_.particlesize_handle);

    glDrawArrays(GL_POINTS, 0, particleSystem_->GetParticleCount());

    glDisableVertexAttribArray(sh.position_handle);
    glDisableVertexAttribArray(sh.particlesize_handle);
  }

  void NormalsRefractEffect(float time) {
    // first pass: render particles to fbo_, according to point.ps
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    sh_point_.SetWorld(scale_, width_, height_);
    glBindTexture(GL_TEXTURE_2D, blob_normal_tex_);
    DrawParticleBuffers(sh_point_);

    glDisable(GL_BLEND);

    // second pass: render fbo_ as one quad to screen, apply final graphical
    // effects (see fulls.ps)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    sh_fulls_.SetWorld(1, 1, 1);
    float angle = sinf(time) - M_PI / 2;
    b2Vec3 lightdir(cosf(angle), sinf(angle), 1.0f);
    lightdir.Normalize();
    sh_fulls_.Set3f("lightdir", lightdir);
    glBindTexture(GL_TEXTURE_2D, fbo_tex_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, background_tex_);
    glActiveTexture(GL_TEXTURE0);
    DrawUnitQuad(sh_fulls_);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void TemporalBlendEffect(float framedelta) {
    // first pass:
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    // First, darken what's already in the framebuffer gently.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_SRC_ALPHA);

    sh_color_.SetWorld(1, 1, 1);
    // Set the alpha to be the darkening multiplier.
    // Note how this value is hardcoded to look good assuming the device
    // hits 60fps or so, it was originally a value derived from frametime,
    // but then variances in frametime would give the effect of whole screen
    // "flickers" as things got instantly darker/brighter.
    sh_color_.Set4f("color", b2Vec4(0.0f, 0.0f, 0.0f, 0.85f));
    DrawUnitQuad(sh_color_);

    // Then render the particles on top of that.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

    sh_blob_.SetWorld(scale_, width_, height_);
    glBindTexture(GL_TEXTURE_2D, blob_temporal_tex_);
    DrawParticleBuffers(sh_blob_);

    glDisable(GL_BLEND);

    // second pass:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    sh_blobfs_.SetWorld(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, fbo_tex_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, background_tex_);
    glActiveTexture(GL_TEXTURE0);
    DrawUnitQuad(sh_blobfs_);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // Tear down the EGL context_ currently associated with the display.
  void Terminate() {
    // The window is being hidden or closed, clean it up.
    delete world_;
    world_ = NULL;

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
    animating_ = 0;
    display_ = EGL_NO_DISPLAY;
    context_ = EGL_NO_CONTEXT;
    surface_ = EGL_NO_SURFACE;
  }

  void GainedFocus() {
    // When our app gains focus, we start monitoring the accelerometer.
    if (accelerometer_sensor_ != NULL) {
      ASensorEventQueue_enableSensor(sensor_event_queue_, accelerometer_sensor_);
      // We'd like to get 60 events per second (in us).
      ASensorEventQueue_setEventRate(sensor_event_queue_,
          accelerometer_sensor_, (1000L/60)*1000);
    }
    animating_ = 1;
  }

  void LostFocus() {
    // When our app loses focus, we stop monitoring the accelerometer.
    // This is to avoid consuming battery while not being used.
    if (accelerometer_sensor_ != NULL) {
      ASensorEventQueue_disableSensor(sensor_event_queue_,
                                      accelerometer_sensor_);
    }
    // Also stop animating_.
    animating_ = 0;
    DrawFrame();
  }

  void SaveState() {
    // The system has asked us to save our current state.
    app_->savedState = malloc(sizeof(SavedState));
    *((struct SavedState*)app_->savedState) = state_;
    app_->savedStateSize = sizeof(SavedState);
  }

  void InitWindow() {
    // The window is being shown, get it ready.
    if (app_->window != NULL) {
      if (!Init(app_)) {
        assert(0);
        ANativeActivity_finish(app_->activity);
      }
      DrawFrame();
    }
  }

  int DeviceRotation(ANativeActivity *activity) {
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

  void InitApp(android_app *app) {
    char model_string[PROP_VALUE_MAX+1];
    __system_property_get("ro.product.model", model_string);
    LOGI("model = %s", model_string);

    // determine if we should assume the default is landscape mode
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

    if (app->savedState != NULL) {
      // We are starting with a previous saved state; restore from it.
      state_ = *(struct SavedState*)app->savedState;
    }
  }

  void PollEvents(android_app *app) {
    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source* source;

    // If not animating_, we will block forever waiting for events.
    // If animating_, we loop until all events are read, then continue
    // to draw the next frame of animation.
    while ((ident = ALooper_pollAll(animating_ ? 0 : -1, NULL, &events,
        (void**)&source)) >= 0) {

      // Process this event.
      if (source != NULL) {
        source->process(app, source);
      }

      // If a sensor has data, process it now.
      if (ident == LOOPER_ID_USER) {
        if (accelerometer_sensor_ != NULL) {
          ASensorEvent event;
          while (ASensorEventQueue_getEvents(sensor_event_queue_,
              &event, 1) > 0) {
            if (world_) {
              world_->SetGravity(landscape_device_
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

  void Touch() {
    which_effect_++;
  }

private:
  android_app *app_;

  ASensorManager *sensor_manager_;
  const ASensor *accelerometer_sensor_;
  ASensorEventQueue *sensor_event_queue_;

  int animating_;
  EGLDisplay display_;
  EGLSurface surface_;
  EGLContext context_;
  int32_t width_;
  int32_t height_;
  float scale_;
  SavedState state_;
  bool landscape_device_;

  EyeCandyShader sh_color_;   // simple single color polygons
  EyeCandyShader sh_texture_; // simple single textured polygons
  EyeCandyShader sh_point_;   // render particles into FBO
  EyeCandyShader sh_fulls_;   // use FBO to render water effect
  EyeCandyShader sh_blob_;    // generic particle blob
  EyeCandyShader sh_blobfs_;  // blob fullscreen quantizer

  GLuint fbo_, fbo_tex_;
  GLuint background_tex_;
  GLuint mover_tex_;
  GLuint blob_normal_tex_, blob_temporal_tex_;

  b2World *world_;
  b2ParticleSystem *particleSystem_;
  b2RevoluteJoint *joint_;
  b2Body *mover_;

  timespec start_time_;
  float prev_time_;

  std::vector<float> particle_sizes_;

  int which_effect_;
};

// Process the next input event.
static int32_t HandleInput(android_app *app, AInputEvent* event) {
  Engine *engine = (Engine *)app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    auto action = AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
    if (action == AMOTION_EVENT_ACTION_DOWN)
      engine->Touch();
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
