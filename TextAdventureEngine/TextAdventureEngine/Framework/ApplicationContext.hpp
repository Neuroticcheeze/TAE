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

//=====================================================================================
// A .cpp containing the implementation of a derived version of IApplicationContext
// must define 'g_TheContext' to be a pointer to an instance of the derived version.
extern IApplicationContext * g_TheContext;

#endif//APPLICATIONCONTEXT_H