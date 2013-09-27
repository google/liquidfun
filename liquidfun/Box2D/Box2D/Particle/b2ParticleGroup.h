#ifndef B2_PARTICLE_GROUP
#define B2_PARTICLE_GROUP

#include <Box2D/Particle/b2Particle.h>

class b2Shape;
class b2World;
class b2ParticleSystem;
class b2ParticleGroup;
struct b2ParticleColor;

/// A particle group definition holds all the data needed to construct a particle group.
/// You can safely re-use these definitions.
struct b2ParticleGroupDef
{

	b2ParticleGroupDef()
	{
		flags = 0;
		position = b2Vec2_zero;
		angle = 0;
		linearVelocity = b2Vec2_zero;
		angularVelocity = 0;
		color = b2ParticleColor_zero;
		strength = 1;
		shape = NULL;
		destroyAutomatically = true;
		userData = NULL;
	}

	/// The logical sum of particle type flags.
	uint32 flags;

	/// The world position of the group.
	b2Vec2 position;

	/// The world angle of the group in radians.
	float32 angle;

	/// The linear velocity of the group's origin in world co-ordinates.
	b2Vec2 linearVelocity;

	/// The angular velocity of the group.
	float32 angularVelocity;

	/// The color of all particles in the group.
	b2ParticleColor color;

	/// The scale of force inside the group with flag b2_elasticParticle or b2_springParticle.
	float32 strength;

	/// The shape, this must be set. The shape will not be retained, so you
	/// can create the shape on the stack.
	const b2Shape* shape;

	/// If true, the group will be destroyed when all particles of it are destroyed.
	/// The initial value is true.
	bool destroyAutomatically;

	/// Use this to store application specific group data.
	void* userData;

};

/// A group of particles. These are created via b2World::CreateParticleGroup.
class b2ParticleGroup
{

public:

	/// Get the next group.
	b2ParticleGroup* GetNext();
	const b2ParticleGroup* GetNext() const;

	/// Get the number of particles.
	int32 GetParticleCount() const;

	/// Get the particle data.
	/// @return the pointer to the head of the particle data.
	uint32* GetParticleFlagsBuffer();
	b2Vec2* GetParticlePositionBuffer();
	b2Vec2* GetParticleVelocityBuffer();
	b2ParticleColor* GetParticleColorBuffer();
	void** GetParticleUserDataBuffer();
	const uint32* GetParticleFlagsBuffer() const;
	const b2Vec2* GetParticlePositionBuffer() const;
	const b2Vec2* GetParticleVelocityBuffer() const;
	const b2ParticleColor* GetParticleColorBuffer() const;
	void* const* GetParticleUserDataBuffer() const;

	/// Get the flags of the group.
	int32 GetFlags() const;

	/// Get the statistics of the group.
	float32 GetMass() const;
	float32 GetInertia() const;
	b2Vec2 GetCenter() const;
	b2Vec2 GetLinearVelocity() const;
	float32 GetAngularVelocity() const;

	/// Get the group transform for the group's origin.
	/// It is updated only for rigid particles.
	/// @return the world transform of the group's origin.
	const b2Transform& GetTransform() const;
	const b2Vec2& GetPosition() const;
	float32 GetAngle() const;

	/// Get the user data pointer that was provided in the group definition.
	void* GetUserData() const;

	/// Set the user data. Use this to store your application specific data.
	void SetUserData(void* data);

private:

	friend class b2ParticleSystem;

	b2ParticleSystem* m_system;
	int32 m_firstIndex, m_lastIndex;
	uint32 m_flags;
	b2ParticleGroup* m_prev;
	b2ParticleGroup* m_next;

	mutable int32 m_timestamp;
	mutable float32 m_mass;
	mutable float32 m_inertia;
	mutable b2Vec2 m_center;
	mutable b2Vec2 m_linearVelocity;
	mutable float32 m_angularVelocity;
	mutable b2Transform m_transform;

	unsigned m_destroyAutomatically:1;
	unsigned m_toBeDestroyed:1;
	unsigned m_toBeSplit:1;

	void* m_userData;

	b2ParticleGroup();
	~b2ParticleGroup();
	void UpdateStatistics() const;

};

inline b2ParticleGroup* b2ParticleGroup::GetNext()
{
	return m_next;
}

inline const b2ParticleGroup* b2ParticleGroup::GetNext() const
{
	return m_next;
}

inline int32 b2ParticleGroup::GetParticleCount() const
{
	return m_lastIndex - m_firstIndex;
}

#endif
