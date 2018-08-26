#include "ViewDraggable.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewDraggable::ViewDraggable( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Enabled( false )
	, m_Drag( false )
	, m_UseExplicitOffset( false )
{
	SetIsInteractible( true );
	SetEnabled( true );
}

//=====================================================================================
void ViewDraggable::OnTick( float a_DeltaTime )
{
	if ( !m_Drag )
	{
		return;
	}

	Vector2 parentPosition;
	if ( GetParent() )
	{
		parentPosition = GetParent()->GetPosition();
	}

	const Vector2 & offset = m_UseExplicitOffset ? m_ExplicitOffset : m_TempGrabOffset;

	SetBordersFromSizeAndOffset( GetSize(), ( InputManager::Instance().GetMousePosition() - offset ) - parentPosition );
}

//=====================================================================================
void ViewDraggable::OnMouseEnter( const Vector2 & m_MousePosition )
{
	if ( !m_Enabled )
	{
		return;
	}
}

//=====================================================================================
void ViewDraggable::OnMouseLeave( const Vector2 & a_MousePosition )
{
	if ( !m_Enabled )
	{
		return;
	}
}

//=====================================================================================
void ViewDraggable::OnMouseClick( const Vector2 & a_MousePosition, InputManager::MouseButton a_MouseButton )
{
	if ( !m_Enabled )
	{
		return;
	}

	m_Drag = true;

	if ( !m_UseExplicitOffset )
	{
		m_TempGrabOffset = a_MousePosition - GetPosition();
	}
}

//=====================================================================================
void ViewDraggable::OnMouseReleased( const Vector2 & a_MousePosition, InputManager::MouseButton a_MouseButton )
{
	if ( !m_Enabled )
	{
		return;
	}

	m_Drag = false;
}

//=====================================================================================
void ViewDraggable::SetEnabled( bool a_Enabled )
{
	if ( a_Enabled != m_Enabled )
	{
		if ( m_Enabled = a_Enabled ) // when enabled
		{
		}

		else // when disabled
		{
		}
	}
}

//=====================================================================================
void ViewDraggable::SetExplicitOffset( bool a_Flag, const Vector2 & a_Offset )
{
	m_UseExplicitOffset = a_Flag;
	m_ExplicitOffset = a_Offset;
}