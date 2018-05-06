#include "Emitter.hpp"
#include "ParticleSystem.hpp"
#include "Particle.hpp"
#include <Framework/Math/Math.hpp>

#define MAX_EMIT_PER_FRAME 256u

//=====================================================================================
EmitterInfo & Emitter::GetProperties()
{
	return m_Info;
}

//=====================================================================================
const EmitterInfo & Emitter::GetProperties() const
{
	return m_Info;
}

//=====================================================================================
void Emitter::Tick( float a_DeltaTime )
{
	float prev = m_Time;
	m_Time += a_DeltaTime;

	if ( StridedOverInterval( m_Info.m_Rate, prev, m_Time ) && m_Time > 0.0F )
	{
		uint32_t m = 1;
		int p = 1.0F / a_DeltaTime;
		if ( m_Info.m_Rate > p )
		{
			m = Min( MAX_EMIT_PER_FRAME, ( uint32_t )( m_Info.m_Rate / p ) );
		}

		for ( uint32_t k = 0; k < m; ++k )
		{
			Vector2 offset = m_Owner->GetRand().RandomVector2();

			Vector2 velocity = m_Owner->GetRand().RandomVector2().Normalised() * m_Info.m_Throw.Length() * ( 1.0F - m_Info.m_Accuracy ) + 
							   m_Info.m_Throw * m_Info.m_Accuracy;

			float km = (float)k / (float)m;
			Vector2 interpPos = m_PrevPosition * ( 1.0F - km ) + m_Info.m_Position * km;

			m_Owner->Emit( interpPos + offset * m_Info.m_Radius, velocity );
		}
	}

	m_PrevPosition = m_Info.m_Position;
}

//=====================================================================================
void Emitter::Setup( const EmitterInfo & a_Info )
{
	m_Dead = false;
	m_Time = 0.0F;
	m_Info = a_Info;
	m_PrevPosition = m_Info.m_Position;
}