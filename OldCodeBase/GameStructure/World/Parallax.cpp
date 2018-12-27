#include "Parallax.hpp"
#include <GameStructure/Globals.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Engine.hpp>

//=====================================================================================
Parallax::Parallax( const Array< Layer > & a_Layers )
	: m_LayerArray( a_Layers )
{}

//=====================================================================================
void Parallax::DrawComposite( float a_DeltaTime )
{
	if ( m_LayerArray.Count() == 0 )
	{
		ASSERT_WARN( false, "Parallax::DrawComposite: No layers to draw!" );
		return;
	}

	const Vector2 camOffset = m_ParallaxCamera.m_CameraOffset;
	const Vector2 camViewArea = m_ParallaxCamera.m_CameraViewArea;
	const float camNScale = m_ParallaxCamera.m_ForeScale;
	const float camFScale = m_ParallaxCamera.m_BackScale;

	// Descending order so farthest layers get drawn first.
	m_LayerArray.Sort( []( const Layer & a_LHS, const Layer & a_RHS ){ return a_LHS.GetDistance() > a_RHS.GetDistance(); } );

	const float furthestLayerDist = m_LayerArray.Last()->m_Distance;
	const float nearestLayerDist = m_LayerArray.First()->m_Distance;

	GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
	GraphicsManager::Instance().SetColour( Colour::WHITE, GraphicsManager::COL_PRIMARY );

	auto it = m_LayerArray.First();
	const auto end = m_LayerArray.End();
	while ( it != end )
	{
		const float layerDist = it->m_Distance;

		const float layerDistF = 1.0F - ( ( layerDist - nearestLayerDist ) / ( furthestLayerDist - nearestLayerDist ) );
		float layerFinalScale = Lerp( camNScale, camFScale, layerDistF );

		ASSERT_WARN( layerFinalScale >= 1.0F, "Final layer scaling is less than 1.0! Either the Parallax ForeScale or BackScale is less than 1.0 - Please revise." );
		layerFinalScale = Max( 1.0F, layerFinalScale * it->m_PerspectiveModifier.m_Upscale );

		// Calculate final draw parameters for the GraphicsManager draw call.
		const Vector2 drawSize = camViewArea * layerFinalScale;
		const Vector2 drawPos = ( -camOffset * camViewArea * ( layerFinalScale - 1.0F ) * it->m_PerspectiveModifier.m_Movability ) + ( -drawSize * 0.5F ) + ( Engine::Instance().GetDisplaySize() * 0.5F );

		const Vector2 scale0 = camViewArea / Engine::Instance().GetDisplaySize();

		const bool hasListeners = it->m_Listeners.Count() > 0;

		if ( hasListeners )
		{
			GraphicsManager::Instance().TfPush();
			GraphicsManager::Instance().TfTranslate( drawPos );
			GraphicsManager::Instance().TfScale( scale0 * layerFinalScale );

			auto itListener = it->m_Listeners.First();
			auto endListener = it->m_Listeners.End();
			while ( itListener != endListener )
			{
				( *itListener )->OnPreDraw( *it );
				++itListener;
			}

			GraphicsManager::Instance().TfPop();

			//Restore previous colour+transp state for parallax layer draw
			GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
			GraphicsManager::Instance().SetColour( Colour::WHITE, GraphicsManager::COL_PRIMARY );
		}

		uint32_t samp = GraphicsManager::Instance().SetTexture( it->m_Texture );
		GraphicsManager::Instance().GfxDrawQuadTextured( drawPos, drawSize, samp, false, 7.5F );

		if ( hasListeners )
		{
			GraphicsManager::Instance().TfPush();
			GraphicsManager::Instance().TfTranslate( drawPos );
			GraphicsManager::Instance().TfScale( scale0 * layerFinalScale );

			auto itListener = it->m_Listeners.First();
			auto endListener = it->m_Listeners.End();
			while ( itListener != endListener )
			{
				( *itListener )->OnPostDraw( *it );
				++itListener;
			}

			GraphicsManager::Instance().TfPop();

			if ( it != m_LayerArray.Last() )
			{
				//Restore previous colour+transp state for parallax layer draw
				GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
				GraphicsManager::Instance().SetColour( Colour::WHITE, GraphicsManager::COL_PRIMARY );
			}
		}

		++it;
	}

	GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, false );
}

//=====================================================================================
const Parallax::Layer & Parallax::operator[]( int32_t a_Index ) const
{
	return m_LayerArray[ ( uint32_t )a_Index ];
}

//=====================================================================================
Parallax::Layer & Parallax::operator[]( int32_t a_Index )
{
	return m_LayerArray[ ( uint32_t )a_Index ];
}

//=====================================================================================
void Parallax::RecalculateCameraOffset()
{
	Vector2 & offset = m_ParallaxCamera.m_CameraOffset;
	
	offset.x = Clamp( offset.x, -0.5F, 0.5F );
	offset.x = Cos( ( offset.x - 0.5F ) * PI ) * 0.5F;

	offset.y = Clamp( offset.y, -0.5F, 0.5F );
	offset.y = Cos( ( offset.y - 0.5F ) * PI ) * 0.5F;
}

//=====================================================================================
void Parallax::Layer::RegisterListener( IListener & a_Listener )
{
	m_Listeners.Insert( &a_Listener );
}

//=====================================================================================
void Parallax::Layer::DeregisterListener( IListener & a_Listener )
{
	m_Listeners.Remove( &a_Listener );
}