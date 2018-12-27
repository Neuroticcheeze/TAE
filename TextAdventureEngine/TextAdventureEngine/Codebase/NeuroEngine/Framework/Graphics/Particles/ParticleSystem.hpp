#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <Framework/Template/System.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Utils/MemoryExt.hpp>
#include <Framework/Utils/Random.hpp>
#include "Particle.hpp"
#include "Emitter.hpp"
#include "EmitterInfo.hpp"

//=====================================================================================
struct Vector2;

//=====================================================================================
class ParticleSystem final : public System< ParticleSystem >
{
public:
	
	ParticleSystem() : ParticleSystem( 0, ParticleInfo() ) {}
	ParticleSystem( uint32_t a_MaxParticles, const ParticleInfo & a_ParticleInfo );
	~ParticleSystem();

	ParticleSystem( const ParticleSystem & a_Other )
	{
		*this = a_Other;
	}

	ParticleSystem & operator=( const ParticleSystem & a_Other )
	{
		m_ParticlesMax = a_Other.m_ParticlesMax;
		m_ParticlesNum = a_Other.m_ParticlesNum;
		m_ParticleInfo = a_Other.m_ParticleInfo;
		m_SimSpeed = a_Other.m_SimSpeed;
		m_Random = a_Other.m_Random;

		if ( m_Particles )
		{
			delete[] m_Particles;
			m_Particles = nullptr;
		}

		m_Particles = new Particle[ m_ParticlesMax ];
		if ( a_Other.m_Particles )
		{
			auto it = a_Other.m_Particles;
			auto end = it + m_ParticlesMax;
			while ( it != end )
			{
				m_Particles[ it - a_Other.m_Particles ] = *it;
				++it;
			}
		}

		m_Emitters.Clear();

		auto it = a_Other.m_Emitters.First();
		auto end = a_Other.m_Emitters.End();
		while ( it != end )
		{
			CreateEmitter( ( *it )->GetProperties() );
			++it;
		}

		return *this;
	}

	WeakPointer< Emitter > CreateEmitter( const EmitterInfo & a_Info );
	void DestroyEmitter( const WeakPointer< Emitter > & a_EmitterToDestroy );
	void Emit( const Vector2 & a_Position, const Vector2 & a_Velocity );
	void Clear();
	void SetSimSpeed( float a_Speed );
	inline uint32_t GetNumParticles() const
	{
		return m_ParticlesNum;
	}

	void Tick( float a_DeltaTime = 0.0F );

	Random & GetRand();

private:
	Particle * m_Particles;
	uint32_t m_ParticlesMax;
	uint32_t m_ParticlesNum;
	Array< Pointer< Emitter > > m_Emitters;
	ParticleInfo m_ParticleInfo;
	float m_SimSpeed;
	Random m_Random;
};

#endif//PARTICLESYSTEM_H