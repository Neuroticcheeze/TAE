#include "GameEntity.hpp"

//=====================================================================================
// Registry of all GameEntity type classes, add to this to register a new GameEntity
#include "EntityPlayer.hpp"
#include "EntityBullet.hpp"
//=====================================================================================
void GameEntity::Registry()
{
	REGISTER_GAME_ENTITY_TYPE( EntityPlayer );
	REGISTER_GAME_ENTITY_TYPE( EntityBullet );
}

//=====================================================================================
Dictionary< GameEntityTypeId, Functor< GameEntity*() > > GameEntity::s_GameEntityFactory;

//=====================================================================================
GameEntity * GameEntity::CreateFromTypeId( GameEntityTypeId a_TypeId )
{
	Functor< GameEntity*() > * factory = s_GameEntityFactory[ a_TypeId ];
	if ( factory )
	{
		return ( *factory )();
	}

	return nullptr;
}