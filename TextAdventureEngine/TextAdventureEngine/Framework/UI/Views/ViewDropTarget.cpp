#include "ViewDropTarget.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewDropTarget::ViewDropTarget( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
	, m_Enabled( false )
{
	SetEnabled( true );
}

//=====================================================================================
void ViewDropTarget::SetEnabled( bool a_Enabled )
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
void ViewDropTarget::OnDrop( ViewDraggable * a_Draggable )
{
	if ( m_Enabled )
	{
		View::OnDrop( *this, *a_Draggable );
	}
}