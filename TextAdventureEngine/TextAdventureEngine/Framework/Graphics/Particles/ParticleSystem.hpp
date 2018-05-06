#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <Framework/Template/System.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Utils/MemoryExt.hpp>
#include <Framework/Utils/Random.hpp>
#include "Particle.hpp"

//=====================================================================================
struct Particle;
class Emitter;
struct EmitterInfo;
struct Vector2;

//=====================================================================================
class ParticleSystem final : public System< ParticleSystem >
{
public:
	
	ParticleSystem( uint32_t a_MaxParticles, const ParticleInfo & a_ParticleInfo );
	~ParticleSystem();

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