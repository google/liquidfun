#ifndef DAM_BREAK_H
#define DAM_BREAK_H

class DamBreak : public Test
{
public:

	DamBreak()
	{
		{
			b2BodyDef bd;
			b2Body* ground = m_world->CreateBody(&bd);

			b2ChainShape shape;
			const b2Vec2 vertices[4] = {
				b2Vec2(-20, 0),
				b2Vec2(20, 0),
				b2Vec2(20, 40),
				b2Vec2(-20, 40)};
			shape.CreateLoop(vertices, 4);
			ground->CreateFixture(&shape, 0.0f);

		}

		m_world->SetParticleRadius(0.2f);
		{
			b2PolygonShape shape;
			shape.SetAsBox(5, 10, b2Vec2(-15, 10.1f), 0);
			b2ParticleGroupDef pd;
			pd.shape = &shape;
			m_world->CreateParticleGroup(pd);
		}

	}

	static Test* Create()
	{
		return new DamBreak;
	}
};

#endif
