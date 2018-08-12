#pragma once
#ifndef GAME_H
#define GAME_H

#include <Framework/ApplicationContext.hpp>

//=====================================================================================
class PageMainMenu;
class PageOptions;

//=====================================================================================
class Game final
	: public IApplicationContext
{
public:
	
	void Initialise() override;
	void Tick( float a_DeltaTime ) override;
	void Finalise() override;
	const char * GetName() const override { return "TAE Game"; }

	static PageMainMenu * GetMainMenuUI() { return m_MainMenu; }
	static PageOptions * GetOptionsUI() { return m_Options; }

private:
	static PageMainMenu * m_MainMenu;
	static PageOptions * m_Options;
};

#endif//GAME_H