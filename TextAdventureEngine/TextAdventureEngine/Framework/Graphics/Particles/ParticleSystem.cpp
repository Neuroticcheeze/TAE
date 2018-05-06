#include "ParticleSystem.hpp"
#include "Emitter.hpp"
#include <Framework/Containers/CString.hpp>
#include <SIGIL-0.9.0\sl.h>

//=====================================================================================
ParticleSystem::ParticleSystem( uint32_t a_MaxParticles, const ParticleInfo & a_ParticleInfo )
	: m_ParticlesMax( a_MaxParticles )
	, m_ParticlesNum( 0 )
	, m_Particles( new Particle[ a_MaxParticles ] )
	, m_SimSpeed( 1.0F )
	, m_ParticleInfo( a_ParticleInfo )
{
}

ParticleSystem::~ParticleSystem()
{
	delete[] m_Particles;
}

//=====================================================================================
WeakPointer< Emitter > ParticleSystem::CreateEmitter( const EmitterInfo & a_Info )
{
	Pointer< Emitter > em = Pointer< Emitter >( Emitter() );
	em->Setup( a_Info );
	em->m_Owner = this;
	m_Emitters.Append( em );
	return WeakPointer< Emitter >( em );
}

//=====================================================================================
void ParticleSystem::DestroyEmitter( const WeakPointer< Emitter > & a_EmitterToDestroy )
{
	m_Emitters.Remove( a_EmitterToDestroy );
}

//=====================================================================================
void ParticleSystem::Emit( const Vector2 & a_Position, const Vector2 & a_Velocity )
{
	if ( m_ParticlesNum == m_ParticlesMax )
	{
		return;
	}

	Particle ptcl;
	ptcl.m_Position = a_Position;
	ptcl.m_Velocity = a_Velocity;
	ptcl.m_Life = m_ParticleInfo.Life.GetValue();
	ptcl.m_Torque = m_ParticleInfo.Torque.GetValue();
	m_Particles[ m_ParticlesNum ] = ptcl;
	++m_ParticlesNum;
}

//=====================================================================================
void ParticleSystem::Clear()
{
	m_ParticlesNum = 0;
}

//=====================================================================================
void ParticleSystem::SetSimSpeed( float a_Speed )
{
	m_SimSpeed = Clamp( a_Speed, 0.0F, 10.0F );
}

//=====================================================================================
void ParticleSystem::Tick( float a_DeltaTime )
{
	const double dt = (double)a_DeltaTime * (double)m_SimSpeed;

	if ( m_Emitters.Count() > 0 )
	{
		uint32_t count = m_Emitters.Count() - 1;
		for ( int32_t k = count; k >= 0; --k )
		{
			Emitter * emitter = m_Emitters[ (uint32_t)k ].Ptr();
			
			emitter->Tick( dt );

			if ( emitter->m_Dead )
			{
				m_Emitters.RemoveAt( k );
			}
		}
	}

	Array< Particle > sortedArray;
	sortedArray.Resize( m_ParticlesNum );
	BCopy( m_Particles, sortedArray.First(), sizeof( Particle ) * m_ParticlesNum );
	//sortedArray.Sort( []( const Particle & a_Left, const Particle & a_Right ){ return a_Left.m_Time < a_Right.m_Time; } );
	BCopy( sortedArray.First(), m_Particles, sizeof( Particle ) * m_ParticlesNum );

	if ( m_ParticlesNum > 0 )
	{
		uint32_t count = m_ParticlesNum - 1;
		for ( int32_t k = count; k >= 0; --k )
		{
			Particle & p = m_Particles[ (uint32_t)k ];
			const double t = p.m_Time / p.m_Life;

			Colour col = m_ParticleInfo.TintOverTime.Evaluate( t );
			float scale = m_ParticleInfo.ScaleOverTime.Evaluate( t );

			slSetForeColor(col.r, col.g, col.b, col.a);
			slPush();
			slTranslate(p.m_Position.x, p.m_Position.y);
			slRotate( p.m_Angle );
			slScale(scale, scale);
			m_ParticleInfo.Sprite.DrawSprite( t * m_ParticleInfo.Sprite.SpritesPerAxis().x * m_ParticleInfo.Sprite.SpritesPerAxis().y, Vector2::ZERO, Vector2::ONE );
			slCircleFill( 0, 0, 0.2F, 5 );
			slPop();

			p.m_Angle += (double)p.m_Torque * dt;
			p.m_Position.x += (double)p.m_Velocity.x * dt * (double)m_ParticleInfo.Density;
			p.m_Position.y += (double)p.m_Velocity.y * dt * (double)m_ParticleInfo.Density;
			p.m_Velocity.x += (double)m_ParticleInfo.Acceleration.x * dt * (double)m_ParticleInfo.Density;
			p.m_Velocity.y += (double)m_ParticleInfo.Acceleration.y * dt * (double)m_ParticleInfo.Density;

			p.m_Time += dt;


			if ( p.m_Time >= p.m_Life )
			{
				m_Particles[ k ] = m_Particles[ --m_ParticlesNum ];
			}
		}
	}
}

//=====================================================================================
Random & ParticleSystem::GetRand()
{
	return m_Random;
}