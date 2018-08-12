#pragma once
#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

#include <Framework/ApplicationContext.hpp>

//=====================================================================================
class SceneEditor final
	: public IApplicationContext
{
public:
	
	void Initialise() override;
	void Tick( float a_DeltaTime ) override;
	void Finalise() override;
	const char * GetName() const override { return "Scene Editor"; }
};

#endif//SCENEEDITOR_H