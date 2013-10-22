#include "gtest/gtest.h"
#include "Box2D/Box2D.h"
#include <stdio.h>
#include "BodyTracker.h"
#include "AndroidUtil/AndroidMainWrapper.h"
#define EPSILON 0.001f

class FunctionTests : public ::testing::Test {
    protected:
	virtual void SetUp();
	virtual void TearDown();

	b2World *m_world;
};

void
FunctionTests::SetUp()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	m_world = new b2World(gravity);

}

void
FunctionTests::TearDown()
{
	// Intentionally blank.
}

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

TEST_F(FunctionTests, DestroyParticle) {
	b2ParticleDef def;
	int index = m_world->CreateParticle(def);
	m_world->DestroyParticle(index);
	m_world->Step(0.001f, 1, 1);
	EXPECT_EQ(m_world->GetParticleCount(), 0);
}

TEST_F(FunctionTests, CreateParticleGroup) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	b2ParticleGroup *group = m_world->CreateParticleGroup(def);
	EXPECT_NE(group, (b2ParticleGroup *)0);
	EXPECT_EQ(m_world->GetParticleGroupCount(), 1);
	EXPECT_NE(m_world->GetParticleCount(), 0);
	EXPECT_EQ(m_world->GetParticleCount(), group->GetParticleCount());
}

TEST_F(FunctionTests, DestroyParticleGroup) {
	b2ParticleGroupDef def;
	b2PolygonShape shape;
	shape.SetAsBox(10, 10);
	def.shape = &shape;
	b2ParticleGroup *group = m_world->CreateParticleGroup(def);
	m_world->DestroyParticleGroup(group);
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
	EXPECT_EQ(m_world->GetParticleFlagsBuffer(), group->GetParticleFlagsBuffer());
	EXPECT_EQ(m_world->GetParticlePositionBuffer(), group->GetParticlePositionBuffer());
	EXPECT_EQ(m_world->GetParticleVelocityBuffer(), group->GetParticleVelocityBuffer());
	EXPECT_EQ(m_world->GetParticleColorBuffer(), group->GetParticleColorBuffer());
	EXPECT_EQ(m_world->GetParticleUserDataBuffer(), group->GetParticleUserDataBuffer());
	const b2World *constWorld = m_world;
	EXPECT_EQ(m_world->GetParticleFlagsBuffer(), constWorld->GetParticleFlagsBuffer());
	EXPECT_EQ(m_world->GetParticlePositionBuffer(), constWorld->GetParticlePositionBuffer());
	EXPECT_EQ(m_world->GetParticleVelocityBuffer(), constWorld->GetParticleVelocityBuffer());
	EXPECT_EQ(m_world->GetParticleColorBuffer(), constWorld->GetParticleColorBuffer());
	EXPECT_EQ(m_world->GetParticleUserDataBuffer(), constWorld->GetParticleUserDataBuffer());
	const b2ParticleGroup *constGroup = group;
	EXPECT_EQ(group->GetParticleFlagsBuffer(), constGroup->GetParticleFlagsBuffer());
	EXPECT_EQ(group->GetParticlePositionBuffer(), constGroup->GetParticlePositionBuffer());
	EXPECT_EQ(group->GetParticleVelocityBuffer(), constGroup->GetParticleVelocityBuffer());
	EXPECT_EQ(group->GetParticleColorBuffer(), constGroup->GetParticleColorBuffer());
	EXPECT_EQ(group->GetParticleUserDataBuffer(), constGroup->GetParticleUserDataBuffer());
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
	EXPECT_LE(b2Distance(group->GetLinearVelocity(), def.linearVelocity), EPSILON);
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
	const b2ParticleGroup *list = ((const b2World *)m_world)->GetParticleGroupList();
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
	EXPECT_NE(m_world->GetParticleBodyContacts(), (const b2ParticleBodyContact *)NULL);
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

int
main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
