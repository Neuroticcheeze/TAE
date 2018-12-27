#include "SceneEditor.hpp"


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
#include <Framework/Utils/Hash.hpp>
#include <GameStructure/UI/Pages/PageWorldMap.hpp>
#include <Framework/Graphics/Vector/VectorGraphic.hpp>
#include <Framework/Graphics/Vector/VectorEllipse.hpp>

PageWorldMap * g_PageWorldMap = nullptr;
VectorGraphic * _vgraphic;

//=====================================================================================
#ifdef APP_IS_SCENE_EDITOR
IApplicationContext * g_TheContext = new SceneEditor;
#endif

//=====================================================================================
void SceneEditor::Initialise()
{
	g_PageWorldMap = new PageWorldMap();
	PageManager::Instance().Push(g_PageWorldMap);
	GraphicsManager::Instance().SetColour(Colour(0.2F, 0.2F, 0.25F), GraphicsManager::Instance().COL_BACKGROUND);
	GraphicsManager::Instance().AllocateRenderTexture(WSID("RT0"), Vector2(32, 32), 1, GraphicsManager::PF_RGBA32F, TextureFilterMode::LINEAR, TextureWrapMode::CLAMP, true);
	_vgraphic = new VectorGraphic();
	_vgraphic->Shapes.Append(new VectorEllipse({ 20.0F, 10.0F }, Colour::RED));
	_vgraphic->Generate();
}

//=====================================================================================
void SceneEditor::Tick( float a_DeltaTime )
{
	if (InputManager::Instance().GetKeyState(InputManager::KEYCODE_A) == InputManager::ButtonState::PRESSED)
	{
		GraphicsManager::Instance().ResizeRenderTexture(WSID("RT0"),
			GraphicsManager::Instance().QueryRenderTextureSize(GraphicsManager::Instance().GetRenderTexture(WSID("RT0"))) * 1.1f
		);
	}

	GraphicsManager::RenderTexture rt0 = GraphicsManager::Instance().GetRenderTexture(WSID("RT0"));
	Vector2 size = GraphicsManager::Instance().QueryRenderTextureSize(rt0);
	//GraphicsManager::Instance().ResizeRenderTexture(WSID("RT0"), Vector2(256, 128));
	//GraphicsManager::Instance().ResizeRenderTexture(rt0, Vector2(256, 128));
	GraphicsManager::Instance().ActivateRenderTexture( rt0, { 0 } );
	GraphicsManager::Instance().TfPushIdentity(false);
	GraphicsManager::Instance().TfViewport(Vector2::ZERO, size);
	GraphicsManager::Instance().SetState(GraphicsManager::RS_TRANSPARENCY, true);
	GraphicsManager::Instance().TfRotate(Engine::Instance().GetTime() * 90);
	_vgraphic->GfxDraw();
	GraphicsManager::Instance().GfxDrawText("FSPSFPSPFP", GraphicsManager::Instance().GetBitmapFont(WSID("BMF_Fps")), UINT_MAX);
	GraphicsManager::Instance().SetState(GraphicsManager::RS_TRANSPARENCY, false);
	////GraphicsManager::Instance().GfxDrawQuadTextured( )
	GraphicsManager::Instance().TfPop();
	GraphicsManager::Instance().DeactivateRenderTexture();

	int32_t sampler = GraphicsManager::Instance().SetTexture(rt0, 0, 4);

	GraphicsManager::Instance().TfPushIdentity(true);
	GraphicsManager::Instance().TfTranslate(Vector2(10));
	GraphicsManager::Instance().GfxDrawQuadTextured(Vector2::ZERO, size * 4, sampler, false, 1.0F, { Vector2::UP, Vector2::ZERO, Vector2::RIGHT, Vector2::ONE } );
	GraphicsManager::Instance().TfPop();
}

//=====================================================================================
void SceneEditor::Finalise()
{

	GraphicsManager::Instance().DeallocateRenderTexture(WSID("RT0"));
	PageManager::Instance().Pop();
	delete g_PageWorldMap;
	g_PageWorldMap = nullptr;
}