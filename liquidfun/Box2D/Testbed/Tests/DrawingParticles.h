#ifndef DRAWING_PARITLCES_H
#define DRAWING_PARITLCES_H

class DrawingParticles : public Test
{
public:

	DrawingParticles()
	{
		{
			b2BodyDef bd;
			b2Body* ground = m_world->CreateBody(&bd);

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-40, -10),
					b2Vec2(40, -10),
					b2Vec2(40, 0),
					b2Vec2(-40, 0)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-40, -1),
					b2Vec2(-20, -1),
					b2Vec2(-20, 20),
					b2Vec2(-40, 30)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(20, -1),
					b2Vec2(40, -1),
					b2Vec2(40, 30),
					b2Vec2(20, 20)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2ParticleGroupDef pd;
				pd.destroyAutomatically = false;
				m_waterGroup = m_world->CreateParticleGroup(pd);
			}
		}

		m_world->SetParticleRadius(0.5f);
	}

	void MouseMove(const b2Vec2& p)
	{
		Test::MouseMove(p);
		b2CircleShape shape;
		shape.m_p = p;
		shape.m_radius = 2.0f;
		b2ParticleGroupDef pd;
		pd.shape = &shape;
		struct Callback : b2QueryCallback
		{
			bool ReportFixture(b2Fixture* fixture)
			{
				return false;
			}

			bool ReportParticle(int32 index)
			{
				b2Transform xf;
				xf.SetIdentity();
				if (shape->TestPoint(xf, world->GetParticlePositionBuffer()[index]))
				{
					world->DestroyParticle(index);
				}
				return true;
			}

			b2World* world;
			b2Shape* shape;

		} callback;
		callback.world = m_world;
		callback.shape = &shape;
		b2AABB aabb;
		b2Transform xf;
		xf.SetIdentity();
		shape.ComputeAABB(&aabb, xf, 0);
		m_world->QueryAABB(&callback, aabb);
		b2ParticleGroup* group = m_world->CreateParticleGroup(pd);
		m_world->JoinParticleGroups(m_waterGroup, group);
		m_mouseTracing = false;
	}

	static Test* Create()
	{
		return new DrawingParticles;
	}

	b2ParticleGroup* m_waterGroup;
};

#endif
