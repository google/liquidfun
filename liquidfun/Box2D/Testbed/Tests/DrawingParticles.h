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
					b2Vec2(-40, -20),
					b2Vec2(40, -20),
					b2Vec2(40, 0),
					b2Vec2(-40, 0)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-40, -20),
					b2Vec2(-20, -20),
					b2Vec2(-20, 60),
					b2Vec2(-40, 60)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(20, -20),
					b2Vec2(40, -20),
					b2Vec2(40, 60),
					b2Vec2(20, 60)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-40, 40),
					b2Vec2(40, 40),
					b2Vec2(40, 60),
					b2Vec2(-40, 60)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}
		}

		m_world->SetParticleRadius(0.5f);
		m_lastGroup = NULL;
		m_drawing = true;
		m_particleFlags = 0;
		m_groupFlags = 0;
	}

	void Keyboard(unsigned char key)
	{
		m_drawing = key != 'X';
		m_particleFlags = 0;
		m_groupFlags = 0;
		switch (key)
		{
		case 'E':
			m_particleFlags = b2_elasticParticle;
			m_groupFlags = b2_solidParticleGroup;
			break;
		case 'P':
			m_particleFlags = b2_powderParticle;
			break;
		case 'R':
			m_groupFlags = b2_rigidParticleGroup | b2_solidParticleGroup;
			break;
		case 'S':
			m_particleFlags = b2_springParticle;
			m_groupFlags = b2_solidParticleGroup;
			break;
		case 'T':
			m_particleFlags = b2_tensileParticle;
			break;
		case 'V':
			m_particleFlags = b2_viscousParticle;
			break;
		case 'W':
			m_particleFlags = b2_wallParticle;
			m_groupFlags = b2_solidParticleGroup;
			break;
		case 'Z':
			m_particleFlags = b2_zombieParticle;
			break;
		}
	}

	void MouseMove(const b2Vec2& p)
	{
		Test::MouseMove(p);
		if (m_drawing)
		{
			b2CircleShape shape;
			shape.m_p = p;
			shape.m_radius = 2.0f;
			b2Transform xf;
			xf.SetIdentity();

			m_world->DestroyParticlesInShape(shape, xf);

			b2ParticleGroupDef pd;
			pd.shape = &shape;
			pd.flags = m_particleFlags;
			pd.groupFlags = m_groupFlags;
			b2ParticleGroup* group = m_world->CreateParticleGroup(pd);
			if (m_lastGroup && group->GetGroupFlags() == m_lastGroup->GetGroupFlags())
			{
				m_world->JoinParticleGroups(m_lastGroup, group);
			}
			else
			{
				m_lastGroup = group;
			}
			m_mouseTracing = false;
		}
	}

	void MouseUp(const b2Vec2& p)
	{
		Test::MouseUp(p);
		m_lastGroup = NULL;
	}

	void ParticleGroupDestroyed(b2ParticleGroup* group) {
		Test::ParticleGroupDestroyed(group);
		if (group == m_lastGroup)
		{
			m_lastGroup = NULL;
		}
	}

	void Step(Settings* settings)
	{
		Test::Step(settings);
		m_debugDraw.DrawString(5, m_textLine, "Keys: (L) liquid, (E) elastic, (S) spring");
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(5, m_textLine, "(R) rigid, (W) wall, (V) viscous, (T) tensile");
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(5, m_textLine, "(Z) erase, (X) move");
		m_textLine += DRAW_STRING_NEW_LINE;
	}

	static Test* Create()
	{
		return new DrawingParticles;
	}

	b2ParticleGroup* m_lastGroup;
	bool m_drawing;
	int32 m_particleFlags;
	int32 m_groupFlags;

};

#endif
