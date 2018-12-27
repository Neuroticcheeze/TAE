#include "ParticleManager.hpp"
#include <Framework/Utils/Hash.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Graphics/Particles/ParticleMemoryPool.hpp>
#include <Framework/Compute/ComputePrecompiled.hpp>
#include <Framework/Graphics/GL/GLDraw.hpp>
#include <Framework/Graphics/GL/GLShaderStorageBuffer.hpp>
#include <Framework/Graphics/GPUGradient.hpp>

//=====================================================================================
void ParticleManager::Init()
{
}

//=====================================================================================
void ParticleManager::InitPost()
{
	auto & cm = ComputeManager::Instance();
	auto & gfxm = GraphicsManager::Instance();

	m_Program = cm.LoadComputeProgram( CRC32( "__Internal_GfxParticles" ), { "builtin/gfx_particle.cl" }, {} );
	m_CommandQueue = cm.CreateCommandQueue();

	// Load the shaders
	m_ParticleShaders[ (int32_t)ParticleShading::PSHD_UBER ] = gfxm.LoadShader( CRC32( "__Internal_GfxParticlesShader_Uber" ), "builtin/gfx_particle.shader", CRC32( "vUBER" ), CRC32( "fUBER" ) );

	// Load the processors
	m_ProcessorKernels[ (int32_t)ParticleFormat::PTFM_POS2VEL2LIFE2 ] = cm.LoadComputeKernel( "Proc_POS2VEL2LIFE2", m_Program );
}

//=====================================================================================
//void ParticleManager::Tick( float a_Dt )
//{
//}

//=====================================================================================
void ParticleManager::Finalise()
{
	auto & cm = ComputeManager::Instance();
	auto & gfxm = GraphicsManager::Instance();
	
	cm.DestroyCommandQueue( m_CommandQueue );

	// Unload the processors
	for ( uint32_t i = 0; i < ParticleFormatCount; ++i )
	{
		cm.UnloadComputeKernel( m_ProcessorKernels[ i ] );
		//cm.UnloadComputeKernel( m_GfxBuilderKernels[ i ] );
	}

	// Unload the shaders
	for ( uint32_t i = 0; i < ParticleFormatCount; ++i )
	{
		gfxm.UnloadShader( m_ParticleShaders[ i ] );
	}

	cm.UnloadComputeProgram( m_Program );
}

//=====================================================================================
void ParticleManager::Emit( ParticleMemoryPool & a_Pool, uint32_t a_Count, const Functor< bool( float* ) > & a_EmitterCallback )
{
	// Pre-checks
	if ( a_Count == 0 )
	{
		return;
	}
	const FormatInfo * fInf = &GetFormatInfo( a_Pool.m_Format );
	uint32_t particleCap = a_Pool.m_pHostMemSize / fInf->m_NumFloats;
	if ( ( a_Pool.m_pCount + a_Count ) > particleCap )
	{
		if ( ( a_Count = particleCap - a_Pool.m_pCount ) == 0 )
		{
			return;
		}
	}

	// Wait on cl
	ComputeManager & cmpmgr = ComputeManager::Instance();
	cmpmgr.SetCommandQueue( m_CommandQueue );
	if ( auto cqueue = ___ext_active_cqueue_p )
		cqueue->finish();

	// Write to buffer from callback
	const uint32_t oldCount = a_Pool.m_pCount;
	float * const begin = reinterpret_cast< float* >( a_Pool.m_pHostMem ) + oldCount * fInf->m_NumFloats;
	const float * const end = reinterpret_cast< float* >( a_Pool.m_pHostMem ) + ( oldCount + a_Count ) * fInf->m_NumFloats;
	for ( float * it = begin; it < end; it += fInf->m_NumFloats )
	{
		if ( a_EmitterCallback( it ) )
		{
			++a_Pool.m_pCount;
		}
	}

	// No new particles have been written to buffer..
	if ( a_Pool.m_pCount == oldCount )
	{
		return;
	}

	// Upload modified buffer
	auto & cbuff = ___ref_buffer( ( *reinterpret_cast< ComputeManager::Buffer* >( a_Pool.m_pCMemObj ) ) );
	if ( auto cqueue = ___ext_active_cqueue_p )
	{
		const size_t offset = ( oldCount ) * fInf->m_NumFloats * sizeof( float );
		const size_t size = ( a_Count ) * fInf->m_NumFloats * sizeof( float );
		cqueue->enqueueWriteBuffer( cbuff, true, offset, size, reinterpret_cast< uint8_t* >( a_Pool.m_pHostMem ) + offset );
	}
}

//=====================================================================================
void ParticleManager::Simulate( ParticleMemoryPool & a_Pool, const ParticleInfo & a_ParticleInfo, float a_Delta )
{
	ComputeManager & cmpmgr = ComputeManager::Instance();
	const ComputeManager::ComputeKernel & kernel = m_ProcessorKernels[ static_cast< uint32_t >( a_Pool.m_Format ) ];
	ComputeManager::Buffer & buffer = *reinterpret_cast< ComputeManager::Buffer* >( a_Pool.m_pCMemObj );
	const FormatInfo * fInf = &GetFormatInfo( a_Pool.m_Format );

	cmpmgr.SetCommandQueue( m_CommandQueue );

	auto & cbuff = ___ref_buffer( buffer );
	auto kernel0 = ___kernel_p( kernel );
	kernel0->setArg( 0, cbuff );
	kernel0->setArg( 1, a_ParticleInfo );
	kernel0->setArg( 2, a_Delta );

	if ( auto cqueue = ___ext_active_cqueue_p )
	{
		cqueue->enqueueNDRangeKernel( *kernel0, cl::NullRange, 
												cl::NDRange( NextMultipleOf( a_Pool.m_pCount, cmpmgr.GetMaxWorkGroupSize() ) ), 
												cl::NDRange( cmpmgr.GetMaxWorkGroupSize() ) );
		cqueue->finish();
		cqueue->enqueueReadBuffer( cbuff, true, 0, a_Pool.m_pCount * fInf->m_NumFloats * sizeof( float ), a_Pool.m_pHostMem );
	}
}

//=====================================================================================
void ParticleManager::ReSort( ParticleMemoryPool & a_Pool )
{
	ComputeManager & cmpmgr = ComputeManager::Instance();
	cmpmgr.SetCommandQueue( m_CommandQueue );

	if ( auto cqueue = ___ext_active_cqueue_p )
		cqueue->finish();

	const FormatInfo * fInf = &GetFormatInfo( a_Pool.m_Format );
	float * const begin = reinterpret_cast< float* >( a_Pool.m_pHostMem );
	
	float * last = begin + ( ( a_Pool.m_pCount - 1 ) * fInf->m_NumFloats );

	for ( float * it = last; it >= begin; it -= fInf->m_NumFloats )
	{
		if ( it[ fInf->m_OneCheckOffset ] == 1.0F )
		{
			BCopy( last, it, fInf->m_NumFloats * sizeof( float ) );
			last = begin + ( ( --a_Pool.m_pCount - 1 ) * fInf->m_NumFloats );
		}
	}

	auto & cbuff = ___ref_buffer( ( *reinterpret_cast< ComputeManager::Buffer* >( a_Pool.m_pCMemObj ) ) );
	if ( auto cqueue = ___ext_active_cqueue_p )
	{
		cqueue->enqueueWriteBuffer( cbuff, true, 0, a_Pool.m_pCount * fInf->m_NumFloats * sizeof( float ), a_Pool.m_pHostMem );
	}
}

//=====================================================================================
void ParticleManager::DrawGfxData( ParticleMemoryPool & a_Pool, ParticleShading a_ParticleShading )
{
	GraphicsManager::Instance().SetColour( Colour::ORANGE, GraphicsManager::COL_PRIMARY );

	GLMesh* mesh = reinterpret_cast< GLMesh* >( a_Pool.m_pVBuff );
	if ( !mesh )
	{
		ASSERT_WARN( false, __FUNCTION__ ": No mesh data to draw!" );
		return;
	}

	ComputeManager & cmpmgr = ComputeManager::Instance();
	cmpmgr.SetCommandQueue( m_CommandQueue );
	if ( auto cqueue = ___ext_active_cqueue_p )
	{
		cqueue->finish();
	}

	glFlush();

	GraphicsManager & gfxm = GraphicsManager::Instance();

	static_cast< GLShaderStorageBuffer* >( a_Pool.m_pSSBuff )->WriteBuffer( a_Pool.m_pHostMem, a_Pool.m_pHostMemSize );
	GLDraw::Instance().BindShaderStorageBuffer( static_cast< GLShaderStorageBuffer* >( a_Pool.m_pSSBuff ), 3 );

	// TEMP CODE
	static GraphicsManager::Texture myTex = nullptr;
	if ( !myTex )
	{
		Gradient< Colour > gradient1;
		gradient1.AddKeyFrame( 0.0F, Colour::WHITE );
		gradient1.AddKeyFrame( 0.2F, Colour::CYAN );
		gradient1.AddKeyFrame( 0.55F, Colour::VIOLET );
		gradient1.AddKeyFrame( 1.0F, Colour::VIOLET.WithAlpha( 0.0F ) );
	
		myTex = GPUGradient::CreateFromGradient( gradient1, 8 );
	}
	////////////
	gfxm.TfPushIdentity();
	gfxm.SetColour( Colour::WHITE, GraphicsManager::COL_PRIMARY );
	gfxm.GfxDrawQuadTextured( Vector2::ZERO, Vector2(64,1)*10.0F, gfxm.SetTexture(myTex), false);
	gfxm.TfPop();

	switch ( a_ParticleShading )
	{
	case ParticleManager::ParticleShading::PSHD_UBER:
	{
		auto & shader = m_ParticleShaders[ ( int32_t )a_ParticleShading ];

		int32_t transformLoc = gfxm.GetShaderGlobalLocation( shader, "uTransform" );
		int32_t tintGradsLoc = gfxm.GetShaderGlobalLocation( shader, "uTintGradient" );

		gfxm.SetShader( shader );
		gfxm.SetShaderGlobalFloat3x3( shader, transformLoc, gfxm.TfGetTop() );
		gfxm.SetShaderGlobalSampler( shader, tintGradsLoc, gfxm.SetTexture( myTex ) );
	}
		break;
	default: return;
	}
	
	gfxm.SetState( GraphicsManager::RS_ADDITIVE_BLEND, true );
	gfxm.SetState( GraphicsManager::RS_TRANSPARENCY, true );
	GLDraw::Instance().DrawMesh( mesh, a_Pool.m_pCount * 4 );
	gfxm.SetState( GraphicsManager::RS_TRANSPARENCY, false );
	gfxm.SetState( GraphicsManager::RS_ADDITIVE_BLEND, false );
}