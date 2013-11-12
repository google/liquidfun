// Copyright 2013 Google. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
#ifndef LIQUID_TIMER_H
#define LIQUID_TIMER_H

class LiquidTimer : public Test
{
public:

	LiquidTimer()
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

		m_world->SetParticleRadius(0.15f);
		{
			b2PolygonShape shape;
			shape.SetAsBox(20, 4, b2Vec2(0, 36), 0);
			b2ParticleGroupDef pd;
			pd.flags = b2_tensileParticle | b2_viscousParticle;
			pd.shape = &shape;
			m_world->CreateParticleGroup(pd);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(-20, 32), b2Vec2(-12, 32));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(-11, 32), b2Vec2(20, 32));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(-12, 32), b2Vec2(-12, 28));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(-11, 32), b2Vec2(-11, 28));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(-16, 24), b2Vec2(8, 20));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(16, 16), b2Vec2(-8, 12));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(-12, 8), b2Vec2(-12, 0));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(-4, 8), b2Vec2(-4, 0));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(4, 8), b2Vec2(4, 0));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(12, 8), b2Vec2(12, 0));
			body->CreateFixture(&shape, 0.1f);
		}
	}

	static Test* Create()
	{
		return new LiquidTimer;
	}
};

#endif
