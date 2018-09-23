#include "ViewNodePlug.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Math/Curve/Bezier.hpp>
#include <Framework/Engine.hpp>
#include <[Tool] Dialogue Editor/Globals.hpp>

//=====================================================================================
Set< ViewNodePlug* > ViewNodePlug::m_GlobalList;

//=====================================================================================
ViewNodePlug::ViewNodePlug( uint32_t a_Id, uint32_t a_Sub, Type a_Type, const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Id( a_Id )
	, m_Sub( a_Sub )
	, m_Type( a_Type )
	, m_Drag( false )
	, m_Connection( nullptr )
{
	SetEnabled();
	SetIsInteractible( true );
}

//=====================================================================================
void ViewNodePlug::OnEnabled()
{
	m_GlobalList.Insert( this );
}

//=====================================================================================
void ViewNodePlug::OnDisabled()
{
	m_GlobalList.Remove( this );
}

//=====================================================================================
void ViewNodePlug::OnTick( float a_DeltaTime )
{
	if ( m_Type == Type::OUTPUT )
	{
		GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_PRIMARY );
		GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_SECONDARY );
		GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_TERTIARY );

		GraphicsManager::Instance().TfPush();
		GraphicsManager::Instance().TfTranslate( GetPosition() );
		GraphicsManager::Instance().TfScale( GetSize() );
		GraphicsManager::Instance().TfTranslate( Vector2::ONE * 0.5F );
		GraphicsManager::Instance().TfScale( 0.5F );
		GraphicsManager::Instance().TfTranslate( Vector2::ONE * -0.5F );
		GraphicsManager::Instance().GfxDrawQuad( Vector2::ZERO, Vector2( 0.5F, 1.0F ) );
		GraphicsManager::Instance().GfxDrawTriangle( Vector2( 0.5F, 0.0F ), Vector2( 0.5F, 1.0F ), Vector2( 1.0F, 0.5F ) );
		GraphicsManager::Instance().TfPop();

		if ( m_Drag )
		{
			int32_t tag = -1;
			auto it = m_GlobalList.First();
			auto end = m_GlobalList.End();
			while ( it != end )
			{
				ViewNodePlug & plug = ( **it );

				if ( plug.GetPlugType() == ViewNodePlug::Type::INPUT &&
					 plug.m_Id != m_Id && 
					 plug.m_Connection != this &&
					 plug.GetMouseOver() )
				{
					tag = ( plug.m_Connection ? 1 : 0 ) | ( m_Connection ? 2 : 0 );

					break;
				}

				++it;
			}
			const float pulse = Clamp( Sin( Engine::Instance().GetTime() * 4.0F ) * 0.25F + 0.75F );
			Colour tint = GetTint();
			switch ( tag )
			{
			case 0: // When about to connect to vacant input
				tint = Colour::GREEN; 
				break;
			case 1: // When about to connect to occupied input
				tint = Colour::RED.Lerp( Colour::ORANGE, 0.3F ).WithSaturation( 0.8F ).WithAlpha( pulse ); 
				break;
			case 2: // When about to connect to vacant input, but already connected
				tint = Colour::COBALT.Lerp( Colour::CYAN, 0.3F ).WithSaturation( 0.8F ).WithAlpha( pulse ); 
				break;
			case 1 | 2: // When about to connect to occupied input, but already connected
				tint = Colour::VIOLET.Lerp( Colour::MAGENTA, 0.2F ).WithSaturation( 0.8F ).WithAlpha( pulse ); 
				break;
			default:
				break;
			}

			float f = Abs( InputManager::Instance().GetMousePosition().x - ( GetPosition() + GetSize() * 0.5F ).x ) / 1.5F;
			const Vector2 bezierStart = GetPosition() + GetSize() * Vector2( 0.725F, 0.5F );

			Bezier bezier;
			bezier.AddControlPoint( bezierStart );
			bezier.AddControlPoint( bezierStart + Vector2::RIGHT * f );
			bezier.AddControlPoint( InputManager::Instance().GetMousePosition() + Vector2::RIGHT * -f );
			bezier.AddControlPoint( InputManager::Instance().GetMousePosition() );

			GraphicsManager::Instance().SetColour( tint, GraphicsManager::COL_PRIMARY );
			GraphicsManager::Instance().SetColour( Colour::INVISIBLE, GraphicsManager::COL_SECONDARY );
			GraphicsManager::Instance().GfxDrawBezier( bezier, 4.0F, 30.0F );

			CString str = CString().Format("0x%llu <-> 0x%llu", (unsigned long long)this, (unsigned long long)m_Connection);
			GraphicsManager::Instance().TfPush();
			GraphicsManager::Instance().TfTranslate( bezier.Evaluate( 0.5F ) + Vector2::UP * 32.0F );
			GraphicsManager::Instance().GfxDrawText( str.Get(), GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultTiny ), 999, false, GraphicsManager::TA_CENTER, GraphicsManager::TA_CENTER );
			GraphicsManager::Instance().TfPop();
		}
		else if ( m_Connection )
		{
			const Vector2 bezierEnd = m_Connection->GetPosition() + m_Connection->GetSize() * Vector2( 0.725F, 0.5F );

			float f = Abs( bezierEnd.x - ( GetPosition() + GetSize() * 0.5F ).x ) / 1.5F;
			const Vector2 bezierStart = GetPosition() + GetSize() * Vector2( 0.725F, 0.5F );

			Bezier bezier;
			bezier.AddControlPoint( bezierStart );
			bezier.AddControlPoint( bezierStart + Vector2::RIGHT * f );
			bezier.AddControlPoint( bezierEnd + Vector2::RIGHT * -f );
			bezier.AddControlPoint( bezierEnd );

			GraphicsManager::Instance().GfxDrawBezier( bezier, 4.0F );

			CString str = CString().Format("0x%llu <-> 0x%llu", (unsigned long long)this, (unsigned long long)m_Connection);
			GraphicsManager::Instance().TfPush();
			GraphicsManager::Instance().TfTranslate( bezier.Evaluate( 0.5F ) + Vector2::UP * 32.0F );
			GraphicsManager::Instance().GfxDrawText( str.Get(), GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultTiny ), 999, false, GraphicsManager::TA_CENTER, GraphicsManager::TA_CENTER );
			GraphicsManager::Instance().TfPop();
		}
	}

	else
	{
		GraphicsManager::Instance().TfPush();
		GraphicsManager::Instance().GfxDrawQuad( GetPosition() + Vector2( 0.0F, GetSize().y * 0.25F ), GetSize() * Vector2( 1.0F, 0.5F ) );
		GraphicsManager::Instance().TfPop();
	}
}

//=====================================================================================
void ViewNodePlug::OnMouseEnter( const Vector2 & m_MousePosition )
{
}

//=====================================================================================
void ViewNodePlug::OnMouseLeave( const Vector2 & a_MousePosition )
{
}

//=====================================================================================
void ViewNodePlug::OnMouseClick( const Vector2 & a_MousePosition, InputManager::MouseButton a_MouseButton )
{
	if ( m_Type == Type::OUTPUT )
	{
		m_Drag = true;
	}
}

//=====================================================================================
void ViewNodePlug::OnMouseReleased( const Vector2 & a_MousePosition, InputManager::MouseButton a_MouseButton )
{
	if ( m_Drag )
	{
		auto it = m_GlobalList.First();
		auto end = m_GlobalList.End();

		while ( it != end )
		{
			ViewNodePlug & plug = ( **it );

			if ( plug.GetPlugType() == ViewNodePlug::Type::INPUT &&
				 plug.m_Id != m_Id && 
				 &plug != this &&
				 plug.GetMouseOver() )
			{
				if ( m_Connection )// clear prev connection pairing
				{
					auto prevConnConn = m_Connection;

					m_Connection->m_Connection->m_Connection = nullptr;
					prevConnConn->m_Connection->OnConnectionChanged();
					prevConnConn->m_Connection = nullptr;
					prevConnConn->OnConnectionChanged();
				}

				m_Connection = &plug;
				OnConnectionChanged();

				if ( m_Connection->m_Connection )
				{
					auto prevConnConn = m_Connection;
					m_Connection->m_Connection->m_Connection = nullptr;
					prevConnConn->m_Connection->OnConnectionChanged();
				}

				m_Connection->m_Connection = this;
				m_Connection->OnConnectionChanged();

				break;
			}

			++it;
		}

		if ( it == end )
		{
			if ( m_Connection )// clear prev connection pairing
			{
				auto prevConnConn = m_Connection;

				m_Connection->m_Connection->m_Connection = nullptr;
				prevConnConn->m_Connection->OnConnectionChanged();

				prevConnConn->m_Connection = nullptr;
				prevConnConn->OnConnectionChanged();
			}
		}
	}

	m_Drag = false;
}

//=====================================================================================
void ViewNodePlug::OnConnectionChanged()
{
	if ( m_Connection == nullptr )
	{
		PRINT( "Node Connection Changed [t=%d, id=%d, sub=%d]->[id=%d, sub=%d]", (int)GetPlugType(), m_Id, m_Sub, 0, 0 );
		return;
	}

	PRINT( "Node Connection Changed [t=%d, id=%d, sub=%d]->[id=%d, sub=%d]", (int)GetPlugType(), m_Id, m_Sub, m_Connection->m_Id, m_Connection->m_Sub );

	if ( OnConnectEventFunctor )
	{
		OnConnectEventFunctor( *this );
	}
}