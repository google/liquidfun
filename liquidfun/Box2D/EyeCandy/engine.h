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

#ifndef __EyeCandy__engine__
#define __EyeCandy__engine__

#include <platform.h>
#include <Box2D/Box2D.h>
#include <vector>

// represents any of the shaders loaded for this sample,
// looks up standard uniforms etc.
struct EyeCandyShader {
  EyeCandyShader();

  void SetWorld(float scale, float width, float height);
  GLint Handle(const char *name);
  void Set1f(const char *name, float f);
  void Set2f(const char *name, const b2Vec2 &v);
  void Set3f(const char *name, const b2Vec3 &v);
  void Set4f(const char *name, const b2Vec4 &v);
  int Create(const char *vs_source, const char *ps_source,
             int vs_len, int ps_len);

  GLuint program;
  GLint position_handle;
  GLint particlesize_handle;
  GLint extents_handle;
  GLint scale_handle;
  GLint tex_coord_handle;
};

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
  Engine(SystemRef system);

  bool Init();

  void Terminate();

  void SetSize(int width, int height);

  void SetGravity(float x, float y, float z);

  const SavedState &GetState() const;

  void SetState(const SavedState &state);

  void SetAnimating(bool animating);

  bool IsAnimating() const;

  void TouchUp(float x, float y, int id);

  void TouchDown(float x, float y, int id);

  void TouchMove(float x, float y, int id);

  void TouchCancel(float x, float y, int id);

  void DrawFrame();

private:

  enum { kEffectTemporalBlend, kEffectRefraction, kEffectMax };

  GLuint PrecomputeBlobTexture(int effect);

  void DrawUnitQuad(const EyeCandyShader &sh);

  void DrawParticleBuffers(const EyeCandyShader &sh);

  void NormalsRefractEffect(float time);

  void TemporalBlendEffect(float framedelta);

  SystemRef system_;
  int animating_;
  int32_t width_;
  int32_t height_;
  float scale_;
  SavedState state_;

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

  double prev_time_;

  std::vector<float> particle_sizes_;

  int which_effect_;

  friend class DrawScene;
};

#endif
