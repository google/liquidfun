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

		m_world->SetParticleRadius(0.15f);
		m_world->SetParticleDamping(0.2f);

		{
			b2PolygonShape shape;
			shape.SetAsBox(8, 10, b2Vec2(-12, 10.1f), 0);
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
