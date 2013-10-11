#ifndef B2_PARTICLb2_SYSTEM_H
#define B2_PARTICLb2_SYSTEM_H

#include <Box2D/Particle/b2Particle.h>
#include <Box2D/Dynamics/b2TimeStep.h>

/// You need not to directly access b2ParticleSystem.
/// To access particle data, use functions in b2World or b2ParticleGroup.

class b2World;
class b2Body;
class b2Shape;
class b2ParticleGroup;
class b2BlockAllocator;
class b2StackAllocator;
class b2QueryCallback;
class b2RayCastCallback;
struct b2ParticleGroupDef;
struct b2Vec2;
struct b2AABB;

struct b2ParticleContact
{
	int32 indexA, indexB;
	uint32 flags;
	float32 weight;
	b2Vec2 normal;
};

struct b2ParticleBodyContact
{
	int32 index;
	b2Body* body;
	float32 weight;
	b2Vec2 normal;
	float32 mass;
};

class b2ParticleSystem
{

private:

	friend class b2World;
	friend class b2ParticleGroup;

	/// Used for detecting particle contacts
	struct Proxy
	{
		int32 index;
		uint32 tag;
		friend inline bool operator<(const Proxy &a, const Proxy &b)
		{
			return a.tag < b.tag;
		}
		friend inline bool operator<(uint32 a, const Proxy &b)
		{
			return a < b.tag;
		}
		friend inline bool operator<(const Proxy &a, uint32 b)
		{
			return a.tag < b;
		}
	};

	/// Connection between two particles
	struct Pair
	{
		int32 indexA, indexB;
		uint32 flags;
		float32 strength;
		float32 distance;
	};

	/// Connection between three particles
	struct Triad
	{
		int32 indexA, indexB, indexC;
		uint32 flags;
		float32 strength;
		b2Vec2 pa, pb, pc;
		float32 ka, kb, kc, s;
	};

	struct CreateParticleGroupCallback
	{
		void operator()(int32 a, int32 b, int32 c) const;
		b2ParticleSystem* system;
		const b2ParticleGroupDef* def;
		int32 firstIndex;
	};

	struct JoinParticleGroupsCallback
	{
		void operator()(int32 a, int32 b, int32 c) const;
		b2ParticleSystem* system;
		b2ParticleGroup* groupA;
		b2ParticleGroup* groupB;
	};

	/// All particle types that require creating pairs
	static const int32 k_pairFlags =
		b2_springParticle;
	/// All particle types that require creating triads
	static const int32 k_triadFlags =
		b2_elasticParticle;
	/// All particle types that require computing depth
	static const int32 k_noPressureFlags =
		b2_powderParticle;
	/// All particle types that require computing depth
	static const int32 k_depthFlags =
		b2_elasticParticle|
		b2_rigidParticle|
		b2_wallParticle;

	b2ParticleSystem();
	~b2ParticleSystem();

	template <typename T>
	void ReallocateBuffer(T *&buffer, int32 oldCapacity, int32 newCapacity, bool required);
	template <typename T> T *RequestParticleBuffer(T *&buffer);

	int32 CreateParticle(const b2ParticleDef& def);
	void DestroyParticle(int32 index);
	void DestroyParticlesInShape(const b2Shape* shape, const b2Transform& xf);
	b2ParticleGroup* CreateParticleGroup(const b2ParticleGroupDef& def);
	void JoinParticleGroups(b2ParticleGroup* groupA, b2ParticleGroup* groupB);
	void DestroyParticleGroup(b2ParticleGroup* group);
	void ComputeDepthForGroup(b2ParticleGroup* group);

	void AddContact(int32 a, int32 b);
	void UpdateContacts();
	void UpdateBodyContacts();

	void Solve(const b2TimeStep& step);
	void SolveCollision(const b2TimeStep& step);
	void SolvePressure(const b2TimeStep& step);
	void SolveDamping(const b2TimeStep& step);
	void SolveWall(const b2TimeStep& step);
	void SolveRigid(const b2TimeStep& step);
	void SolveElastic(const b2TimeStep& step);
	void SolveSpring(const b2TimeStep& step);
	void SolveTensile(const b2TimeStep& step);
	void SolveViscous(const b2TimeStep& step);
	void SolvePowder(const b2TimeStep& step);
	void SolveDepth(const b2TimeStep& step);
	void SolveZombie();
	void RotateBuffer(int32 start, int32 mid, int32 end);

	void SetParticleRadius(float32 radius);
	float32 GetParticleRadius() const;
	void SetParticleDensity(float32 density);
	float32 GetParticleDensity() const;
	void SetParticleGravityScale(float32 gravityScale);
	float32 GetParticleGravityScale() const;
	void SetParticleDamping(float32 damping);
	float32 GetParticleDamping() const;
	float32 GetCriticalVelocity(const b2TimeStep& step) const;
	float32 GetCriticalVelocitySquared(const b2TimeStep& step) const;
	float32 GetCriticalPressure(const b2TimeStep& step) const;
	float32 GetParticleStride() const;
	float32 GetParticleMass() const;
	float32 GetParticleInvMass() const;

	b2ParticleGroup* GetParticleGroupList();
	const b2ParticleGroup* GetParticleGroupList() const;
	int32 GetParticleGroupCount() const;
	int32 GetParticleCount() const;
	uint32* GetParticleFlagsBuffer();
	b2Vec2* GetParticlePositionBuffer();
	b2Vec2* GetParticleVelocityBuffer();
	b2ParticleColor* GetParticleColorBuffer();
	void** GetParticleUserDataBuffer();
	const uint32* GetParticleFlagsBuffer() const;
	const b2Vec2* GetParticlePositionBuffer() const;
	const b2Vec2* GetParticleVelocityBuffer() const;
	const b2ParticleColor* GetParticleColorBuffer() const;
	b2ParticleGroup* const* GetParticleGroupBuffer() const;
	void* const* GetParticleUserDataBuffer() const;

	void QueryAABB(b2QueryCallback* callback, const b2AABB& aabb) const;
	void RayCast(b2RayCastCallback* callback, const b2Vec2& point1, const b2Vec2& point2) const;
	float32 ComputeParticleCollisionEnergy() const;

	int32 m_timestamp;
	int32 m_allFlags;
	float32 m_density;
	float32 m_inverseDensity;
	float32 m_gravityScale;
	float32 m_particleDiameter;
	float32 m_inverseDiameter;
	float32 m_squaredDiameter;

	int32 m_count;
	int32 m_capacity;
	uint32* m_flagsBuffer;
	b2Vec2* m_positionBuffer;
	b2Vec2* m_velocityBuffer;
	float32* m_accumulationBuffer; // temporary values
	b2Vec2* m_accumulation2Buffer; // temporary vectorial values
	float32* m_depthBuffer; // distance from the surface
	b2ParticleColor* m_colorBuffer;
	b2ParticleGroup** m_groupBuffer;
	void** m_userDataBuffer;

	int32 m_proxyCount;
	int32 m_proxyCapacity;
	Proxy* m_proxyBuffer;

	int32 m_contactCount;
	int32 m_contactCapacity;
	b2ParticleContact* m_contactBuffer;

	int32 m_bodyContactCount;
	int32 m_bodyContactCapacity;
	b2ParticleBodyContact* m_bodyContactBuffer;

	int32 m_pairCount;
	int32 m_pairCapacity;
	Pair* m_pairBuffer;

	int32 m_triadCount;
	int32 m_triadCapacity;
	Triad* m_triadBuffer;

	int32 m_groupCount;
	b2ParticleGroup* m_groupList;

	float32 m_pressureStrength;
	float32 m_dampingStrength;
	float32 m_elasticStrength;
	float32 m_springStrength;
	float32 m_viscousStrength;
	float32 m_surfaceTensionStrengthA;
	float32 m_surfaceTensionStrengthB;
	float32 m_powderStrength;
	float32 m_ejectionStrength;

	b2World* m_world;
};

inline b2ParticleGroup* b2ParticleSystem::GetParticleGroupList()
{
	return m_groupList;
}

inline const b2ParticleGroup* b2ParticleSystem::GetParticleGroupList() const
{
	return m_groupList;
}

inline int32 b2ParticleSystem::GetParticleGroupCount() const
{
	return m_groupCount;
}

inline int32 b2ParticleSystem::GetParticleCount() const
{
	return m_count;
}

#endif
