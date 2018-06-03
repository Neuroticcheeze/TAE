#include "ViewButton.hpp"
#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewButton::ViewButton( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Background( "Background", a_ContainerPage, this )
	, m_Label( "Label", a_ContainerPage, this )
	, m_Enabled( false )
{
	SetEnabled( true );

	m_Background.SetBorder( View::Alignment::LEFT, 0.0F );
	m_Background.SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Background.SetBorder( View::Alignment::TOP, 0.0F );
	m_Background.SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Background.SetZOrder( 1 );
	m_Background.SetTint( Colour::WHITE );
	AddChild( &m_Background );

	m_Label.SetBorder( View::Alignment::LEFT, 0.0F );
	m_Label.SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Label.SetBorder( View::Alignment::TOP, 0.0F );
	m_Label.SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Label.SetVAlign( ViewText::TextVAlignment::CENTER );
	m_Label.SetHAlign( ViewText::TextHAlignment::MIDDLE );
	m_Label.SetFont( _GetFont( "DEFAULT" ) );
	m_Label.SetFontSize( 72.0F );
	m_Label.SetTint( Colour::WHITE );
	AddChild( &m_Label );
}

//=====================================================================================
void ViewButton::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void ViewButton::OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
	GetParent()->OnButtonPress( *this );
}

//=====================================================================================
void ViewButton::RequestInitialEvents( IActionListener * a_ActionListener )
{
	if ( m_Enabled )
	{
		a_ActionListener->OnButtonEnabled( *this );
	}
	else
	{
		a_ActionListener->OnButtonDisabled( *this );
	}
}

//=====================================================================================
void ViewButton::SetEnabled( bool a_Enabled )
{
	if ( a_Enabled != m_Enabled )
	{
		if ( m_Enabled = a_Enabled ) // when enabled
		{
			SetTint( Colour::WHITE );
			GetParent()->OnButtonEnabled( *this );
		}

		else // when disabled
		{
			SetTint( Colour::GRAY.Lerp( Colour::RED, 0.5F ) );
			GetParent()->OnButtonDisabled( *this );
		}
	}
}