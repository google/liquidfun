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
#ifndef B2_PARTICLE_SYSTEM_H
#define B2_PARTICLE_SYSTEM_H

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
class b2Fixture;
struct b2ParticleGroupDef;
struct b2Vec2;
struct b2AABB;

struct b2ParticleContact
{
	/// Indices of the respective particles making contact.
	int32 indexA, indexB;

	/// The logical sum of the particle behaviors that have been set.
	/// See the b2ParticleFlag enum.
	uint32 flags;

	/// Weight of the contact. A value between 0.0f and 1.0f.
	/// 0.0f ==> particles are just barely touching
	/// 1.0f ==> particles are perfectly on top of each other
	float32 weight;

	/// The normalized direction from A to B.
	b2Vec2 normal;
};

struct b2ParticleBodyContact
{
	/// Index of the particle making contact.
	int32 index;

	/// The body making contact.
	b2Body* body;

	/// The specific fixture making contact
	b2Fixture* fixture;

	/// Weight of the contact. A value between 0.0f and 1.0f.
	float32 weight;

	/// The normalized direction from the particle to the body.
	b2Vec2 normal;

	/// The effective mass used in calculating force.
	float32 mass;
};

struct b2ParticleSystemDef
{
	b2ParticleSystemDef()
	{
		// Initialize particle radius
		particleRadius = 1.0f;
		// Initialize physical coefficients to the maximum values that
		// maintain numerical stability.
		pressureStrength = 0.05f;
		dampingStrength = 1.0f;
		elasticStrength = 0.25f;
		springStrength = 0.25f;
		viscousStrength = 0.25f;
		surfaceTensionPressureStrength = 0.2f;
		surfaceTensionNormalStrength = 0.2f;
		powderStrength = 0.5f;
		ejectionStrength = 0.5f;
		staticPressureStrength = 0.2f;
		staticPressureRelaxation = 0.2f;
		staticPressureIterations = 8;
		colorMixingStrength = 0.5f;
	}

	/// Particle radius
	float32 particleRadius;

	/// Increases pressure in response to compression
	/// Smaller values allow more compression
	float32 pressureStrength;

	/// Reduces velocity along the collision normal
	/// Smaller value reduces less
	float32 dampingStrength;

	/// Restores shape of elastic particle groups
	/// Larger values increase elastic particle velocity
	float32 elasticStrength;

	/// Restores length of spring particle groups
	/// Larger values increase sprint particle velocity
	float32 springStrength;

	/// Reduces relative velocity of viscous particles
	/// Larger values slow down viscous particles more
	float32 viscousStrength;

	/// Produces pressure on tensile particles
	/// TODO: document further
	float32 surfaceTensionPressureStrength;

	/// Smoothes outline of tensile particles
	/// TODO: document further
	float32 surfaceTensionNormalStrength;

	/// Produces repulsion between powder particles
	/// Larger values repulse more
	float32 powderStrength;

	/// Pushes particles out of solid particle group
	/// Larger values repulse more
	float32 ejectionStrength;

	/// Produces static pressure
	/// Larger values increase the pressure on neighboring partilces
	/// For a description of static pressure, see
	/// http://en.wikipedia.org/wiki/Static_pressure#Static_pressure_in_fluid_dynamics
	float32 staticPressureStrength;

	/// Reduces instability in static pressure calculation
	/// Larger values make stabilize static pressure with fewer iterations
	float32 staticPressureRelaxation;

	/// Computes static pressure more precisely
	/// See SetParticleStaticPressureIterations for details
	int32 staticPressureIterations;

	/// Determines how fast colors are mixed
	/// 1.0f ==> mixed immediately
	/// 0.5f ==> mixed half way each simulation step (see Step())
	float32 colorMixingStrength;
};

class b2ParticleSystem
{
public:
	/// Create a particle whose properties have been defined.
	/// No reference to the definition is retained.
	/// A simulation step must occur before it's possible to interact with a
	/// newly created particle.  For example, DestroyParticleInShape() will
	/// not destroy a particle until Step() has been called.
	/// @warning This function is locked during callbacks.
	/// @return the index of the particle.
	int32 CreateParticle(const b2ParticleDef& def);

	/// Destroy a particle.
	/// The particle is removed after the next simulation step (see Step()).
	void DestroyParticle(int32 index)
	{
		DestroyParticle(index, false);
	}

	/// Destroy a particle.
	/// The particle is removed after the next step.
	/// @param Index of the particle to destroy.
	/// @param Whether to call the destruction listener just before the
	/// particle is destroyed.
	void DestroyParticle(int32 index, bool callDestructionListener);

	/// Destroy particles inside a shape without enabling the destruction
	/// callback for destroyed particles.
	/// This function is locked during callbacks.
	/// For more information see
	/// DestroyParticleInShape(const b2Shape&, const b2Transform&,bool).
	/// @param Shape which encloses particles that should be destroyed.
	/// @param Transform applied to the shape.
	/// @warning This function is locked during callbacks.
	/// @return Number of particles destroyed.
	int32 DestroyParticlesInShape(const b2Shape& shape, const b2Transform& xf)
	{
		return DestroyParticlesInShape(shape, xf, false);
	}

	/// Destroy particles inside a shape.
	/// This function is locked during callbacks.
	/// In addition, this function immediately destroys particles in the shape
	/// in constrast to DestroyParticle() which defers the destruction until
	/// the next simulation step.
	/// @param Shape which encloses particles that should be destroyed.
	/// @param Transform applied to the shape.
	/// @param Whether to call the world b2DestructionListener for each
	/// particle destroyed.
	/// @warning This function is locked during callbacks.
	/// @return Number of particles destroyed.
	int32 DestroyParticlesInShape(const b2Shape& shape, const b2Transform& xf,
	                              bool callDestructionListener);


	/// Create a particle group whose properties have been defined. No
	/// reference to the definition is retained.
	/// @warning This function is locked during callbacks.
	b2ParticleGroup* CreateParticleGroup(const b2ParticleGroupDef& def);

	/// Join two particle groups.
	/// @param the first group. Expands to encompass the second group.
	/// @param the second group. It is destroyed.
	/// @warning This function is locked during callbacks.
	void JoinParticleGroups(b2ParticleGroup* groupA, b2ParticleGroup* groupB);

	/// Destroy particles in a group.
	/// This function is locked during callbacks.
	/// @param The particle group to destroy.
	/// @param Whether to call the world b2DestructionListener for each
	/// particle is destroyed.
	/// @warning This function is locked during callbacks.
	void DestroyParticlesInGroup(b2ParticleGroup* group,
								 bool callDestructionListener);

	/// Destroy particles in a group without enabling the destruction
	/// callback for destroyed particles.
	/// This function is locked during callbacks.
	/// @param The particle group to destroy.
	/// @warning This function is locked during callbacks.
	void DestroyParticlesInGroup(b2ParticleGroup* group)
	{
		DestroyParticlesInGroup(group, false);
	}

	/// Get the world particle group list. With the returned group, use
	/// b2ParticleGroup::GetNext to get the next group in the world list.
	/// A NULL group indicates the end of the list.
	/// @return the head of the world particle group list.
	b2ParticleGroup* GetParticleGroupList();
	const b2ParticleGroup* GetParticleGroupList() const;

	/// Get the number of particle groups.
	int32 GetParticleGroupCount() const;

	/// Get the number of particles.
	int32 GetParticleCount() const;

	/// Get the maximum number of particles.
	int32 GetParticleMaxCount() const;

	/// Set the maximum number of particles.
	/// By default, there is no maximum. The particle buffers can continue to
	/// grow while b2World's block allocator still has memory.
	/// Note: If you try to CreateParticle() with more than this count,
	/// b2_invalidParticleIndex is returned.
	void SetParticleMaxCount(int32 count);

	/// Change the particle density.
	/// Particle density affects the mass of the particles, which in turn
	/// affects how the partcles interact with b2Bodies. Note that the density
	/// does not affect how the particles interact with each other.
	void SetParticleDensity(float32 density);

	/// Get the particle density.
	float32 GetParticleDensity() const;

	/// Change the particle gravity scale. Adjusts the effect of the global
	/// gravity vector on particles. Default value is 1.0f.
	void SetParticleGravityScale(float32 gravityScale);

	/// Get the particle gravity scale.
	float32 GetParticleGravityScale() const;

	/// Damping is used to reduce the velocity of particles. The damping
	/// parameter can be larger than 1.0f but the damping effect becomes
	/// sensitive to the time step when the damping parameter is large.
	void SetParticleDamping(float32 damping);

	/// Get damping for particles
	float32 GetParticleDamping() const;

	/// Change the number of iterations when calculating the static pressure of
	/// particles. By default, 8 iterations. You can reduce the number of
	/// iterations down to 1 in some situations, but this may cause
	/// instabilities when many particles come together. If you see particles
	/// popping away from each other like popcorn, you may have to increase the
	/// number of iterations.
	/// For a description of static pressure, see
	/// http://en.wikipedia.org/wiki/Static_pressure#Static_pressure_in_fluid_dynamics
	void SetParticleStaticPressureIterations(int32 iterations);

	/// Get the number of iterations for static pressure of particles.
	int32 GetParticleStaticPressureIterations() const;

	/// Change the particle radius.
	/// You should set this only once, on world start.
	/// If you change the radius during execution, existing particles may
	/// explode, shrink, or behave unexpectedly.
	void SetParticleRadius(float32 radius);

	/// Get the particle radius.
	float32 GetParticleRadius() const;

	/// Get the position of each particle
	/// Array is length GetParticleCount()
	/// @return the pointer to the head of the particle positions array.
	b2Vec2* GetParticlePositionBuffer();
	const b2Vec2* GetParticlePositionBuffer() const;

	/// Get the velocity of each particle
	/// Array is length GetParticleCount()
	/// @return the pointer to the head of the particle velocities array.
	b2Vec2* GetParticleVelocityBuffer();
	const b2Vec2* GetParticleVelocityBuffer() const;

	/// Get the color of each particle
	/// Array is length GetParticleCount()
	/// @return the pointer to the head of the particle colors array.
	b2ParticleColor* GetParticleColorBuffer();
	const b2ParticleColor* GetParticleColorBuffer() const;

	/// Get the particle-group of each particle.
	/// Array is length GetParticleCount()
	/// @return the pointer to the head of the particle group array.
	b2ParticleGroup* const* GetParticleGroupBuffer();
	const b2ParticleGroup* const* GetParticleGroupBuffer() const;

	/// Get the user-specified data of each particle.
	/// Array is length GetParticleCount()
	/// @return the pointer to the head of the particle user-data array.
	void** GetParticleUserDataBuffer();
	void* const* GetParticleUserDataBuffer() const;

	/// Get the flags for each particle. See the b2ParticleFlag enum.
	/// Array is length GetParticleCount()
	/// @return the pointer to the head of the particle-flags array.
	const uint32* GetParticleFlagsBuffer() const;

	/// Set flags for a particle. See the b2ParticleFlag enum.
	void SetParticleFlags(int32 index, uint32 flags);

	/// Set an external buffer for particle data.
	/// Normally, the b2World's block allocator is used for particle data.
	/// However, sometimes you may have an OpenGL or Java buffer for particle
	/// data. To avoid data duplication, you may supply this external buffer.
	///
	/// Note that, when b2World's block allocator is used, the particle data
	/// buffers can grow as required. However, when external buffers are used,
	/// the maximum number of particles is clamped to the size of the smallest
	/// external buffer.
	///
	/// @param buffer is a pointer to a block of memory.
	/// @param size is the number of values in the block.
	void SetParticleFlagsBuffer(uint32* buffer, int32 capacity);
	void SetParticlePositionBuffer(b2Vec2* buffer, int32 capacity);
	void SetParticleVelocityBuffer(b2Vec2* buffer, int32 capacity);
	void SetParticleColorBuffer(b2ParticleColor* buffer, int32 capacity);
	void SetParticleUserDataBuffer(void** buffer, int32 capacity);

	/// Get contacts between particles
	/// Contact data can be used for many reasons, for example to trigger
	/// rendering or audio effects.
	const b2ParticleContact* GetParticleContacts();
	int32 GetParticleContactCount();

	/// Get contacts between particles and bodies
	/// Contact data can be used for many reasons, for example to trigger
	/// rendering or audio effects.
	const b2ParticleBodyContact* GetParticleBodyContacts();
	int32 GetParticleBodyContactCount();

	/// Compute the kinetic energy that can be lost by damping force
	float32 ComputeParticleCollisionEnergy() const;

	/// Get the next particle-system in the world's particle-system list.
	b2ParticleSystem* GetNext();
	const b2ParticleSystem* GetNext() const;

	/// Query the particle system for all particles that potentially overlap
	/// the provided AABB.
	/// @param callback a user implemented callback class.
	/// @param aabb the query box.
	void QueryAABB(b2QueryCallback* callback, const b2AABB& aabb) const;

	/// Query the particle system for all particles that potentially overlap
	/// the provided shape's AABB. Calls QueryAABB internally.
	/// @param callback a user implemented callback class.
	/// @param shape the query shape
	/// @param xf the transform of the AABB
	void QueryShapeAABB(b2QueryCallback* callback, const b2Shape& shape,
						const b2Transform& xf) const;

private:

	friend class b2World;
	friend class b2ParticleGroup;
	friend class b2ParticleBodyContactRemovePredicate;

	template <typename T>
	struct ParticleBuffer
	{
		ParticleBuffer()
		{
			data = NULL;
			userSuppliedCapacity = 0;
		}
		T* data;
		int32 userSuppliedCapacity;
	};

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

	// Callback used with b2VoronoiDiagram.
	class UpdateTriadsCallback
	{
	public:
		void operator()(int32 a, int32 b, int32 c) const;
		b2ParticleSystem* system;
		b2ParticleGroup* groupA;
		b2ParticleGroup* groupB;
	};

	/// All particle types that require creating pairs
	static const int32 k_pairFlags =
		b2_springParticle |
		b2_barrierParticle;
	/// All particle types that require creating triads
	static const int32 k_triadFlags =
		b2_elasticParticle;
	/// All particle types that do not produce dynamic pressure
	static const int32 k_noPressureFlags =
		b2_powderParticle |
		b2_tensileParticle;
	/// All particle types that apply extra damping force with bodies
	static const int32 k_extraDampingFlags =
		b2_staticPressureParticle;

	b2ParticleSystem(const b2ParticleSystemDef* def, b2World* world);
	~b2ParticleSystem();

	template <typename T> void FreeBuffer(T** b, int capacity);
	template <typename T> void FreeParticleBuffer(ParticleBuffer<T>* b);
	template <typename T> T* ReallocateBuffer(T* buffer, int32 oldCapacity,
											  int32 newCapacity);
	template <typename T> T* ReallocateBuffer(
		T* buffer, int32 userSuppliedCapacity, int32 oldCapacity,
		int32 newCapacity, bool deferred);
	template <typename T> T* ReallocateBuffer(
		ParticleBuffer<T>* buffer, int32 oldCapacity, int32 newCapacity,
		bool deferred);
	template <typename T> T* RequestParticleBuffer(T* buffer);

	void ReallocateInternalAllocatedBuffers(int32 capacity);
	int32 CreateParticleForGroup(
		const b2ParticleGroupDef& groupDef,
		const b2Transform& xf, const b2Vec2& position);
	void CreateParticlesStrokeShapeForGroup(
		const b2ParticleGroupDef& groupDef, const b2Transform& xf);
	void CreateParticlesFillShapeForGroup(
		const b2ParticleGroupDef& groupDef, const b2Transform& xf);
	void DestroyParticleGroup(b2ParticleGroup* group);

	void UpdatePairsAndTriads(
		int32 firstIndex, int32 lastIndex,
		b2ParticleGroup* groupA, b2ParticleGroup* groupB);
	void ComputeDepth();

	void UpdateAllParticleFlags();
	void UpdateAllGroupFlags();
	void AddContact(int32 a, int32 b);
	void UpdateContacts(bool exceptZombie);
	void UpdateBodyContacts();

	void Solve(const b2TimeStep& step);
	void SolveCollision(const b2TimeStep& step);
	void LimitVelocity(const b2TimeStep& step);
	void SolveGravity(const b2TimeStep& step);
	void SolveBarrier(const b2TimeStep& step);
	void SolveStaticPressure(const b2TimeStep& step);
	void ComputeWeight();
	void SolvePressure(const b2TimeStep& step);
	void SolveDamping(const b2TimeStep& step);
	void SolveExtraDamping(const b2TimeStep& step);
	void SolveWall();
	void SolveRigid(const b2TimeStep& step);
	void SolveElastic(const b2TimeStep& step);
	void SolveSpring(const b2TimeStep& step);
	void SolveTensile(const b2TimeStep& step);
	void SolveViscous();
	void SolvePowder(const b2TimeStep& step);
	void SolveSolid(const b2TimeStep& step);
	void SolveColorMixing();
	void SolveZombie();
	void RotateBuffer(int32 start, int32 mid, int32 end);

	void SetStrictContactCheck(bool enabled);
	bool GetStrictContactCheck() const;

	float32 GetCriticalVelocity(const b2TimeStep& step) const;
	float32 GetCriticalVelocitySquared(const b2TimeStep& step) const;
	float32 GetCriticalPressure(const b2TimeStep& step) const;
	float32 GetParticleStride() const;
	float32 GetParticleMass() const;
	float32 GetParticleInvMass() const;

	template <typename T> void SetParticleBuffer(
		ParticleBuffer<T>* buffer, T* newBufferData, int32 newCapacity);

	void SetParticleGroupFlags(b2ParticleGroup* group, uint32 flags);

	void RayCast(b2RayCastCallback* callback, const b2Vec2& point1,
				 const b2Vec2& point2) const;

	void RemoveSpuriousBodyContacts();
	static bool BodyContactCompare(const b2ParticleBodyContact& lhs, const b2ParticleBodyContact& rhs);

	int32 m_timestamp;
	int32 m_allParticleFlags;
	bool m_needsUpdateAllParticleFlags;
	int32 m_allGroupFlags;
	bool m_needsUpdateAllGroupFlags;
	int32 m_iterationIndex;
	float32 m_density;
	float32 m_inverseDensity;
	float32 m_gravityScale;
	float32 m_particleDiameter;
	float32 m_inverseDiameter;
	float32 m_squaredDiameter;
	bool m_strictContactCheck;

	int32 m_count;
	int32 m_internalAllocatedCapacity;
	int32 m_maxCount;
	ParticleBuffer<uint32> m_flagsBuffer;
	ParticleBuffer<b2Vec2> m_positionBuffer;
	ParticleBuffer<b2Vec2> m_velocityBuffer;
	/// m_weightBuffer is populated in ComputeWeight and used in
	/// ComputeDepth(), SolveStaticPressure() and SolvePressure().
	float32* m_weightBuffer;
	/// When any particles have the flag b2_staticPressureParticle,
	/// m_staticPressureBuffer is first allocated and used in
	/// SolveStaticPressure() and SolvePressure().  It will be reallocated on
	/// subsequent CreateParticle() calls.
	float32* m_staticPressureBuffer;
	/// m_accumulationBuffer is used in many functions as a temporary buffer
	/// for scalar values.
	float32* m_accumulationBuffer;
	/// When any particles have the flag b2_tensileParticle,
	/// m_accumulation2Buffer is first allocated and used in SolveTensile()
	/// as a temporary buffer for vector values.  It will be reallocated on
	/// subsequent CreateParticle() calls.
	b2Vec2* m_accumulation2Buffer;
	/// When any particle groups have the flag b2_solidParticleGroup,
	/// m_depthBuffer is first allocated and populated in ComputeDepth() and
	/// used in SolveSolid(). It will be reallocated on subsequent
	/// CreateParticle() calls.
	float32* m_depthBuffer;
	ParticleBuffer<b2ParticleColor> m_colorBuffer;
	b2ParticleGroup** m_groupBuffer;
	ParticleBuffer<void*> m_userDataBuffer;

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

	b2ParticleSystemDef m_def;

	b2World* m_world;
	b2ParticleSystem* m_prev;
	b2ParticleSystem* m_next;
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

inline const b2ParticleContact* b2ParticleSystem::GetParticleContacts()
{
	return m_contactBuffer;
}

inline int32 b2ParticleSystem::GetParticleContactCount()
{
	return m_contactCount;
}

inline const b2ParticleBodyContact* b2ParticleSystem::GetParticleBodyContacts()
{
	return m_bodyContactBuffer;
}

inline int32 b2ParticleSystem::GetParticleBodyContactCount()
{
	return m_bodyContactCount;
}

inline b2ParticleSystem* b2ParticleSystem::GetNext()
{
	return m_next;
}

inline const b2ParticleSystem* b2ParticleSystem::GetNext() const
{
	return m_next;
}

#endif
