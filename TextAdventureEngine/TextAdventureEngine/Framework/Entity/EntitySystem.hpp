#pragma once
#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H

#include <Framework/Template/System.hpp>

//=====================================================================================
class EntitySystem final : public System< EntitySystem >
{
public:

	EntitySystem();
	~EntitySystem();

	void Tick( float a_DeltaTime );

private:

};

#endif//ENTITYSYSTEM_H