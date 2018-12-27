#include "AudioManager.hpp"
#include <Framework/File/FileManager.hpp>

#define SFML_STATIC
#pragma comment( lib, "openal32.lib" )
#ifdef _DEBUG
#pragma comment( lib, "sfml-system-s-d.lib" )
#pragma comment( lib, "sfml-audio-s-d.lib" )
#else
#pragma comment( lib, "sfml-system-s.lib" )
#pragma comment( lib, "sfml-audio-s.lib" )
#endif//_DEBUG
#pragma comment( lib, "vorbis.lib" )
#pragma comment( lib, "vorbisenc.lib" )
#pragma comment( lib, "vorbisfile.lib" )
#pragma comment( lib, "flac.lib" )
#pragma comment( lib, "ogg.lib" )
#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundStream.hpp>

#include <Framework/Console/ConsoleManager.hpp>

//=====================================================================================
void AudioManager::Init()
{
}

//=====================================================================================
void AudioManager::InitPost()
{
}

//=====================================================================================
void AudioManager::Finalise()
{
}

//=====================================================================================
void AudioManager::Tick( float a_DeltaTime )
{
	auto it = m_ProcessedSoundSourceRefs.Last();
	auto itF = m_ProcessedSoundSourceRefs.First() - 1;

	while ( it && it != itF )
	{
		if ( !IsPlaying( *it ) )
		{
			SoundSource ssrc = *it;
			m_ProcessedSoundSourceRefs.Remove( ssrc );
			DestroySoundSource( ssrc );
		}

		--it;
	}
}

//=====================================================================================
AudioManager::AudioBuffer AudioManager::LoadAudioBuffer( uint32_t a_HashName, const char * a_Path )
{
	if ( !m_AudioBuffers.Contains( a_HashName ) )
	{
		FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE, "res/audio/" );
		const CString & fullPath = FileManager::Instance().GetWorkingDirectory( a_Path );
		
		auto soundBuff = new sf::SoundBuffer;
		bool loaded = soundBuff->loadFromFile( fullPath.Get() );
		if ( !ASSERT_WARN( loaded, "Failed to load audio file [id=%u, path='%s']", a_HashName, fullPath.Get() ) )
		{
			return AudioBuffer();
		}

		m_AudioBuffers.Put( a_HashName, PAudioBuffer( soundBuff ) );

		ConsoleManager::Instance().Praise( CString::Format( "Loaded Audio: \"%s\"", fullPath.Get() ) );
	}
	
	return AudioBuffer( *m_AudioBuffers[ a_HashName ] );
}

//=====================================================================================
bool AudioManager::UnloadAudioBuffer( uint32_t a_HashName )
{
	if ( ASSERT_WARN( m_AudioBuffers.Contains( a_HashName ), "(%s) The audio file by ID '%u' does not exist, ignoring.", __FUNCTION__, a_HashName ) )
	{
		PAudioBuffer & audioFile = *m_AudioBuffers[ a_HashName ];
		
		auto soundBuf = reinterpret_cast< sf::SoundBuffer* >( *audioFile.Ptr() );
		delete soundBuf;

		return m_AudioBuffers.Remove( a_HashName );
	}

	return false;
}

//=====================================================================================
bool AudioManager::UnloadAudioBuffer( const AudioBuffer & a_AudioBuffer )
{
	auto pair = m_AudioBuffers.GetEntries().Find< const AudioBuffer & >( []( const AudioBuffer & a_It, const Dictionary< uint32_t, PAudioBuffer >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_AudioBuffer );
	
	if ( pair )
	{
		return UnloadAudioBuffer( pair->Key );
	}

	return false;
}

//=====================================================================================
AudioManager::AudioBuffer AudioManager::GetAudioBuffer( uint32_t a_HashName ) const
{
	PAudioBuffer * ptr = m_AudioBuffers[ a_HashName ];
	return ptr ? AudioBuffer( *ptr ) : AudioBuffer();
}

//=====================================================================================
float AudioManager::GetAudioBufferDuration( const AudioBuffer & a_AudioBuffer ) const
{
	if ( a_AudioBuffer )
	{
		auto soundBuf = reinterpret_cast< sf::SoundBuffer* >( *a_AudioBuffer.Ptr() );
		ASSERT( soundBuf, "Unhandled missing buffer... fatal error!" );

		return soundBuf->getDuration().asSeconds();
	}

	return 0.0F;
}

//=====================================================================================
AudioManager::SoundSource AudioManager::CreateSoundSource( const SoundParameters & a_InitialParameters = SoundParameters() )
{
	PSoundSource ss( SoundSource::PtrType( new sf::Sound, SoundParameters() ) );
	m_SoundSources.Append( ss );
	SoundSource ssw( *m_SoundSources.Last() );
	SetSoundParameters( ssw, a_InitialParameters );
	return ssw;
}

//=====================================================================================
void AudioManager::DestroySoundSource( const SoundSource & a_SoundSource )
{
	Stop( a_SoundSource );
	m_SoundSources.Remove( a_SoundSource );
}

//=====================================================================================
SoundParameters AudioManager::GetSoundParameters( const SoundSource & a_SoundSource ) const
{
	return a_SoundSource->Value1;
}

//=====================================================================================
void AudioManager::SetSoundParameters( const SoundSource & a_SoundSource, const SoundParameters & a_Params )
{
	if ( a_SoundSource )
	{
		auto sound = reinterpret_cast< sf::Sound* >( a_SoundSource->Value0 );
		ASSERT( sound, "Unhandled missing sound... fatal error!" );

		// Volume
		if ( a_SoundSource->Value1.Volume != a_Params.Volume )
		{
			sound->setVolume( a_Params.Volume );
		}

		// Pitch
		if ( a_SoundSource->Value1.Pitch != a_Params.Pitch )
		{
			sound->setPitch( a_Params.Pitch );
		}

		// Loop
		if ( a_SoundSource->Value1.Loop != a_Params.Loop )
		{
			sound->setLoop( a_Params.Loop );
		}

		// Position
		if ( a_SoundSource->Value1.Position != a_Params.Position )
		{
			sound->setPosition( sf::Vector3f( a_Params.Position.x, a_Params.Position.y, 0.1F/*z*/ ) );
		}

		// Min Distance
		if ( a_SoundSource->Value1.MinDistance != a_Params.MinDistance )
		{
			sound->setMinDistance( a_Params.MinDistance );
		}

		// Attenuation
		if ( a_SoundSource->Value1.Attenuation != a_Params.Attenuation )
		{
			sound->setAttenuation( a_Params.Attenuation );
		}

		// Relative
		if ( a_SoundSource->Value1.Relative != a_Params.Relative )
		{
			sound->setRelativeToListener( a_Params.Relative );
		}

		// Buffer Offset
		if ( a_SoundSource->Value1.BufferOffset != a_Params.BufferOffset )
		{
			sound->setPlayingOffset( sf::seconds( a_Params.BufferOffset ) );
		}

		// Buffer
		AudioBuffer lhs = a_SoundSource->Value1.Buffer;
		AudioBuffer rhs = a_Params.Buffer;
		if ( a_SoundSource->Value1.Buffer != a_Params.Buffer )
		{
			auto soundBuf = reinterpret_cast< sf::SoundBuffer* >( *a_Params.Buffer.Ptr() );
			sound->setBuffer( *soundBuf );
		}

		a_SoundSource->Value1 = a_Params;
	}
}

//=====================================================================================
void AudioManager::Play( const SoundSource & a_SoundSource )
{
	if ( a_SoundSource )
	{
		auto sound = reinterpret_cast< sf::Sound* >( a_SoundSource->Value0 );
		ASSERT( sound, "Unhandled missing sound... fatal error!" );

		sound->play();
	}
}

//=====================================================================================
void AudioManager::Pause( const SoundSource & a_SoundSource )
{
	if ( a_SoundSource )
	{
		auto sound = reinterpret_cast< sf::Sound* >( a_SoundSource->Value0 );
		ASSERT( sound, "Unhandled missing sound... fatal error!" );

		sound->pause();
	}
}

//=====================================================================================
void AudioManager::Stop( const SoundSource & a_SoundSource )
{
	if ( a_SoundSource )
	{
		auto sound = reinterpret_cast< sf::Sound* >( a_SoundSource->Value0 );
		ASSERT( sound, "Unhandled missing sound... fatal error!" );

		sound->stop();
	}
}

//=====================================================================================
bool AudioManager::IsPlaying( const SoundSource & a_SoundSource ) const
{
	if ( a_SoundSource )
	{
		auto sound = reinterpret_cast< sf::Sound* >( a_SoundSource->Value0 );
		ASSERT( sound, "Unhandled missing sound... fatal error!" );

		return sound->getStatus() == sf::Sound::Status::Playing;
	}

	return false;
}

//=====================================================================================
bool AudioManager::IsPaused( const SoundSource & a_SoundSource ) const
{
	if ( a_SoundSource )
	{
		auto sound = reinterpret_cast< sf::Sound* >( a_SoundSource->Value0 );
		ASSERT( sound, "Unhandled missing sound... fatal error!" );

		return sound->getStatus() == sf::Sound::Status::Paused;
	}

	return false;
}

//=====================================================================================
bool AudioManager::IsStopped( const SoundSource & a_SoundSource ) const
{
	if ( a_SoundSource )
	{
		auto sound = reinterpret_cast< sf::Sound* >( a_SoundSource->Value0 );
		ASSERT( sound, "Unhandled missing sound... fatal error!" );

		return sound->getStatus() == sf::Sound::Status::Stopped;
	}

	return false;
}

//=====================================================================================
void AudioManager::SfxPlaySound( const SoundParameters & a_Params )
{
	float duration = GetAudioBufferDuration( a_Params.Buffer );

	if ( duration == 0.0F )
	{
		return;
	}

	ASSERT_WARN( ( duration - a_Params.BufferOffset ) <= 10.0F, "SfxPlaySound: Playing a singleshot sound that has more than 10 seconds of playing time.. It's advised to use soundsources instead." );

	SoundSource ss;

	if ( a_Params.Loop )
	{
		SoundParameters params = a_Params;
		params.Loop = false;

		ASSERT_WARN( false, "SfxPlaySound: An attempt to play a looping sound this way is not allowed, (LOOP set to FALSE). Use soundsources instead!" );

		ss = CreateSoundSource( params );
	}

	else
	{
		ss = CreateSoundSource( a_Params );
	}

	Play( ss );
	m_ProcessedSoundSourceRefs.Append( ss );
}