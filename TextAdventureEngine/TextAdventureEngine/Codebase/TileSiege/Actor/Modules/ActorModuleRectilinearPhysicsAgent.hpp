#pragma once
#ifndef ACTORMODULEPHYSICSSPACIAL_H
#define ACTORMODULEPHYSICSSPACIAL_H

#include <NeuroEngine/Framework/Physics/RectilinearPhysicsSystem.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <TileSiege/Actor/ActorModule.hpp>

//=====================================================================================
ACTOR_MODULE_CONFIG_DECL( ActorModuleRectilinearPhysicsAgent )
{
	ACTOR_MODULE_CONFIG( ActorModuleRectilinearPhysicsAgent );

	void Write( DataStream & a_Writer ) const override;
	void Read( DataStream & a_Reader ) override;

	float m_MovementSpeed = 5.0F;

public:

	inline float GetMovementSpeed() const { return m_MovementSpeed; }
};

//=====================================================================================
class ActorModuleRectilinearPhysicsAgent
	: public IActorModule
{
	ACTOR_MODULE( ActorModuleRectilinearPhysicsAgent )
public:

	ActorModuleRectilinearPhysicsAgent();
	~ActorModuleRectilinearPhysicsAgent();

	virtual void Tick( float a_DeltaTime ) override;
	virtual bool IsRequestingTicks() const override { return true; }

	bool Move( RectilinearPhysicsSystem::Direction a_Direction );

private:

	RectilinearPhysicsSystem::PhysObject m_PhysicsProxy;
	Vector2 m_DeficitToProxy;
};

#endif//ACTORMODULEPHYSICSSPACIAL_H