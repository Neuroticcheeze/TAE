#include "Game.hpp"
#include <GameStructure/Globals.hpp>
#include <Framework/Engine.hpp>
#include <Framework/UI/PageManager.hpp>
#include <GameStructure/UI/Pages/PageMainMenu.hpp>
#include <GameStructure/UI/Pages/PageOptions.hpp>
#include <GameStructure/UI/Pages/PageDialogue.hpp>

//=====================================================================================
#ifdef APP_IS_GAME
IApplicationContext * g_TheContext = new Game;
#endif

//=====================================================================================
PageMainMenu * Game::m_MainMenu = nullptr;
PageOptions * Game::m_Options = nullptr;
PageDialogue * Game::m_Dialogue = nullptr;


#include <Framework/Graphics/Particles/ParticleSystem.hpp>
#include <Framework/Graphics/Particles/Particle.hpp>
#include <Framework/Graphics/Particles/Emitter.hpp>
#include <Framework/Graphics/Particles/EmitterInfo.hpp>
#include <Framework/Utils/Blackboard.hpp>
#include <Framework/Data/String/StringTable.hpp>
#include <GameStructure/Dialogue/Dialogue.hpp>
static ParticleSystem psys;
WeakPointer< Emitter > emitter;
#include <GameStructure/World/Parallax.hpp>
#include <Framework/Data/Xml/XmlManager.hpp>
#include <Framework/File/FileManager.hpp>

Parallax parallax( { Parallax::Layer(), Parallax::Layer(), Parallax::Layer(), Parallax::Layer(), Parallax::Layer() } );
class Listener : public Parallax::Layer::IListener
{
	void OnPreDraw( Parallax::Layer & a_Layer )
	{

		psys.Tick(Engine::Instance().GetDeltaTime());
	}

	void OnPostDraw( Parallax::Layer & a_Layer )
	{
	}
} myListener;


#include <functional>

void foo(const std::function< void() > & func) { func(); }
StringTable tbl;
StringEntry::Iterator * iter;
Vector2 formattedTextOffset = Vector2::ZERO;

//=====================================================================================
void Game::Initialise()
{
	Globals::CreateBitmapFonts();

	m_MainMenu = new PageMainMenu();
	m_Options = new PageOptions();
	m_Dialogue = new PageDialogue();

	FadeManager::Instance().PushFadeData( FadeManager::POST_UI, Colour::BLACK );
	FadeManager::Instance().PushFadeData( FadeManager::POST_UI, Colour::BLACK * Colour::INVISIBLE, 1.0F );

	PageManager::Instance().Push( Game::GetMainMenuUI() );





	parallax[0].SetDistance( 0.0F );
	parallax[0].SetTexture(_GetTexture("Scene3"));
	parallax[1].SetDistance(550.0F);
	parallax[1].SetTexture(_GetTexture("Scene3"));
	parallax[2].SetDistance( 900.0F );
	parallax[2].SetTexture(_GetTexture("Scene2"));
	parallax[3].SetDistance( 990.0F );
	parallax[3].SetTexture(_GetTexture("Scene1"));
	parallax[4].SetDistance( 1000.0F );
	parallax[4].SetTexture(_GetTexture("Scene0"));
	parallax.SetBackScale( 1.0F );
	parallax.SetForeScale( 1.333F );
	parallax.SetCameraViewArea( Engine::Instance().GetDisplaySize() );

	ParticleInfo particleInfo;
	particleInfo.Acceleration = Vector2( 0.0F, -9.8F );
	particleInfo.Life = RandomRange( 2.0F, 6.0F );
	particleInfo.TintOverTime.AddKeyFrame( 0.0F, Colour::BLACK );
	particleInfo.TintOverTime.AddKeyFrame( 0.1F, Colour::ORANGE.Lerp( Colour::WHITE, 0.5F ) );
	particleInfo.TintOverTime.AddKeyFrame( 1.0F, Colour::BLACK );
	particleInfo.AdditiveBlendOverTime.AddKeyFrame( 0.0F, 1.0F );
	particleInfo.AdditiveBlendOverTime.AddKeyFrame( 1.0F, 1.0F );
	particleInfo.ScaleOverTime.AddKeyFrame( 0.0F, 50.0F );
	particleInfo.ScaleOverTime.AddKeyFrame( 1.0F, 5.0F );
	particleInfo.Density = -1.2F;
	particleInfo.Sprite = _GetSpriteSheet( "WEATHER_ICONS" );
	particleInfo.AnimationOverTime.AddKeyFrame( 0.0F, 1.0F );
	particleInfo.AnimationOverTime.AddKeyFrame( 0.5F, 2.0F );
	psys = ParticleSystem(64, particleInfo);
	EmitterInfo emitterInfo;
	emitterInfo.m_Position = Vector2(1500.0F, 150.0F);
	emitterInfo.m_Rate = 0.1F;
	emitterInfo.m_Accuracy = 0.2F;
	emitterInfo.m_Radius = 20.0F;
	emitter = psys.CreateEmitter(emitterInfo);
	
	parallax[2].RegisterListener(myListener);

	

	Blackboard::Value blackboard;
	Blackboard::Value playerDataCompound;
	playerDataCompound.PutValue("Health", Blackboard::Value().PutValue("Current", 33.0F).PutValue("Prev", 32.0F));
	playerDataCompound.PutValue("Position", Blackboard::Value().PutValue("InWater", true).PutValue("X", -500.0F).PutValue("Y", 21200.0F));
	blackboard.PutValue("PlayerData", playerDataCompound);
	blackboard.GetValue(WSID("PlayerData")).PutValue("Health", Blackboard::Value::NIL);
	PRINT( blackboard.ToString().Get() );
}
#include <Framework/Math/Curve/Bezier.hpp>
//=====================================================================================
void Game::Tick( float a_DeltaTime )
{
	//parallax.SetBackScale( 1.0F + Sin( Engine::Instance().GetTime() ) * 0.5F + 0.5F );
	parallax.SetCameraOffset( ( InputManager::Instance().GetMousePosition() - Engine::Instance().GetDisplaySize() / 2 ) * 0.001F );
	parallax.DrawComposite( a_DeltaTime );

	/*
	GraphicsManager::Instance().SetState(GraphicsManager::RS_TRANSPARENCY, true);
	GraphicsManager::Instance().SetColour(Colour::WHITE * Colour(1,1,1,0.25F), GraphicsManager::COL_PRIMARY);
	for ( float i = 0; i < 2000; i += 50.0F )
	{
		GraphicsManager::Instance().GfxDrawLine( Vector2( i, 0.0F ), Vector2(i, 2000.0F), 1 );
		GraphicsManager::Instance().GfxDrawLine( Vector2( 0.0F, i ), Vector2(2000.0F,i), 1 );
	}*/
}

//=====================================================================================
void Game::Finalise()
{
	while ( PageManager::Instance().Peek() != nullptr )
	{
		PageManager::Instance().Pop();
		PageManager::Instance().Tick( 0.0F );
	}

	Free( m_MainMenu );
	Free( m_Options );
	Free( m_Dialogue );

	Globals::ReleaseBitmapFonts();
}