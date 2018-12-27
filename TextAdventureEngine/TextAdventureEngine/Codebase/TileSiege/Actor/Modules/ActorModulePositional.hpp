#pragma once
#ifndef ACTORMODULEPOSITIONAL_H
#define ACTORMODULEPOSITIONAL_H

#include <TileSiege/Actor/ActorModule.hpp>
#include <NeuroEngine/Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
ACTOR_MODULE_CONFIG_DECL( ActorModulePositional )
{
	ACTOR_MODULE_CONFIG( ActorModulePositional );

	void Write( DataStream & a_Writer ) const override {}
	void Read( DataStream & a_Reader ) override {}

public:
};

//=====================================================================================
class ActorModulePositional
	: public IActorModule
{
	ACTOR_MODULE( ActorModulePositional )
public:

	virtual bool IsRequestingTicks() const override { return false; }

	void SetPosition( const Vector2 & a_Position );
	const Vector2 & GetPosition() const;

private:

	Vector2 m_Position;
};

#endif//ACTORMODULEPOSITIONAL_H