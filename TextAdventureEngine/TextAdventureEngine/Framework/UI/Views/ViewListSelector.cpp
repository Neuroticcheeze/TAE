#include "ViewListSelector.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewListSelector::ViewSelection::ViewSelection( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Text( new ViewText( "Text", a_ContainerPage, this ) )
{
	SetEnabled();
	SetIsInteractible( true );

	m_Text->SetBorder( View::Alignment::LEFT, 0.0F );
	m_Text->SetBorder( View::Alignment::RIGHT, 0.0F );
	m_Text->SetBorder( View::Alignment::TOP, 0.0F );
	m_Text->SetBorder( View::Alignment::BOTTOM, 0.0F );
	m_Text->SetVAlign( GraphicsManager::TA_CENTER );
	m_Text->SetHAlign( GraphicsManager::TA_CENTER );
	m_Text->SetFont( _GetFont( "DEFAULT" ) );
	m_Text->SetFontSize( 32.0F );
	m_Text->SetTint( Colour::WHITE );
	AddChild( m_Text );
}

//=====================================================================================
ViewListSelector::ViewSelection::~ViewSelection()
{
	DetatchChild( m_Text );
	Free( m_Text );
}

//=====================================================================================
void ViewListSelector::ViewSelection::OnMouseEnter( const Vector2 & a_MousePosition )
{
	reinterpret_cast< ViewListSelector* >( GetParent() )->OnSelectionHoverBegin( this );
}

//=====================================================================================
void ViewListSelector::ViewSelection::OnMouseLeave( const Vector2 & m_MousePosition )
{
	reinterpret_cast< ViewListSelector* >( GetParent() )->OnSelectionHoverEnd( this );
}

//=====================================================================================
void ViewListSelector::ViewSelection::OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton )
{
	reinterpret_cast< ViewListSelector* >( GetParent() )->OnSelection( this );
}

//=====================================================================================
ViewListSelector::ViewListSelector( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_SelectedIndex( 0 )
	, m_SingleClickSelection( true )
{
	SetEnabled();
	InputManager::Instance().AttachListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
}

//=====================================================================================
ViewListSelector::~ViewListSelector()
{
	InputManager::Instance().DetatchListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
	SetList( {} );
}

//=====================================================================================
void ViewListSelector::OnKeyPressed( InputManager::Key a_Key )
{
	switch ( a_Key )
	{
	case InputManager::Key::KEYCODE_UP:
		SetSelectedIndex( GetSelectedIndex() - 1 );
		break;
	case InputManager::Key::KEYCODE_DOWN:
		SetSelectedIndex( GetSelectedIndex() + 1 );
		break;
	case InputManager::Key::KEYCODE_RETURN:
		OnListSelectionConfirmed( *this, GetSelectedIndex() );
		break;
	}
}

//=====================================================================================
void ViewListSelector::RequestInitialEvents( IActionListener * a_ActionListener )
{
	View::RequestInitialEvents( a_ActionListener );
	a_ActionListener->OnListSelectionChanged( *this, GetSelectedIndex() );
}

//=====================================================================================
void ViewListSelector::OnSelection( ViewSelection * a_Selection )
{
	int32_t sel = m_List.IndexOf( a_Selection );
	
	if ( m_SingleClickSelection )
	{
		SetSelectedIndex( sel );
		OnListSelectionConfirmed( *this, GetSelectedIndex() );
	}
	
	else
	{
		if ( GetSelectedIndex() != sel )
		{
			SetSelectedIndex( sel );

		}
		else
		{
			OnListSelectionConfirmed( *this, GetSelectedIndex() );
		}
	}
}

//=====================================================================================
void ViewListSelector::OnSelectionHoverBegin( ViewSelection * a_Selection )
{
	if ( m_List.IndexOf( a_Selection ) != m_SelectedIndex )
	{
		a_Selection->SetTint( Colour::YELLOW );
	}
}

//=====================================================================================
void ViewListSelector::OnSelectionHoverEnd( ViewSelection * a_Selection )
{
	if ( m_List.IndexOf( a_Selection ) != m_SelectedIndex )
	{
		a_Selection->SetTint( Colour::GRAY );
	}
}

//=====================================================================================
void ViewListSelector::SetList( const Array< CString > & a_List )
{
	auto it = m_List.First();
	const auto end = m_List.End();
	while ( it != end )
	{
		DetatchChild( *it );
		Free( *it );
		++it;
	}

	m_TextList.Clear();
	m_List.Clear();

	const int32_t c = a_List.Count();
	const float vc = 1.0F / static_cast< float >( c );

	auto itSrc = a_List.First();
	const auto endSrc = a_List.End();
	while ( itSrc != endSrc )
	{
		const int32_t i = static_cast< int32_t >( itSrc - a_List.First() );

		ViewSelection * view = new ViewSelection( CString().Format( "%s$%d", GetName().Get(), i ).Get(), GetContainerPage(), this );

		float f = static_cast< float >( i ) * vc;

		view->SetTint( Colour::WHITE );
		view->SetBorder( View::Alignment::LEFT, 0.0F );
		view->SetBorder( View::Alignment::RIGHT, 0.0F );
		view->SetBorder( View::Alignment::TOP, f );
		view->SetBorder( View::Alignment::BOTTOM, 1.0F - ( f + vc ) );
		view->GetLabel()->SetText( itSrc->Get(), true );

		AddChild( view );
		m_TextList.Append( *itSrc );
		m_List.Append( view );
		++itSrc;
	}

	RefreshSelection();
}

//=====================================================================================
void ViewListSelector::SetSelectedIndex( int32_t a_Index )
{
	if ( m_SelectedIndex != a_Index )
	{
		m_SelectedIndex = a_Index;
		RefreshSelection();
		OnListSelectionChanged( *this, GetSelectedIndex() );
	}
}

//=====================================================================================
int32_t ViewListSelector::GetSelectedIndex() const
{
	return m_SelectedIndex;
}

//=====================================================================================
void ViewListSelector::RefreshSelection()
{
	if ( m_List.Count() == 0 )
	{
		m_SelectedIndex = 0;
		return;
	}

	m_SelectedIndex = Wrap( m_SelectedIndex, 0, ( int32_t )m_List.Count() - 1 );

	for ( uint32_t idx = 0; idx < m_List.Count(); ++idx )
	{
		ViewSelection * vs = m_List[ idx ];

		if ( idx == m_SelectedIndex )
		{
			vs->SetTint( Colour::WHITE );
		}

		else
		{
			vs->SetTint( Colour::GRAY );
		}
	}
}