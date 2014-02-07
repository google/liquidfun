/*
 * Copyright (c) 2014 Google, Inc.
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

// turns a point sprite into a colored dot with smooth falloff

precision mediump float;

uniform vec4 color;

void main() {

  // define our cone
  vec2 p = gl_PointCoord * 2.0 - 1.0;
  float distsqr = dot(p, p);
  float falloff = 1.0 - distsqr;
  float smooth = smoothstep(0.0, 1.0, falloff); // outside circle drops to 0

  gl_FragColor = falloff > 0.0
    ? vec4(color.xyz * smooth, 1.0)
    : vec4(0.0, 0.0, 0.0, 0.0);
}
