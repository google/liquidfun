#include "gtest/gtest.h"
#include "Box2D/Box2D.h"
#include <stdio.h>
#include "BodyTracker.h"
#include "AndroidUtil/AndroidMainWrapper.h"
#define EPSILON 0.001f

class CallbackTests : public ::testing::Test {
    protected:
	virtual void SetUp();
	virtual void TearDown();

	b2World *m_world;
	b2ParticleGroup *m_group;
};

void
CallbackTests::SetUp()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	m_world = new b2World(gravity);

	// Creawte particles
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_group = m_world->CreateParticleGroup(def);
}

void
CallbackTests::TearDown()
{
	// Intentionally blank.
}

struct QueryCallback : b2QueryCallback
{
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
	int32 m_count;
};

TEST_F(CallbackTests, QueryCallback) {
	QueryCallback callback;
	b2AABB aabb;
	aabb.lowerBound.Set(10, -10);
	aabb.upperBound.Set(20, 10);
	m_world->QueryAABB(&callback, aabb);
	EXPECT_EQ(callback.m_count, 0);
	aabb.lowerBound.Set(-10, -10);
	aabb.upperBound.Set(10, 10);
	m_world->QueryAABB(&callback, aabb);
	EXPECT_NE(callback.m_count, 0);
}

struct RayCastCallback : b2RayCastCallback
{
	RayCastCallback()
	{
		m_count = 0;
	}
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		return 0;
	}
	float32 ReportParticle(int32 index, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		m_count++;
		return 0;
	}
	int32 m_count;
};

TEST_F(CallbackTests, RayCastCallback) {
	RayCastCallback callback;
	m_world->RayCast(&callback, b2Vec2(21, 0), b2Vec2(0, 21));
	EXPECT_EQ(callback.m_count, 0);
	m_world->RayCast(&callback, b2Vec2(-10, -10), b2Vec2(10, 10));
	EXPECT_NE(callback.m_count, 0);
}

struct DestructionListener : b2DestructionListener
{
	DestructionListener()
	{
		m_count = 0;
	}
	void SayGoodbye(b2Joint* joint)
	{
	}
	void SayGoodbye(b2Fixture* fixture)
	{
	}
	void SayGoodbye(b2ParticleGroup* group)
	{
		m_count++;
	}
	int32 m_count;
};

TEST_F(CallbackTests, DestructionListener) {
	DestructionListener listener;
	m_world->SetDestructionListener(&listener);
	m_world->DestroyParticleGroup(m_group);
	m_group = NULL;
	EXPECT_EQ(listener.m_count, 1);
}

int
main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
