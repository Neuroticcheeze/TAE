#include "ParticleMemoryPool.hpp"
#include <Framework/Compute/ComputeManager.hpp>
#include <Framework/Graphics/GL/GLDraw.hpp>
#include <Framework/Graphics/GL/GLMesh.hpp>
#include <Framework/Graphics/GL/GLShaderStorageBuffer.hpp>

//=====================================================================================
ParticleMemoryPool::ParticleMemoryPool( uint32_t a_Capacity, ParticleManager::ParticleFormat a_Format )
	: m_Format( a_Format )
	, m_pCount( 0 )
	, m_pHostMemSize( a_Capacity * ParticleManager::Instance().GetFormatInfo( a_Format ).m_NumFloats )
{
	m_pHostMem = BAllocate< float >( m_pHostMemSize );
	m_pCMemObj = new ComputeManager::Buffer( 
		ComputeManager::Instance().CreateBuffer(
			nullptr, 
			m_pHostMemSize * sizeof( float ), 
			ComputeManager::MEM_ALLOC_HOST_PTR | ComputeManager::MEM_READ_WRITE
		) 
	);

	m_pVBuff = new GLMesh( "ParticleSystem", VFD_POS2, a_Capacity * 4, GLMesh::DrawMode::DM_QUADS );
	m_pSSBuff = new GLShaderStorageBuffer( "ParticleSystem", m_pHostMemSize );
}

//=====================================================================================
ParticleMemoryPool::~ParticleMemoryPool()
{
	ComputeManager::Instance().DestroyBuffer( *static_cast< ComputeManager::Buffer* >( m_pCMemObj ) );
	BFree( m_pHostMem );
	delete static_cast< GLMesh* >( m_pVBuff );
	delete static_cast< GLShaderStorageBuffer* >( m_pSSBuff );
}
