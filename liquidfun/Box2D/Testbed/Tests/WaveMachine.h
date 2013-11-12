// Copyright 2013 Google. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
#ifndef WAVE_MACHINE_H
#define WAVE_MACHINE_H

class WaveMachine : public Test
{
public:

	WaveMachine()
	{
		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = m_world->CreateBody(&bd);
		}

		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.allowSleep = false;
			bd.position.Set(0.0f, 10.0f);
			b2Body* body = m_world->CreateBody(&bd);

			b2PolygonShape shape;
			shape.SetAsBox(0.5f, 10.0f, b2Vec2( 20.0f, 0.0f), 0.0);
			body->CreateFixture(&shape, 5.0f);
			shape.SetAsBox(0.5f, 10.0f, b2Vec2(-20.0f, 0.0f), 0.0);
			body->CreateFixture(&shape, 5.0f);
			shape.SetAsBox(20.0f, 0.5f, b2Vec2(0.0f, 10.0f), 0.0);
			body->CreateFixture(&shape, 5.0f);
			shape.SetAsBox(20.0f, 0.5f, b2Vec2(0.0f, -10.0f), 0.0);
			body->CreateFixture(&shape, 5.0f);

			b2RevoluteJointDef jd;
			jd.bodyA = ground;
			jd.bodyB = body;
			jd.localAnchorA.Set(0.0f, 10.0f);
			jd.localAnchorB.Set(0.0f, 0.0f);
			jd.referenceAngle = 0.0f;
			jd.motorSpeed = 0.05f * b2_pi;
			jd.maxMotorTorque = 1e7f;
			jd.enableMotor = true;
			m_joint = (b2RevoluteJoint*)m_world->CreateJoint(&jd);
		}

		m_world->SetParticleRadius(0.15f);
		{
			b2ParticleGroupDef pd;

			b2PolygonShape shape;
			shape.SetAsBox(9.0f, 9.0f, b2Vec2(0.0f, 10.0f), 0.0);

			pd.shape = &shape;
			m_world->CreateParticleGroup(pd);

		}

		m_time = 0;
	}

	void Step(Settings* settings)
	{
		Test::Step(settings);
		if (settings->hz > 0)
		{
			m_time += 1 / settings->hz;
		}
		m_joint->SetMotorSpeed(0.05f * cosf(m_time) * b2_pi);
	}

	static Test* Create()
	{
		return new WaveMachine;
	}

	b2RevoluteJoint* m_joint;
	float32 m_time;
};

#endif
