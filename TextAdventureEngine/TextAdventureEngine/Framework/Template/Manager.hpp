#pragma once
#ifndef MANAGER_H
#define MANAGER_H

#include <Framework/Template/Singleton.hpp>

//=====================================================================================
template< typename T, typename ... PARAMS >
class Manager : public Singleton< T >
{
public:
	virtual void Init( PARAMS ... a_Parameters ) = 0;
	virtual void InitPost() {}
	virtual void Finalise() = 0;
};

//=====================================================================================
template< typename T, typename ... PARAMS >
class TickableManager : public Manager< T, PARAMS ... >
{
public:
	virtual void Tick( float a_DeltaTime = 0.0F ) = 0;
};

#endif//MANAGER_H