#include "gtest/gtest.h"
#include "Box2D/Box2D.h"
#include <stdio.h>
#include "BodyTracker.h"
#define EPSILON 0.001f
#define DELTA_T 0.01f
#define NUMBER_OF_STEPS 200


class ConservationTests : public ::testing::Test {
    protected:
	virtual void SetUp();
	virtual void TearDown();

	b2World *m_world;
};

void
ConservationTests::SetUp()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, 0.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	m_world = new b2World(gravity);

	// Create two particle groups colliding each other
	b2ParticleGroupDef pd;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	pd.shape = &shape;

	pd.position = b2Vec2(-10, 0);
	pd.linearVelocity = b2Vec2(1, 0);
	m_world->CreateParticleGroup(pd);

	pd.position = b2Vec2(10, 0);
	pd.linearVelocity = b2Vec2(-1, 0);
	m_world->CreateParticleGroup(pd);
}

void
ConservationTests::TearDown()
{
	// Intentionally blank.
}

TEST_F(ConservationTests, GravityCenter) {
	int32 particleCount = m_world->GetParticleCount();
	const b2Vec2 *positionBuffer = m_world->GetParticlePositionBuffer();
	b2Vec2 before = b2Vec2_zero;
	for (int32 i = 0; i < particleCount; i++) {
		before += positionBuffer[i];
	}
	before *= 1 / particleCount;
	for (int32 t = 0; t < NUMBER_OF_STEPS; t++) {
		m_world->Step(DELTA_T, 1, 1);
	}
	b2Vec2 after = b2Vec2_zero;
	for (int32 i = 0; i < particleCount; i++) {
		after += positionBuffer[i];
	}
	after *= 1 / particleCount;
	EXPECT_TRUE(b2Distance(before, after) < EPSILON) <<
		"the gravity center changed from (" << before.x << "," << before.y << ") to (" << after.x << "," << after.y << ")";
}

TEST_F(ConservationTests, LinearMomentum) {
	int32 particleCount = m_world->GetParticleCount();
	const b2Vec2 *velocityBuffer = m_world->GetParticleVelocityBuffer();
	b2Vec2 before = b2Vec2_zero;
	for (int32 i = 0; i < particleCount; i++) {
		before += velocityBuffer[i];
	}
	for (int32 t = 0; t < NUMBER_OF_STEPS; t++) {
		m_world->Step(DELTA_T, 1, 1);
	}
	b2Vec2 after = b2Vec2_zero;
	for (int32 i = 0; i < particleCount; i++) {
		after += velocityBuffer[i];
	}
	EXPECT_TRUE(b2Distance(before, after) < EPSILON) <<
		"the linear momentum changed from (" << before.x << "," << before.y << ") to (" << after.x << "," << after.y << ")";
}

TEST_F(ConservationTests, AngularMomentum) {
	int32 particleCount = m_world->GetParticleCount();
	const b2Vec2 *positionBuffer = m_world->GetParticlePositionBuffer();
	const b2Vec2 *velocityBuffer = m_world->GetParticleVelocityBuffer();
	float32 before = 0;
	for (int32 i = 0; i < particleCount; i++) {
		before += b2Cross(positionBuffer[i], velocityBuffer[i]);
	}
	for (int32 t = 0; t < NUMBER_OF_STEPS; t++) {
		m_world->Step(DELTA_T, 1, 1);
	}
	float32 after = 0;
	for (int32 i = 0; i < particleCount; i++) {
		after += b2Cross(positionBuffer[i], velocityBuffer[i]);
	}
	EXPECT_TRUE(std::abs(before - after) < EPSILON) <<
		"the angular momentum changed from " << before << " to " << after;
}

TEST_F(ConservationTests, KineticEnergy) {
	int32 particleCount = m_world->GetParticleCount();
	const b2Vec2 *velocityBuffer = m_world->GetParticleVelocityBuffer();
	float32 before = 0;
	for (int32 i = 0; i < particleCount; i++) {
		b2Vec2 v = velocityBuffer[i];
		before += b2Dot(v, v) / 2;
	}
	for (int32 t = 0; t < NUMBER_OF_STEPS; t++) {
		m_world->Step(DELTA_T, 1, 1);
	}
	float32 after = 0;
	for (int32 i = 0; i < particleCount; i++) {
		b2Vec2 v = velocityBuffer[i];
		after += b2Dot(v, v) / 2;
	}
	ASSERT_GE(before, after) <<
		"the kinetic energy increased from " << before << " to " << after;
}

int
main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
