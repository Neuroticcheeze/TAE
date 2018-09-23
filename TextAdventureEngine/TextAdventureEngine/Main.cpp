#pragma comment( linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup" )

#include <Framework/ApplicationContext.hpp>
#include <Framework/Engine.hpp>
#include <Framework/Audio/AudioManager.hpp>
#include <Framework/File/Save/SaveManager.hpp>
#include <Framework/File/FileManager.hpp>
#include <Framework/Data/Xml/XmlManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Graphics/Filter/ImageFilterManager.hpp>
#include <Framework/Input/CommandLineManager.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Thread/ThreadManager.hpp>
#include <Framework/Data/String/StringManager.hpp>
#include <Framework/UI/FadeManager.hpp>
#include <Framework/UI/PageManager.hpp>
#include <Framework/Console/ConsoleManager.hpp>

#include <GameStructure/Items/ItemRegistry.hpp>
#include <GameStructure/World/Nature/TimeFlow.hpp>
#include <GameStructure/World/WorldManager.hpp>
#include <GameStructure/PlayerSaveHandler.hpp>

#include <Framework/Graphics/Graphics.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
#define INIT_MANAGER( NAME, ... ) NAME ::Instance().Init( __VA_ARGS__ )
#define INITPOST_MANAGER( NAME ) NAME ::Instance().InitPost()
#define TICK_MANAGER( NAME, DT ) NAME ::Instance().Tick( DT )
#define FINIT_MANAGER( NAME ) NAME ::Instance().Finalise()

//=====================================================================================
void LoadAssets();

//=====================================================================================
// the intermediate rendertarget that all content is drawn onto before rendering it to the screen.
// screen-space effects such as gaussian blur is performed on this target.
GraphicsManager::RenderTexture g_MainRenderTarget;

//=====================================================================================
int main( int args, char *argv[] )
{
	// Allow profiling to begin after this point of the program's lifetime. Any statically
	// defined objects which inherently invoke 'PROFILEX' procedure will not contribute to
	// the final dump, this saves us a few issues with std::unordered_map.
	PROFILE_BEGIN;

#define DECL_SAVEFILE( NAME, HANDLER ) { WSID( #NAME ), #NAME, & HANDLER ::Instance() },
#define SAVEFILES\
	DECL_SAVEFILE( playerdata.dat, PlayerSaveHandler ) /* For saving out/loading in player stats, inventory, world, quests, etc */\
	
#define SCREEN_DIMENSIONS Vector2( 2048.0F, 1280.0F )

	INIT_MANAGER( CommandLineManager, ( const char ** )argv, ( uint32_t )args );
	INIT_MANAGER( ThreadManager );
	INIT_MANAGER( Engine, g_TheContext->GetName(), SCREEN_DIMENSIONS );
	INIT_MANAGER( FileManager );
	INIT_MANAGER( XmlManager );
	INIT_MANAGER( SaveManager, { SAVEFILES }, WSID( "SDVer_AVOCADO" ) );
	INIT_MANAGER( AudioManager );
	INIT_MANAGER( GraphicsManager );
	INIT_MANAGER( ImageFilterManager );
	INIT_MANAGER( InputManager );
	INIT_MANAGER( PageManager );
	INIT_MANAGER( FadeManager );
	INIT_MANAGER( TimeFlowManager );
	INIT_MANAGER( WorldManager );
	INIT_MANAGER( ConsoleManager );

#undef DECL_SAVEFILE
#undef SAVEFILES

	INITPOST_MANAGER( Engine );

	LoadAssets();

	INITPOST_MANAGER( CommandLineManager );
	INITPOST_MANAGER( ThreadManager );
	INITPOST_MANAGER( FileManager );
	INITPOST_MANAGER( XmlManager );
	INITPOST_MANAGER( SaveManager );
	INITPOST_MANAGER( AudioManager );
	INITPOST_MANAGER( GraphicsManager );
	INITPOST_MANAGER( ImageFilterManager );
	INITPOST_MANAGER( InputManager );
	INITPOST_MANAGER( PageManager );
	INITPOST_MANAGER( FadeManager );
	INITPOST_MANAGER( TimeFlowManager );
	INITPOST_MANAGER( WorldManager );
	INITPOST_MANAGER( ConsoleManager );

	Engine::Instance().StartGame( []( float a_DeltaTime )
	{
		static bool isFirstUpdate = true;
		if ( isFirstUpdate )
		{
			g_MainRenderTarget = GraphicsManager::Instance().AllocateRenderTexture( WSID( "__Main__" ), Engine::Instance().GetDisplaySize(), 3, GraphicsManager::PF_RGBA32F, TextureFilterMode::LINEAR, TextureWrapMode::CLAMP );
			g_TheContext->Initialise();

			isFirstUpdate = false;
			return;
		}
		PROFILE2( "Frame" );

		GraphicsManager::Instance().TfPush();
		GraphicsManager::Instance().TfViewport( Vector2::ZERO, Engine::Instance().GetDisplaySize() );
		GraphicsManager::Instance().ActivateRenderTexture( g_MainRenderTarget, { 0 }, true );

		TICK_MANAGER( ThreadManager, a_DeltaTime );
		TICK_MANAGER( AudioManager, a_DeltaTime );
		TICK_MANAGER( FadeManager, a_DeltaTime );
		TICK_MANAGER( InputManager, a_DeltaTime );
		
		g_TheContext->Tick( a_DeltaTime );
		
		FadeManager::Instance().DrawLayer( FadeManager::POST_ENV );
		TICK_MANAGER( PageManager, a_DeltaTime );
		FadeManager::Instance().DrawLayer( FadeManager::POST_UI );
		
		TICK_MANAGER( GraphicsManager, a_DeltaTime );

		// Display final image
		GraphicsManager::Instance().DeactivateRenderTexture();
		GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, false );
		GraphicsManager::Instance().SetState( GraphicsManager::RS_STENCILING, false );
		GraphicsManager::Instance().SetState( GraphicsManager::RS_STENCILING_WRITE, false );
		GraphicsManager::Instance().SetColour( Colour::WHITE, GraphicsManager::COL_PRIMARY );
		int32_t mainrtsamp = GraphicsManager::Instance().SetTexture( g_MainRenderTarget, 0, GraphicsManager::TS_NEXT_AVAILABLE | GraphicsManager::TS_FORCE_LAST_IF_FULL, false );
		GraphicsManager::Instance().GfxDrawQuadTextured( Vector2::ZERO, Engine::Instance().GetDisplaySize(), mainrtsamp, false, 1.0F, { Vector2::UP, Vector2::ZERO, Vector2::RIGHT, Vector2::ONE } );
		
		// Display FPS
		{
			static bool s_DisplayFPS = true;
			if ( InputManager::Instance().GetKeyState( InputManager::KEYCODE_1 ) == InputManager::ButtonState::PRESSED )
			{
				s_DisplayFPS = !s_DisplayFPS;
			}
			if ( s_DisplayFPS )
			{
				auto bitmapFont = GraphicsManager::Instance().LoadBitmapFont( WSID( "BMF_Fps" ), "0123456789FPS", nullptr, 32 );
				CString fpsText = CString().Format( "%u", Engine::Instance().GetFPS() );
				GraphicsManager::Instance().TfPush();
				GraphicsManager::Instance().TfTranslate( Vector2( 0.0F ), true, true );
				GraphicsManager::Instance().SetColour( Colour::WHITE, GraphicsManager::COL_PRIMARY );
				GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
				GraphicsManager::Instance().GfxDrawText( fpsText.Get(), bitmapFont, GraphicsManager::TA_RIGHT, GraphicsManager::TA_TOP );
				GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, false );
				GraphicsManager::Instance().TfPop();
			}
		}

		GraphicsManager::Instance().TfPop();
	} );

	g_TheContext->Finalise();

	FINIT_MANAGER( ConsoleManager );
	FINIT_MANAGER( WorldManager );
	FINIT_MANAGER( TimeFlowManager );
	FINIT_MANAGER( FadeManager );
	FINIT_MANAGER( PageManager );
	FINIT_MANAGER( InputManager );
	FINIT_MANAGER( GraphicsManager );
	FINIT_MANAGER( ImageFilterManager );
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
			GraphicsManager::Instance().LoadFont( WSID( assetName.Get() ), assetPath.Get(), 32 );
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
						GraphicsManager::Instance().LoadTextureAsAtlas( WSID( assetName.Get() ), assetPath.Get(), Vector2( x, y ) );
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