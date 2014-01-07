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

// This shader constructs several parameters to be blended into an FBO to later
// be used in the actual full screen water shader.
// We want to compute information that will allow us to have a blended normal.
// This is tricky for a 2 reasons:
// 1) Because we generally don't have floating point FBO's, we have to make
//    sure to fit all the values within an 8-bit range, while taking into
//    account up to 6 or so particles can blend for any pixel, which gives
//    serious precision issues.
// 2) To make the particles appear to smoothly transition into eachother
//    as if they were a single liquid under additive blending we have to choose
//    our math carefully.
// For this reason, we cannot simply blend normals. Instead, we blend a
// a directional vector and a fluid height each in their own way, and
// reconstruct the normal later. This is also useful for the refraction term.

precision mediump float;

void main() {

  // define our cone
  vec2 p = gl_PointCoord * 2.0 - 1.0;
  float distsqr = dot(p, p);
  float falloff = 1.0 - distsqr;
  float smooth = smoothstep(0.0, 1.0, falloff); // outside circle drops to 0

  // the more we scale the distance for exp(), the more fluid the transition
  // looks, but also the more precision problems we cause on the rim.
  // 4 is a good tradeoff
  // exp() works better than linear/smoothstep/hemisphere because it
  // makes the fluid transition nicer (less visible transition boundaries)
  float waterheight = exp(distsqr * -4.0);

  // this value represents the 0 point for the directional components
  // it needs to be fairly low to make sure we can fit many blended samples
  // (this depends on how "particlesize" for point.vs is computed, the larger,
  // the more overlapping particles, and we can't allow them saturate to 1)
  // But, the lower, the more precision problems you get.
  const float bias = 0.075;

  // the w component effectively holds the number of particles that were
  // blended to this pixel, i.e. the total bias.
  // xy is the directional vector. we reduce the magnitude of this vector by
  // a smooth version of the distance from the center to reduce its
  // contribution to the blend, this works well because vectors at the edges
  // tend to have opposed directions, this keeps it smooth and sort of
  // normalizes (since longer vectors get reduced more) at the cost of
  // precision at the far ends.
  // The z component is the fluid height, and unlike the xy is meant to
  // saturate under blending.
  const float falloff_min = 0.05;
  gl_FragColor = falloff > falloff_min
    ? vec4(p.xy * 0.5 * smooth + bias, waterheight, bias)
    : vec4(0.0, 0.0, 0.0, 0.0);

  // See fullscreen.glslf (starting at vec4 samp) for how these values are
  // used.
}
