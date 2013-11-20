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
