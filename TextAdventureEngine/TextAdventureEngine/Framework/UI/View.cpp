#include "View.hpp"
#include "Page.hpp"
#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
Comparer< View * const & > View::ZSorter = []( View * const & a_Left, View * const & a_Right )
{
	return a_Left->GetZOrder() < a_Right->GetZOrder();
};

//=====================================================================================
View::View( const char * a_Name, Page * a_ContainerPage, View * a_Parent, bool a_CanFocus )
	: m_Name( a_Name )
	, m_ParentView( a_Parent )
	, m_ContainerPage( a_ContainerPage )
	, m_ZOrder( -1 )
	, m_ExplicitSize( false )
	, m_PrevMouseEntered( false )
	, m_CanFocus( a_CanFocus )
	, m_IsGrabbed( false )
{
	if ( !m_ParentView && a_ContainerPage && a_ContainerPage->GetRootView() )
	{
		a_ContainerPage->GetRootView()->AddChild( this );
	}

	m_Borders[ ( int32_t )Alignment::LEFT ] = 0.0F;
	m_Borders[ ( int32_t )Alignment::RIGHT ] = 0.0F;
	m_Borders[ ( int32_t )Alignment::TOP ] = 0.0F;
	m_Borders[ ( int32_t )Alignment::BOTTOM ] = 0.0F;

	InputManager::Instance().AttachListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
	InputManager::Instance().AttachListener( InputManager::InputMouseEvent::ON_MOUSE_RELEASED, this );

	SetZOrder( 0 );
}

//=====================================================================================
View::~View()
{
	auto it = m_ChildrenViews.First();
	const auto end = m_ChildrenViews.End();
	while ( it != end )
	{
		DetatchChild( *it );
		++it;
	}
	m_ChildrenViews.Clear();

	Destruct();

	View *& focus = m_ContainerPage->m_FocusedView;
	if ( focus == this )
	{
		m_ContainerPage->m_FocusedView = nullptr;
	}

	InputManager::Instance().DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
	InputManager::Instance().DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_RELEASED, this );
}

//=====================================================================================
void View::OnMousePressed( InputManager::MouseButton a_MouseButton )
{
	if ( m_PrevMouseEntered )
	{
		View *& focus = m_ContainerPage->m_FocusedView;
		if ( m_CanFocus )
		{
			if ( m_ContainerPage->m_FocusedView )
			{
				m_ContainerPage->m_FocusedView->OnLoseFocus();
			}

			m_ContainerPage->m_FocusedView = this;
			OnGainFocus();
		}

		m_IsGrabbed = true;
		OnMouseClick( InputManager::Instance().GetMousePosition(), a_MouseButton );
	}
}

//=====================================================================================
void View::OnMouseReleased( InputManager::MouseButton a_MouseButton )
{
	if ( m_IsGrabbed )
	{
		m_IsGrabbed = false;
		OnMouseReleased( InputManager::Instance().GetMousePosition(), a_MouseButton );
	}
}

//=====================================================================================
void View::SetZOrder( int16_t a_ZOrder )
{
	if ( a_ZOrder != m_ZOrder && m_ParentView )
	{
		m_ParentView->OnChildZOrderChanged( this, a_ZOrder );
	}

	m_ZOrder = a_ZOrder;
}

//=====================================================================================
int16_t View::GetZOrder() const
{
	return m_ZOrder;
}

//=====================================================================================
void View::SetBorder( Alignment a_Alignment, float a_Amount )
{
	float & val = m_Borders[ ( uint32_t )a_Alignment ];
	
	if ( val == 0.0F || val != a_Amount )
	{
		val = Clamp( a_Amount, -1.0F, 1.0F );

		RecalculateTrueRectangle();
	}
}

//=====================================================================================
float View::GetBorder( Alignment a_Alignment )
{
	return m_Borders[ ( uint32_t )a_Alignment ];
}

//=====================================================================================
void View::Tick( float a_DeltaTime )
{
	const Vector2 & mpos = InputManager::Instance().GetMousePosition();
	const Vector2 & bl = GetPosition();
	const Vector2 & tr = GetPosition() + GetSize();

	bool minside = InRange( mpos.x, bl.x, tr.x ) && InRange( mpos.y, bl.y, tr.y );
	
	if ( minside != m_PrevMouseEntered )
	{
		if ( minside )
		{
			this->OnMouseEnter( mpos - bl );
		}

		else
		{
			this->OnMouseLeave( mpos - bl );
		}

		m_PrevMouseEntered = minside;
	}

//#define DEBUG_VIEWS
#ifdef DEBUG_VIEWS
	if ( m_ParentView )
	{
		Graphics::SetForeColor( minside ? Colour::GREEN : Colour::RED );
		Graphics::DrawRectangleOutline( GetCenter(), GetSize() );
	}
#endif

	Colour tint = m_Tint;

	if ( m_ParentView )
	{
		tint *= m_ParentView->m_Tint;
	}

	Graphics::SetForeColor( tint );
	OnTick( a_DeltaTime );

	if ( m_ChildrenViews.Count() > 0 )
	{
		const int32_t l = m_ChildrenViews.Count() - 1;
		for ( int32_t c = l; c >= 0; --c )
		{
			View * childView = m_ChildrenViews[ ( uint32_t )c ];
			childView->Tick( a_DeltaTime );
		}
	}
}

//=====================================================================================
void View::OnChildZOrderChanged( const View * a_Child, uint32_t a_ZOrder )
{
	m_ChildrenViews.Sort( View::ZSorter );
}

//=====================================================================================
void View::OnParentRectangleChanged( const Vector2 & a_TrueOffset, const Vector2 & a_TrueSize, const View * a_Parent )
{
	RecalculateTrueRectangle();
}

void View::RecalculateTrueRectangle()
{
	const float & l = m_Borders[ ( uint32_t )Alignment::LEFT ];
	const float & r = m_Borders[ ( uint32_t )Alignment::RIGHT ];
	const float & t = m_Borders[ ( uint32_t )Alignment::TOP ];
	const float & b = m_Borders[ ( uint32_t )Alignment::BOTTOM ];

	m_TrueOffset = m_ParentView->m_TrueOffset + m_ParentView->m_TrueSize * Vector2( l, b );

	if ( !m_ExplicitSize )
	{
		m_TrueSize = m_ParentView->m_TrueSize * Vector2( 1.0F - Clamp( l + r ), 1.0F - Clamp( t + b ) );
	}

	for ( int32_t c = 0; c < static_cast< int32_t >( m_ChildrenViews.Count() ); ++c )
	{
		m_ChildrenViews[ ( uint32_t )c ]->OnParentRectangleChanged( m_TrueOffset, m_TrueSize, this );
	}

	OnRectangleChanged( m_TrueOffset, m_TrueSize );
}

//=====================================================================================
void View::Reset()
{
	for ( uint32_t k = 0; k < m_ChildrenViews.Count(); ++k )
	{
		DetatchChild( m_ChildrenViews[ k ] );
	}
}

//=====================================================================================
bool View::HasFocus() const
{
	return GetViewWithFocus() == this;
}

//=====================================================================================
View * View::GetViewWithFocus() const
{
	return m_ContainerPage ? ( m_ContainerPage->m_FocusedView ) : nullptr;
}

//=====================================================================================
void View::AddActionListener( IActionListener * a_ActionListener, bool a_SendInitialEvents )
{
	if ( a_ActionListener )
	{
		if ( a_SendInitialEvents )
		{
			RequestInitialEvents( a_ActionListener );

			auto it = m_ChildrenViews.First();
			const auto end = m_ChildrenViews.End();

			while ( it != end )
			{
				( *it )->RequestInitialEvents( a_ActionListener );
				++it;
			}
		}

		m_ActionListeners.Insert( a_ActionListener );
	}
}

//=====================================================================================
void View::RemoveActionListener( IActionListener * a_ActionListener )
{
	m_ActionListeners.Remove( a_ActionListener );
}

//=====================================================================================
void View::OnButtonPress( ViewButton & a_ViewButton )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnButtonPress( a_ViewButton );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnButtonPress( a_ViewButton );
	}
}

//=====================================================================================
void View::OnButtonDisabled( ViewButton & a_ViewButton )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnButtonDisabled( a_ViewButton );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnButtonDisabled( a_ViewButton );
	}
}

//=====================================================================================
void View::OnButtonEnabled( ViewButton & a_ViewButton )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnButtonEnabled( a_ViewButton );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnButtonEnabled( a_ViewButton );
	}
}

//=====================================================================================
void View::OnTextFieldDisabled( ViewTextField & a_ViewTextField )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnTextFieldDisabled( a_ViewTextField );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnTextFieldDisabled( a_ViewTextField );
	}
}

//=====================================================================================
void View::OnTextFieldEnabled( ViewTextField & a_ViewTextField )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnTextFieldEnabled( a_ViewTextField );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnTextFieldEnabled( a_ViewTextField );
	}
}

//=====================================================================================
void View::OnTextFieldFocus( ViewTextField & a_ViewTextField )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnTextFieldFocus( a_ViewTextField );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnTextFieldFocus( a_ViewTextField );
	}
}

//=====================================================================================
void View::OnTextFieldFocusLost( ViewTextField & a_ViewTextField )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnTextFieldFocusLost( a_ViewTextField );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnTextFieldFocusLost( a_ViewTextField );
	}
}

//=====================================================================================
void View::OnTextFieldSubmit( ViewTextField & a_ViewTextField )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnTextFieldSubmit( a_ViewTextField );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnTextFieldSubmit( a_ViewTextField );
	}
}

//=====================================================================================
void View::OnTextFieldValueChanged( ViewTextField & a_ViewTextField, const CString & a_StringValue )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnTextFieldValueChanged( a_ViewTextField, a_StringValue );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnTextFieldValueChanged( a_ViewTextField, a_StringValue );
	}
}

//=====================================================================================
void View::OnTickBoxValueChanged( ViewTickBox & a_ViewTickBox, bool a_Flag )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnTickBoxValueChanged( a_ViewTickBox, a_Flag );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnTickBoxValueChanged( a_ViewTickBox, a_Flag );
	}
}

//=====================================================================================
void View::OnSliderValueChanged( ViewSlider & a_ViewSlider, float a_PreviousValue, float a_NewValue )
{
	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnSliderValueChanged( a_ViewSlider, a_PreviousValue, a_NewValue );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnSliderValueChanged( a_ViewSlider, a_PreviousValue, a_NewValue );
	}
}