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
#include "Box2D/Box2D.h"
#include "gtest/gtest.h"

#include "AndroidUtil/AndroidMainWrapper.h"
#include "TestCommon.h"

class FunctionTests : public ::testing::Test {
protected:
	virtual void SetUp()
	{
		// Define the gravity vector.
		b2Vec2 gravity(0.0f, -10.0f);
		// Construct a world object, which will hold and simulate the rigid
		// bodies.
		m_world = new b2World(gravity);

		const b2ParticleSystemDef particleSystemDef;
		m_particleSystem = m_world->CreateParticleSystem(&particleSystemDef);
	}
	virtual void TearDown()
	{
		// Clean up the world after each test.
		m_world->DestroyParticleSystem(m_particleSystem);
		delete m_world;
	}

	b2World *m_world;
	b2ParticleSystem *m_particleSystem;
};

TEST_F(FunctionTests, CreateParticle) {
	b2ParticleDef def;
	def.flags = b2_elasticParticle | b2_springParticle;
	def.position.Set(1, 2);
	def.velocity.Set(3, 4);
	def.color.Set(1, 2, 3, 4);
	def.userData = this;
	int index = m_particleSystem->CreateParticle(def);
	EXPECT_EQ(index, 0);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 1);
	EXPECT_EQ(m_particleSystem->GetParticleFlagsBuffer()[index], def.flags);
	EXPECT_EQ(m_particleSystem->GetParticlePositionBuffer()[index], def.position);
	EXPECT_EQ(m_particleSystem->GetParticleVelocityBuffer()[index], def.velocity);
	EXPECT_EQ(m_particleSystem->GetParticleColorBuffer()[index].r, def.color.r);
	EXPECT_EQ(m_particleSystem->GetParticleColorBuffer()[index].g, def.color.g);
	EXPECT_EQ(m_particleSystem->GetParticleColorBuffer()[index].b, def.color.b);
	EXPECT_EQ(m_particleSystem->GetParticleColorBuffer()[index].a, def.color.a);
	EXPECT_EQ(m_particleSystem->GetParticleUserDataBuffer()[index], def.userData);
}

TEST_F(FunctionTests, CreateParticleInExistingGroup) {
	b2ParticleGroupDef groupDef;
	b2ParticleGroup *groupA = m_particleSystem->CreateParticleGroup(groupDef);
	b2ParticleGroup *groupB = m_particleSystem->CreateParticleGroup(groupDef);
	b2ParticleDef def;
	for (int i = 0; i < 20; i++)
	{
		if (i % 2)
		{
			def.position.Set(1, 0);
			def.group = groupA;
		}
		else
		{
			def.position.Set(2, 0);
			def.group = groupB;
		}
		m_particleSystem->CreateParticle(def);
	}
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 20);
	EXPECT_EQ(groupA->GetParticleCount(), 10);
	EXPECT_EQ(groupB->GetParticleCount(), 10);
	b2Vec2 *positionBuffer = m_particleSystem->GetParticlePositionBuffer();
	b2ParticleGroup *const *groupBuffer = m_particleSystem->GetParticleGroupBuffer();
	for (int i = 0; i < 10; i++)
	{
		int a = groupA->GetBufferIndex() + i;
		int b = groupB->GetBufferIndex() + i;
		EXPECT_EQ((int)positionBuffer[a].x, 1);
		EXPECT_EQ((int)positionBuffer[b].x, 2);
		EXPECT_EQ(groupBuffer[a], groupA);
		EXPECT_EQ(groupBuffer[b], groupB);
	}
}

TEST_F(FunctionTests, ParticleRadius) {
	float r = 12.3f;
	m_particleSystem->SetParticleRadius(r);
	EXPECT_EQ(m_particleSystem->GetParticleRadius(), r);
}

TEST_F(FunctionTests, ParticleDensity) {
	float r = 12.3f;
	m_particleSystem->SetParticleDensity(r);
	EXPECT_EQ(m_particleSystem->GetParticleDensity(), r);
}

TEST_F(FunctionTests, ParticleGravityScale) {
	float g = 12.3f;
	m_particleSystem->SetParticleGravityScale(g);
	EXPECT_EQ(m_particleSystem->GetParticleGravityScale(), g);
}

TEST_F(FunctionTests, ParticleDamping) {
	float r = 12.3f;
	m_particleSystem->SetParticleDamping(r);
	EXPECT_EQ(m_particleSystem->GetParticleDamping(), r);
}

TEST_F(FunctionTests, ParticleStaticPressureItearations) {
	int n = 123;
	m_particleSystem->SetParticleStaticPressureIterations(n);
	EXPECT_EQ(m_particleSystem->GetParticleStaticPressureIterations(), n);
}

// Verify that it's possible to destroy a particle using
// DestroyParticle(int32) and DestroyParticle(int32, bool).
TEST_F(FunctionTests, DestroyParticle) {
	b2ParticleDef def;
	int index = m_particleSystem->CreateParticle(def);
	m_particleSystem->DestroyParticle(index);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 0);

	CreateAndDestroyParticle(m_world, m_particleSystem, 0, true);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 0);

	CreateAndDestroyParticle(m_world, m_particleSystem, 0, false);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 0);
}

// Attempt and fail to destroy a particle with a shape that is outside the
// perimeter of the shape.
TEST_F(FunctionTests, DestroyParticlesInShapeNoneInShape) {
	b2PolygonShape shape;
	shape.SetAsBox(2, 2);
	b2Transform xf;
	xf.SetIdentity();

	b2ParticleDef def;
	def.position.x = 3;
	m_particleSystem->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	int destroyed = m_particleSystem->DestroyParticlesInShape(shape, xf);
	EXPECT_EQ(destroyed, 0);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 1);
}

// Destroy a particle within a shape using
// DestroyParticlesInShape(b2Shape&, b2Transform&) and
// DestroyParticlesInShape(b2Shape&, b2Transform&, bool).
TEST_F(FunctionTests, DestroyParticlesInShape) {
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	b2Transform xf;
	xf.SetIdentity();
	int32 destroyed;
	b2ParticleDef def;

	m_particleSystem->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	destroyed = m_particleSystem->DestroyParticlesInShape(shape, xf);
	EXPECT_EQ(destroyed, 1);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 1);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 0);

	m_particleSystem->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	destroyed = m_particleSystem->DestroyParticlesInShape(shape, xf, true);
	EXPECT_EQ(destroyed, 1);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 1);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 0);

	m_particleSystem->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	destroyed = m_particleSystem->DestroyParticlesInShape(shape, xf, false);
	EXPECT_EQ(destroyed, 1);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 1);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 0);
}

TEST_F(FunctionTests, CreateParticleGroup) {
	b2ParticleGroup *group = CreateBoxShapedParticleGroup(m_particleSystem);
	EXPECT_NE(group, (b2ParticleGroup *)NULL);
	EXPECT_EQ(m_particleSystem->GetParticleGroupCount(), 1);
	EXPECT_NE(m_particleSystem->GetParticleCount(), 0);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), group->GetParticleCount());
}

TEST_F(FunctionTests, CreateParticleGroupWithCustomStride) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	def.stride = 1 * m_particleSystem->GetParticleRadius();
	b2ParticleGroup *group1 = m_particleSystem->CreateParticleGroup(def);
	def.stride = 2 * m_particleSystem->GetParticleRadius();
	b2ParticleGroup *group2 = m_particleSystem->CreateParticleGroup(def);
	def.stride = 3 * m_particleSystem->GetParticleRadius();
	b2ParticleGroup *group3 = m_particleSystem->CreateParticleGroup(def);
	EXPECT_GT(group1->GetParticleCount(), group2->GetParticleCount());
	EXPECT_GT(group2->GetParticleCount(), group3->GetParticleCount());
}

TEST_F(FunctionTests, CreateEmptyParticleGroupWithNoShape) {
	b2ParticleGroupDef def;
	def.groupFlags = b2_solidParticleGroup | b2_particleGroupCanBeEmpty;
	b2ParticleGroup *group = m_particleSystem->CreateParticleGroup(def);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 0);
	EXPECT_EQ(group->GetParticleCount(), 0);
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_particleSystem->JoinParticleGroups(group, m_particleSystem->CreateParticleGroup(def));
	EXPECT_GT(m_particleSystem->GetParticleCount(), 0);
	EXPECT_GT(group->GetParticleCount(), 0);
	m_world->Step(0.01f, 1, 1, 1);
	m_particleSystem->DestroyParticlesInGroup(group, true);
	m_world->Step(0.01f, 1, 1, 1);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 0);
	EXPECT_EQ(group->GetParticleCount(), 0);
}

TEST_F(FunctionTests, CreateParticleGroupWithParticleCount) {
	static const int32 particleCount = 100;
	b2Vec2 positionData[particleCount];
	for (int32 i = 0; i < particleCount; i++)
	{
		positionData[i].Set((float32)i, (float32)i);
	}
	b2ParticleGroupDef def;
	def.particleCount = particleCount;
	def.positionData = positionData;
	b2ParticleGroup *group = m_particleSystem->CreateParticleGroup(def);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), particleCount);
	EXPECT_EQ(group->GetParticleCount(), particleCount);
	const b2Vec2 *positionBuffer = m_particleSystem->GetParticlePositionBuffer();
	for (int32 i = 0; i < particleCount; i++)
	{
		ASSERT_EQ(positionBuffer[i].x, (float32) i);
		ASSERT_EQ(positionBuffer[i].y, (float32) i);
	}
}

TEST_F(FunctionTests, CreateParticleGroupInExistingGroup) {
	b2ParticleGroupDef groupDef;
	b2ParticleGroup *groupA = m_particleSystem->CreateParticleGroup(groupDef);
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	groupDef.shape = &shape;
	groupDef.group = groupA;
	b2ParticleGroup *groupB = m_particleSystem->CreateParticleGroup(groupDef);
	groupDef.shape = NULL;
	b2ParticleGroup *groupC = m_particleSystem->CreateParticleGroup(groupDef);
	EXPECT_EQ(groupA, groupB);
	EXPECT_EQ(groupA, groupC);
	EXPECT_EQ(m_particleSystem->GetParticleGroupCount(), 1);
}

TEST_F(FunctionTests, DestroyParticleGroup) {
	b2ParticleGroup *group = CreateBoxShapedParticleGroup(m_particleSystem);
	m_particleSystem->DestroyParticlesInGroup(group);
	EXPECT_EQ(m_particleSystem->GetParticleGroupCount(), 1);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_particleSystem->GetParticleGroupCount(), 0);
	EXPECT_EQ(m_particleSystem->GetParticleCount(), 0);
}

TEST_F(FunctionTests, GetParticleBuffer) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	b2ParticleGroup *group = m_particleSystem->CreateParticleGroup(def);
	EXPECT_EQ(group->GetBufferIndex(), 0);
	const b2ParticleSystem *constParticleSystem = m_particleSystem;
	EXPECT_EQ(m_particleSystem->GetParticleFlagsBuffer(),
			  constParticleSystem->GetParticleFlagsBuffer());
	EXPECT_EQ(m_particleSystem->GetParticlePositionBuffer(),
			  constParticleSystem->GetParticlePositionBuffer());
	EXPECT_EQ(m_particleSystem->GetParticleVelocityBuffer(),
			  constParticleSystem->GetParticleVelocityBuffer());
	EXPECT_EQ(m_particleSystem->GetParticleGroupBuffer(),
			  constParticleSystem->GetParticleGroupBuffer());
	EXPECT_EQ(m_particleSystem->GetParticleColorBuffer(),
			  constParticleSystem->GetParticleColorBuffer());
	EXPECT_EQ(m_particleSystem->GetParticleUserDataBuffer(),
			  constParticleSystem->GetParticleUserDataBuffer());
	const b2ParticleGroup *constGroup = group;
	EXPECT_EQ(group->GetBufferIndex(), constGroup->GetBufferIndex());
}

TEST_F(FunctionTests, SetParticleBuffer) {
	static const int32 size = 256;
	uint32 flagsBuffer[size];
	b2Vec2 positionBuffer[size];
	b2Vec2 velocityBuffer[size];
	b2ParticleColor colorBuffer[size];
	void *userDataBuffer[size];
	m_particleSystem->SetParticleFlagsBuffer(flagsBuffer, size);
	m_particleSystem->SetParticlePositionBuffer(positionBuffer, size);
	m_particleSystem->SetParticleVelocityBuffer(velocityBuffer, size);
	m_particleSystem->SetParticleColorBuffer(colorBuffer, size);
	m_particleSystem->SetParticleUserDataBuffer(userDataBuffer, size);
	EXPECT_EQ(m_particleSystem->GetParticleFlagsBuffer(), flagsBuffer);
	EXPECT_EQ(m_particleSystem->GetParticlePositionBuffer(), positionBuffer);
	EXPECT_EQ(m_particleSystem->GetParticleVelocityBuffer(), velocityBuffer);
	EXPECT_EQ(m_particleSystem->GetParticleColorBuffer(), colorBuffer);
	EXPECT_EQ(m_particleSystem->GetParticleUserDataBuffer(), userDataBuffer);
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_particleSystem->CreateParticleGroup(def);
	EXPECT_LE(m_particleSystem->GetParticleCount(), size);
	EXPECT_EQ(m_particleSystem->GetParticleFlagsBuffer(), flagsBuffer);
	EXPECT_EQ(m_particleSystem->GetParticlePositionBuffer(), positionBuffer);
	EXPECT_EQ(m_particleSystem->GetParticleVelocityBuffer(), velocityBuffer);
	EXPECT_EQ(m_particleSystem->GetParticleColorBuffer(), colorBuffer);
	EXPECT_EQ(m_particleSystem->GetParticleUserDataBuffer(), userDataBuffer);
	uint32 newFlagsBuffer[size];
	b2Vec2 newPositionBuffer[size];
	b2Vec2 newVelocityBuffer[size];
	b2ParticleColor newColorBuffer[size];
	void *newUserDataBuffer[size];
	m_particleSystem->SetParticleFlagsBuffer(newFlagsBuffer, size);
	m_particleSystem->SetParticlePositionBuffer(newPositionBuffer, size);
	m_particleSystem->SetParticleVelocityBuffer(newVelocityBuffer, size);
	m_particleSystem->SetParticleColorBuffer(newColorBuffer, size);
	m_particleSystem->SetParticleUserDataBuffer(newUserDataBuffer, size);
	EXPECT_EQ(m_particleSystem->GetParticleFlagsBuffer(), newFlagsBuffer);
	EXPECT_EQ(m_particleSystem->GetParticlePositionBuffer(), newPositionBuffer);
	EXPECT_EQ(m_particleSystem->GetParticleVelocityBuffer(), newVelocityBuffer);
	EXPECT_EQ(m_particleSystem->GetParticleColorBuffer(), newColorBuffer);
	EXPECT_EQ(m_particleSystem->GetParticleUserDataBuffer(), newUserDataBuffer);
}

TEST_F(FunctionTests, GroupData) {
	b2ParticleGroupDef def;
	def.flags = b2_elasticParticle | b2_springParticle;
	def.position.Set(1, 2);
	def.angle = 3;
	def.linearVelocity.Set(4, 5);
	def.color.Set(1, 2, 3, 4);
	def.userData = this;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	b2ParticleGroup *group = m_particleSystem->CreateParticleGroup(def);
	EXPECT_NE(group->GetParticleCount(), 0);
	EXPECT_EQ(group->GetPosition(), def.position);
	EXPECT_EQ(group->GetAngle(), def.angle);
	EXPECT_LE(b2Distance(group->GetLinearVelocity(), def.linearVelocity),
			  EPSILON);
	EXPECT_EQ(group->GetUserData(), def.userData);
	group->SetUserData(NULL);
	EXPECT_EQ(group->GetUserData(), (void*)NULL);
}

TEST_F(FunctionTests, GroupList) {
	b2ParticleGroupDef def;
	b2ParticleGroup *group1 = m_particleSystem->CreateParticleGroup(def);
	b2ParticleGroup *group2 = m_particleSystem->CreateParticleGroup(def);
	b2ParticleGroup *group3 = m_particleSystem->CreateParticleGroup(def);
	b2ParticleGroup *list = m_particleSystem->GetParticleGroupList();
	EXPECT_EQ(list, group3);
	list = list->GetNext();
	EXPECT_EQ(list, group2);
	list = list->GetNext();
	EXPECT_EQ(list, group1);
	list = list->GetNext();
	EXPECT_EQ(list, (b2ParticleGroup *)0);
}

TEST_F(FunctionTests, ConstGroupList) {
	b2ParticleGroupDef def;
	b2ParticleGroup *group1 = m_particleSystem->CreateParticleGroup(def);
	b2ParticleGroup *group2 = m_particleSystem->CreateParticleGroup(def);
	b2ParticleGroup *group3 = m_particleSystem->CreateParticleGroup(def);
	const b2ParticleGroup *list =
		((const b2ParticleSystem *)m_particleSystem)->GetParticleGroupList();
	EXPECT_EQ(list, group3);
	list = list->GetNext();
	EXPECT_EQ(list, group2);
	list = list->GetNext();
	EXPECT_EQ(list, group1);
	list = list->GetNext();
	EXPECT_EQ(list, (const b2ParticleGroup *)NULL);
}

TEST_F(FunctionTests, JoinParticleGroups) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	b2ParticleGroup *group1 = m_particleSystem->CreateParticleGroup(def);
	shape.SetAsBox(10, 20);
	b2ParticleGroup *group2 = m_particleSystem->CreateParticleGroup(def);
	shape.SetAsBox(10, 30);
	b2ParticleGroup *group3 = m_particleSystem->CreateParticleGroup(def);
	int32 count1 = group1->GetParticleCount();
	int32 count2 = group2->GetParticleCount();
	int32 count3 = group3->GetParticleCount();
	EXPECT_EQ(count1 + count2 + count3, m_particleSystem->GetParticleCount());
	EXPECT_EQ(m_particleSystem->GetParticleGroupCount(), 3);
	m_particleSystem->JoinParticleGroups(group1, group2);
	EXPECT_EQ(m_particleSystem->GetParticleGroupCount(), 2);
	EXPECT_EQ(count1 + count2, group1->GetParticleCount());
}

TEST_F(FunctionTests, GroupBuffer) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	b2ParticleGroup *group1 = m_particleSystem->CreateParticleGroup(def);
	shape.SetAsBox(10, 20);
	b2ParticleGroup *group2 = m_particleSystem->CreateParticleGroup(def);
	shape.SetAsBox(10, 30);
	b2ParticleGroup *group3 = m_particleSystem->CreateParticleGroup(def);
	const b2ParticleGroup *const *groupBuffer =
		m_particleSystem->GetParticleGroupBuffer();
	int32 offset1 = group1->GetBufferIndex();
	int32 count1 = group1->GetParticleCount();
	for (int32 i = 0; i < count1; i++) {
		ASSERT_EQ(group1, groupBuffer[offset1 + i]);
	}
	int32 offset2 = group2->GetBufferIndex();
	int32 count2 = group2->GetParticleCount();
	for (int32 i = 0; i < count2; i++) {
		ASSERT_EQ(group2, groupBuffer[offset2 + i]);
	}
	int32 offset3 = group3->GetBufferIndex();
	int32 count3 = group3->GetParticleCount();
	for (int32 i = 0; i < count3; i++) {
		ASSERT_EQ(group3, groupBuffer[offset3 + i]);
	}
	m_particleSystem->JoinParticleGroups(group1, group2);
	m_particleSystem->DestroyParticlesInGroup(group3);
	m_world->Step(0.001f, 1, 1);
	groupBuffer = m_particleSystem->GetParticleGroupBuffer();
	int32 count = m_particleSystem->GetParticleCount();
	for (int32 i = 0; i < count; i++) {
		ASSERT_EQ(group1, groupBuffer[i]);
	}
}

TEST_F(FunctionTests, GetParticleContact) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_particleSystem->CreateParticleGroup(def);
	EXPECT_NE(m_particleSystem->GetParticleContactCount(), 0);
	EXPECT_NE(m_particleSystem->GetParticleContacts(), (const b2ParticleContact *)NULL);
}

TEST_F(FunctionTests, GetParticleBodyContact) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_particleSystem->CreateParticleGroup(def);
	b2BodyDef bodyDef;
	b2Body *body = m_world->CreateBody(&bodyDef);
	body->CreateFixture(&shape, 1.0);
	m_world->Step(0.001f, 1, 1);
	EXPECT_NE(m_particleSystem->GetParticleBodyContactCount(), 0);
	EXPECT_NE(m_particleSystem->GetParticleBodyContacts(),
			  (const b2ParticleBodyContact *)NULL);
}

TEST_F(FunctionTests, ComputeParticleCollisionEnergy) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_particleSystem->CreateParticleGroup(def);
	EXPECT_EQ(m_particleSystem->ComputeParticleCollisionEnergy(), 0);

	def.position.Set(20, 0);
	def.linearVelocity.Set(-1, 0);
	m_particleSystem->CreateParticleGroup(def);
	for (int32 t = 0; t < 1000; t++)
	{
		m_world->Step(0.1f, 1, 1);
	}
	EXPECT_NE(m_particleSystem->ComputeParticleCollisionEnergy(), 0);
}

TEST_F(FunctionTests, PauseSimulation) {
	EXPECT_FALSE(m_particleSystem->IsSimulationPaused());

	b2ParticleDef def;
	def.flags = b2_elasticParticle | b2_springParticle;
	def.position.Set(1, 2);
	def.velocity.Set(3, 4);
	def.color.Set(1, 2, 3, 4);
	def.userData = this;
	m_particleSystem->CreateParticle(def);

	// Step the simulation to ensure the particle is moving.
	const b2Vec2* positions = m_particleSystem->GetParticlePositionBuffer();
	const b2Vec2 initialPosition = positions[0];
	m_world->Step(0.1f, 1, 1);
	const b2Vec2 steppedPosition = positions[0];
	EXPECT_NE(initialPosition, steppedPosition);

	// Pause the particle system, then step simulation again.
	// Ensure the particle hasn't moved.
	m_particleSystem->PauseSimulation(true);
	EXPECT_TRUE(m_particleSystem->IsSimulationPaused());
	m_world->Step(0.1f, 1, 1);
	EXPECT_EQ(steppedPosition, positions[0]);

	// Unpause the particle system. Ensure the particle is moving again.
	m_particleSystem->PauseSimulation(false);
	EXPECT_FALSE(m_particleSystem->IsSimulationPaused());
	m_world->Step(0.1f, 1, 1);
	EXPECT_NE(steppedPosition, positions[0]);
}

// Verify that it's possible to retrieve a handle from a particle index.
TEST_F(FunctionTests, GetParticleHandleFromIndex)
{
	b2ParticleSystem *system = m_world->GetParticleSystemList();
	const b2ParticleDef particleDef;
	const int32 particleIndex = system->CreateParticle(particleDef);
	ASSERT_NE(particleIndex, b2_invalidParticleIndex);
	const b2ParticleHandle* particleHandle =
		system->GetParticleHandleFromIndex(particleIndex);
	EXPECT_EQ(particleIndex, particleHandle->GetIndex());
}

// Ensure particle handles track particles correctly as the array of particles
// is compacted when particles are destroyed.
TEST_F(FunctionTests, ParticleHandleTrackCompactParticles)
{
	static const int32 kNumberOfHandles = 1024;
	b2TrackedBlockAllocator allocator;
	const b2ParticleHandle** const handles =
		(const b2ParticleHandle**)allocator.Allocate(
			kNumberOfHandles * sizeof(*handles));
	int32* const expectedUserData = (int32*)allocator.Allocate(
			kNumberOfHandles * sizeof(*expectedUserData));
	b2ParticleSystem *system = m_world->GetParticleSystemList();

	// Create particles and store a handle to each particle.  Also, associate
	// each particle with user data that contains the index into the array of
	// handles.
	b2ParticleDef particleDef;
	for (int32 i = 0; i < kNumberOfHandles; ++i)
	{
		handles[i] = system->GetParticleHandleFromIndex(
			system->CreateParticle(particleDef));
		EXPECT_TRUE(handles[i] != NULL);
		const int32 particleIndex = handles[i]->GetIndex();
		EXPECT_NE(particleIndex, b2_invalidParticleIndex);
		// NOTE: The user data buffer is retrieved each time since it's
		// possible for the particle system to reallocate it when particles are
		// created.
		system->GetParticleUserDataBuffer()[particleIndex] =
			&expectedUserData[i];
		expectedUserData[i] = i;
	}

	// Update the particle system state.
	m_world->Step(0.001f, 1, 1);

	// Verify that it's possible to retrieve each particle's user data from a
	// handle.
	for (int32 i = 0; i < kNumberOfHandles; ++i)
	{
		const int32 particleIndex = handles[i]->GetIndex();
		EXPECT_EQ(*(((int32**)system->GetParticleUserDataBuffer())[
						particleIndex]), i);
	}

	// Mark half of the particles for deletion and clear the user data
	// associated with each of the handles that is marked for deletion.
	for (int32 i = 0; i < kNumberOfHandles; i += 2)
	{
		const int32 particleIndex = handles[i]->GetIndex();
		system->DestroyParticle(particleIndex, false);
		expectedUserData[i] = -1;
	}

	// Delete the particles.
	// Since every other particle has been deleted this will result in
	// the compaction of particle buffers changing the index of each particle.
	m_world->Step(0.001f, 1, 1);

	// Verify user data referenced by remaining particles is correct.
	for (int32 i = 1; i < kNumberOfHandles; i += 2)
	{
		const int32 particleIndex = handles[i]->GetIndex();
		EXPECT_EQ(*(((int32**)system->GetParticleUserDataBuffer())[
						particleIndex]), i);
	}
}

// Ensure particle handles track particles correctly as the array of particles
// is compacted when particles are destroyed.
TEST_F(FunctionTests, ParticleHandlesTrackGroups)
{
	b2TrackedBlockAllocator allocator;
	b2ParticleSystem *system = m_world->GetParticleSystemList();
	// Create particle groups, join them, check data after resorting.
	b2ParticleGroupDef groupDef;
	b2PolygonShape box;
	box.SetAsBox(10, 10);
	groupDef.shape = &box;
	b2ParticleGroup *groupA = system->CreateParticleGroup(groupDef);
	// Create a particle groupB next to groupA.
	groupDef.position = b2Vec2(10.0f, 0.0f);
	b2ParticleGroup *groupB = system->CreateParticleGroup(groupDef);

	const int32 groupAParticleCount = groupA->GetParticleCount();
	const int32 numberOfGroupParticles = groupAParticleCount +
		groupB->GetParticleCount();
	const b2ParticleHandle **particleHandles =
		(const b2ParticleHandle**)allocator.Allocate(
			numberOfGroupParticles * sizeof(*particleHandles));
	int32* const expectedGroupData = (int32*)allocator.Allocate(
			numberOfGroupParticles * sizeof(*expectedGroupData));

	// Get a handle for each particle in both groups and assign expected
	// user data to each particle.
	for (int32 i = 0; i < numberOfGroupParticles; ++i)
	{
		b2ParticleGroup* group;
		int32 particleIndex;
		if (i < groupAParticleCount)
		{
			group = groupA;
			particleIndex = i;
		}
		else
		{
			group = groupB;
			particleIndex = i - groupAParticleCount;
		}
		particleIndex += group->GetBufferIndex();

		particleHandles[i] = system->GetParticleHandleFromIndex(
			particleIndex);
		expectedGroupData[i] = i;
		system->GetParticleUserDataBuffer()[particleIndex] =
			&expectedGroupData[i];
	}

	m_world->Step(0.001f, 1, 1);

	// Join the particle groups.
	system->JoinParticleGroups(groupA, groupB);
	groupB = NULL;

	m_world->Step(0.001f, 1, 1);

	// Verify the handles still point at valid data by checking the value
	// associated with each particle's user data.
	ASSERT_EQ(numberOfGroupParticles, groupA->GetParticleCount());
	for (int32 i = 0; i < numberOfGroupParticles; ++i)
	{
		EXPECT_EQ(expectedGroupData[i],
				  *((int32*)system->GetParticleUserDataBuffer()[
						particleHandles[i]->GetIndex()]));
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
