#include "ViewTextField.hpp"
#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Engine.hpp>

//=====================================================================================
ViewTextField::ViewTextField( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent, true )
	, m_Background( "Background", a_ContainerPage, this )
	, m_Text( "Text", a_ContainerPage, this )
	, m_EmptyTextView( "TextEmpty", a_ContainerPage, this )
	, m_MultilineEnabled( false )
	, m_CursorPosition( 0 ) // Cursor is initially at the end
	, m_CachedTextLength( 0 )
{
	SetEnabled();
	SetIsInteractible( true );
	SetTint( Colour::WHITE );

	m_Background.SetBorder( View::Alignment::LEFT, 0.0F );
	m_Background.SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Background.SetBorder( View::Alignment::TOP, 0.0F );
	m_Background.SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Background.SetZOrder( 1 );
	m_Background.SetTint( Colour::WHITE );
	AddChild( &m_Background );

	m_Text.SetBorder( View::Alignment::LEFT, 0.05F );
	m_Text.SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Text.SetBorder( View::Alignment::TOP, 0.5F );
	m_Text.SetBorder( View::Alignment::BOTTOM, 0.5F );
	m_Text.SetWordWrap( false );
	m_Text.SetMultiVAlign( false );
	m_Text.SetVAlign( GraphicsManager::TA_CENTER );
	m_Text.SetHAlign( GraphicsManager::TA_LEFT );
	m_Text.SetFont( _GetFont( "DEFAULT" ) );
	m_Text.SetFontSize( 32.0F );
	m_Text.SetTint( Colour::WHITE );
	AddChild( &m_Text );

	
	m_EmptyTextView.SetBorder( View::Alignment::LEFT, 0.05F );
	m_EmptyTextView.SetBorder( View::Alignment::RIGHT, 0.0F );
	m_EmptyTextView.SetBorder( View::Alignment::TOP, 0.0F );
	m_EmptyTextView.SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_EmptyTextView.SetVAlign( GraphicsManager::TA_CENTER );
	m_EmptyTextView.SetHAlign( GraphicsManager::TA_LEFT );
	m_EmptyTextView.SetFont( _GetFont( "DEFAULT" ) );
	m_EmptyTextView.SetFontSize( 32.0F );
	m_EmptyTextView.SetTint( Colour::RED.Lerp( Colour::INVISIBLE, 0.5F ) );
	m_EmptyTextView.SetSkew( true );
	AddChild( &m_EmptyTextView );

	InputManager::Instance().AttachListener( InputManager::InputCharEvent::ON_TEXTUAL_CHAR_TYPED, this );
	InputManager::Instance().AttachListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );

	RefreshText();
}

//=====================================================================================
void ViewTextField::Destruct()
{
	InputManager::Instance().DetatchListener( InputManager::InputCharEvent::ON_TEXTUAL_CHAR_TYPED, this );
	InputManager::Instance().DetatchListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
}

//=====================================================================================
void ViewTextField::OnTick( float a_DeltaTime )
{
	uint32_t len = CString( m_Text.GetText() ).Length();
	if ( m_CachedTextLength != len )
	{
		m_CachedTextLength = len;
		RefreshText();
	}
}

//=====================================================================================
void ViewTextField::OnTickPost( float a_DeltaTime )
{
	if ( HasFocus() )
	{
		// pulse cursor
		if ( ModInterval( Engine::Instance().GetTime(), 0.5F ) )
		{
			const float height = m_TextBB.lineHeight;
			const float cursorThickness = Max( 2.0F, height / 13.5F ); // 2:27 ratio
			float cursorOffset = m_TextBB.xOffset;

			GraphicsManager::Instance().SetColour( m_Text.GetTint() * 2.0F, GraphicsManager::COL_PRIMARY );
			GraphicsManager::Instance().GfxDrawQuad( m_Text.GetPosition() + Vector2( cursorOffset, -height / 2.0F - m_TextBB.yOffset ), Vector2( cursorThickness, height ) );
		}
	}
}

//=====================================================================================
void ViewTextField::OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
}

//=====================================================================================
void ViewTextField::RequestInitialEvents( IActionListener * a_ActionListener )
{
	View::RequestInitialEvents( a_ActionListener );
	
	if ( HasFocus() )
	{
		a_ActionListener->OnTextFieldFocus( *this );
	}

	a_ActionListener->OnTextFieldValueChanged( *this, GetTextView().GetText() );
}

//=====================================================================================
void ViewTextField::RefreshCursor()
{
	CString text = CString( m_Text.GetText() );

	m_CursorPosition = Clamp( m_CursorPosition, 0, ( int32_t )text.Length() );

	text = text.SubString( 0, m_CursorPosition );

	uint32_t numNewLines = text.FindAll( "\n" ).Count();
	int32_t lastNewline = Max( 0, text.FindLast( "\n" ) + 1 );
	text = text.SubString( lastNewline, text.Length() - lastNewline );

	Vector2 size;

	if ( m_Text.GetBitmapFont() )
	{
		size = GraphicsManager::Instance().QueryTextSize( text.Get(), 
															  m_Text.GetBitmapFont(), m_CursorPosition );
	}
	else if ( m_Text.GetFont() )
	{
		size = GraphicsManager::Instance().QueryTextSize( text.SubString( 0, m_CursorPosition ).Get(), 
															  m_Text.GetFont(), 
															  m_Text.GetFontSize() );
	}

	m_TextBB.xOffset = size.x;
	m_TextBB.lineHeight = size.y;
	m_TextBB.yOffset = numNewLines * ( m_TextBB.lineHeight + m_Text.GetLineSpacing() );
}

//=====================================================================================
void ViewTextField::RefreshText()
{
	if ( CString( GetTextView().GetText() ).Length() == 0 )
	{
		m_EmptyTextView.SetText( m_EmptyTextString.Get() );
	}
	else
	{
		m_EmptyTextView.SetText( "" );
	}

	RefreshCursor();
	GetParent()->OnTextFieldValueChanged( *this, GetTextView().GetText() );
}

//=====================================================================================
void ViewTextField::OnCharTyped( char a_Char )
{
	if ( GetEnabled() )
	{
		switch ( a_Char )
		{
		case InputManager::TEXTCODE_NEWLINE:
			if ( m_MultilineEnabled )
			{
				const CString & text = GetTextView().GetText();
				const uint32_t length = text.Length();

				GetTextView().SetText( ( text.SubString( 0, m_CursorPosition ) + '\n' + text.SubString( m_CursorPosition, length - m_CursorPosition ) ).Get() );
				++m_CursorPosition;
			}
			break;
		case InputManager::TEXTCODE_BACKSPACE:
			{
				const CString & text = GetTextView().GetText();
				const uint32_t length = text.Length();
				if ( m_CursorPosition > 0 )
				{
					GetTextView().SetText( ( text.SubString( 0, m_CursorPosition - 1 ) + text.SubString( m_CursorPosition, length - m_CursorPosition ) ).Get() );
					--m_CursorPosition;
				}
			}
			break;
		default:
			{
				const CString & text = GetTextView().GetText();
				const uint32_t length = text.Length();

				GetTextView().SetText( ( text.SubString( 0, m_CursorPosition ) + a_Char + text.SubString( m_CursorPosition, length - m_CursorPosition ) ).Get() );
				++m_CursorPosition;
			}
		}

		RefreshText();
	}
}

//=====================================================================================
void ViewTextField::OnKeyPressed( InputManager::Key a_Key )
{
	if ( GetEnabled() )
	{
		if ( a_Key == InputManager::KEYCODE_DELETE )
		{
			const CString & text = GetTextView().GetText();
			const uint32_t length = text.Length();
			if ( m_CursorPosition < length )
			{
				GetTextView().SetText( ( text.SubString( 0, m_CursorPosition ) + text.SubString( m_CursorPosition + 1, length - m_CursorPosition - 1 ) ).Get() );
			}
		}

		else if ( a_Key == InputManager::KEYCODE_LEFT )
		{
			MoveCursor( -1 );
		}

		else if ( a_Key == InputManager::KEYCODE_RIGHT )
		{
			MoveCursor( 1 );
		}

		else if ( a_Key == InputManager::KEYCODE_UP )
		{
			CString text = GetTextView().GetText();
			text = text.SubString( 0, m_CursorPosition );
			int32_t lastNewline = Max( 0, text.FindLast( "\n" ) + 1 );
			int32_t offset = m_CursorPosition - lastNewline;
			
			int32_t secondlastNewline = Max( 0, text.SubString( 0, Max( 0, lastNewline - 1 ) ).FindLast( "\n" ) + 1 );
			SetCursor( secondlastNewline + Min( offset, lastNewline - secondlastNewline - 1 ) );
		}

		else if ( a_Key == InputManager::KEYCODE_DOWN )
		{
			CString text = GetTextView().GetText();
			CString text2 = text.SubString( 0, m_CursorPosition );
			int32_t lastNewline = Max( 0, text2.FindLast( "\n" ) + 1 );
			int32_t offset = m_CursorPosition - lastNewline;
			
			int32_t postlastNewline = Max( 0, text.SubString( m_CursorPosition, Max( 0, lastNewline - 1 ) ).Find( "\n" ) + 1 );
			SetCursor( postlastNewline + Min( offset, ( int32_t )( text.Length() - postlastNewline ) ) );
		}

		const bool ctrl = InputManager::Instance().GetKeyState( InputManager::KEYCODE_LCTRL ) == InputManager::ButtonState::DOWN ||
					      InputManager::Instance().GetKeyState( InputManager::KEYCODE_RCTRL ) == InputManager::ButtonState::DOWN;

		const bool x = a_Key == InputManager::KEYCODE_X;
		const bool c = a_Key == InputManager::KEYCODE_C;
		const bool v = a_Key == InputManager::KEYCODE_V;

		if ( ctrl )
		{
			if ( c ) // Copy
			{
				
			}

			else if ( v ) // Paste
			{
				CString clipboard = InputManager::Instance().GetClipboard();
				clipboard.ReplaceAll( "\t", "    " );
				clipboard.ReplaceAll( "\r", "" );

				const CString & text = GetTextView().GetText();
				const uint32_t length = text.Length();

				GetTextView().SetText( ( text.SubString( 0, m_CursorPosition ) + clipboard + text.SubString( m_CursorPosition, length - m_CursorPosition ) ).Get() );
				m_CursorPosition += clipboard.Length();
			}

			else if ( x ) // Cut
			{
			}
		}

		RefreshText();
	}
}

//=====================================================================================
void ViewTextField::OnEnabled()
{
	RefreshText();
	GetBackground().SetTint( Colour::WHITE );
}

//=====================================================================================
void ViewTextField::OnDisabled()
{
	GetBackground().SetTint( Colour::GRAY.Lerp( Colour::WHITE, 0.2F ) );
}

//=====================================================================================
void ViewTextField::OnGainFocus()
{
	InputManager::Instance().SetTextualMode( true );
}

//=====================================================================================
void ViewTextField::OnLoseFocus()
{
	InputManager::Instance().SetTextualMode( false );
}