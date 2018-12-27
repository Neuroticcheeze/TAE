#include "ViewDropTarget.hpp"
#include <Framework/Input/InputManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewDropTarget::ViewDropTarget( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: View( a_Name, a_ContainerPage, a_Parent )
{
	SetEnabled();
}

//=====================================================================================
void ViewDropTarget::OnDrop( ViewDraggable * a_Draggable )
{
	if ( GetEnabled() )
	{
		View::OnDrop( *this, *a_Draggable );
	}
}