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
	}
	virtual void TearDown()
	{
		delete m_world;
	}

	b2World *m_world;
};

TEST_F(FunctionTests, CreateParticle) {
	b2ParticleDef def;
	def.flags = b2_elasticParticle | b2_springParticle;
	def.position.Set(1, 2);
	def.velocity.Set(3, 4);
	def.color.Set(1, 2, 3, 4);
	def.userData = this;
	int index = m_world->CreateParticle(def);
	EXPECT_EQ(index, 0);
	EXPECT_EQ(m_world->GetParticleCount(), 1);
	EXPECT_EQ(m_world->GetParticleFlagsBuffer()[index], def.flags);
	EXPECT_EQ(m_world->GetParticlePositionBuffer()[index], def.position);
	EXPECT_EQ(m_world->GetParticleVelocityBuffer()[index], def.velocity);
	EXPECT_EQ(m_world->GetParticleColorBuffer()[index].r, def.color.r);
	EXPECT_EQ(m_world->GetParticleColorBuffer()[index].g, def.color.g);
	EXPECT_EQ(m_world->GetParticleColorBuffer()[index].b, def.color.b);
	EXPECT_EQ(m_world->GetParticleColorBuffer()[index].a, def.color.a);
	EXPECT_EQ(m_world->GetParticleUserDataBuffer()[index], def.userData);
}

TEST_F(FunctionTests, ParticleRadius) {
	float r = 12.3f;
	m_world->SetParticleRadius(r);
	EXPECT_EQ(m_world->GetParticleRadius(), r);
}

TEST_F(FunctionTests, ParticleDensity) {
	float r = 12.3f;
	m_world->SetParticleDensity(r);
	EXPECT_EQ(m_world->GetParticleDensity(), r);
}

TEST_F(FunctionTests, ParticleGravityScale) {
	float g = 12.3f;
	m_world->SetParticleGravityScale(g);
	EXPECT_EQ(m_world->GetParticleGravityScale(), g);
}

TEST_F(FunctionTests, ParticleDamping) {
	float r = 12.3f;
	m_world->SetParticleDamping(r);
	EXPECT_EQ(m_world->GetParticleDamping(), r);
}

// Verify that it's possible to destroy a particle using
// DestroyParticle(int32) and DestroyParticle(int32, bool).
TEST_F(FunctionTests, DestroyParticle) {
	b2ParticleDef def;
	int index = m_world->CreateParticle(def);
	m_world->DestroyParticle(index);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_world->GetParticleCount(), 0);

	CreateAndDestroyParticle(m_world, 0, true);
	EXPECT_EQ(m_world->GetParticleCount(), 0);

	CreateAndDestroyParticle(m_world, 0, false);
	EXPECT_EQ(m_world->GetParticleCount(), 0);
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
	m_world->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	int destroyed = m_world->DestroyParticlesInShape(shape, xf);
	EXPECT_EQ(destroyed, 0);
	EXPECT_EQ(m_world->GetParticleCount(), 1);
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

	m_world->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	destroyed = m_world->DestroyParticlesInShape(shape, xf);
	EXPECT_EQ(destroyed, 1);
	EXPECT_EQ(m_world->GetParticleCount(), 1);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_world->GetParticleCount(), 0);

	m_world->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	destroyed = m_world->DestroyParticlesInShape(shape, xf, true);
	EXPECT_EQ(destroyed, 1);
	EXPECT_EQ(m_world->GetParticleCount(), 1);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_world->GetParticleCount(), 0);

	m_world->CreateParticle(def);
	m_world->Step(0.001f, 1, 1);
	destroyed = m_world->DestroyParticlesInShape(shape, xf, false);
	EXPECT_EQ(destroyed, 1);
	EXPECT_EQ(m_world->GetParticleCount(), 1);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_world->GetParticleCount(), 0);
}

TEST_F(FunctionTests, CreateParticleGroup) {
	b2ParticleGroup *group = CreateBoxShapedParticleGroup(m_world);
	EXPECT_NE(group, (b2ParticleGroup *)NULL);
	EXPECT_EQ(m_world->GetParticleGroupCount(), 1);
	EXPECT_NE(m_world->GetParticleCount(), 0);
	EXPECT_EQ(m_world->GetParticleCount(), group->GetParticleCount());
}

TEST_F(FunctionTests, DestroyParticleGroup) {
	b2ParticleGroup *group = CreateBoxShapedParticleGroup(m_world);
	m_world->DestroyParticlesInGroup(group);
	EXPECT_EQ(m_world->GetParticleGroupCount(), 1);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_world->GetParticleGroupCount(), 0);
	EXPECT_EQ(m_world->GetParticleCount(), 0);
}

TEST_F(FunctionTests, GetParticleBuffer) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	b2ParticleGroup *group = m_world->CreateParticleGroup(def);
	EXPECT_EQ(group->GetBufferIndex(), 0);
	const b2World *constWorld = m_world;
	EXPECT_EQ(m_world->GetParticleFlagsBuffer(),
			  constWorld->GetParticleFlagsBuffer());
	EXPECT_EQ(m_world->GetParticlePositionBuffer(),
			  constWorld->GetParticlePositionBuffer());
	EXPECT_EQ(m_world->GetParticleVelocityBuffer(),
			  constWorld->GetParticleVelocityBuffer());
	EXPECT_EQ(m_world->GetParticleGroupBuffer(),
			  constWorld->GetParticleGroupBuffer());
	EXPECT_EQ(m_world->GetParticleColorBuffer(),
			  constWorld->GetParticleColorBuffer());
	EXPECT_EQ(m_world->GetParticleUserDataBuffer(),
			  constWorld->GetParticleUserDataBuffer());
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
	m_world->SetParticleFlagsBuffer(flagsBuffer, size);
	m_world->SetParticlePositionBuffer(positionBuffer, size);
	m_world->SetParticleVelocityBuffer(velocityBuffer, size);
	m_world->SetParticleColorBuffer(colorBuffer, size);
	m_world->SetParticleUserDataBuffer(userDataBuffer, size);
	EXPECT_EQ(m_world->GetParticleFlagsBuffer(), flagsBuffer);
	EXPECT_EQ(m_world->GetParticlePositionBuffer(), positionBuffer);
	EXPECT_EQ(m_world->GetParticleVelocityBuffer(), velocityBuffer);
	EXPECT_EQ(m_world->GetParticleColorBuffer(), colorBuffer);
	EXPECT_EQ(m_world->GetParticleUserDataBuffer(), userDataBuffer);
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_world->CreateParticleGroup(def);
	EXPECT_LE(m_world->GetParticleCount(), size);
	EXPECT_EQ(m_world->GetParticleFlagsBuffer(), flagsBuffer);
	EXPECT_EQ(m_world->GetParticlePositionBuffer(), positionBuffer);
	EXPECT_EQ(m_world->GetParticleVelocityBuffer(), velocityBuffer);
	EXPECT_EQ(m_world->GetParticleColorBuffer(), colorBuffer);
	EXPECT_EQ(m_world->GetParticleUserDataBuffer(), userDataBuffer);
	uint32 newFlagsBuffer[size];
	b2Vec2 newPositionBuffer[size];
	b2Vec2 newVelocityBuffer[size];
	b2ParticleColor newColorBuffer[size];
	void *newUserDataBuffer[size];
	m_world->SetParticleFlagsBuffer(newFlagsBuffer, size);
	m_world->SetParticlePositionBuffer(newPositionBuffer, size);
	m_world->SetParticleVelocityBuffer(newVelocityBuffer, size);
	m_world->SetParticleColorBuffer(newColorBuffer, size);
	m_world->SetParticleUserDataBuffer(newUserDataBuffer, size);
	EXPECT_EQ(m_world->GetParticleFlagsBuffer(), newFlagsBuffer);
	EXPECT_EQ(m_world->GetParticlePositionBuffer(), newPositionBuffer);
	EXPECT_EQ(m_world->GetParticleVelocityBuffer(), newVelocityBuffer);
	EXPECT_EQ(m_world->GetParticleColorBuffer(), newColorBuffer);
	EXPECT_EQ(m_world->GetParticleUserDataBuffer(), newUserDataBuffer);
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
	b2ParticleGroup *group = m_world->CreateParticleGroup(def);
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
	b2ParticleGroup *group1 = m_world->CreateParticleGroup(def);
	b2ParticleGroup *group2 = m_world->CreateParticleGroup(def);
	b2ParticleGroup *group3 = m_world->CreateParticleGroup(def);
	b2ParticleGroup *list = m_world->GetParticleGroupList();
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
	b2ParticleGroup *group1 = m_world->CreateParticleGroup(def);
	b2ParticleGroup *group2 = m_world->CreateParticleGroup(def);
	b2ParticleGroup *group3 = m_world->CreateParticleGroup(def);
	const b2ParticleGroup *list =
		((const b2World *)m_world)->GetParticleGroupList();
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
	b2ParticleGroup *group1 = m_world->CreateParticleGroup(def);
	shape.SetAsBox(10, 20);
	b2ParticleGroup *group2 = m_world->CreateParticleGroup(def);
	shape.SetAsBox(10, 30);
	b2ParticleGroup *group3 = m_world->CreateParticleGroup(def);
	int32 count1 = group1->GetParticleCount();
	int32 count2 = group2->GetParticleCount();
	int32 count3 = group3->GetParticleCount();
	EXPECT_EQ(count1 + count2 + count3, m_world->GetParticleCount());
	EXPECT_EQ(m_world->GetParticleGroupCount(), 3);
	m_world->JoinParticleGroups(group1, group2);
	EXPECT_EQ(m_world->GetParticleGroupCount(), 2);
	EXPECT_EQ(count1 + count2, group1->GetParticleCount());
}

TEST_F(FunctionTests, GroupBuffer) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	b2ParticleGroup *group1 = m_world->CreateParticleGroup(def);
	shape.SetAsBox(10, 20);
	b2ParticleGroup *group2 = m_world->CreateParticleGroup(def);
	shape.SetAsBox(10, 30);
	b2ParticleGroup *group3 = m_world->CreateParticleGroup(def);
	const b2ParticleGroup *const *groupBuffer = m_world->GetParticleGroupBuffer();
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
	m_world->JoinParticleGroups(group1, group2);
	m_world->DestroyParticlesInGroup(group3);
	m_world->Step(0.001f, 1, 1);
	groupBuffer = m_world->GetParticleGroupBuffer();
	int32 count = m_world->GetParticleCount();
	for (int32 i = 0; i < count; i++) {
		ASSERT_EQ(group1, groupBuffer[i]);
	}
}

TEST_F(FunctionTests, GetParticleContact) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_world->CreateParticleGroup(def);
	EXPECT_NE(m_world->GetParticleContactCount(), 0);
	EXPECT_NE(m_world->GetParticleContacts(), (const b2ParticleContact *)NULL);
}

TEST_F(FunctionTests, GetParticleBodyContact) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_world->CreateParticleGroup(def);
	b2BodyDef bodyDef;
	b2Body *body = m_world->CreateBody(&bodyDef);
	body->CreateFixture(&shape, 1.0);
	m_world->Step(0.001f, 1, 1);
	EXPECT_NE(m_world->GetParticleBodyContactCount(), 0);
	EXPECT_NE(m_world->GetParticleBodyContacts(),
			  (const b2ParticleBodyContact *)NULL);
}

TEST_F(FunctionTests, ComputeParticleCollisionEnergy) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	m_world->CreateParticleGroup(def);
	EXPECT_EQ(m_world->ComputeParticleCollisionEnergy(), 0);

	def.position.Set(20, 0);
	def.linearVelocity.Set(-1, 0);
	m_world->CreateParticleGroup(def);
	for (int32 t = 0; t < 1000; t++)
	{
		m_world->Step(0.1f, 1, 1);
	}
	EXPECT_NE(m_world->ComputeParticleCollisionEnergy(), 0);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
