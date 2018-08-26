#include "ViewSizer.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
ViewSizer::ViewSizer( const char * a_Name, Page * a_ContainerPage, View * a_Parent )
	: ViewDraggable( a_Name, a_ContainerPage, a_Parent )
{
}

//=====================================================================================
void ViewSizer::OnTick( float a_DeltaTime )
{
	ViewDraggable::OnTick( a_DeltaTime );

	if ( IsBeingDragged() && GetParent() )
	{
		//TODO
		SetBordersFromSizeAndOffset( Vector2( 32.0F ), GetPosition() );
		GetParent()->SetBordersFromSizeAndOffset( InputManager::Instance().GetMousePosition() - GetParent()->GetPosition(), GetParent()->GetPosition() );
	}

	Vector2 a = GetPosition() + GetSize() * 0.1F;
	Vector2 b = GetPosition() + GetSize() * 0.9F;
	
	GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_PRIMARY );
	GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_SECONDARY );
	GraphicsManager::Instance().SetColour( GetTint(), GraphicsManager::COL_TERTIARY );
	GraphicsManager::Instance().GfxDrawTriangle( a, b, Vector2( b.x, a.y ) );
}