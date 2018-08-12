#include "ViewTickBox.hpp"
#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewTickBox::ViewTickBox( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Enabled( false )
	, m_Flag( false )
{
	SetEnabled( true );

	m_Sprites[ SPRITE_BACKGROUND ].SetBorder( View::Alignment::LEFT, 0.0F );
	m_Sprites[ SPRITE_BACKGROUND ].SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Sprites[ SPRITE_BACKGROUND ].SetBorder( View::Alignment::TOP, 0.0F );
	m_Sprites[ SPRITE_BACKGROUND ].SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Sprites[ SPRITE_BACKGROUND ].SetZOrder( 1 );
	m_Sprites[ SPRITE_BACKGROUND ].SetTint( Colour::WHITE );
	AddChild( m_Sprites + SPRITE_BACKGROUND );

	m_Sprites[ SPRITE_TICKMARK ].SetBorder( View::Alignment::LEFT, 0.0F );
	m_Sprites[ SPRITE_TICKMARK ].SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Sprites[ SPRITE_TICKMARK ].SetBorder( View::Alignment::TOP, 0.0F );
	m_Sprites[ SPRITE_TICKMARK ].SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Sprites[ SPRITE_TICKMARK ].SetTint( Colour::WHITE );
	AddChild( m_Sprites + SPRITE_TICKMARK );

	SetValue( true );
}

//=====================================================================================
void ViewTickBox::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void ViewTickBox::OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
	SetValue( !GetValue() );
}

//=====================================================================================
void ViewTickBox::RequestInitialEvents( IActionListener * a_ActionListener )
{
	a_ActionListener->OnTickBoxValueChanged( *this, GetValue() );
}

//=====================================================================================
void ViewTickBox::SetEnabled( bool a_Enabled )
{
	if ( a_Enabled != m_Enabled )
	{
		if ( m_Enabled = a_Enabled ) // when enabled
		{
			SetTint( Colour::WHITE );
		}

		else // when disabled
		{
			SetTint( Colour::GRAY.Lerp( Colour::RED, 0.5F ) );
		}
	}
}

//=====================================================================================
void ViewTickBox::SetValue( bool a_Value )
{
	if ( m_Flag != a_Value )
	{
		m_Flag = a_Value;
		m_Sprites[ SPRITE_TICKMARK ].SetTint( m_Flag ? Colour::WHITE : Colour::INVISIBLE );
		GetParent()->OnTickBoxValueChanged( *this, a_Value );
	}
}