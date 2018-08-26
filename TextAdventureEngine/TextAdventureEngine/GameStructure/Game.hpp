#pragma once
#ifndef GAME_H
#define GAME_H

#include <Framework/ApplicationContext.hpp>

//=====================================================================================
class PageMainMenu;
class PageOptions;
class PageDialogue;

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
	static PageDialogue * GetDialogueUI() { return m_Dialogue; }

private:

	static PageMainMenu * m_MainMenu;
	static PageOptions * m_Options;
	static PageDialogue * m_Dialogue;
};

#endif//GAME_H