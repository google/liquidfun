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
#include <vector>

#include "Box2D/Box2D.h"
#include "gtest/gtest.h"

#include "AndroidUtil/AndroidMainWrapper.h"
#include "TestCommon.h"


using namespace std;

class CallbackTests : public ::testing::Test {
protected:
	virtual void SetUp()
	{
		// Define the gravity vector.
		b2Vec2 gravity(0.0f, -10.0f);

		// Construct a world object, which will hold and simulate the rigid
		// bodies.
		m_world = new b2World(gravity);
	}

	virtual void TearDown()
	{
		// Clean up the world after each test.
		delete m_world;
	}

	b2World *m_world;
};

// Query callback which counts the number of times ReportParticle() is called.
class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback()
	{
		m_count = 0;
	}
	bool ReportFixture(b2Fixture* fixture)
	{
		return false;
	}
	bool ReportParticle(int32 index)
	{
		m_count++;
		return true;
	}

public:
	int32 m_count;
};

// Verify that it's possible to determine the set of particles within an
// axis-aligned bounding box using the query callback.
TEST_F(CallbackTests, QueryCallback) {
	QueryCallback callback;
	b2AABB aabb;
	CreateBoxShapedParticleGroup(m_world);
	aabb.lowerBound.Set(10, -10);
	aabb.upperBound.Set(20, 10);
	m_world->QueryAABB(&callback, aabb);
	EXPECT_EQ(callback.m_count, 0);
	aabb.lowerBound.Set(-10, -10);
	aabb.upperBound.Set(10, 10);
	m_world->QueryAABB(&callback, aabb);
	EXPECT_NE(callback.m_count, 0);
}

// Ray cast callback which counts the number of times ReportParticle() is
// called.
class RayCastCallback : public b2RayCastCallback
{
public:
	RayCastCallback()
	{
		m_count = 0;
	}
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
	                      const b2Vec2& normal, float32 fraction)
	{
		return 0;
	}
	float32 ReportParticle(int32 index, const b2Vec2& point,
	                       const b2Vec2& normal, float32 fraction)
	{
		m_count++;
		return 0;
	}

public:
	int32 m_count;
};

// Verify that it's possible to determine the set of particles intersected by
// a ray using a ray cast callback.
TEST_F(CallbackTests, RayCastCallback) {
	RayCastCallback callback;
	CreateBoxShapedParticleGroup(m_world);
	m_world->RayCast(&callback, b2Vec2(21, 0), b2Vec2(0, 21));
	EXPECT_EQ(callback.m_count, 0);
	m_world->RayCast(&callback, b2Vec2(-10, -10), b2Vec2(10, 10));
	EXPECT_NE(callback.m_count, 0);
}

// Destruction listener which records references to particles or particle
// groups objects that have been destroyed.
class DestructionListener : public b2DestructionListener {
public:
	virtual ~DestructionListener() {}

	virtual void SayGoodbye(b2Joint* joint) {}
	virtual void SayGoodbye(b2Fixture* fixture) {}

	virtual void SayGoodbye(b2ParticleGroup* group) {
		m_destroyedParticleGroups.push_back(group);
	}

	virtual void SayGoodbye(int32 index) {
		m_destroyedParticles.push_back(index);
	}

public:
	vector<b2ParticleGroup*> m_destroyedParticleGroups;
	vector<int32> m_destroyedParticles;
};


// Ensure the destroy callback is not called when destroying a particle when
// the b2_destructionListener flag is not set.
TEST_F(CallbackTests, DestroyParticleWithNoCallback) {
	DestructionListener listener;
	m_world->SetDestructionListener(&listener);

	b2ParticleDef def;
	m_world->DestroyParticle(m_world->CreateParticle(def));
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(listener.m_destroyedParticles.size(), 0U);

	CreateAndDestroyParticle(m_world, 0, false);
	EXPECT_EQ(listener.m_destroyedParticles.size(), 0U);
}

// Test the particle destroy callback using the various methods to enable the
// functionality.
TEST_F(CallbackTests, DestroyParticleWithCallback) {
	int32 index;
	DestructionListener listenerNoFlags;
	m_world->SetDestructionListener(&listenerNoFlags);
	index = CreateAndDestroyParticle(m_world, 0, true);
	EXPECT_EQ(listenerNoFlags.m_destroyedParticles.size(), 1U);
	EXPECT_EQ(listenerNoFlags.m_destroyedParticles[0], index);

	DestructionListener listenerFlag;
	m_world->SetDestructionListener(&listenerFlag);
	index = CreateAndDestroyParticle(m_world, b2_destructionListener, false);
	EXPECT_EQ(listenerFlag.m_destroyedParticles.size(), 1U);
	EXPECT_EQ(listenerFlag.m_destroyedParticles[0], index);
}

// Test destroying particles in a shape without a callback on particle
// destruction.
TEST_F(CallbackTests, DestroyParticlesInShapeWithNoCallback) {
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	b2Transform xf;
	xf.SetIdentity();
	DestructionListener listener;
	m_world->SetDestructionListener(&listener);

	int32 destroyed;
	b2ParticleDef def;
	m_world->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	destroyed = m_world->DestroyParticlesInShape(shape, xf);
	EXPECT_EQ(destroyed, 1);
	EXPECT_EQ(listener.m_destroyedParticles.size(), 0U);

	m_world->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	destroyed = m_world->DestroyParticlesInShape(shape, xf, false);
	EXPECT_EQ(destroyed, 1);
	EXPECT_EQ(listener.m_destroyedParticles.size(), 0U);
}

// Test destroying particles in a shape with a callback on particle
// destruction.
TEST_F(CallbackTests, DestroyParticlesInShapeWithCallback) {
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	b2Transform xf;
	xf.SetIdentity();
	DestructionListener listener;
	m_world->SetDestructionListener(&listener);

	int32 destroyed;
	b2ParticleDef def;
	int32 index = m_world->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	destroyed = m_world->DestroyParticlesInShape(shape, xf, true);
	EXPECT_EQ(destroyed, 1);
	EXPECT_EQ(listener.m_destroyedParticles.size(), 0U);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(listener.m_destroyedParticles.size(), 1U);
	EXPECT_EQ(listener.m_destroyedParticles[0], index);
}

// Verify the destruction callback is called when a particle group is
// destroyed.
TEST_F(CallbackTests, DestroyParticleGroupWithCallback) {
	DestructionListener listener;
	m_world->SetDestructionListener(&listener);
	b2ParticleGroup *group = CreateBoxShapedParticleGroup(m_world);
	m_world->DestroyParticlesInGroup(group);
	EXPECT_EQ(listener.m_destroyedParticleGroups.size(), 0U);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(listener.m_destroyedParticleGroups.size(), 1U);
	EXPECT_EQ(listener.m_destroyedParticleGroups[0], group);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
