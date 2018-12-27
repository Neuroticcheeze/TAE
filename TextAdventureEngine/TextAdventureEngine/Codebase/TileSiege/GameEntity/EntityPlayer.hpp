#pragma once
#ifndef ENTITYPLAYER_H
#define ENTITYPLAYER_H

#include <TileSiege/GameEntity/GameEntity.hpp>
#include <TileSiege/Input/GameInputManager.hpp>

//=====================================================================================
class EntityPlayer 
	: private GameEntity
	, private GameInputManager::IListener
{
	GAME_ENTITY_TYPE( EntityPlayer, 0x4f6c89df );
public:

	EntityPlayer();
	~EntityPlayer();

	bool IsRequestingTicks() const override { return true; }
	void Tick( float a_DeltaTime ) override;

	void OnInputBegin( uint32_t a_InputEvent, float a_Value ) override;
	void OnInputUpdate( uint32_t a_InputEvent, float a_Value ) override;
	void OnInputEnd( uint32_t a_InputEvent, float a_Value ) override;

private:

	Vector2 m_DeficitToProxy;
};

#endif//ENTITYPLAYER_H