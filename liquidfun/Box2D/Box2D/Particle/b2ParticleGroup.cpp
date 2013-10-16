#include <Box2D/Particle/b2ParticleGroup.h>
#include <Box2D/Particle/b2ParticleSystem.h>

b2ParticleGroup::b2ParticleGroup()
{

	m_system = NULL;
	m_firstIndex = 0;
	m_lastIndex = 0;
	m_flags = 0;
	m_strength = 1;
	m_prev = NULL;
	m_next = NULL;

	m_timestamp = -1;
	m_mass = 0;
	m_inertia = 0;
	m_center = b2Vec2_zero;
	m_linearVelocity = b2Vec2_zero;
	m_angularVelocity = 0;
	m_transform.SetIdentity();

	m_destroyAutomatically = true;
	m_toBeDestroyed = false;
	m_toBeSplit = false;

	m_userData = NULL;

}

b2ParticleGroup::~b2ParticleGroup()
{
}

uint32* b2ParticleGroup::GetParticleFlagsBuffer()
{
	return m_system->GetParticleFlagsBuffer() + m_firstIndex;
}

b2Vec2* b2ParticleGroup::GetParticlePositionBuffer()
{
	return m_system->GetParticlePositionBuffer() + m_firstIndex;
}

b2Vec2* b2ParticleGroup::GetParticleVelocityBuffer()
{
	return m_system->GetParticleVelocityBuffer() + m_firstIndex;
}

b2ParticleColor* b2ParticleGroup::GetParticleColorBuffer()
{
	return m_system->GetParticleColorBuffer() + m_firstIndex;
}

void** b2ParticleGroup::GetParticleUserDataBuffer()
{
	return m_system->GetParticleUserDataBuffer() + m_firstIndex;
}

const uint32* b2ParticleGroup::GetParticleFlagsBuffer() const
{
	return m_system->GetParticleFlagsBuffer() + m_firstIndex;
}

const b2Vec2* b2ParticleGroup::GetParticlePositionBuffer() const
{
	return m_system->GetParticlePositionBuffer() + m_firstIndex;
}

const b2Vec2* b2ParticleGroup::GetParticleVelocityBuffer() const
{
	return m_system->GetParticleVelocityBuffer() + m_firstIndex;
}

const b2ParticleColor* b2ParticleGroup::GetParticleColorBuffer() const
{
	return m_system->GetParticleColorBuffer() + m_firstIndex;
}

void* const* b2ParticleGroup::GetParticleUserDataBuffer() const
{
	return m_system->GetParticleUserDataBuffer() + m_firstIndex;
}

int32 b2ParticleGroup::GetFlags() const
{
	return m_flags;
}

float32 b2ParticleGroup::GetMass() const
{
	UpdateStatistics();
	return m_mass;
}

float32 b2ParticleGroup::GetInertia() const
{
	UpdateStatistics();
	return m_inertia;
}

b2Vec2 b2ParticleGroup::GetCenter() const
{
	UpdateStatistics();
	return m_center;
}

b2Vec2 b2ParticleGroup::GetLinearVelocity() const
{
	UpdateStatistics();
	return m_linearVelocity;
}

float32 b2ParticleGroup::GetAngularVelocity() const
{
	UpdateStatistics();
	return m_angularVelocity;
}

const b2Transform& b2ParticleGroup::GetTransform() const
{
	return m_transform;
}

const b2Vec2& b2ParticleGroup::GetPosition() const
{
	return m_transform.p;
}

float32 b2ParticleGroup::GetAngle() const
{
	return m_transform.q.GetAngle();
}

void* b2ParticleGroup::GetUserData() const
{
	return m_userData;
}

void b2ParticleGroup::SetUserData(void* data)
{
	m_userData = data;
}

void b2ParticleGroup::UpdateStatistics() const
{
	if (m_timestamp != m_system->m_timestamp)
	{
		float32 m = m_system->GetParticleMass();
		m_mass = 0;
		m_center.SetZero();
		m_linearVelocity.SetZero();
		for (int32 i = m_firstIndex; i < m_lastIndex; i++)
		{
			m_mass += m;
			m_center += m * m_system->m_positionBuffer[i];
			m_linearVelocity += m * m_system->m_velocityBuffer[i];
		}
		if (m_mass > 0)
		{
			m_center *= 1 / m_mass;
			m_linearVelocity *= 1 / m_mass;
		}
		m_inertia = 0;
		m_angularVelocity = 0;
		for (int32 i = m_firstIndex; i < m_lastIndex; i++)
		{
			b2Vec2 p = m_system->m_positionBuffer[i] - m_center;
			b2Vec2 v = m_system->m_velocityBuffer[i] - m_linearVelocity;
			m_inertia += m * b2Dot(p, p);
			m_angularVelocity += m * b2Cross(p, v);
		}
		if (m_inertia > 0)
		{
			m_angularVelocity *= 1 / m_inertia;
		}
		m_timestamp = m_system->m_timestamp;
	}
}
