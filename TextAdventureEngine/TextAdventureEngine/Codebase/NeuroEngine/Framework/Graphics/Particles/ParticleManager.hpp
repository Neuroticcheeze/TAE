#pragma once
#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Compute/ComputeManager.hpp>

//=====================================================================================
class ParticleMemoryPool;

//=====================================================================================
class ParticleManager final : public Manager< ParticleManager >
{
public:
	
	ENUMCLASS( ParticleShading, int32_t )
		PSHD_UBER,
	END_ENUMCLASS( ParticleShading, int32_t );

	ENUMCLASS( ParticleFormat, int32_t )
		PTFM_POS2VEL2LIFE2,
	END_ENUMCLASS( ParticleFormat, int32_t );

	void Init() override;
	void InitPost() override;
	//void Tick( float a_Dt ) override;
	void Finalise() override;

	struct FormatInfo final
	{
		uint32_t m_NumFloats;
		uint32_t m_OneCheckOffset; // Which value = 1 when the particle dies
	};

	inline const FormatInfo & GetFormatInfo( ParticleFormat a_Format ) const
	{
		return m_FormatInfos[ static_cast< uint32_t >( a_Format ) ];
	}

	struct ParticleInfo final
	{
		float m_Acceleration[ 2 ] = { 0.0F, 0.0F };
		float m_Damping = 1.0F;
	};

	void Simulate( ParticleMemoryPool & a_Pool, const ParticleInfo & a_ParticleInfo, float a_Delta );
	void ReSort( ParticleMemoryPool & a_Pool );
	void Emit( ParticleMemoryPool & a_Pool, uint32_t a_Count, const Functor< bool( float* ) > & a_EmitterCallback );
	void DrawGfxData( ParticleMemoryPool & a_Pool, ParticleShading a_ParticleShading = ParticleShading::PSHD_UBER );

private:

	GraphicsManager::Shader m_ParticleShaders[ ParticleShadingCount ];

	ComputeManager::CommandQueue m_CommandQueue;
	ComputeManager::ComputeProgram m_Program;

	ComputeManager::ComputeKernel m_ProcessorKernels[ ParticleFormatCount ];
	//ComputeManager::ComputeKernel m_GfxBuilderKernels[ ParticleFormatCount ];

	const FormatInfo m_FormatInfos[ ParticleFormatCount ] = 
	{
		FormatInfo{ 6, 4 }
	};
};

#endif//PARTICLEMANAGER_H