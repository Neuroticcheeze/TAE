#include "ViewButton.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewButton::ViewButton( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Background( "Background", a_ContainerPage, this )
	, m_Label( "Label", a_ContainerPage, this )
{
	SetEnabled();
	SetIsInteractible( true );

	m_Background.SetBorder( View::Alignment::LEFT, 0.0F );
	m_Background.SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Background.SetBorder( View::Alignment::TOP, 0.0F );
	m_Background.SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Background.SetZOrder( 1 );
	m_Background.SetTint( Colour::WHITE );
	AddChild( &m_Background );

	m_Label.SetBorder( View::Alignment::LEFT, 0.5F );
	m_Label.SetBorder( View::Alignment::RIGHT, 0.5F );
	m_Label.SetBorder( View::Alignment::TOP, 0.5F );
	m_Label.SetBorder( View::Alignment::BOTTOM, 0.5F );
	m_Label.SetVAlign( GraphicsManager::TA_CENTER );
	m_Label.SetHAlign( GraphicsManager::TA_CENTER );
	m_Label.SetFont( _GetFont( "DEFAULT" ) );
	m_Label.SetFontSize( 32.0F );
	m_Label.SetTint( Colour::WHITE );
	AddChild( &m_Label );
}

//=====================================================================================
void ViewButton::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void ViewButton::OnMouseEnter( const Vector2 & m_MousePosition )
{
	if ( GetEnabled() )
	{
		m_Background.SetTint( Colour::YELLOW );
	}
}

//=====================================================================================
void ViewButton::OnMouseLeave( const Vector2 & m_MousePosition )
{
	if ( GetEnabled() )
	{
		m_Background.SetTint( Colour::WHITE );
	}
}

//=====================================================================================
void ViewButton::OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
	if ( GetEnabled() )
	{
		GetParent()->OnButtonPress( *this );
		m_Background.SetTint( Colour::ORANGE );
	}
}

//=====================================================================================
void ViewButton::OnMouseReleased( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
	if ( GetEnabled() )
	{
		m_Background.SetTint( Colour::WHITE );
	}
}

//=====================================================================================
void ViewButton::RequestInitialEvents( IActionListener * a_ActionListener )
{
	View::RequestInitialEvents( a_ActionListener );
}

//=====================================================================================
void ViewButton::OnEnabled()
{
	SetTint( Colour::WHITE );
}

//=====================================================================================
void ViewButton::OnDisabled()
{
	SetTint( Colour::GRAY.Lerp( Colour::RED, 0.5F ) );
}