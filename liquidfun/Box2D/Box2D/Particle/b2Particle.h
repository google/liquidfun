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
#ifndef B2_PARTICLE
#define B2_PARTICLE

#include <Box2D/Common/b2Math.h>
#include <Box2D/Common/b2Settings.h>

struct b2Color;

/// The particle type. Can be combined with | operator.
/// Zero means liquid.
enum b2ParticleFlag
{
	b2_waterParticle =       0,
	b2_zombieParticle =      1 << 1, // removed after next step
	b2_wallParticle =        1 << 2, // zero velocity
	b2_springParticle =      1 << 3, // with restitution from stretching
	b2_elasticParticle =     1 << 4, // with restitution from deformation
	b2_viscousParticle =     1 << 5, // with viscosity
	b2_powderParticle =      1 << 6, // without isotropic pressure
	b2_tensileParticle =     1 << 7, // with surface tension
	b2_colorMixingParticle = 1 << 8, // mixing color between contacting particles
	b2_destructionListener = 1 << 9, // call b2DestructionListener on destruction
	b2_barrierParticle     = 1 << 10, // prevents other particles from leaking
};

/// Small color object for each particle
class b2ParticleColor
{
public:
	b2ParticleColor() {}
	/// Constructor with four elements: r (red), g (green), b (blue), and a
	/// (opacity).
	/// Each element can be specified 0 to 255.
	b2Inline b2ParticleColor(uint8 r, uint8 g, uint8 b, uint8 a)
	{
		Set(r, g, b, a);
	}

	/// Constructor that initializes the above four elements with the value of
	/// the b2Color object.
	b2ParticleColor(const b2Color& color);

	/// True when all four color elements equal 0. When true, a particle color
	/// buffer isn't allocated by CreateParticle().
	///
	bool IsZero() const
	{
		return !r && !g && !b && !a;
	}

	/// Used internally to convert the value of b2Color.
	///
	b2Color GetColor() const;

	/// Sets color for current object using the four elements described above.
	///
	b2Inline void Set(int32 r_, int32 g_, int32 b_, int32 a_)
	{
		r = r_;
		g = g_;
		b = b_;
		a = a_;
	}

	/// Initializes the object with the value of the b2Color.
	///
	void Set(const b2Color& color);

	/// Assign a b2ParticleColor to this instance.
	b2ParticleColor& operator = (const b2ParticleColor &color)
	{
		Set(color.r, color.g, color.b, color.a);
		return *this;
	}

	/// Multiplies r, g, b, a members by s where s is a value between 0.0
	/// and 1.0.
	b2ParticleColor& operator *= (float32 s)
	{
		Set((uint8)(r * s), (uint8)(g * s), (uint8)(b * s), (uint8)(a * s));
		return *this;
	}

	/// Scales r, g, b, a members by s where s is a value between 0 and 255.
	b2ParticleColor& operator *= (uint8 s)
	{
		// 1..256 to maintain the complete dynamic range.
		const int32 scale = (int32)s + 1;
		Set((uint8)(((int32)r * scale) >> k_bitsPerComponent),
			(uint8)(((int32)g * scale) >> k_bitsPerComponent),
			(uint8)(((int32)b * scale) >> k_bitsPerComponent),
			(uint8)(((int32)a * scale) >> k_bitsPerComponent));
		return *this;
	}

	/// Scales r, g, b, a members by s returning the modified b2ParticleColor.
	b2ParticleColor operator * (float32 s) const
	{
		return MultiplyByScalar(s);
	}

	/// Scales r, g, b, a members by s returning the modified b2ParticleColor.
	b2ParticleColor operator * (uint8 s) const
	{
		return MultiplyByScalar(s);
	}

	/// Add two colors.  This is a non-saturating addition so values
	/// overflows will wrap.
	b2Inline b2ParticleColor& operator += (const b2ParticleColor &color)
	{
		r += color.r;
		g += color.g;
		b += color.b;
		a += color.a;
		return *this;
	}

	/// Add two colors.  This is a non-saturating addition so values
	/// overflows will wrap.
	b2ParticleColor operator + (const b2ParticleColor &color) const
	{
		b2ParticleColor newColor(*this);
		newColor += color;
		return newColor;
	}

	/// Subtract a color from this color.  This is a subtraction without
	/// saturation so underflows will wrap.
	b2Inline b2ParticleColor& operator -= (const b2ParticleColor &color)
	{
		r -= color.r;
		g -= color.g;
		b -= color.b;
		a -= color.a;
		return *this;
	}

	/// Subtract a color from this color returning the result.  This is a
	/// subtraction without saturation so underflows will wrap.
	b2ParticleColor operator - (const b2ParticleColor &color) const
	{
		b2ParticleColor newColor(*this);
		newColor -= color;
		return newColor;
	}

	/// Compare this color with the specified color.
	bool operator == (const b2ParticleColor &color) const
	{
		return r == color.r && g == color.g && b == color.b && a == color.a;
	}

	/// Mix mixColor with this color using strength to control how much of
	/// mixColor is mixed with this color and vice versa.  The range of
	/// strength is 0..128 where 0 results in no color mixing and 128 results
	/// in an equal mix of both colors.  strength 0..128 is analogous to an
	/// alpha channel value between 0.0f..0.5f.
	b2Inline void Mix(b2ParticleColor * const mixColor, const int32 strength)
	{
		MixColors(this, mixColor, strength);
	}

	/// Mix colorA with colorB using strength to control how much of
	/// colorA is mixed with colorB and vice versa.  The range of
	/// strength is 0..128 where 0 results in no color mixing and 128 results
	/// in an equal mix of both colors.  strength 0..128 is analogous to an
	/// alpha channel value between 0.0f..0.5f.
	static b2Inline void MixColors(b2ParticleColor * const colorA,
							 b2ParticleColor * const colorB,
							 const int32 strength)
	{
		const uint8 dr = (strength * (colorB->r - colorA->r)) >>
			   			 k_bitsPerComponent;
		const uint8 dg = (strength * (colorB->g - colorA->g)) >>
			   			 k_bitsPerComponent;
		const uint8 db = (strength * (colorB->b - colorA->b)) >>
			   			 k_bitsPerComponent;
		const uint8 da = (strength * (colorB->a - colorA->a)) >>
			   			 k_bitsPerComponent;
		colorA->r += dr;
		colorA->g += dg;
		colorA->b += db;
		colorA->a += da;
		colorB->r -= dr;
		colorB->g -= dg;
		colorB->b -= db;
		colorB->a -= da;
	}

private:
	/// Generalization of the multiply operator using a scalar in-place
	/// multiplication.
	template <typename T>
	b2ParticleColor MultiplyByScalar(T s) const
	{
		b2ParticleColor color(*this);
		color *= s;
		return color;
	}

public:
	uint8 r, g, b, a;

protected:
	/// Maximum value of a b2ParticleColor component.
	static const float32 k_maxValue;
	/// 1.0 / k_maxValue.
	static const float32 k_inverseMaxValue;
	/// Number of bits used to store each b2ParticleColor component.
	static const uint8 k_bitsPerComponent;
};

extern b2ParticleColor b2ParticleColor_zero;

/// A particle definition holds all the data needed to construct a particle.
/// You can safely re-use these definitions.
struct b2ParticleDef
{

	b2ParticleDef()
	{
		flags = 0;
		position = b2Vec2_zero;
		velocity = b2Vec2_zero;
		color = b2ParticleColor_zero;
		userData = NULL;
	}

	/// Specifies the type of particle. A particle may be more than one type.
	/// Multiple types are chained by logical sums, for example:
	/// pd.flags = b2_elasticParticle | b2_viscousParticle
	uint32 flags;

	/// The world position of the particle.
	b2Vec2 position;

	/// The linear velocity of the particle in world co-ordinates.
	b2Vec2 velocity;

	/// The color of the particle.
	b2ParticleColor color;

	/// Use this to store application-specific body data.
	void* userData;

};

#endif
