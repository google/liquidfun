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
#include "gtest/gtest.h"
#include "Box2D/Box2D.h"
#include <stdio.h>
#include <complex>
#include "BodyTracker.h"
#include "AndroidUtil/AndroidMainWrapper.h"
#define EPSILON 0.001f
#define DELTA_T 0.01f
#define NUMBER_OF_STEPS 200


class ConservationTests : public ::testing::Test {
    protected:
	virtual void SetUp();
	virtual void TearDown();

	b2World *m_world;
	b2ParticleSystem *m_particleSystem;
};

void
ConservationTests::SetUp()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, 0.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	m_world = new b2World(gravity);

	const b2ParticleSystemDef particleSystemDef;
	m_particleSystem = m_world->CreateParticleSystem(&particleSystemDef);

	// Create two particle groups colliding each other
	b2ParticleGroupDef pd;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	pd.shape = &shape;

	pd.position = b2Vec2(-10, 0);
	pd.linearVelocity = b2Vec2(1, 0);
	m_particleSystem->CreateParticleGroup(pd);

	pd.position = b2Vec2(10, 0);
	pd.linearVelocity = b2Vec2(-1, 0);
	m_particleSystem->CreateParticleGroup(pd);
}

void
ConservationTests::TearDown()
{
	// Intentionally blank.
}

TEST_F(ConservationTests, GravityCenter) {
	int32 particleCount = m_particleSystem->GetParticleCount();
	const b2Vec2 *positionBuffer = m_particleSystem->GetPositionBuffer();
	float64 beforeX = 0;
	float64 beforeY = 0;
	for (int32 i = 0; i < particleCount; i++) {
		beforeX += positionBuffer[i].x;
		beforeY += positionBuffer[i].y;
	}
	beforeX /= particleCount;
	beforeY /= particleCount;
	for (int32 t = 0; t < NUMBER_OF_STEPS; t++) {
		m_world->Step(DELTA_T, 1, 1);
	}
	float64 afterX = 0;
	float64 afterY = 0;
	for (int32 i = 0; i < particleCount; i++) {
		afterX += positionBuffer[i].x;
		afterY += positionBuffer[i].y;
	}
	afterX /= particleCount;
	afterY /= particleCount;
	EXPECT_TRUE(std::abs(beforeX - afterX) < EPSILON &&
				std::abs(beforeY - afterY) < EPSILON) <<
		"the gravity center changed from (" << beforeX << "," <<
		beforeY << ") to (" << afterX << "," << afterY << ")";
}

TEST_F(ConservationTests, LinearMomentum) {
	int32 particleCount = m_particleSystem->GetParticleCount();
	const b2Vec2 *velocityBuffer = m_particleSystem->GetVelocityBuffer();
	float64 beforeX = 0;
	float64 beforeY = 0;
	for (int32 i = 0; i < particleCount; i++) {
		beforeX += velocityBuffer[i].x;
		beforeY += velocityBuffer[i].y;
	}
	for (int32 t = 0; t < NUMBER_OF_STEPS; t++) {
		m_world->Step(DELTA_T, 1, 1);
	}
	float64 afterX = 0;
	float64 afterY = 0;
	for (int32 i = 0; i < particleCount; i++) {
		afterX += velocityBuffer[i].x;
		afterY += velocityBuffer[i].y;
	}
	EXPECT_TRUE(std::abs(beforeX - afterX) < EPSILON &&
				std::abs(beforeY - afterY) < EPSILON) <<
		"the linear momentum changed from (" << beforeX << "," <<
		beforeY << ") to (" << afterX << "," << afterY << ")";
}

TEST_F(ConservationTests, AngularMomentum) {
	int32 particleCount = m_particleSystem->GetParticleCount();
	const b2Vec2 *positionBuffer = m_particleSystem->GetPositionBuffer();
	const b2Vec2 *velocityBuffer = m_particleSystem->GetVelocityBuffer();
	float64 before = 0;
	for (int32 i = 0; i < particleCount; i++) {
		before += b2Cross(positionBuffer[i], velocityBuffer[i]);
	}
	for (int32 t = 0; t < NUMBER_OF_STEPS; t++) {
		m_world->Step(DELTA_T, 1, 1);
	}
	float64 after = 0;
	for (int32 i = 0; i < particleCount; i++) {
		after += b2Cross(positionBuffer[i], velocityBuffer[i]);
	}
	EXPECT_TRUE(std::abs(before - after) < EPSILON) <<
		"the angular momentum changed from " << before << " to " << after;
}

TEST_F(ConservationTests, KineticEnergy) {
	int32 particleCount = m_particleSystem->GetParticleCount();
	const b2Vec2 *velocityBuffer = m_particleSystem->GetVelocityBuffer();
	float64 before = 0;
	for (int32 i = 0; i < particleCount; i++) {
		b2Vec2 v = velocityBuffer[i];
		before += b2Dot(v, v) / 2;
	}
	for (int32 t = 0; t < NUMBER_OF_STEPS; t++) {
		m_world->Step(DELTA_T, 1, 1);
	}
	float64 after = 0;
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
