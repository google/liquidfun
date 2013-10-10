#include "gtest/gtest.h"
#include "Box2D/Box2D.h"
#include <stdio.h>
#include "BodyTracker.h"
#define EPSILON 0.0001

class HelloWorldTests : public ::testing::Test {
    protected:
	virtual void SetUp();
	virtual void TearDown();

	b2World *m_world;
	b2Body *m_groundBody;
	b2Body *m_body;
};

void
HelloWorldTests::SetUp()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	m_world = new b2World(gravity);

	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	m_groundBody = m_world->CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2PolygonShape groundBox;

	// The extents are the half-widths of the box.
	groundBox.SetAsBox(50.0f, 10.0f);

	// Add the ground fixture to the ground body.
	m_groundBody->CreateFixture(&groundBox, 0.0f);

	// Define the dynamic body. We set its position and call the body factory.
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);
	m_body = m_world->CreateBody(&bodyDef);

	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;

	// Set the box density to be non-zero, so it will be dynamic.
	fixtureDef.density = 1.0f;

	// Override the default friction.
	fixtureDef.friction = 0.3f;

	// Add the shape to the body.
	m_body->CreateFixture(&fixtureDef);
}
void
HelloWorldTests::TearDown()
{
	// Intentionally blank.
}

TEST_F(HelloWorldTests, Allocation) {
	EXPECT_TRUE(m_world != NULL)       << "Failed allocation of b2World";
	EXPECT_TRUE(m_groundBody  != NULL) << "Failed allocation of static b2Body";
	EXPECT_TRUE(m_body !=  NULL)       << "Failed allocation of dynamic b2Body";
}

TEST_F(HelloWorldTests, PositionAngleTest) {
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;
	BodyTracker tracker("baselines/HelloWorldPositionAngle.txt",
			"testOutput/HelloWorldPositionAngle.txt",
			BodyTracker::TRACK_POSITION | BodyTracker::TRACK_ANGLE);

	tracker.TrackBody(m_body, "DynamicBody");

	// Check that the tracker engaged properly and call the test failed if it doesn't.
	EXPECT_TRUE(tracker.BeginTracking()) << "Problems setting up BodyTracker";

	for (int32 i = 0; i < 60; ++i)
	{
		// Instruct the world to perform a single step of simulation.
		// It is generally best to keep the time step and iterations fixed.
		m_world->Step(timeStep, velocityIterations, positionIterations);

		// Now print the position and angle of the body.
		b2Vec2 position = m_body->GetPosition();
		float32 angle = m_body->GetAngle();
		tracker.TrackStep(m_body, i);
	}
	tracker.EndTracking();
	bool matched = tracker.CompareToBaseline(m_body,
			BodyTracker::TRACK_POSITION | BodyTracker::TRACK_ANGLE, EPSILON);
	std::string errString;
	if (!matched) {
		const std::vector<std::string> &errors = tracker.GetErrors();
		for (int32 i = 0 ; i < errors.size() ; i++ )
		errString += "\t" + errors[i] + "\n";
	}
	EXPECT_TRUE(matched) << errString;
}

int
main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
