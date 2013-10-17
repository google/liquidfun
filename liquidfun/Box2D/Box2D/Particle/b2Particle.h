#ifndef B2_PARTICLE
#define B2_PARTICLE

#include <Box2D/Common/b2Math.h>

struct b2Color;

/// The particle type. Can be combined with | operator.
/// Zero means liquid.
enum b2ParticleFlag
{
	b2_elasticParticle =   1 << 5,  // with restitution from deformation
	b2_powderParticle =    1 << 16, // wihthout isotropic pressure
	b2_rigidParticle =     1 << 18, // keeps its shape
	b2_springParticle =    1 << 19, // with restitution from stretching
	b2_tensileParticle =   1 << 20, // with surface tension
	b2_viscousParticle =   1 << 22, // with viscosity
	b2_wallParticle =      1 << 23, // zero velocity
	b2_zombieParticle =    1 << 26, // removed after next step
};

/// Small-sized color object for each particle
struct b2ParticleColor
{
	uint8 r,g,b,a;
	b2ParticleColor() {}
	b2ParticleColor(int32 r, int32 g, int32 b, int32 a) : r(r), g(g), b(b), a(a) {}
	b2ParticleColor(const b2Color& color);
	bool IsZero() const
	{
		return !r && !g && !b && !a;
	}
	b2Color GetColor() const;
	void Set(int32 r_, int32 g_, int32 b_, int32 a_)
	{
		r = r_;
		g = g_;
		b = b_;
		a = a_;
	}
	void Set(const b2Color& color);
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

	/// The logical sum of particle type flags.
	uint32 flags;

	/// The world position of the particle.
	b2Vec2 position;

	/// The linear velocity of the particle in world co-ordinates.
	b2Vec2 velocity;

	/// The color of the particle.
	b2ParticleColor color;

	/// Use this to store application specific body data.
	void* userData;

};

#endif
