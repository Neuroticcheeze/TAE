#include "Engine.hpp"
#include <SDL/SDL.h>
#include <SDL/SDL_video.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

//=====================================================================================
void Engine::Init()
{
	m_GameLoopTimer.Stop();

	if ( ASSERT( !m_Initialised, "Engine: Already initialised. Call Finalise() first" ) )
	{
		int32_t success = SDL_Init( SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_VIDEO );
		if ( ASSERT( success == 0, "Engine: Failed to initialise Engine! [SDL ERROR: %s]", SDL_GetError() ) )
		{
			success = TTF_Init();
			if ( ASSERT( success == 0, "Engine: Failed to initialise Engine! [SDL ERROR: %s]", TTF_GetError() ) )
			{
				success = IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG );
				if ( ASSERT( success == ( IMG_INIT_PNG | IMG_INIT_JPG ), "Engine: Failed to initialise Engine! [SDL ERROR: %s]", IMG_GetError() ) )
				{
					success = Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
					if ( ASSERT( success == 0, "Engine: Failed to initialise Engine! [SDL ERROR: %s]", Mix_GetError() ) )
					{
						m_Initialised = true;
					}
					else
					{
						IMG_Quit();
						TTF_Quit();
						SDL_Quit();
					}
				}
				else
				{
					TTF_Quit();
					SDL_Quit();
				}
			}
			else
			{
				SDL_Quit();
			}
		}
	}
}

//=====================================================================================
void Engine::InitPost()
{
	if ( ASSERT( m_Initialised && !m_PostInitialised, "Engine: Not initialised, OR already post-initialised; this is an illegal state!" ) )
	{
		int32_t success = SDL_CreateWindowAndRenderer( 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL, 
					reinterpret_cast< SDL_Window** >( &m_Window ),
					reinterpret_cast< SDL_Renderer** >( &m_Renderer ) );

		if ( ASSERT( success == 0, "Engine: Failed to perform Post-Init! [SDL ERROR: %s]", SDL_GetError() ) )
		{
			m_PostInitialised = true;
		}
	}
}

void Engine::StartGame( Consumer< float > a_GameLoopEvent )
{
	if ( ASSERT( m_Initialised && m_PostInitialised, "Engine: Cannot start game! Engine not initialised and post-initialised." ) )
	{
		m_ClientCallback = a_GameLoopEvent;
		m_GameLoopTimer.SetIntervalEvent( Engine::GameLoopCallback );
		m_GameLoopTimer.Start();
		m_RequestedQuit = false;
		while ( m_GameLoopTimer.Tick() );
	}
}

#include <Framework/Input/InputManager.hpp>
//=====================================================================================
void Engine::GameLoopCallback( float a_DT )
{
	Engine * thiz = &Engine::Instance();

	// Poll events
	SDL_Event evt;
	SDL_PollEvent( &evt );
	switch ( evt.type )
	{
	case SDL_QUIT:
		thiz->Quit();
		return;
	case SDL_TEXTINPUT:
		InputManager::Instance().m_TextEvents.Push( evt.text.text[ 0 ] );
	}

	// Invoke the client callback
	if ( thiz->m_ClientCallback )
	{
		thiz->m_ClientCallback( a_DT );
	}	

	// Render
	SDL_Renderer * renderer = static_cast< SDL_Renderer* >( thiz->m_Renderer );
	VERIFY( SDL_RenderClear( renderer ), == 0, "Engine: Failed render op! [SDL ERROR: %s]", SDL_GetError() );
	SDL_RenderPresent( renderer );
}

//=====================================================================================
void Engine::Quit()
{
	m_RequestedQuit = true;
	m_GameLoopTimer.Stop();
}

//=====================================================================================
void Engine::Finalise()
{
	if ( ASSERT( m_Initialised, "Engine: Not initialised. Call Init() first" ) )
	{
		if ( m_PostInitialised )
		{
			SDL_DestroyRenderer( reinterpret_cast< SDL_Renderer* >( m_Renderer ) );
			m_Renderer = nullptr;

			SDL_DestroyWindow( reinterpret_cast< SDL_Window* >( m_Window ) );
			m_Window = nullptr;

			m_PostInitialised = false;
		}

		Mix_CloseAudio();
		IMG_Quit();
		TTF_Quit();
		SDL_Quit(); 
		m_Initialised = false;
	}
}

//=====================================================================================
float Engine::GetTime() const
{
	return m_GameLoopTimer.GetElapsed();
}

//=====================================================================================
float Engine::GetDeltaTime() const
{
	return m_GameLoopTimer.GetDeltaTime();
}

//=====================================================================================
uint32_t Engine::GetFPS() const
{
	return Max( 1U, static_cast< uint32_t >( m_GameLoopTimer.GetFramesPerSecond() ) );
}