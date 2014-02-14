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

 // full screen shader that renders the water effect

precision mediump float;

varying vec2 texcoord;  // spanning the entire screen 0..1
uniform sampler2D tex0; // the fullscreen FBO

void main() {
  vec3 color = texture2D(tex0, texcoord).xyz;
  // Add additional foam curve to outer edges of water.
  // Outer edges defined by a range in blue's intensity.
  const float minfoam = 0.02;
  const float maxfoam = 0.40;
  if (color.b < maxfoam && color.b > minfoam) {
    float midfoam = (minfoam + maxfoam) / 2.0;
    vec3 foamcolor = vec3(0.8, 0.8, 1.0);
    float foamintensity = smoothstep(1.0, 0.0, abs(color.b - midfoam) * 5.0);
    color += foamcolor * foamintensity;
  }
  const float levels = 8.0; // amount of discrete levels to divide the range in
  // round color range to closest level
  color = floor(color * levels + 0.5) / levels;
  gl_FragColor = vec4(color, 1.0);
}
