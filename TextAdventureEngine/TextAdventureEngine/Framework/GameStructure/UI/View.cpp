#include "View.hpp"
#include "Page.hpp"
#include <SIGIL-0.9.0/sl.h>
#include <Framework/Input/InputManager.hpp>

//=====================================================================================
View::View( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: m_Name( a_Name )
	, m_ParentView( a_Parent )
	, m_ContainerPage( a_ContainerPage )
	, m_ZOrder( 0 )
	, m_ExplicitSize( false )
	, m_PrevMouseEntered( false )
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
}

//=====================================================================================
View::~View()
{
	InputManager::Instance().DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
}

//=====================================================================================
void View::OnMousePressed( InputManager::MouseButton a_MouseButton )
{
	if ( m_PrevMouseEntered )
	{
		OnMouseClick( InputManager::Instance().GetMousePosition(), a_MouseButton );
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
		val = Clamp( a_Amount );

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

	//if (!minside)
	//slRectangleOutline( GetCenter().x, GetCenter().y, GetSize().x, GetSize().y );
	
	Colour tint = m_Tint;

	if ( m_ParentView )
	{
		tint *= m_ParentView->m_Tint;
	}

	slSetForeColor( tint.r, tint.g, tint.b, tint.a );
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
	m_ChildrenViews.Sort( []( View * const & a_Left, View * const & a_Right )
	{
		return a_Left->GetZOrder() < a_Right->GetZOrder();
	});
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

	for ( int32_t c = 0; c < m_ChildrenViews.Count(); ++c )
	{
		m_ChildrenViews[ ( uint32_t )c ]->OnParentRectangleChanged( m_TrueOffset, m_TrueSize, this );
	}
}

//=====================================================================================
void View::Reset()
{
	for ( uint32_t k = 0; k < m_ChildrenViews.Count(); ++k )
	{
		DetatchChild( m_ChildrenViews[ k ] );
	}
}