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

 // full screen shader that renders the water effect

precision mediump float;

varying vec2 texcoord;  // spanning the entire screen 0..1
uniform sampler2D tex0; // the fullscreen FBO
uniform sampler2D tex1; // background texture
uniform vec3 lightdir;  // a directional (ortho) light source

void main() {
  // This contains very specific parameters packed into the output color in
  // point.ps (from the texture created in PrecomputeBlobTexture()),
  // read that first to be able to understand what this shader does.
  vec4 samp = texture2D(tex0, texcoord);

  float totalbias = samp.w;

  // We need to reject any pixels that are outside of any blended particles.
  // Due to bilinear filtering of the parameter texture this is a tricky value
  // to get right, it needs to be just less than the bias value, taking into
  // account the 8bit quantization this has undergone.
  const float min_filtered_bias = 0.074;

  if (totalbias < min_filtered_bias) {
    // This pixel contains no fluid at all.
    // We sample the background texture to just display it, but artificially
    // darken it a bit to make the fluid pop out a bit more.
    const float darkenbg = 0.7;
    gl_FragColor = vec4(texture2D(tex1, texcoord).xyz
                        * lightdir.z
                        * darkenbg, 1);

  } else {
    // Fluid pixel, use data computed in point.ps

    // Reconstruct normalized 2d directional vector.
    vec2 dir = normalize(samp.xy / totalbias - 1.0);

    // Reconstruct normal based on waterheight.
    // This (intentionally) cancels out the 2d dir in the middle of fluids
    // since waterheight is often saturated to 1 there.
    float waterheight = samp.z;
    vec3 normal = vec3(cos(asin(waterheight)) * dir, waterheight);

    // Vectors needed for Phong shading.
    const vec3 camdir = vec3(0.0, 0.0, 1.0);
    vec3 halfangle = normalize(camdir + lightdir);
    float NdotH = dot(normal, halfangle);
    float NdotL = dot(normal, lightdir);
    float NdotC = dot(normal, camdir);

    float spec = pow(NdotH, 128.0);  // Apply sharp specular.

    // We bias the light a bit to make shading less dark, since most liquids
    // are transparent. We don't want to lose shading entirely though, since
    // it is hard to show the shape of the liquid otherwise in 2d.
    float light = (NdotL * 0.8 + 0.2) + spec;

    // Very simple fresnel with a bit smaller range than usual (keep it subtle)
    float fresnel = pow(1.0 - abs(NdotC), 15.0);

    // This an entirely artificial refraction, but our conveniently available
    // 2d dir gives us per particle perturbance that's not available in the
    // normal anymore.
    // TODO: make constants depend on actual screen ratio
    const vec2 refractionscale = vec2(90.0, 60.0);
    vec2 refractionvector = dir / refractionscale;
    vec3 refractedcolor = texture2D(tex1, texcoord + refractionvector).xyz;

    // Simple ambient/global contribution, has to be super subtle to not
    // "wash out" the water (haha, get it? :)
    const vec3 ambientwatercolor = vec3(0.01, 0.03, 0.10);

    const vec3 fresnelcolor = vec3(0.4, 0.4, 0.6);

    // You're supposed to lerp the fresnel not add it, but this looks better
    // since we don't have proper reflections.
    gl_FragColor = vec4(refractedcolor * light
                        + fresnel * fresnelcolor
                        + ambientwatercolor, 1.0);
  }
}
