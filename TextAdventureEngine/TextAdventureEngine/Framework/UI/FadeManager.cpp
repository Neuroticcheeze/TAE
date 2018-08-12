#include "FadeManager.hpp"
#include <Framework/Containers/CString.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Engine.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
FadeManager::FadeLayerData::FadeLayerData( const Colour & a_Source, const Colour & a_Target, float a_FadeTime, float a_HoldTime )
	: m_Source( a_Source )
	, m_Target( a_Target )
	, m_FadeTime( a_FadeTime )
	, m_HoldTime( a_HoldTime )
	, m_CurrentTime( 0.0F )
	, m_FadeBarrierID( 0 )
{
}

//=====================================================================================
bool FadeManager::FadeLayerData::Tick( float a_DeltaTime, Colour & a_Result )
{
	float t = Clamp( m_CurrentTime / m_FadeTime );
	a_Result = m_Source.Lerp( m_Target, t );
	m_CurrentTime += a_DeltaTime;
	return m_CurrentTime > ( m_FadeTime + m_HoldTime + a_DeltaTime );
}

//=====================================================================================
void FadeManager::Init()
{
	for ( uint32_t k = 0; k < LayerCount; ++k )
	{
		m_LayerResults[ k ] = Colour::INVISIBLE;

		while ( m_LayerDatas[ k ].Count() > 0 )
		{
			m_LayerDatas[ k ].Pop();
		}
	}
}

//=====================================================================================
void FadeManager::Finalise()
{
	Init();
}

//=====================================================================================
void FadeManager::Tick( float a_DeltaTime )
{
	for ( uint32_t k = 0; k < LayerCount; ++k )
	{
		Colour & result = m_LayerResults[ k ];
		Queue< FadeLayerData > & queue = m_LayerDatas[ k ];

		if ( queue.Count() > 0 )
		{
			if ( queue.Peek()->Tick( a_DeltaTime, result ) )
			{
				uint32_t barrier = queue.Peek()->m_FadeBarrierID;
				if ( barrier != 0 )
				{
					auto & dict = m_FadeBarrierListeners[ k ];
					if ( dict.Contains( barrier ) )
					{
						auto arr = dict[ barrier ];
						for ( uint32_t n = 0; n < arr->Count(); ++n )
						{
							if ( ( *arr )[ n ] )
							{
								( *arr )[ n ]->OnFadeBarrier( barrier );
							}
						}

						dict.Remove( barrier );
					}
				}

				queue.Pop();
			}
		}
	}
}

//=====================================================================================
void FadeManager::PushFadeData( Layer a_Layer, Colour a_Value, float a_FadeTime, float a_HoldTime )
{
	Queue< FadeLayerData > & queue = m_LayerDatas[ a_Layer ];
	const Colour & src = queue.Count() > 0 ? ( queue.Last()->m_Target ) : Colour::INVISIBLE;

	FadeLayerData newdat( src, a_Value, a_FadeTime, a_HoldTime );

	queue.Push( newdat );
}

//=====================================================================================
void FadeManager::PushFadeBarrier( Layer a_Layer, uint32_t a_FadeBarrierID )
{
	Queue< FadeLayerData > & queue = m_LayerDatas[ a_Layer ];
	if ( queue.Count() == 0 )
	{
		queue.Push( FadeLayerData( Colour::INVISIBLE, Colour::INVISIBLE, 0.0F, 0.0F ) );
	}

	queue.Last()->m_FadeBarrierID = a_FadeBarrierID;
}

//=====================================================================================
void FadeManager::ListenForFadeBarrier( Layer a_Layer, uint32_t a_FadeBarrierID, IListener & a_Listener )
{
	auto & dict = m_FadeBarrierListeners[ a_Layer ];

	if ( !dict.Contains( a_FadeBarrierID ) )
	{
		dict.Put( a_FadeBarrierID, Array< IListener * >() );
	}

	auto arr = dict[ a_FadeBarrierID ];
	if ( arr->IndexOf( &a_Listener ) == -1 )
	{
		arr->Append( &a_Listener );
	}
}

//=====================================================================================
void FadeManager::DrawLayer( Layer a_Layer ) const
{
	GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
	GraphicsManager::Instance().SetColour( m_LayerResults[ a_Layer ], GraphicsManager::COL_PRIMARY );
	GraphicsManager::Instance().GfxDrawQuad(
		Vector2::ZERO, 
		{ Engine::Instance().GetDisplaySize().x,
		  Engine::Instance().GetDisplaySize().y } );
}