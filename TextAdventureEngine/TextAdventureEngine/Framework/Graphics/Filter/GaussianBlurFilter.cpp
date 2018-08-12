#include "GaussianBlurFilter.hpp"
#include <Framework/Utils/Hash.hpp>
#include <Framework/Engine.hpp>
#include <Framework/Graphics/GL/GLMesh.hpp>
#include <Framework/Graphics/GL/GLDraw.hpp>

//=====================================================================================
GaussianBlurFilter::GaussianBlurFilter()
{
	m_HVPass = GraphicsManager::Instance().LoadShader( WSID( "GaussianBlurHVPass" ), "gaussian_blur.shader", WSID( "MAIN" ), WSID( "HVPASS" ) );
	
	m_HVPassProp_BlurRadius = GraphicsManager::Instance().GetShaderGlobalLocation( m_HVPass, "uBlurRadius" );
	m_HVPassProp_SrcTex = GraphicsManager::Instance().GetShaderGlobalLocation( m_HVPass, "uPrevPassResult" );

	m_FullQuad = GLMesh::CreateQuad( "GaussianBlurFullQuad", -Vector2::ONE );
}

//=====================================================================================
GaussianBlurFilter::~GaussianBlurFilter()
{
	GraphicsManager::Instance().UnloadShader( m_HVPass );
	delete m_FullQuad;
	m_FullQuad = nullptr;
}

//=====================================================================================
bool GaussianBlurFilter::Dispatch()
{
	// Must have h+v shader for obvious reasons..
	// And, must have an active rendertarget
	if ( ! m_HVPass ||
		 ! GraphicsManager::Instance().GetActiveRenderTexture() )
	{
		return false;
	}

	auto gm = GraphicsManager::Instance();
	auto gldraw = GLDraw::Instance();

	// Perform the horizontal pass
	gm.ActivateRTDrawBuffers( { 1 }, false );
	//gm.ActivateRTDrawBuffers( { static_cast< uint32_t >( m_Parameters.m_VDest.m_oRTColIndex ) }, false );
	//gm.SetShaderGlobalFloat1( m_HPass, m_HPassProp_BlurRadius, m_Parameters.m_pRadius );
	gm.SetShaderGlobalSampler( m_HVPass, m_HVPassProp_SrcTex, m_Parameters.m_iSource );
	gm.SetShader( m_HVPass );
	gldraw.DrawMesh( m_FullQuad );

	// Set Horizontal-Pass result as source for Vertical-Pass
	//int32_t hpassSrcSampler = gm.SetTexture( gm.GetActiveRenderTexture(), m_Parameters.m_HDest.m_oRTColIndex, 7, false );
	//
	//// Perform the vertical pass
	//gm.ActivateRTDrawBuffers( { static_cast< uint32_t >( m_Parameters.m_VDest.m_oRTColIndex ) }, false );
	//gm.SetShader( m_VPass );
	//gm.SetShaderGlobalFloat1( m_VPass, m_VPassProp_BlurRadius, m_Parameters.m_pRadius );
	//gm.SetShaderGlobalSampler( m_VPass, m_VPassProp_SrcTex, hpassSrcSampler );
	//gldraw.DrawMesh( m_FullQuad );

	return true;
}