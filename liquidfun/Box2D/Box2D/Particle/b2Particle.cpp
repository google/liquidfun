// Copyright 2013 Google. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
#include <Box2D/Particle/b2Particle.h>
#include <Box2D/Common/b2Draw.h>

b2ParticleColor b2ParticleColor_zero(0, 0, 0, 0);

b2ParticleColor::b2ParticleColor(const b2Color& color)
{
	r = (int8) (255 * color.r);
	g = (int8) (255 * color.g);
	b = (int8) (255 * color.b);
	a = (int8) 255;
}

b2Color b2ParticleColor::GetColor() const
{
	return b2Color(
		(float32) 1 / 255 * r,
		(float32) 1 / 255 * g,
		(float32) 1 / 255 * b);
}

void b2ParticleColor::Set(const b2Color& color)
{
	r = (int8) (255 * color.r);
	g = (int8) (255 * color.g);
	b = (int8) (255 * color.b);
	a = (int8) 255;
}
