#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H

//=====================================================================================
template< typename T >
class System
{
public:
	virtual void Tick( float a_DeltaTime = 0.0F ) {}
};

#endif//SYSTEM_H