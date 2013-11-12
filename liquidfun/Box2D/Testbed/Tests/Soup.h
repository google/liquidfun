// Copyright 2013 Google. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
#ifndef SOUP_H
#define SOUP_H

class Soup : public Test
{
public:

	Soup()
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
		}

		m_world->SetParticleRadius(0.2f);
		{
			b2PolygonShape shape;
			shape.SetAsBox(20, 10, b2Vec2(0, 10), 0);
			b2ParticleGroupDef pd;
			pd.shape = &shape;
			m_world->CreateParticleGroup(pd);
		}

		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			b2Body* body = m_world->CreateBody(&bd);
			b2CircleShape shape;
			shape.m_p.Set(0, 5);
			shape.m_radius = 1;
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			b2Body* body = m_world->CreateBody(&bd);
			b2PolygonShape shape;
			shape.SetAsBox(1, 1, b2Vec2(-10, 5), 0);
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			b2Body* body = m_world->CreateBody(&bd);
			b2PolygonShape shape;
			shape.SetAsBox(1, 1, b2Vec2(10, 5), 0.5f);
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(0, 20), b2Vec2(1, 21));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(3, 20), b2Vec2(4, 21));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.Set(b2Vec2(-3, 21), b2Vec2(-2, 20));
			body->CreateFixture(&shape, 0.1f);
		}
	}

	static Test* Create()
	{
		return new Soup;
	}
};

#endif
