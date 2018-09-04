#include "ViewDraggable.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewDraggable::ViewDraggable( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Drag( false )
	, m_UseExplicitOffset( false )
{
	SetEnabled();
	SetIsInteractible( true );
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
}

//=====================================================================================
void ViewDraggable::OnMouseLeave( const Vector2 & a_MousePosition )
{
}

//=====================================================================================
void ViewDraggable::OnMouseClick( const Vector2 & a_MousePosition, InputManager::MouseButton a_MouseButton )
{
	m_Drag = true;

	if ( !m_UseExplicitOffset )
	{
		m_TempGrabOffset = a_MousePosition - GetPosition();
	}
}

//=====================================================================================
void ViewDraggable::OnMouseReleased( const Vector2 & a_MousePosition, InputManager::MouseButton a_MouseButton )
{
	m_Drag = false;
}

//=====================================================================================
void ViewDraggable::SetExplicitOffset( bool a_Flag, const Vector2 & a_Offset )
{
	m_UseExplicitOffset = a_Flag;
	m_ExplicitOffset = a_Offset;
}