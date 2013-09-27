#ifndef PARTICLES_RIGID_H
#define PARTICLES_RIGID_H

class RigidParticles : public Test
{
public:

	RigidParticles()
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
					b2Vec2(-40, 20)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(20, -1),
					b2Vec2(40, -1),
					b2Vec2(40, 20),
					b2Vec2(20, 20)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}
		}

		m_world->SetParticleRadius(0.5f);

		{
			b2CircleShape shape;
			shape.m_p.Set(0, 30);
			shape.m_radius = 5;
			b2ParticleGroupDef pd;
			pd.flags = b2_rigidParticle;
			pd.shape = &shape;
			pd.color.Set(255, 0, 0, 255);
			m_world->CreateParticleGroup(pd);
		}

		{
			b2CircleShape shape;
			shape.m_p.Set(-10, 30);
			shape.m_radius = 5;
			b2ParticleGroupDef pd;
			pd.flags = b2_rigidParticle;
			pd.shape = &shape;
			pd.color.Set(0, 255, 0, 255);
			m_world->CreateParticleGroup(pd);
		}

		{
			b2PolygonShape shape;
			shape.SetAsBox(10, 5);
			b2ParticleGroupDef pd;
			pd.flags = b2_rigidParticle;
			pd.position.Set(10, 40);
			pd.angle = -0.5f;
			pd.angularVelocity = 2.0f;
			pd.shape = &shape;
			pd.color.Set(0, 0, 255, 255);
			m_world->CreateParticleGroup(pd);
		}

		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			b2Body* body = m_world->CreateBody(&bd);
			b2CircleShape shape;
			shape.m_p.Set(0, 80);
			shape.m_radius = 5;
			body->CreateFixture(&shape, 0.5f);
		}
	}

	static Test* Create()
	{
		return new RigidParticles;
	}
};

#endif
