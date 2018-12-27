#pragma once
#ifndef APPLICATIONCONTEXT_H
#define APPLICATIONCONTEXT_H

//=====================================================================================
class IApplicationContext abstract
{
public:

	virtual void Initialise() abstract;
	virtual void Tick( float a_DeltaTime ) abstract;
	virtual void Finalise() abstract;
	virtual const char * GetName() const abstract;
};

#endif//APPLICATIONCONTEXT_H