/*
* Copyright (c) 2014 Google, Inc.
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
#include "AndroidUtil/AndroidMainWrapper.h"

class BodyContactTests : public ::testing::Test {
protected:
	virtual void SetUp();
	virtual void TearDown();
	void drop();

	b2World *m_world;
	b2Body *m_body;
	b2ParticleSystem *m_particleSystem;

	// total contacts per run of drop()
	int32 m_contacts;
	// total stuck per run of drop()
	int32 m_stuck;
};

void
BodyContactTests::SetUp()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);

	// Construct a world object, which will simulate the contacts.
	m_world = new b2World(gravity);

	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, 0.0f);

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	m_body = m_world->CreateBody(&groundBodyDef);

	// Construct a valley out of many fixtures and add to ground
	// body, as in the AntiPointy test in Testbed.
	float32 i;
	const float32 step = 1.0;

	for (i = -10.0; i < 10.0; i+=step)
	{
		b2PolygonShape shape;
		const b2Vec2 vertices[3] = {
			b2Vec2(i, -10.0),
			b2Vec2(i+step, -10.0),
			b2Vec2(0.0, 15.0)};
		shape.Set(vertices, 3);
		m_body->CreateFixture(&shape, 0.0f);
	}
	for (i = -10.0; i < 35.0; i+=step)
	{
		b2PolygonShape shape;
		const b2Vec2 vertices[3] = {
			b2Vec2(-10.0, i),
			b2Vec2(-10.0, i+step),
			b2Vec2(0.0, 15.0)};
		shape.Set(vertices, 3);
		m_body->CreateFixture(&shape, 0.0f);

		const b2Vec2 vertices2[3] = {
			b2Vec2(10.0, i),
			b2Vec2(10.0, i+step),
			b2Vec2(0.0, 15.0)};
		shape.Set(vertices2, 3);
		m_body->CreateFixture(&shape, 0.0f);
	}

	const b2ParticleSystemDef particleSystemDef;
	m_particleSystem = m_world->CreateParticleSystem(&particleSystemDef);
}
void
BodyContactTests::TearDown()
{
	// Reset these between tests so no one forgets to when we add more
	// tests to this file.
	m_particleSystem->SetStrictContactCheck(false);
	m_particleSystem->SetStuckThreshold(0);
}

void BodyContactTests::drop()
{
	int32 i;

	// reset counters
	m_contacts = 0;
	m_stuck = 0;

	int32 count = m_particleSystem->GetParticleCount();
	for (i = 0; i < count; ++i)
	{
		m_particleSystem->DestroyParticle(i);
	}

	const float32 timeStep = 1.0f / 60.0f;
	const int32 timeout = (int32) (1.0f / timeStep) * 10; // 10 "seconds"
	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;

	// step once to eliminate particles
	m_world->Step(timeStep, velocityIterations, positionIterations);

	b2ParticleDef pd;
	pd.position.Set(0.0, 33.0);
	pd.velocity.Set(0.0, -1.0);
	m_particleSystem->CreateParticle(pd);

	for (i = 0; i < timeout; ++i)
	{
		m_world->Step(timeStep, velocityIterations, positionIterations);
		m_contacts += m_particleSystem->GetBodyContactCount();
		int32 stuck = m_particleSystem->GetStuckCandidateCount();
		if (stuck)
		{
			m_stuck += stuck;
			// should always be particle 0
			EXPECT_EQ(*(m_particleSystem->GetStuckCandidates()), 0);
		}
	}
}

TEST_F(BodyContactTests, ParticleDrop) {

	m_particleSystem->SetStrictContactCheck(false);

	drop();
	EXPECT_GT(m_contacts, 0) << "No contacts within timeout";
	int32 contacts = m_contacts;
	m_particleSystem->SetStrictContactCheck(true);

	drop();
	EXPECT_GT(m_contacts, 0) << "No strict contacts within timeout";
	EXPECT_LT(m_contacts, contacts) << "No contact pruning performed";
}

TEST_F(BodyContactTests, Stuck) {
	drop();
	EXPECT_GT(m_contacts, 0) << "No contacts within timeout";
	EXPECT_EQ(m_stuck, 0) << "Stuck particle detected when detection disabled";
	int32 stuck = 0x7fffffff;

	for (int32 i = 1; i < 256; i *= 2)
	{
		m_particleSystem->SetStuckThreshold(i);
		drop();
		EXPECT_GT(m_stuck, 0) << "No stuck particles detected";
		EXPECT_GT(stuck, m_stuck) << "Fewer stuck particle reports expected";
		stuck = m_stuck;
	}
}

int
main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
