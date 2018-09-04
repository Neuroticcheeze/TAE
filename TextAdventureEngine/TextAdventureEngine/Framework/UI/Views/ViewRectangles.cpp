#include "ViewRectangles.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Math/Curve/Bezier.hpp>

//=====================================================================================
void ViewRectangles::OnTick( float a_DeltaTime )
{
	auto it = m_RectInfos.First();
	const auto end = m_RectInfos.End();

	while ( it != end )
	{
		const Vector2 & post = ( it->Position * GetSize() ) + GetPosition();
		const Vector2 & size = ( it->Size * GetSize() );

		if ( it->TintA.a < 1.0F || it->TintB.a < 1.0F  )
		{
			GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
		}

		GraphicsManager::Instance().SetColour( it->TintA, GraphicsManager::COL_PRIMARY );
		GraphicsManager::Instance().SetColour( it->TintB, GraphicsManager::COL_SECONDARY );
		GraphicsManager::Instance().GfxDrawQuad( post, size, it->Thickness );

		++it;
	}

	float f = Abs( InputManager::Instance().GetMousePosition().x - ( GetPosition() + Vector2::UP * GetSize().y * 0.5F ).x ) / 1.5F;
	
	Bezier bezier;
	bezier.AddControlPoint( GetPosition() + Vector2::UP * GetSize().y * 0.5F );
	bezier.AddControlPoint( GetPosition() + Vector2::UP * GetSize().y * 0.5F + Vector2::RIGHT * -f );
	bezier.AddControlPoint( InputManager::Instance().GetMousePosition() + Vector2::RIGHT * f );
	bezier.AddControlPoint( InputManager::Instance().GetMousePosition() );
	GraphicsManager::Instance().GfxDrawBezier( bezier, 3.0F );
}