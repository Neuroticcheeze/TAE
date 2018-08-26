#include "DialogueEditor.hpp"


#include <Framework/Engine.hpp>
#include <Framework/Audio/AudioManager.hpp>
#include <Framework/File/Save/SaveManager.hpp>
#include <Framework/File/FileManager.hpp>
#include <Framework/Data/Xml/XmlManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Input/CommandLineManager.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Thread/ThreadManager.hpp>
#include <Framework/Data/String/StringManager.hpp>
#include <Framework/UI/FadeManager.hpp>
#include <Framework/UI/PageManager.hpp>
#include <GameStructure/Items/ItemRegistry.hpp>
#include <GameStructure/World/Nature/TimeFlow.hpp>
#include <GameStructure/World/WorldManager.hpp>
#include <GameStructure/PlayerSaveHandler.hpp>
#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Utils/Hash.hpp>
#include <[Tool] Dialogue Editor/UI/Pages/PageDialogueEditor.hpp>
#include <[Tool] Dialogue Editor/Globals.hpp>

PageDialogueEditor * g_PageDialogueEditor = nullptr;

//=====================================================================================
#ifdef APP_IS_DIALOGUE_EDITOR
IApplicationContext * g_TheContext = new DialogueEditor;
#endif

//=====================================================================================
void DialogueEditor::Initialise()
{
	Globals::CreateBitmapFonts();
	g_PageDialogueEditor = new PageDialogueEditor();
	PageManager::Instance().Push( g_PageDialogueEditor );
}

//=====================================================================================
void DialogueEditor::Tick( float a_DeltaTime )
{
}

//=====================================================================================
void DialogueEditor::Finalise()
{
	PageManager::Instance().Pop();
	Free( g_PageDialogueEditor );
	Globals::ReleaseBitmapFonts();
}