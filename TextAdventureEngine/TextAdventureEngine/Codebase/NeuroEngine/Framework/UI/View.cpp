#include "View.hpp"
#include "Page.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/UI/PageManager.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
#ifdef _DEBUG
bool View::g_DebugViews = false;
#endif//_DEBUG

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
	, m_PrevMouseEntered( false )
	, m_CanFocus( a_CanFocus )
	, m_IsGrabbed( false )
	, m_Interactible( false )
	, m_Enabled( false )
	, m_Hover( false )
{
	if ( !m_ParentView && a_ContainerPage && a_ContainerPage->GetRootView() )
	{
		a_ContainerPage->GetRootView()->AddChild( this );
	}


	if ( m_ParentView && m_ParentView->m_ChildrenViews.IndexOf( this ) == -1 )
	{
		m_ParentView->AddChild( this );
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

	SetEnabled( false );
	Destruct();

	if ( m_ContainerPage != nullptr )
	{
		View *& focus = m_ContainerPage->m_FocusedView;
		if ( focus == this )
		{
			m_ContainerPage->m_FocusedView = nullptr;
		}
	}

	InputManager::Instance().DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
	InputManager::Instance().DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_RELEASED, this );
}

//=====================================================================================
void View::SetEnabled( bool a_Flag )
{
	if ( a_Flag != m_Enabled )
	{
		m_Enabled = a_Flag;
		if ( a_Flag )
		{
			OnEnabled();
			if ( dynamic_cast< Page::ViewPage* >( this ) )
			{
				OnEnabled( this );
			}
			else if ( GetParent() )
			{
				GetParent()->OnEnabled( this );
			}
		}
		else
		{
			OnDisabled();
			if ( dynamic_cast< Page::ViewPage* >( this ) )
			{
				OnDisabled( this );
			}
			else if ( GetParent() )
			{
				GetParent()->OnDisabled( this );
			}
		}
	}
}

//=====================================================================================
bool View::GetEnabled() const
{
	auto it = this;

	do
	{
		if ( !it->m_Enabled )
		{
			return false;
		}
	}
	while ( it = it->GetParent() );

	return true;
}

//=====================================================================================
void View::OnMousePressed( InputManager::MouseButton a_MouseButton )
{
	if ( !GetEnabled() || !m_Interactible || !m_ContainerPage || m_ContainerPage->IsBeingBlocked() )
	{
		return;
	}

	if ( m_PrevMouseEntered )
	{
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
	// Release is impervious to enabled/disabled state
	if ( !m_ContainerPage || m_ContainerPage->IsBeingBlocked() )
	{
		return;
	}

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
		val = a_Amount;
		m_BorderPxs[ ( uint32_t )a_Alignment ] = INFINITY;

		RecalculateTrueRectangle();
	}
}

//=====================================================================================
void View::SetBorderPx( Alignment a_Alignment, float a_Amount )
{
	if ( ASSERT_WARN( a_Amount != INFINITY, __FUNCTION__ "To unlock a border px setting, use SetBorder(..)" ) )
	{
		m_BorderPxs[ ( uint32_t )a_Alignment ] = a_Amount;
		RecalculateTrueRectangle();
	}
}

//=====================================================================================
float View::GetBorder( Alignment a_Alignment )
{
	return m_Borders[ ( uint32_t )a_Alignment ];
}

//=====================================================================================
void View::SetBordersFromSizeAndOffset( const Vector2 & a_Size, const Vector2 & a_Offset )
{
	if ( m_ParentView )
	{
		Vector2 o = ( a_Offset - m_ParentView->m_Offset ) / m_ParentView->m_Size;
		Vector2 s = a_Size / m_ParentView->m_Size;


		m_Borders[ ( uint32_t )Alignment::BOTTOM ] = o.y;
		m_BorderPxs[ ( uint32_t )Alignment::BOTTOM ] = INFINITY;

		m_Borders[ ( uint32_t )Alignment::LEFT ] = o.x;
		m_BorderPxs[ ( uint32_t )Alignment::LEFT ] = INFINITY;

		m_Borders[ ( uint32_t )Alignment::TOP ] = 1.0F - ( o.y + s.y );
		m_BorderPxs[ ( uint32_t )Alignment::TOP ] = INFINITY;

		m_Borders[ ( uint32_t )Alignment::RIGHT ] = 1.0F - ( o.x + s.x );
		m_BorderPxs[ ( uint32_t )Alignment::RIGHT ] = INFINITY;

		RecalculateTrueRectangle();
	}

	else
	{
		m_Offset = a_Offset;
		m_Size = a_Size;
	}
}

//=====================================================================================
void View::AppendToFrameCacheList()
{
	PROFILE;

	m_ContainerPage->m_AllViews.Append( this );

	if ( m_ChildrenViews.Count() > 0 )
	{
		const int32_t l = m_ChildrenViews.Count() - 1;
		for ( int32_t c = l; c >= 0; --c )
		{
			View * childView = m_ChildrenViews[ ( uint32_t )c ];
			childView->AppendToFrameCacheList();
		}
	}
}

//=====================================================================================
bool View::CheckInput( bool a_ReleaseOnly )
{
	if ( m_Interactible == false || !GetEnabled() )
	{
		return false;
	}

	const Vector2 & mpos = InputManager::Instance().GetMousePosition();
	const Vector2 & bl = GetPosition();
	const Vector2 & tr = GetPosition() + GetSize();

	m_Hover = !a_ReleaseOnly && InRange( mpos.x, bl.x, tr.x ) && InRange( mpos.y, bl.y, tr.y );
	
	if ( m_Hover != m_PrevMouseEntered )
	{
		if ( m_Hover )
		{
			this->OnMouseEnter( mpos - bl );
		}

		else
		{
			this->OnMouseLeave( mpos - bl );
		}

		m_PrevMouseEntered = m_Hover;
	}

	return m_Hover;
}

//=====================================================================================
bool View::GetMouseOver() const
{	
	const Vector2 & mpos = InputManager::Instance().GetMousePosition();
	const Vector2 & bl = GetPosition();
	const Vector2 & tr = GetPosition() + GetSize();

	return InRange( mpos.x, bl.x, tr.x ) && InRange( mpos.y, bl.y, tr.y );
}

//=====================================================================================
void View::Tick( float a_DeltaTime )
{
	PROFILE;

	if ( !GetEnabled() )
	{
		return;
	}

	Colour tint = m_Tint;

	if ( m_ParentView )
	{
		tint *= m_ParentView->m_Tint;
	}

	GraphicsManager::Instance().SetColour( tint, GraphicsManager::COL_PRIMARY );
	{
		PROFILE2( "Children" );
		OnTick( a_DeltaTime );
	}

	if ( m_ChildrenViews.Count() > 0 )
	{
		const int32_t l = m_ChildrenViews.Count() - 1;
		for ( int32_t c = l; c >= 0; --c )
		{
			View * childView = m_ChildrenViews[ ( uint32_t )c ];
			childView->Tick( a_DeltaTime );
		}
	}

	{
		PROFILE2( "OnTickPost" );
		OnTickPost( a_DeltaTime );
	}

#ifdef _DEBUG
	if ( g_DebugViews && m_ParentView )
	{
		GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
		GraphicsManager::Instance().SetColour( Colour( m_PrevMouseEntered ? Colour::GREEN : Colour::RED ).WithAlpha( 0.75F ), GraphicsManager::COL_SECONDARY );
		GraphicsManager::Instance().SetColour( Colour::INVISIBLE, GraphicsManager::COL_PRIMARY);
		GraphicsManager::Instance().GfxDrawQuad( GetPosition() + Vector2::ONE * 4.0F * m_PrevMouseEntered, GetSize() - Vector2::ONE * 8.0F * m_PrevMouseEntered, 4.0F );
	}
#endif//_DEBUG
}

//=====================================================================================
void View::OnChildZOrderChanged( const View * a_Child, uint32_t a_ZOrder )
{
	m_ChildrenViews.Sort( View::ZSorter );
}

//=====================================================================================
void View::OnParentRectangleChanged( const Vector2 & a_Offset, const Vector2 & a_Size, const View * a_Parent )
{
	RecalculateTrueRectangle();
}

//=====================================================================================
void View::RecalculateTrueRectangle()
{
	float l = m_Borders[ ( uint32_t )Alignment::LEFT ];
	float r = m_Borders[ ( uint32_t )Alignment::RIGHT ];
	float t = m_Borders[ ( uint32_t )Alignment::TOP ];
	float b = m_Borders[ ( uint32_t )Alignment::BOTTOM ];

	const float & l_px = m_BorderPxs[ ( uint32_t )Alignment::LEFT ];
	const float & r_px = m_BorderPxs[ ( uint32_t )Alignment::RIGHT ];
	const float & t_px = m_BorderPxs[ ( uint32_t )Alignment::TOP ];
	const float & b_px = m_BorderPxs[ ( uint32_t )Alignment::BOTTOM ];

	if ( l_px != INFINITY ) { l = 1.0F - ( l_px / m_ParentView->m_Size.x ); }
	if ( r_px != INFINITY ) { r = 1.0F - ( r_px / m_ParentView->m_Size.x ); }
	if ( t_px != INFINITY ) { t = 1.0F - ( t_px / m_ParentView->m_Size.y ); }
	if ( b_px != INFINITY ) { b = 1.0f - ( b_px / m_ParentView->m_Size.y ); }

	m_Offset = m_ParentView->m_Offset + m_ParentView->m_Size * Vector2( l, b );
	m_Size = m_ParentView->m_Size * Vector2( 1.0F - ( l + r ), 1.0F - ( t + b ) );

	for ( int32_t c = 0; c < static_cast< int32_t >( m_ChildrenViews.Count() ); ++c )
	{
		m_ChildrenViews[ ( uint32_t )c ]->OnParentRectangleChanged( m_Offset, m_Size, this );
	}

	OnRectangleChanged( m_Offset, m_Size );
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
void View::RequestInitialEvents( IActionListener * a_ActionListener )
{
	if ( GetEnabled() )
	{
		a_ActionListener->OnEnabled( this );
	}
	else
	{
		a_ActionListener->OnDisabled( this );
	}
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
void View::OnEnabled( View * a_View )
{
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnEnabled( a_View );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnEnabled( a_View );
	}
}

//=====================================================================================
void View::OnDisabled( View * a_View )
{
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnDisabled( a_View );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnDisabled( a_View );
	}
}

//=====================================================================================
void View::OnTextFieldFocus( ViewTextField & a_ViewTextField )
{
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

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
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

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
	if ( m_ActionListeners.Count() > 0 )
	{
		auto it = m_ActionListeners.First();
		const auto end = m_ActionListeners.End();

		while ( it != end )
		{
			( *it )->OnTextFieldSubmit( a_ViewTextField );
			++it;
		}
	}

	if ( m_ParentView )
	{
		m_ParentView->OnTextFieldSubmit( a_ViewTextField );
	}
}

//=====================================================================================
void View::OnTextFieldValueChanged( ViewTextField & a_ViewTextField, const CString & a_StringValue )
{
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

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
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

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
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

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

//=====================================================================================
void View::OnListSelectionChanged( ViewListSelector & a_ViewListSelector, int32_t a_SelectionIndex )
{
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnListSelectionChanged( a_ViewListSelector, a_SelectionIndex );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnListSelectionChanged( a_ViewListSelector, a_SelectionIndex );
	}
}

//=====================================================================================
void View::OnListSelectionConfirmed( ViewListSelector & a_ViewListSelector, int32_t a_SelectionIndex )
{
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnListSelectionConfirmed( a_ViewListSelector, a_SelectionIndex );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnListSelectionConfirmed( a_ViewListSelector, a_SelectionIndex );
	}
}

//=====================================================================================
void View::OnDragBegin( ViewDraggable & a_ViewDraggable )
{
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnDragBegin( a_ViewDraggable );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnDragBegin( a_ViewDraggable );
	}
}

//=====================================================================================
void View::OnDragEnd( ViewDraggable & a_ViewDraggable )
{
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnDragEnd( a_ViewDraggable );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnDragEnd( a_ViewDraggable );
	}
}

//=====================================================================================
void View::OnDrop( ViewDropTarget & a_ViewDropTarget, ViewDraggable & a_ViewDraggable )
{
	if ( m_ActionListeners.Count() == 0 )
	{
		return;
	}

	auto it = m_ActionListeners.First();
	const auto end = m_ActionListeners.End();

	while ( it != end )
	{
		( *it )->OnDrop( a_ViewDropTarget, a_ViewDraggable );
		++it;
	}

	if ( m_ParentView )
	{
		m_ParentView->OnDrop( a_ViewDropTarget, a_ViewDraggable );
	}
}

//=====================================================================================
void View::SendMessageUpward( uint32_t a_Message )
{
	auto it = GetParent();

	while ( it != nullptr )
	{
		if ( it->OnMessage( this, a_Message ) )
		{
			it = it->GetParent();
		}

		else
		{
			break;
		}
	}
}

//=====================================================================================
View * View::FindChild( const CString & a_Name ) const
{
	View ** ptr = m_ChildrenViews.Find< const CString & >( []( const CString &  a_UserData, View * const & a_Elem )
	{ 
		return a_Elem->GetName() == a_UserData; 
	}, a_Name );

	if ( ptr )
	{
		return *ptr;
	}

	return nullptr;
}

//=====================================================================================
const View * View::FindDescendant( const CString & a_Path ) const
{
	const Array< CString > splits = a_Path.Split( "/" );

	auto it = splits.First();
	const auto end = splits.End();


	const View * parentOfSearchable = this;
	while ( it != end && parentOfSearchable )
	{
		parentOfSearchable = parentOfSearchable->FindChild( *it );
		++it;
	}

	return parentOfSearchable;
}

//=====================================================================================
View * View::FindDescendant( const CString & a_Path )
{
	const Array< CString > splits = a_Path.Split( "/" );

	auto it = splits.First();
	const auto end = splits.End();


	View * parentOfSearchable = this;
	while ( it != end && parentOfSearchable )
	{
		parentOfSearchable = parentOfSearchable->FindChild( *it );
		++it;
	}

	return parentOfSearchable;
}