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
	int32 drop();

	b2World *m_world;
	b2Body *m_body;
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

	// Construct a triangle out of many fixtures and add to ground
	// body, as in the Pointy test in Testbed.
	const float32 xstep = 1.0f;
	for (float32 x = -10.0f; x < 10.0f; x += xstep)
	{
		b2PolygonShape shape;
		const b2Vec2 vertices[3] = {
			b2Vec2(x, -10.0f),
			b2Vec2(x+xstep, -10.0f),
			b2Vec2(0.0f, 25.0f)};
		shape.Set(vertices, 3);
		m_body->CreateFixture(&shape, 0.0f);
	}

	m_world->SetParticleRadius(1.0f);
}
void
BodyContactTests::TearDown()
{
	// Reset this between tests so no one forgets to when we add more
	// tests to this file.
	m_world->SetStrictParticleContactCheck(false);
}

int32 BodyContactTests::drop()
{
	const float32 timeStep = 1.0f / 60.0f;
	const int32 timeout = (int32) (1.0f / timeStep) * 10; // 10 seconds
	const int32 velocityIterations = 6;
	const int32 positionIterations = 2;

	b2ParticleDef pd;
	pd.position.Set(0.0, 33.0);
	pd.velocity.Set(0.0, -1.0);
	m_world->CreateParticle(pd);

	int32 contacts;
	for (int32 i = 0; i < timeout; ++i)
	{
		// Instruct the world to perform a single step of simulation.
		// It is generally best to keep the time step and iterations fixed.
		m_world->Step(timeStep, velocityIterations, positionIterations);
		contacts = m_world->GetParticleBodyContactCount();
		if(contacts > 0)
			return contacts;
	}
	return -1;
}

TEST_F(BodyContactTests, ParticleDrop) {

	m_world->SetStrictParticleContactCheck(false);

	int32 contacts = drop();
	EXPECT_GT(contacts, 0) << "No contacts within timeout";

	m_world->SetStrictParticleContactCheck(true);

	int32 strictContacts = drop();
	EXPECT_GT(strictContacts, 0) << "No strict contacts within timeout";
	EXPECT_LT(strictContacts, contacts) << "No contact pruning performed";
}

int
main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
