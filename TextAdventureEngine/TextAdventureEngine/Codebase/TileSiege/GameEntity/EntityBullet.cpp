#include "EntityBullet.hpp"
#include <TileSiege/WorldManager.hpp>
#include <NeuroEngine/Framework/Graphics/GraphicsManager.hpp>

//=====================================================================================
EntityBullet::EntityBullet()
{
}

//=====================================================================================
EntityBullet::~EntityBullet()
{
}

//=====================================================================================
void EntityBullet::Tick( float a_DeltaTime )
{
	const PosInt32 & physPos = GetPhysicsProxy()->GetPosition();

	GraphicsManager::Instance().SetColour( Colour::GRAY.Lerp( Colour::WHITE, 0.3F ).WithSaturation( 0.4F ), GraphicsManager::COL_PRIMARY );
	GraphicsManager::Instance().SetColour( Colour::GRAY.Lerp( Colour::WHITE, 0.6F ).WithSaturation( 0.4F ) * 0.5F, GraphicsManager::COL_SECONDARY );
	GraphicsManager::Instance().GfxDrawQuad( Vector2( physPos.XPos, physPos.YPos ), Vector2::ONE, 0.05F );

	if ( m_MoveDelayTimer >= 0.2F )
	{
		m_MoveDelayTimer = 0.0F;
		WorldManager::Instance().GetPhysSystem()->MovePhysObject( GetPhysicsProxy(), RectilinearPhysicsSystem::DIR_RIGHT );
	}

	else
	{
		m_MoveDelayTimer += a_DeltaTime;
	}
}