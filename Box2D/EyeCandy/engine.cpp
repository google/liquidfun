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

#include "engine.h"
#include <platform.h>

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

EyeCandyShader::EyeCandyShader()
    : program(0),
      position_handle(0),
      particlesize_handle(0),
      extents_handle(0),
      scale_handle(0),
      tex_coord_handle(0)
  {}

void EyeCandyShader::SetWorld(float scale, float width, float height) {
  glUseProgram(program);
  glUniform2f(extents_handle, width / scale, height / scale);
  glUniform1f(scale_handle, scale);
}

GLint EyeCandyShader::Handle(const char *name) {
  GLint handle = glGetUniformLocation(program, name);
  assert(handle >= 0);
  return handle;
}

void EyeCandyShader::Set1f(const char *name, float f) {
  glUniform1f(Handle(name), f);
}

void EyeCandyShader::Set2f(const char *name, const b2Vec2 &v) {
  glUniform2fv(Handle(name), 1, &v.x);
}

void EyeCandyShader::Set3f(const char *name, const b2Vec3 &v) {
  glUniform3fv(Handle(name), 1, &v.x);
}

void EyeCandyShader::Set4f(const char *name, const b2Vec4 &v) {
  glUniform4fv(Handle(name), 1, &v.x);
}

int EyeCandyShader::Create(const char *vs_source,
                           const char *ps_source,
                           int vs_len,
                           int ps_len) {
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

FileRef OpenFile(SystemRef system, const char *name) {
  FileRef file = System_openFile(system, name);
  if (!file) LOGE("failed to open file: %s\n", name);
  return file;
}

GLuint LoadTextureFromTGAFile(SystemRef system, const char *name) {
  FileRef file = OpenFile(system, name);
  if (!file)
    return 0;
  GLuint tex = CreateTextureFromTGAMemory(File_getBuffer(file));
  File_close(file);
  return tex;
}

bool CreateShaderFromFiles(SystemRef system, EyeCandyShader &sh,
                            const char *vs_name, const char *ps_name) {
  FileRef vs_file = OpenFile(system, vs_name);
  if (!vs_file)
    return false;
  FileRef ps_file = OpenFile(system, ps_name);
  if (!ps_file) {
    File_close(vs_file);
    return false;
  }
  int ret = sh.Create(
            reinterpret_cast<const char *>(File_getBuffer(vs_file)),
            reinterpret_cast<const char *>(File_getBuffer(ps_file)),
            static_cast<int>(File_getLength(vs_file)),
            static_cast<int>(File_getLength(ps_file)));
  File_close(vs_file);
  File_close(ps_file);
  return ret >= 0;
}

class DrawScene : public b2Draw
{
public:
  DrawScene(Engine *e) : engine(e) {}
  void DrawPolygon(const b2Vec2* vertices,
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

  void DrawSolidPolygon(const b2Vec2* vertices,
                        int32 vertexCount,
                        const b2Color& color) {
    DrawPolygon(vertices, vertexCount, color);
  };

  void DrawCircle(const b2Vec2& center,
                  float32 radius,
                  const b2Color& color) {
  };

  void DrawSolidCircle(const b2Vec2& center, float32 radius,
                       const b2Vec2& axis, const b2Color& color) {
    DrawCircle(center, radius, color);
  };

  void DrawParticles(const b2Vec2 *centers, float32 radius,
                     const b2ParticleColor *colors, int32 count) {
  };

  void DrawSegment(const b2Vec2& p1,
                   const b2Vec2& p2,
                   const b2Color& color) {
  };

  void DrawTransform(const b2Transform& xf) {
  };

private:
  Engine *engine;
};

/**
 * Engine encapsulates all state for a running sample, which involves
 * graphics context_, resources, and the physics world_
 * it gets recreated depending on lifecycle events.
 */

Engine::Engine(SystemRef system) :
  system_(system),
  animating_(0),
  width_(0), height_(0), scale_(1),
  fbo_(0), fbo_tex_(0), background_tex_(0),
  blob_normal_tex_(0), blob_temporal_tex_(0),
  world_(nullptr), particleSystem_(nullptr),
  joint_(nullptr), mover_(nullptr),
  which_effect_(0)
{}

bool Engine::Init() {
  // initialize OpenGL ES

  LOGI("GL Version = %s\n", glGetString(GL_VERSION));
  LOGI("GL Vendor = %s\n", glGetString(GL_VENDOR));
  LOGI("GL Renderer = %s\n", glGetString(GL_RENDERER));
  LOGI("GL Extensions = %s\n", glGetString(GL_EXTENSIONS));

  LOGI("setup graphics: (%d, %d)", width_, height_);

  if (!CreateShaderFromFiles(system_, sh_color_, "color.glslv",
                                                  "color.glslf"))
    return false;
  if (!CreateShaderFromFiles(system_, sh_texture_, "texture.glslv",
                                                    "texture.glslf"))
    return false;
  if (!CreateShaderFromFiles(system_, sh_point_, "point.glslv",
                                                  "point.glslf"))
    return false;
  if (!CreateShaderFromFiles(system_, sh_fulls_, "fullscreen.glslv",
                                                  "fullscreen.glslf"))
    return false;
  if (!CreateShaderFromFiles(system_, sh_blob_, "blob.glslv",
                                                 "blob.glslf"))
    return false;
  if (!CreateShaderFromFiles(system_, sh_blobfs_, "blobfullscreen.glslv",
                                                   "blobfullscreen.glslf"))
    return false;

  glViewport(0, 0, width_, height_);

  glGenFramebuffers(1, &fbo_);
  fbo_tex_ = CreateTexture(width_, height_, nullptr,
                                   true, true, false);
  GLint prev_fbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           fbo_tex_, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);

  background_tex_ = LoadTextureFromTGAFile(system_, "background_s.tga");
  if (!background_tex_)
    return false;

  mover_tex_ = LoadTextureFromTGAFile(system_, "mover_s.tga");
  if (!mover_tex_)
    return false;

  blob_normal_tex_ = PrecomputeBlobTexture(kEffectRefraction);
  blob_temporal_tex_ = PrecomputeBlobTexture(kEffectTemporalBlend);
  assert(blob_normal_tex_ && blob_temporal_tex_);

  prev_time_ = System_getTime(system_);

  // initialize physics

  assert(!world_);
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

  return true;
}

void Engine::Terminate() {
  delete world_;
  world_ = NULL;
  animating_ = false;
}

void Engine::SetSize(int width, int height) {
  width_ = width;
  height_ = height;
}

void Engine::SetGravity(float x, float y, float z) {
  if (world_) {
    world_->SetGravity(b2Vec2(x, y));
  }
}

const SavedState &Engine::GetState() const {
  return state_;
}

void Engine::SetState(const SavedState &state) {
  state_ = state;
}

void Engine::SetAnimating(bool animating) {
  animating_ = animating;
}

bool Engine::IsAnimating() const {
  return animating_;
}

void Engine::TouchDown(float x, float y, int id) {
  which_effect_++;
}

void Engine::TouchUp(float x, float y, int id) {
}

void Engine::TouchMove(float x, float y, int id) {
}

void Engine::TouchCancel(float x, float y, int id) {
}

/**
 * Just the current frame in the display.
 */
void Engine::DrawFrame() {
  if (!animating_) {
    return;
  }

  double time = System_getTime(system_);
  double framedelta = time - prev_time_;
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
}

static float saturate(float x) {
  return std::max(std::min(x, 1.0f), 0.0f);
}

static float smoothstep(float x) {
  x = saturate(x);
  return x * x * (3 - 2 * x);
}

static unsigned char quantize(float x) {
  return saturate(x) * 255.0f;
}

GLuint Engine::PrecomputeBlobTexture(int effect) {
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

void Engine::DrawUnitQuad(const EyeCandyShader &sh) {
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

void Engine::DrawParticleBuffers(const EyeCandyShader &sh) {
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

void Engine::NormalsRefractEffect(float time) {
  // first pass: render particles to fbo_, according to point.ps
  GLint prev_fbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
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
  glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);

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

void Engine::TemporalBlendEffect(float framedelta) {
  // first pass:
  GLint prev_fbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
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
  glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);

  sh_blobfs_.SetWorld(1, 1, 1);
  glBindTexture(GL_TEXTURE_2D, fbo_tex_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, background_tex_);
  glActiveTexture(GL_TEXTURE0);
  DrawUnitQuad(sh_blobfs_);
  glBindTexture(GL_TEXTURE_2D, 0);
}
