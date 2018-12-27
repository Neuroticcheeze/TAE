#include "ViewFlyoutMenu.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewFlyoutMenu::ViewFlyoutMenuItem::ViewFlyoutMenuItem( const char * a_Name, const MenuAction & a_MenuAction, const KeyShortcut & a_KeyShortcut, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Text( new ViewText( "Text", a_ContainerPage, this ) )
	, m_TextSmall( new ViewText( "TextSmall", a_ContainerPage, this ) )
	, m_MenuAction( a_MenuAction ? a_MenuAction : [](){} )
	, m_KeyShortcut( a_KeyShortcut )
	, m_Child( nullptr )
{
	SetEnabled();
	SetIsInteractible( true );

	m_Text->SetBorder( View::Alignment::LEFT, 0.05F );
	m_Text->SetBorder( View::Alignment::RIGHT, 0.05F );
	m_Text->SetBorder( View::Alignment::TOP, 0.05F );
	m_Text->SetBorder( View::Alignment::BOTTOM, 0.05F );
	m_Text->SetHAlign( GraphicsManager::TA_LEFT );
	m_Text->SetVAlign( GraphicsManager::TA_CENTER );
	m_Text->SetFont( _GetFont( "DEFAULT" ) );
	m_Text->SetFontSize( 30.0F );
	m_Text->SetTint( Colour::WHITE.WithAlpha( 0.75F ) );

	if ( !a_KeyShortcut.IsNull() )
	{
		m_TextSmall->SetBorder( View::Alignment::LEFT, 0.05F );
		m_TextSmall->SetBorder( View::Alignment::RIGHT, 0.05F );
		m_TextSmall->SetBorder( View::Alignment::TOP, 0.05F );
		m_TextSmall->SetBorder( View::Alignment::BOTTOM, 0.05F );
		m_TextSmall->SetHAlign( GraphicsManager::TA_RIGHT );
		m_TextSmall->SetVAlign( GraphicsManager::TA_BOTTOM );
		m_TextSmall->SetFont( _GetFont( "DEFAULT" ) );
		m_TextSmall->SetFontSize( 20.0F );
		m_TextSmall->SetTint( Colour::WHITE.WithAlpha( 0.25F ) );

		CString shortcut;
		if ( a_KeyShortcut.HeldKeyCode & KeyShortcut::CTRL ) { shortcut += "Ctl+"; }
		if ( a_KeyShortcut.HeldKeyCode & KeyShortcut::SHIFT ) { shortcut += "Sft+"; }
		if ( a_KeyShortcut.HeldKeyCode & KeyShortcut::ALT ) { shortcut += "Alt+"; }
		shortcut += ( char )( a_KeyShortcut.KeyCode - InputManager::KEYCODE_A ) + 'A'; // Only handles letters
		m_TextSmall->SetText( shortcut.Get(), false );
	}
	
	InputManager::Instance().AttachListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
}

//=====================================================================================
ViewFlyoutMenu::ViewFlyoutMenuItem::~ViewFlyoutMenuItem()
{
	InputManager::Instance().DetatchListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );

	DetatchChild( m_Text );
	Free( m_Text );

	DetatchChild( m_Child );
	Free( m_Child );
}

//=====================================================================================
void ViewFlyoutMenu::ViewFlyoutMenuItem::OnMouseEnter( const Vector2 & a_MousePosition )
{
	SendMessageUpward( CRC32( "HOVER_BEGIN" ) );

	m_Text->SetTint( Colour::WHITE );
	if ( m_Child )
	{
		m_Child->SetEnabled( true );
	}
}

//=====================================================================================
void ViewFlyoutMenu::ViewFlyoutMenuItem::OnMouseLeave( const Vector2 & m_MousePosition )
{
	SendMessageUpward( CRC32( "HOVER_END" ) );

	if ( m_HideProhibitorCounter > 0 || ( GetParent() && GetParent()->GetMouseOver() ) )
	{
		return;
	}

	m_Text->SetTint( Colour::WHITE.WithAlpha( 0.5F ) );
	if ( m_Child )
	{
		m_Child->SetEnabled( false );
	}
}

//=====================================================================================
void ViewFlyoutMenu::ViewFlyoutMenuItem::OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
	m_MenuAction();
}

//=====================================================================================
void ViewFlyoutMenu::ViewFlyoutMenuItem::OnKeyPressed( InputManager::Key a_Key )
{
	if ( a_Key == m_KeyShortcut.KeyCode )
	{
		bool ctrl = InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_LCTRL ) == InputManager::ButtonState::DOWN ||
					InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_RCTRL ) == InputManager::ButtonState::DOWN;

		if ( ( m_KeyShortcut.HeldKeyCode & KeyShortcut::CTRL ) && !ctrl )
		{
			return;
		}

		bool shift = InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_LSHIFT ) == InputManager::ButtonState::DOWN ||
					 InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_RSHIFT ) == InputManager::ButtonState::DOWN;

		if ( ( m_KeyShortcut.HeldKeyCode & KeyShortcut::SHIFT ) && !shift )
		{
			return;
		}

		bool alt = InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_LALT ) == InputManager::ButtonState::DOWN ||
				   InputManager::Instance().GetKeyState( InputManager::Key::KEYCODE_RALT ) == InputManager::ButtonState::DOWN;

		if ( ( m_KeyShortcut.HeldKeyCode & KeyShortcut::ALT ) && !alt )
		{
			return;
		}

		m_MenuAction();
	}
}

//=====================================================================================
void ViewFlyoutMenu::ViewFlyoutMenuItem::OnTick( float a_DeltaTime )
{
	if ( GetMouseOver() )
	{
		GraphicsManager::Instance().SetColour( Colour::GRAY * 1.2F, GraphicsManager::COL_SECONDARY );
		GraphicsManager::Instance().SetColour( Colour::GRAY * 1.0F, GraphicsManager::COL_PRIMARY );
		GraphicsManager::Instance().GfxDrawQuad( GetPosition(), GetSize(), 3.0F );
	}
}

//=====================================================================================
bool ViewFlyoutMenu::ViewFlyoutMenuItem::OnMessage( View * a_Child, uint32_t a_Message )
{
	if ( CRC32( "HOVER_BEGIN" ) == a_Message )
	{
		++m_HideProhibitorCounter;
	}

	else if ( CRC32( "HOVER_END" ) == a_Message && m_HideProhibitorCounter > 0 )
	{
		--m_HideProhibitorCounter;
	}

	return true;
}

//=====================================================================================
ViewFlyoutMenu::ViewFlyoutMenu( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
{
	SetEnabled();
}

//=====================================================================================
ViewFlyoutMenu::~ViewFlyoutMenu()
{
	SetList( {} );
}

//=====================================================================================
void ViewFlyoutMenu::OnTick( float a_DeltaTime )
{
	GraphicsManager::Instance().SetColour( GetTint() * 1.5F, GraphicsManager::COL_SECONDARY );
	GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_PRIMARY );
	GraphicsManager::Instance().GfxDrawQuad( GetPosition(), GetSize(), 3.0F );

	if ( m_HideProhibitorCounter > 0 || ( GetParent() && GetParent()->GetMouseOver() && dynamic_cast< ViewFlyoutMenuItem* >( GetParent() ) ) )
	{
		return;
	}

	if ( !GetMouseOver() )
	{
		SetEnabled( false );
	}
}

//=====================================================================================
void ViewFlyoutMenu::SetList( const Array< FlyoutContentTree > & a_List )
{
	static const float ITEM_HEIGHT_PX = 48.0F;

	auto it = m_List.First();
	const auto end = m_List.End();
	while ( it != end )
	{
		DetatchChild( *it );
		Free( *it );
		++it;
	}

	m_List.Clear();

	const float c = static_cast< float >( a_List.Count() );
	const float vc = 1.0F / static_cast< float >( c );

	SetBordersFromSizeAndOffset( Vector2( GetSize().x, c * ITEM_HEIGHT_PX ), GetPosition() + ( -Vector2::UP * ( c - 1.0F ) * ITEM_HEIGHT_PX ) );

	auto itSrc = a_List.First();
	const auto endSrc = a_List.End();
	while ( itSrc != endSrc )
	{
		const int32_t i = static_cast< int32_t >( itSrc - a_List.First() );

		ViewFlyoutMenuItem * view = new ViewFlyoutMenuItem( CString::Format( "%s$%d", GetName().Get(), i ).Get(), itSrc->ExecuteAction, itSrc->KeyShortcut, GetContainerPage(), this );

		float f = static_cast< float >( i ) * vc;

		view->SetTint( Colour::WHITE );
		view->SetBorder( View::Alignment::LEFT, 0.0F );
		view->SetBorder( View::Alignment::RIGHT, 0.0F );
		view->SetBorder( View::Alignment::TOP, f );
		view->SetBorder( View::Alignment::BOTTOM, 1.0F - ( f + vc ) );
		
		CString name = itSrc->Name + ( itSrc->Members.Count() > 0 ? "..." : "" );
		view->GetLabel()->SetText( name.Get(), false );

		if ( itSrc->Members.Count() > 0 )
		{
			view->m_Child = new ViewFlyoutMenu( CString::Format( "%s$%d_members", GetName().Get(), i ).Get(), GetContainerPage(), view );
			view->m_Child->SetBorder( View::Alignment::RIGHT, -1.0F );
			view->m_Child->SetBorder( View::Alignment::LEFT, 1.0F );
			view->m_Child->SetBorder( View::Alignment::TOP, 0.0F );
			view->m_Child->SetBorder( View::Alignment::BOTTOM, 0.0F );
			view->m_Child->SetTint( Colour::GRAY * 0.75F );

			view->m_Child->SetList( itSrc->Members );
		}

		m_List.Append( view );
		++itSrc;
	}
}

//=====================================================================================
void ViewFlyoutMenu::SetContentTree( const Array< FlyoutContentTree > & a_ContentTree )
{
	SetList( a_ContentTree );
}

//=====================================================================================
bool ViewFlyoutMenu::OnMessage( View * a_Child, uint32_t a_Message )
{
	if ( CRC32( "HOVER_BEGIN" ) == a_Message )
	{
		++m_HideProhibitorCounter;
	}

	else if ( CRC32( "HOVER_END" ) == a_Message && m_HideProhibitorCounter > 0 )
	{
		--m_HideProhibitorCounter;
	}

	return true;
}