#include "ViewColour.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewColour::ViewColour( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
{
	SetEnabled();
	SetIsInteractible( true );
}

//=====================================================================================
void ViewColour::OnTick( float a_DeltaTime )
{
	GraphicsManager::Instance().SetColour( Colour( 1.0F, 0.0F, 0.0F ), GraphicsManager::COL_PRIMARY );
	GraphicsManager::Instance().SetColour( Colour( 0.0F, 1.0F, 0.0F ), GraphicsManager::COL_SECONDARY );
	GraphicsManager::Instance().SetColour( Colour( 0.0F, 0.0F, 1.0F ), GraphicsManager::COL_TERTIARY );

	GraphicsManager::Instance().TfPush();
	GraphicsManager::Instance().TfTranslate( GetPosition() );
	GraphicsManager::Instance().TfScale( GetSize() );
	GraphicsManager::Instance().GfxDrawTriangle( Vector2::ZERO, Vector2::UP, Vector2::ONE );
	GraphicsManager::Instance().TfPop();

}

//=====================================================================================
void ViewColour::OnMouseEnter( const Vector2 & m_MousePosition )
{
}

//=====================================================================================
void ViewColour::OnMouseLeave( const Vector2 & a_MousePosition )
{
}

//=====================================================================================
void ViewColour::OnMouseClick( const Vector2 & a_MousePosition, InputManager::MouseButton a_MouseButton )
{
}