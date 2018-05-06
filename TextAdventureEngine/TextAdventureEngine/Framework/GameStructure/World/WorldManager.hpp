#pragma once
#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Template/Registry.hpp>
#include "Locality.hpp"

//=====================================================================================
class WorldManager final : public Manager< WorldManager >
						 , public Registry< WorldManager, Locality, 1024 >
{
public:

	void Init();
	void Finalise();



private:
};

#endif//WORLDMANAGER_H