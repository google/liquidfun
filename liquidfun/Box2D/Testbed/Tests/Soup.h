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
