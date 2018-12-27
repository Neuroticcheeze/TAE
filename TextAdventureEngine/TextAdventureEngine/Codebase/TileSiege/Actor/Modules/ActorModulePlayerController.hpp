#pragma once
#ifndef ACTORMODULEPLAYERCONTROLLER_H
#define ACTORMODULEPLAYERCONTROLLER_H

#include <NeuroEngine/Framework/Physics/RectilinearPhysicsSystem.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <TileSiege/Actor/ActorModule.hpp>
#include <TileSiege/Actor/Actor.hpp>
#include <TileSiege/Input/GameInputManager.hpp>

//=====================================================================================
ACTOR_MODULE_CONFIG_DECL( ActorModulePlayerController )
{
	ACTOR_MODULE_CONFIG( ActorModulePlayerController );

	void Write( DataStream & a_Writer ) const override;
	void Read( DataStream & a_Reader ) override;

public:
	float f;
};

//=====================================================================================
class ActorModulePlayerController
	: public IActorModule
	, private GameInputManager::IListener
{
	ACTOR_MODULE( ActorModulePlayerController )
public:

	ActorModulePlayerController();
	~ActorModulePlayerController();

	virtual bool IsRequestingTicks() const override { return false; }

private:

	void OnInputBegin( uint32_t a_InputEvent, float a_Value ) override;
	void OnInputUpdate( uint32_t a_InputEvent, float a_Value ) override;
	void OnInputEnd( uint32_t a_InputEvent, float a_Value ) override;
};

#endif//ACTORMODULEPLAYERCONTROLLER_H