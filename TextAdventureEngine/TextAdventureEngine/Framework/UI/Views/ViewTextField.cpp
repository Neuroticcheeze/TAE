#include "ViewTextField.hpp"
#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewTextField::ViewTextField( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent, true )
	, m_Background( "Background", a_ContainerPage, this )
	, m_Text( "Text", a_ContainerPage, this )
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

	m_Text.SetBorder( View::Alignment::LEFT, 0.0F );
	m_Text.SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Text.SetBorder( View::Alignment::TOP, 0.0F );
	m_Text.SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Text.SetVAlign( ViewText::TextVAlignment::CENTER );
	m_Text.SetHAlign( ViewText::TextHAlignment::MIDDLE );
	m_Text.SetFont( _GetFont( "DEFAULT" ) );
	m_Text.SetFontSize( 72.0F );
	m_Text.SetTint( Colour::WHITE );
	AddChild( &m_Text );

	InputManager::Instance().AttachListener( InputManager::InputCharEvent::ON_TEXTUAL_CHAR_TYPED, this );
}

//=====================================================================================
void ViewTextField::Destruct()
{
	InputManager::Instance().DetatchListener( InputManager::InputCharEvent::ON_TEXTUAL_CHAR_TYPED, this );
}

//=====================================================================================
void ViewTextField::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void ViewTextField::OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
}

//=====================================================================================
void ViewTextField::RequestInitialEvents( IActionListener * a_ActionListener )
{
	if ( m_Enabled )
	{
		a_ActionListener->OnTextFieldEnabled( *this );
	}
	else
	{
		a_ActionListener->OnTextFieldDisabled( *this );
	}
	
	if ( HasFocus() )
	{
		a_ActionListener->OnTextFieldFocus( *this );
	}

	a_ActionListener->OnTextFieldValueChanged( *this, GetTextView().GetText() );
}

//=====================================================================================
void ViewTextField::OnCharTyped( char a_Char )
{
	if ( GetEnabled() )
	{
		switch ( a_Char )
		{
		//case InputManager::TEXTCODE_NEWLINE:
		//	break;
		case InputManager::TEXTCODE_BACKSPACE:
			{
				const CString & text = GetTextView().GetText();
				const uint32_t length = text.Length();
				if ( length > 0 )
				{
					GetTextView().SetText( text.SubString( 0, length - 1 ).Get() );
				}
			}
			break;
		default:
			GetTextView().SetText( ( GetTextView().GetText() + a_Char ).Get() );
		}

		GetParent()->OnTextFieldValueChanged( *this, GetTextView().GetText() );
	}
}

//=====================================================================================
void ViewTextField::SetEnabled( bool a_Enabled )
{
	if ( a_Enabled != m_Enabled )
	{
		if ( m_Enabled = a_Enabled ) // when enabled
		{
			SetTint( Colour::WHITE );
			GetParent()->OnTextFieldEnabled( *this );
		}

		else // when disabled
		{
			SetTint( Colour::GRAY.Lerp( Colour::RED, 0.5F ) );
			GetParent()->OnTextFieldDisabled( *this );
		}
	}
}

//=====================================================================================
void ViewTextField::OnGainFocus()
{
	InputManager::Instance().SetTextualMode( true );
	SetEnabled( true );
}

//=====================================================================================
void ViewTextField::OnLoseFocus()
{
	InputManager::Instance().SetTextualMode( false );
	SetEnabled( false );
}