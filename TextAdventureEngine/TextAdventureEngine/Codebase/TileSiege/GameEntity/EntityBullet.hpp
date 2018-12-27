#pragma once
#ifndef ENTITYBULLET_H
#define ENTITYBULLET_H

#include <TileSiege/GameEntity/GameEntity.hpp>

//=====================================================================================
class EntityBullet 
	: private GameEntity
{
	GAME_ENTITY_TYPE( EntityBullet, 0x4f6c89da );
public:

	EntityBullet();
	~EntityBullet();

	bool IsRequestingTicks() const override { return true; }
	void Tick( float a_DeltaTime ) override;

private:

	float m_MoveDelayTimer = 0.0F;
};

#endif//ENTITYPLAYER_H