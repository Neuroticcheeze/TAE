#include "ViewSizer.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewSizer::ViewSizer( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Drag( false )
{
	SetEnabled();
	SetIsInteractible( true );
}

//=====================================================================================
void ViewSizer::OnTick( float a_DeltaTime )
{
	Vector2 dir;
	dir.x = m_XAxor == View::Alignment::LEFT ? -1.0F : ( m_XAxor == View::Alignment::RIGHT ? 1.0F : 0.0F );
	dir.y = m_YAxor == View::Alignment::BOTTOM ? -1.0F : ( m_YAxor == View::Alignment::TOP ? 1.0F : 0.0F );

	float ang = ATan2F( dir.x, dir.y ) * RAD2DEG;

	GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_PRIMARY );
	GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_SECONDARY );
	GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_TERTIARY );

	Vector2 a = 0.2F - 0.5F;
	Vector2 b = 0.8F - 0.5F;

	GraphicsManager::Instance().TfPush();
	GraphicsManager::Instance().TfTranslate( GetPosition() );
	GraphicsManager::Instance().TfScale( GetSize() );
	GraphicsManager::Instance().TfTranslate( Vector2::ONE * 0.5F );
	GraphicsManager::Instance().TfRotate( -45.0F - ang );
	GraphicsManager::Instance().GfxDrawTriangle( a, b, Vector2( b.x, a.y ) );
	GraphicsManager::Instance().TfPop();

	if ( !m_Drag )
	{
		return;
	}

	Vector2 parentPosition;
	if ( GetParent() )
	{
		parentPosition = GetParent()->GetPosition();
	}

	if ( IsBeingDragged() && GetParent() )
	{
		Vector2 n = InputManager::Instance().GetMousePosition() / GetParent()->GetParent()->GetSize();

		//n.x = Max( m_MinXYSizeF.x, n.x );
		//n.y = Min( m_MinXYSizeF.y, n.y );

		if ( m_XAxor != View::Alignment::UNSPECIFIED ) GetParent()->SetBorder( m_XAxor, m_XAxor == View::Alignment::RIGHT ? ( 1.0F - n.x ) : n.x );
		if ( m_YAxor != View::Alignment::UNSPECIFIED ) GetParent()->SetBorder( m_YAxor, m_YAxor == View::Alignment::TOP ? ( 1.0F - n.y ) : n.y  );
	}
}

//=====================================================================================
void ViewSizer::OnMouseEnter( const Vector2 & m_MousePosition )
{
}

//=====================================================================================
void ViewSizer::OnMouseLeave( const Vector2 & a_MousePosition )
{
}

//=====================================================================================
void ViewSizer::OnMouseClick( const Vector2 & a_MousePosition, InputManager::MouseButton a_MouseButton )
{
	m_Drag = true;
}

//=====================================================================================
void ViewSizer::OnMouseReleased( const Vector2 & a_MousePosition, InputManager::MouseButton a_MouseButton )
{
	m_Drag = false;
}