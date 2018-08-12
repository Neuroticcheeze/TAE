#include "Engine.hpp"
#include <Framework/Input/InputManager.hpp>

#include <SDL/SDL.h>
#include <SDL/SDL_video.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#include <GL/glew.h>
#include <SDL/SDL_opengl.h>
#include <gl/glu.h>

//=====================================================================================
#ifdef  _DEBUG
void GLAPIENTRY MessageCallback( GLenum source,
								 GLenum type,
								 GLuint id,
								 GLenum severity,
								 GLsizei length,
								 const GLchar* message,
								 const void* userParam )
{
  PRINT( "Engine GL Error: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}
#endif//_DEBUG

//=====================================================================================
void Engine::Init( const char * a_Title, const Vector2 & a_DisplaySize )
{
	m_Title = a_Title;
	m_DisplaySize = a_DisplaySize;

	m_GameLoopTimer.Stop();

	if ( ASSERT( !m_Initialised, "Engine: Already initialised. Call Finalise() first" ) )
	{
		int32_t success = SDL_Init( SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO );
		if ( ASSERT( success == 0, "Engine: Failed to initialise Engine! [SDL ERROR: %s]", SDL_GetError() ) )
		{
			success = TTF_Init();
			if ( ASSERT( success == 0, "Engine: Failed to initialise Engine! [SDL ERROR: %s]", TTF_GetError() ) )
			{
				success = IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG );
				if ( ASSERT( success == ( IMG_INIT_PNG | IMG_INIT_JPG ), "Engine: Failed to initialise Engine! [SDL ERROR: %s]", IMG_GetError() ) )
				{
					success = Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096 );
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
		SDL_DisplayMode dispMode;
		SDL_GetCurrentDisplayMode( 0, &dispMode );

		int32_t success = SDL_CreateWindowAndRenderer( 
					static_cast< int32_t >( m_DisplaySize.x ), 
					static_cast< int32_t >( m_DisplaySize.y ), 
					SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL,
					reinterpret_cast< SDL_Window** >( &m_Window ),
					reinterpret_cast< SDL_Renderer** >( &m_Renderer ) );

		if ( ASSERT( success == 0, "Engine: Failed to perform Post-Init! [SDL ERROR: %s]", SDL_GetError() ) )
		{
			SDL_SetWindowTitle( reinterpret_cast< SDL_Window* >( &m_Window ), m_Title.Get() );

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

			m_GlCtx = SDL_GL_CreateContext( reinterpret_cast< SDL_Window* >( m_Window ) );

			if ( ASSERT( m_GlCtx, "Engine: Failed to perform Post-Init! [SDL ERROR: %s]", SDL_GetError() ) )
			{
				glewExperimental = GL_TRUE; 
                GLenum glewError = glewInit();

                if ( ASSERT( glewError == GLEW_OK, "Engine: Failed to perform Post-Init! [GLEW ERROR: %s]", glewGetErrorString( glewError ) ) )
                {
#ifdef _DEBUG
					glEnable( GL_DEBUG_OUTPUT );
					glDebugMessageCallback( MessageCallback, 0 );
#endif
					glEnable( GL_SCISSOR_TEST );

					glClearDepth( 1.0F );
					glClearStencil( 0 );

					m_PostInitialised = true;
                }
				
				else
				{
					SDL_GL_DeleteContext( reinterpret_cast< SDL_GLContext* >( m_GlCtx ) );
					m_GlCtx = nullptr;

					SDL_DestroyRenderer( reinterpret_cast< SDL_Renderer* >( m_Renderer ) );
					m_Renderer = nullptr;

					SDL_DestroyWindow( reinterpret_cast< SDL_Window* >( m_Window ) );
					m_Window = nullptr;
				}
			}

			else
			{
				SDL_DestroyRenderer( reinterpret_cast< SDL_Renderer* >( m_Renderer ) );
				m_Renderer = nullptr;

				SDL_DestroyWindow( reinterpret_cast< SDL_Window* >( m_Window ) );
				m_Window = nullptr;
			}
		}
	}
}

//=====================================================================================
void Engine::StartGame( Consumer< float > a_GameLoopEvent )
{
	if ( ASSERT( m_Initialised && m_PostInitialised, "Engine: Cannot start game! Engine not initialised and post-initialised." ) )
	{
		//m_GameLoopTimer.SetIntervalFPS( 120.0F );

		m_ClientCallback = a_GameLoopEvent;
		m_GameLoopTimer.SetIntervalEvent( Engine::GameLoopCallback );
		m_GameLoopTimer.Start();
		m_RequestedQuit = false;
		while ( m_GameLoopTimer.Tick() );
	}
}

//=====================================================================================
void Engine::GameLoopCallback( float a_DT )
{
	Engine * thiz = &Engine::Instance();

	// Poll events
	SDL_Event evt;
	while ( SDL_PollEvent( &evt ) != 0 )
	{
		switch ( evt.type )
		{
		case SDL_QUIT:
			thiz->Quit();
			return;
		case SDL_KEYDOWN:
			if ( evt.key.keysym.scancode == SDL_SCANCODE_BACKSPACE )
			{
				InputManager::Instance().m_TextEvents.Push( InputManager::TEXTCODE_BACKSPACE );
			}
			break;
		case SDL_TEXTINPUT:
			InputManager::Instance().m_TextEvents.Push( evt.text.text[ 0 ] );
		}
	}

	// Invoke the client callback
	if ( thiz->m_ClientCallback )
	{
		thiz->m_ClientCallback( a_DT );
	}	

	// Render
	//SDL_Renderer * renderer = static_cast< SDL_Renderer* >( thiz->m_Renderer );
	//VERIFY( SDL_RenderClear( renderer ), == 0, "Engine: Failed render op! [SDL ERROR: %s]", SDL_GetError() );
	//SDL_RenderPresent( renderer );

	SDL_GL_SwapWindow( reinterpret_cast< SDL_Window* >( thiz->m_Window ) );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
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
			SDL_GL_DeleteContext( reinterpret_cast< SDL_GLContext* >( m_GlCtx ) );
			m_GlCtx = nullptr;

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