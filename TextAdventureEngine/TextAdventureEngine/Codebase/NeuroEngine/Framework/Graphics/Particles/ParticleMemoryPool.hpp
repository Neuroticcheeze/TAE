#pragma once
#ifndef PARTICLEMEMORYPOOL_H
#define PARTICLEMEMORYPOOL_H

#include <Framework/Graphics/Particles/ParticleManager.hpp>

//=====================================================================================
class ParticleMemoryPool final
{
	friend class ParticleManager;

public:

	ParticleMemoryPool( uint32_t a_Capacity, ParticleManager::ParticleFormat a_Format );
	ParticleMemoryPool( const ParticleMemoryPool & a_Other ) = delete;
	~ParticleMemoryPool();

	ParticleMemoryPool & operator=( const ParticleMemoryPool & a_Other ) = delete;

	uint32_t GetMaxParticles() const { return m_pHostMemSize / ParticleManager::Instance().GetFormatInfo( m_Format ).m_NumFloats; }
	uint32_t GetNumParticles() const { return m_pCount; }

private:

	const ParticleManager::ParticleFormat m_Format;
	const uint32_t m_pHostMemSize; // num floats
	uint32_t m_pCount;
	void* m_pHostMem;
	void* m_pCMemObj;
	void* m_pVBuff;
	void* m_pSSBuff;
};

#endif//PARTICLEMEMORYPOOL_H