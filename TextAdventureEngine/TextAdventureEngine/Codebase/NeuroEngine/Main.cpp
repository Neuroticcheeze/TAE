#include "Main.hpp"
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
#include <Framework/Compute/ComputeManager.hpp>
#include <Framework/Data/Compression/CompressionManager.hpp>
#include <Framework/Graphics/Particles/ParticleManager.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
void LoadAssets();

//=====================================================================================
// the intermediate rendertarget that all content is drawn onto before rendering it to the screen.
// screen-space effects such as gaussian blur is performed on this target.
GraphicsManager::RenderTexture g_MainRenderTarget;
const float					   g_MainRenderTargetDownscale = 1.0F;

//=====================================================================================
int Launch( IApplicationContext * a_ClientAppCtx, int a_Args, char *a_Argv[] )
{
	ASSERT( a_ClientAppCtx, "No context created! Aborting." );

	// Allow profiling to begin after this point of the program's lifetime. Any statically
	// defined objects which inherently invoke 'PROFILEX' procedure will not contribute to
	// the final dump, this saves us a few issues with std::unordered_map.
	PROFILE_BEGIN;

#define DECL_SAVEFILE( NAME, HANDLER ) { CRC32( #NAME ), #NAME, & HANDLER ::Instance() },
#define SAVEFILES\
	//DECL_SAVEFILE( playerdata.dat, PlayerSaveHandler ) /* For saving out/loading in player stats, inventory, world, quests, etc */\
	
#define SCREEN_DIMENSIONS Vector2( 2048.0F, 1280.0F )

	INIT_MANAGER( CommandLineManager, ( const char ** )a_Argv, ( uint32_t )a_Args );
	INIT_MANAGER( ThreadManager );
	INIT_MANAGER( Engine, a_ClientAppCtx->GetName(), SCREEN_DIMENSIONS );
	INIT_MANAGER( FileManager );
	INIT_MANAGER( XmlManager );
	INIT_MANAGER( ComputeManager );
	INIT_MANAGER( CompressionManager );
	INIT_MANAGER( SaveManager, { SAVEFILES }, CRC32( "SDVer_AVOCADO" ) );
	INIT_MANAGER( AudioManager );
	INIT_MANAGER( GraphicsManager );
	INIT_MANAGER( ImageFilterManager );
	INIT_MANAGER( InputManager );
	INIT_MANAGER( PageManager );
	INIT_MANAGER( FadeManager );
	INIT_MANAGER( ConsoleManager );
	INIT_MANAGER( ParticleManager );

#undef DECL_SAVEFILE
#undef SAVEFILES

	INITPOST_MANAGER( Engine );

	LoadAssets();

	INITPOST_MANAGER( CommandLineManager );
	INITPOST_MANAGER( ThreadManager );
	INITPOST_MANAGER( FileManager );
	INITPOST_MANAGER( XmlManager );
	INITPOST_MANAGER( ComputeManager );
	INITPOST_MANAGER( CompressionManager );
	INITPOST_MANAGER( SaveManager );
	INITPOST_MANAGER( AudioManager );
	INITPOST_MANAGER( GraphicsManager );
	INITPOST_MANAGER( ImageFilterManager );
	INITPOST_MANAGER( InputManager );
	INITPOST_MANAGER( PageManager );
	INITPOST_MANAGER( FadeManager );
	INITPOST_MANAGER( ConsoleManager );
	INITPOST_MANAGER( ParticleManager );

	// Refresh the working directory
	FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE );

	Engine::Instance().StartGame( [&]( float a_DeltaTime )
	{
		static bool isFirstUpdate = true;
		if ( isFirstUpdate )
		{
			g_MainRenderTarget = GraphicsManager::Instance().AllocateRenderTexture( CRC32( "__Main__" ), Engine::Instance().GetDisplaySize() / g_MainRenderTargetDownscale, 3, GraphicsManager::PF_RGBA32F, TextureFilterMode::LINEAR, TextureWrapMode::CLAMP );
			a_ClientAppCtx->Initialise();

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
		
		a_ClientAppCtx->Tick( a_DeltaTime );
		
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
				static uint32_t s_DispSMinFPS = 0;
				static uint32_t s_DispSMaxFPS = 0;
				static uint32_t s_SMinFPS = 0;
				static uint32_t s_SMaxFPS = 0;
				static float	s_SMinMaxTimer = 0.0F;

				if ( s_SMinMaxTimer >= 1.0F )
				{
					s_DispSMinFPS = s_SMinFPS;
					s_DispSMaxFPS = s_SMaxFPS;

					s_SMinMaxTimer = 0.0F;
					s_SMinFPS = Engine::Instance().GetFPS();
					s_SMaxFPS = Engine::Instance().GetFPS();
				}

				else
				{
					s_SMinFPS = Min( s_SMinFPS, Engine::Instance().GetFPS() );
					s_SMaxFPS = Max( s_SMaxFPS, Engine::Instance().GetFPS() );
					s_SMinMaxTimer += Engine::Instance().GetDeltaTime();
				}

				auto bitmapFont = GraphicsManager::Instance().LoadBitmapFont( CRC32( "BMF_Fps" ), " 0123456789FPS~.ms()-", nullptr, 24 );
				CString fpsText = CString::Format( "%u FPS (%u-%u)", Engine::Instance().GetFPS(), s_DispSMinFPS, s_DispSMaxFPS );
				CString fpsMsText = CString::Format( "~%.3fms", Engine::Instance().GetDeltaTime() * 1000.0F );
				GraphicsManager::Instance().TfPush();
				GraphicsManager::Instance().TfTranslate( Vector2( 0.0F ), true, true );
				GraphicsManager::Instance().SetColour( Colour::WHITE, GraphicsManager::COL_PRIMARY );
				GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );
				float line2yPos = GraphicsManager::Instance().GfxDrawText( fpsText.Get(), bitmapFont, GraphicsManager::TA_RIGHT, GraphicsManager::TA_TOP ).y;
				GraphicsManager::Instance().TfTranslate( Vector2( 0.0F, -line2yPos ) );
				GraphicsManager::Instance().SetColour( Colour::WHITE.WithAlpha( 0.5F ), GraphicsManager::COL_PRIMARY );
				GraphicsManager::Instance().GfxDrawText( fpsMsText.Get(), bitmapFont, GraphicsManager::TA_RIGHT, GraphicsManager::TA_TOP );
				GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, false );
				GraphicsManager::Instance().TfPop();
			}
		}

		GraphicsManager::Instance().TfPop();
	} );

	a_ClientAppCtx->Finalise();

	FINIT_MANAGER( ParticleManager );
	FINIT_MANAGER( ConsoleManager );
	FINIT_MANAGER( FadeManager );
	FINIT_MANAGER( PageManager );
	FINIT_MANAGER( InputManager );
	FINIT_MANAGER( GraphicsManager );
	FINIT_MANAGER( ImageFilterManager );
	FINIT_MANAGER( AudioManager );
	FINIT_MANAGER( SaveManager );
	FINIT_MANAGER( CompressionManager );
	FINIT_MANAGER( ComputeManager );
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

		CString assetPath = "" + ( asset[ "path" ] )[ 0U ].GetValue();
		assetPath.ReplaceAll( "\\", "/" );
		while ( assetPath.Find( "//" ) != -1)
		{
			assetPath.ReplaceAll( "//", "/" );
		}

		if ( assetType == "FONT" )
		{
			GraphicsManager::Instance().LoadFont( CRC32( assetName.Get() ), assetPath.Get(), 32 );
		}
		else if ( assetType == "TEXTURE" )
		{
			GraphicsManager::Instance().LoadTexture( CRC32( assetName.Get() ), assetPath.Get() );
		}
		else if ( assetType == "SPRITESHEET" )
		{
			CString filter = ( asset[ "filter" ] )[ 0U ].GetValue();
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
						GraphicsManager::Instance().LoadTextureAsAtlas( CRC32( assetName.Get() ), assetPath.Get(), Vector2( x, y ), filter == "NEAREST_NEIGHBOUR" ? TextureFilterMode::NEAREST : TextureFilterMode::LINEAR );
					}
				}
			}
		}
		else if ( assetType == "SOUND" )
		{
			AudioManager::Instance().LoadAudioBuffer( CRC32( assetName.Get() ), assetPath.Get() );
		}
		else if ( assetType == "STRINGTABLE" )
		{
			StringManager::Instance().DeclareStringTable( CRC32( assetName.Get() ), assetPath.Get() );
		}
	}
}

//=====================================================================================
#undef INIT_MANAGER
#undef INITPOST_MANAGER
#undef TICK_MANAGER
#undef FINIT_MANAGER