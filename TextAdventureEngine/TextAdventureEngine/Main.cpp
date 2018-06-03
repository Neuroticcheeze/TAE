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
#include <Framework/Config/Config.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
#define INIT_MANAGER( NAME, ... ) NAME ::Instance().Init( __VA_ARGS__ )
#define INITPOST_MANAGER( NAME ) NAME ::Instance().InitPost()
#define TICK_MANAGER( NAME, DT ) NAME ::Instance().Tick( DT )
#define FINIT_MANAGER( NAME ) NAME ::Instance().Finalise()

//=====================================================================================
void LoadAssets();
void Application_Initialise();
void Application_Tick( float a_DelatTime );
void Application_Finalise();

//=====================================================================================
int main( int args, char *argv[] )
{
#define DECL_SAVEFILE( NAME, HANDLER ) { WSID( #NAME ), #NAME, & HANDLER ::Instance() },
#define SAVEFILES\
	DECL_SAVEFILE( playerdata.dat, PlayerSaveHandler ) /* For saving out/loading in player stats, inventory, world, quests, etc */\
	
	INIT_MANAGER( CommandLineManager, ( const char ** )argv, ( uint32_t )args );
	INIT_MANAGER( ThreadManager );
	INIT_MANAGER( Engine );
	INIT_MANAGER( FileManager );
	INIT_MANAGER( XmlManager );
	INIT_MANAGER( SaveManager, { SAVEFILES }, Config::SAVE_VERSION );
	INIT_MANAGER( AudioManager );
	INIT_MANAGER( GraphicsManager );
	INIT_MANAGER( InputManager );
	INIT_MANAGER( PageManager );
	INIT_MANAGER( FadeManager );
	INIT_MANAGER( TimeFlowManager );
	INIT_MANAGER( WorldManager );

#undef DECL_SAVEFILE
#undef SAVEFILES

	//LoadAssets();

	INITPOST_MANAGER( CommandLineManager );
	INITPOST_MANAGER( ThreadManager );
	INITPOST_MANAGER( Engine );
	INITPOST_MANAGER( FileManager );
	INITPOST_MANAGER( XmlManager );
	INITPOST_MANAGER( SaveManager );
	INITPOST_MANAGER( AudioManager );
	INITPOST_MANAGER( GraphicsManager );
	INITPOST_MANAGER( InputManager );
	INITPOST_MANAGER( PageManager );
	INITPOST_MANAGER( FadeManager );
	INITPOST_MANAGER( TimeFlowManager );
	INITPOST_MANAGER( WorldManager );

	Engine::Instance().StartGame( []( float a_DeltaTime )
	{
		static bool isFirstUpdate = true;
		if ( isFirstUpdate )
		{
			Application_Initialise();
			isFirstUpdate = false;
			return;
		}

		TICK_MANAGER( ThreadManager, a_DeltaTime );
		TICK_MANAGER( AudioManager, a_DeltaTime );
		TICK_MANAGER( FadeManager, a_DeltaTime );
		TICK_MANAGER( InputManager, a_DeltaTime );
		TICK_MANAGER( PageManager, a_DeltaTime );

		Application_Tick( a_DeltaTime );

		// Display FPS
		{	
			static float dtCaptureTimer = 0.5F;
			static float dtCapture = 0.0F;

			if ( dtCaptureTimer >= 0.5F )
			{
				dtCaptureTimer = 0.0F;
				dtCapture = a_DeltaTime;
			}

			dtCaptureTimer += a_DeltaTime;

			CString fpsString = CString().Format( "FPS: %.2f", 1.0F / dtCapture );
			Graphics::SetFont( _GetFont( "DEFAULT" ) );
			Graphics::SetForeColor( Colour::CYAN );
			Graphics::SetTextAlign( Graphics::TEXT_ALIGN_LEFT );
			Graphics::DrawText( Vector2( 10.0F, Config::WINDOW_Y - 10.0F - Graphics::GetTextHeight( fpsString.Get() ) ), fpsString.Get() );
		}

		Graphics::SetBackColor( Colour::GRAY );
		
	} );

	Application_Finalise();

	FINIT_MANAGER( WorldManager );
	FINIT_MANAGER( TimeFlowManager );
	FINIT_MANAGER( FadeManager );
	FINIT_MANAGER( PageManager );
	FINIT_MANAGER( InputManager );
	FINIT_MANAGER( GraphicsManager );
	FINIT_MANAGER( AudioManager );
	FINIT_MANAGER( SaveManager );
	FINIT_MANAGER( XmlManager );
	FINIT_MANAGER( FileManager );
	FINIT_MANAGER( Engine );
	FINIT_MANAGER( ThreadManager );
	FINIT_MANAGER( CommandLineManager );
	
	PROFILE_DUMP_RECORDINGS;
	return 0;
}

//=====================================================================================
void LoadAssets()
{
	Array< XmlObject > assets;

	FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE );
	FileStream xmlfile = FileManager::Instance().Open( "res/assets.xml", FileManager::READ );
	if ( xmlfile.IsOpen() )
	{
		XmlObject assetxml = XmlManager::Instance().ParseXml( xmlfile.ReadText().Get() );
		FileManager::Instance().Close( xmlfile );
		assetxml.FindChildren( { 
			{ XmlObject::Filter::NAME, "asset" }, 
			{ XmlObject::Filter::ATTRIBUTE, "name" }, 
			{ XmlObject::Filter::ATTRIBUTE, "type" } },
			assets );
	}

	for ( uint32_t k = 0; k < assets.Count(); ++k )
	{
		const auto & asset = assets[ k ];

		CString assetName, assetType;

		XmlObject::AttributeType type;
		auto nameAttrib = asset.GetAttribute( "name", &type );
		if ( type == XmlObject::AttributeType::STRING )
		{
			assetName = nameAttrib.String;
		}
		auto typeAttrib = asset.GetAttribute( "type", &type );
		if ( type == XmlObject::AttributeType::STRING )
		{
			assetType = typeAttrib.String;
			assetType.ToUpper();
		}

		CString assetPath = "res/" + ( asset[ "path" ] )[ 0U ].GetValue();
		assetPath.ReplaceAll( "\\", "/" );
		while ( assetPath.Find( "//" ) != -1)
		{
			assetPath.ReplaceAll( "//", "/" );
		}

		if ( assetType == "FONT" )
		{
			GraphicsManager::Instance().LoadFont( WSID( assetName.Get() ), assetPath.Get() );
		}
		else if ( assetType == "TEXTURE" )
		{
			GraphicsManager::Instance().LoadTexture( WSID( assetName.Get() ), assetPath.Get() );
		}
		else if ( assetType == "SPRITESHEET" )
		{
			CString divs = ( asset[ "divs" ] )[ 0U ].GetValue();
			const auto & divsSegs = divs.Split( "," );
			if ( divsSegs.Count() >= 2 )
			{
				float x, y;
				const CString & divXStr = divsSegs[ 0 ].Trim();
				const CString & divYStr = divsSegs[ 1 ].Trim();
				if ( CString::Parse( divXStr.Get(), x ) && CString::Parse( divYStr.Get(), y ) )
				{
					if ( x > 0.0F && y > 0.0F )
					{
						GraphicsManager::Instance().LoadTextureAsSpriteSheet( WSID( assetName.Get() ), assetPath.Get(), Vector2( x, y ) );
					}
				}
			}
		}
		else if ( assetType == "SOUND" )
		{
			AudioManager::Instance().LoadAudioFile( WSID( assetName.Get() ), assetPath.Get() );
		}
		else if ( assetType == "STRINGTABLE" )
		{
			StringManager::Instance().DeclareStringTable( WSID( assetName.Get() ), assetPath.Get() );
		}
	}
}

//=====================================================================================
#undef INIT_MANAGER
#undef INITPOST_MANAGER
#undef TICK_MANAGER
#undef FINIT_MANAGER

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////    APPLICATION    //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#include <GameStructure/UI/Pages/PageWorldMap.hpp>
PageWorldMap * g_PageWorldMap = nullptr;

//=====================================================================================
void Application_Initialise()
{
	ItemRegistry::Instance().RegisterItem( Item( Item::GENERIC, WSID( "broken_lantern" ), "Broken Kensington Lantern", "An older Kensington-made lantern. Once a lantern of exceptional brightness, used in the early lighthouses - it vapourised a special mixture of common and rare fuels to produce unrivaled luminosity. Alas, this one in particular has a broken wickplace and fuel-feeder... However, there is still some precious Kensington-grade fuel left in the resovoir - a valuable comodity still.", 6 ) );
	ItemRegistry::Instance().RegisterItem( Item( Item::FOOD, WSID( "apple" ), "Pink Lady Apple", "A delicious crisp apple of the 'Pink Lady' variety.", 7 ) );
	ItemRegistry::Instance().RegisterItem( Item( Item::FOOD, WSID( "item2" ), "Green Lady Apple", "A delicious crisp apple of the 'Green Lady' variety.", 6 ) );
	ItemRegistry::Instance().RegisterItem( Item( Item::FOOD, WSID( "item3" ), "Red Lady Apple", "A delicious crisp apple of the 'Red Lady' variety.", 1 ) );
	ItemRegistry::Instance().RegisterItem( Item( Item::FOOD, WSID( "item4" ), "Purple Lady Apple", "A delicious crisp apple of the 'Purple Lady' variety.", 7 ) );

	g_PageWorldMap = new PageWorldMap();
	PageManager::Instance().Push( g_PageWorldMap );
}

//=====================================================================================
void Application_Tick( float a_DelatTime )
{
}

//=====================================================================================
void Application_Finalise()
{
	PageManager::Instance().Pop();
	delete g_PageWorldMap;
	g_PageWorldMap = nullptr;
}