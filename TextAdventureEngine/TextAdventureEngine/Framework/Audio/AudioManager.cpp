#include "AudioManager.hpp"
#include <PORTAUDIO/portaudio.h>
#include <sndfile.hh>

//=====================================================================================
#define CHECK if ( ASSERT( m_InitialisedApi, "Failed to call \"%s\" - AudioManager has not successfully initialised!", __FUNCTION__ ) ) return

//=====================================================================================
struct SFile final
{
	SNDFILE * File;
	SF_INFO Info;
};

//=====================================================================================
struct StreamRealtimeParameters final
{
	float Volume = 1.0F;
	float Pan = 0.0F;
	float Pitch = 1.0F;
	bool Reverse = false;
};

//=====================================================================================
struct StreamTrackerData final
{
	int32_t LoopsRemaining;
};

//=====================================================================================
struct Stream final
{
	PaStream * StreamPtr;
	AudioStreamConfiguration Config;
	AudioFile SourceFileData;
	StreamRealtimeParameters Params;
	StreamTrackerData TrackerData;
};

//=====================================================================================
static int RealtimeCallback_StandardF32(
	  const void * a_Input
	, void * a_Output
	, unsigned long a_FrameCount
	, const PaStreamCallbackTimeInfo * a_TimeInfo
	, PaStreamCallbackFlags StatusFlags
	, void * a_UserData
)
{
	float * out = ( float* ) a_Output;
	Stream * streamData = ( Stream* ) a_UserData;
	sf_count_t numRead;
	const sf_count_t numChannels = streamData->SourceFileData->Info.channels;
	const uint32_t blockSize = a_FrameCount * numChannels;
	const uint32_t blockSizeBytes = a_FrameCount * numChannels * sizeof( float );

	const float &	PARAM_vol = streamData->Params.Volume;
	const float &	PARAM_pch = streamData->Params.Pitch;
	const float &	PARAM_pan = streamData->Params.Pan;
	const bool &	PARAM_rev = streamData->Params.Reverse;

	// Clear the output buffer and read from loaded audiofile buffer
	BSet( out, 0, blockSizeBytes );
	numRead = sf_read_float( streamData->SourceFileData->File, out, blockSize );

	// Iterate over block and apply runtime parameters to the frames
	const float * end = out + numRead;
	while ( out != end )
	{
		switch ( numChannels )
		{
		case 1:
			( *out++ ) *= PARAM_vol;
			break;
		case 2:
			( *out++ ) *= PARAM_vol * ( PARAM_pan <= 0.0F ? 1.0F : ( 1.0F - PARAM_pan ) );
			( *out++ ) *= PARAM_vol * ( PARAM_pan >= 0.0F ? 1.0F : ( 0.0F - PARAM_pan ) );
			break;
		default:
			out++;
			break;
		}
	}

	// We've reached the end of the stream if we couldn't read a full block
	if ( numRead < blockSize )
	{
		int32_t & lr = streamData->TrackerData.LoopsRemaining;
		if ( streamData->Config.LoopAudio && lr != 0 )
		{
			if ( lr != AudioStreamConfiguration::INF )
			{
				--lr;
			}

			sf_seek( streamData->SourceFileData->File, 0, SF_SEEK_SET );
			sf_read_float( streamData->SourceFileData->File, out + numRead, blockSize - numRead );
			
			return paContinue;
		}

		return paComplete;
	}

	return paContinue;
}

//=====================================================================================
void AudioManager::Init()
{
	if ( !ASSERT( !m_InitialisedApi, "AudioManager is already initialised, call Finalise() first" ) )
	{
		return;
	}

	PROFILE;

#define PA_INIT_ATTEMPTS 5

	m_InitialisedApi = true;

	PaError result;
	uint32_t timeout = PA_INIT_ATTEMPTS;

	do
	{
		result = Pa_Initialize();
		if ( ASSERT( result == paNoError, "Failed PortAudio initialisation attempt #%d with error: %d [%s]", ( PA_INIT_ATTEMPTS + 1 ) - ( int32_t )timeout, result, Pa_GetErrorText( result ) ) )
		{
			break;
		}

		else
		{
			--timeout;
		}
	}
	while ( timeout == 0 );

	if ( !ASSERT( result == paNoError, "Failed to initialise PortAudio - INITIALISATION-TIMED-OUT" ) )
	{
		Finalise();
	}

#undef PA_INIT_ATTEMPTS

	m_MusicFade = 0.0F;
	m_MusicFadeTime = 0.0F;
}

//=====================================================================================
void AudioManager::Finalise()
{
	PROFILE;
	CHECK;

	PaError result;
	ASSERT( ( result = Pa_Terminate() ) == paNoError, "PortAudio failed to terminate! %d [%s]", result, Pa_GetErrorText( result ) );
	m_InitialisedApi = false;

	// Release all AudioStreams
	Pointer< Stream > * audStrIt = m_AudioStreams.First();
	const Pointer< Stream > * audStrEnd = m_AudioStreams.End();
	while ( audStrIt != audStrEnd )
	{
		Pa_CloseStream( ( *audStrIt++ )->StreamPtr );
	}
	m_AudioStreams.Clear();

	// Release all AudioFile
	auto audFileIt = m_AudioFileMap.GetEntries().First();
	auto audFileEnd = m_AudioFileMap.GetEntries().End();
	while ( audFileIt != audFileEnd )
	{
		sf_close( audFileIt->Value->File );
	}
	m_AudioFileMap.GetEntries().Clear();
}

//=====================================================================================
void AudioManager::Tick( float a_DeltaTime )
{
	PROFILE;
	CHECK;

	auto it = m_AudioStreams.First();
	const auto end = m_AudioStreams.End();

	while ( it != end )
	{
		PaError error;
		if ( ( error = Pa_IsStreamActive( ( *it )->StreamPtr ) ) == 0 )
		{
			Pa_CloseStream( ( *it )->StreamPtr );
			m_AudioStreams.Remove( ( const Pointer< Stream > * )it );
		}

		ASSERT( error >= paNoError, "AudioManager: Error occured while updating audiostream" );

		++it;
	}

	// If next music track is queued.
	if ( m_NextMusicTrack )
	{
		// Start fading the value
		m_MusicFade += a_DeltaTime;
		const float t = Clamp( m_MusicFade / m_MusicFadeTime );

		SetAudioStreamParameter( m_CurrentMusicTrack, VOLUME, 1.0F - t );
		SetAudioStreamParameter( m_NextMusicTrack, VOLUME, t );

		// If fade is complete, switch stream over
		if ( t == 1.0F )
		{
			CloseAudioStream( m_CurrentMusicTrack );
			m_CurrentMusicTrack = m_NextMusicTrack;
			m_NextMusicTrack = AudioStream(); // weakpointer to null stream
		}
	}
}

//=====================================================================================
AudioFile AudioManager::LoadAudioFile( uint32_t a_HashName, const char * a_Path )
{
	PROFILE;
	CHECK AudioFile();

	SFile data;
	data.File = sf_open( a_Path, SFM_READ, &data.Info );
	if ( sf_error( data.File ) != SF_ERR_NO_ERROR )
	{
		PRINT( "Failed to load audio file: \"%s\" [%s]", a_Path, sf_strerror( data.File ) );
		return AudioFile();
	}
	
	Pointer< SFile > ptr( data );
	m_AudioFileMap.Put( a_HashName, ptr );

	return AudioFile( ptr );
}

//=====================================================================================
void AudioManager::UnloadAudioFile( uint32_t a_HashName )
{
	PROFILE;
	CHECK;

	if ( m_AudioFileMap.Contains( a_HashName ) )
	{
		auto & ptr = *m_AudioFileMap[ a_HashName ];
		
		sf_close( ptr.Ptr()->File );

		m_AudioFileMap.Remove( ptr );
	}
}

//=====================================================================================
AudioFile AudioManager::GetAudioFile( uint32_t a_HashName ) const
{
	PROFILE;
	CHECK AudioFile();

	if ( m_AudioFileMap.Contains( a_HashName ) )
	{
		return AudioFile( *m_AudioFileMap[ a_HashName ] );
	}

	return AudioFile();
}

//=====================================================================================
AudioStream AudioManager::OpenAudioStream( AudioStreamConfiguration a_Config, const AudioFile & a_SourceAudioData )
{
	PROFILE;
	CHECK AudioStream();

	if ( !ASSERT( a_SourceAudioData, "'OpenAudioStream' - No AudioFile supplied!" ) )
	{
		return AudioStream();
	}

	if ( !ASSERT( !( a_Config.CloseOnFinish && a_Config.LoopAudio && ( a_Config.LoopCount < 1 || a_Config.LoopCount != AudioStreamConfiguration::INF ) ),
						"AudioStreamConfiguration has an illegal combination of values: "
						"The configuration is set to loop indefinitely but also close on finish... "
						"Setting 'CloseOnFinish' to false & 'LoopCount' to INFINITE" ) )
	{
		a_Config.CloseOnFinish = false;
		a_Config.LoopCount = a_Config.INF;
	}

	Pointer< Stream > ptr = Pointer< Stream >( Stream() );
	ptr->Config = a_Config;
	ptr->SourceFileData = a_SourceAudioData;

	PaError error;

	/* Open PaStream with values read from the file */
	error = Pa_OpenDefaultStream( &ptr->StreamPtr
		, 0										/* no input */
		, ptr->SourceFileData->Info.channels    /* stereo out */
		, paFloat32								/* floating point */
		, ptr->SourceFileData->Info.samplerate
		, a_Config.FrameBufferSize
		, RealtimeCallback_StandardF32
		, ptr.Ptr() );							/* our stream wrapper struct provide any and all info we need in realtime*/

	if ( !ASSERT( error == paNoError, "'OpenAudioStream' - Failed to create AudioStream: [%s]", Pa_GetErrorText( error ) ) )
	{
		return AudioStream();
	}

	m_AudioStreams.Append( ptr );
	return AudioStream( ptr );
}

//=====================================================================================
void AudioManager::CloseAudioStream( const AudioStream & a_AudioStream )
{
	PROFILE;
	CHECK;

	if ( ASSERT( a_AudioStream, "'CloseAudioStream' - AudioStream is NULL!" ) )
	{
		const Pointer< Stream > * ptr = m_AudioStreams.Find< const AudioStream & >( []( const AudioStream & a_UserData, const Pointer< Stream > & a_PtrInList )
		{ 
			return a_PtrInList == a_UserData; 
		}, a_AudioStream );

		if ( ptr && *ptr )
		{
			PaError err = Pa_CloseStream( ptr->Ptr()->StreamPtr );

			if ( ASSERT( err == paNoError, "'CloseAudioStream' - Failed to close AudioStream: [%s]", Pa_GetErrorText( err ) ) )
			{
				m_AudioStreams.Remove( ptr );
				return;
			}
		}
	}
}

//=====================================================================================
void AudioManager::PlayAudioStream( const AudioStream & a_AudioStream )
{
	PROFILE;
	CHECK;

	if ( ASSERT( a_AudioStream, "'StopAudioStream' - AudioStream is NULL!" ) )
	{
		a_AudioStream.Ptr()->TrackerData.LoopsRemaining = a_AudioStream.Ptr()->Config.LoopCount;
		auto res = Pa_StartStream( a_AudioStream.Ptr()->StreamPtr );
		ASSERT( res == paNoError, "'StopAudioStream' - ERROR: [%s]", Pa_GetErrorText( res ) );
	}
}

//=====================================================================================
void AudioManager::StopAudioStream( const AudioStream & a_AudioStream, bool a_ForceOperation )
{
	PROFILE;
	CHECK;

	if ( ASSERT( a_AudioStream, "'StopAudioStream' - AudioStream is NULL!" ) )
	{
		PaError res;
		
		if ( a_ForceOperation )
		{
			res = Pa_AbortStream( a_AudioStream.Ptr()->StreamPtr );
		}

		else
		{
			res = Pa_StopStream( a_AudioStream.Ptr()->StreamPtr );
		}
		
		ASSERT( res == paNoError, "'StopAudioStream' - ERROR: [%s]", Pa_GetErrorText( res ) );
	}
}

//=====================================================================================
bool AudioManager::IsAudioStreamPlaying( const AudioStream & a_AudioStream ) const
{
	PROFILE;
	CHECK false;

	if ( ASSERT( a_AudioStream, "'IsAudioStreamPlaying' - AudioStream is NULL!" ) )
	{
		auto res = Pa_IsStreamActive( a_AudioStream.Ptr()->StreamPtr );

		if ( res >= 0 )
		{
			return res == ( PaError )1;
		}

		ASSERT( false, "'IsAudioStreamPlaying' - ERROR: [%s]", Pa_GetErrorText( res ) );
	}

	return false;
}

//=====================================================================================
void AudioManager::SetAudioStreamParameter( const AudioStream & a_AudioStream, ParameterType a_ParameterType, float a_Value )
{
	PROFILE;
	CHECK;

	if ( ASSERT( a_AudioStream, "'SetAudioStreamParameter' - AudioStream is NULL!" ) )
	{
		StreamRealtimeParameters & params = a_AudioStream->Params;

		switch ( a_ParameterType )
		{
		case AudioManager::VOLUME:
			params.Volume = Max( 0.0F, a_Value );
			break;
		case AudioManager::PITCH:
			params.Pitch = Abs( a_Value );
			params.Reverse = a_Value < 0.0F;
			break;
		case AudioManager::PAN:
			params.Pan = Clamp( a_Value );
			break;
		default:
			break;
		}
	}
}

//=====================================================================================
void AudioManager::SetMusicTrack( uint32_t a_HashName, float a_FadeTime )
{
	PROFILE;
	CHECK;

	static const AudioStreamConfiguration s_MusicConfig { false, true };
	if ( !m_CurrentMusicTrack )
	{
		m_CurrentMusicTrack = OpenAudioStream( s_MusicConfig, GetAudioFile( a_HashName ) );
		PlayAudioStream( m_CurrentMusicTrack );
	}
	else
	{
		m_NextMusicTrack = OpenAudioStream( s_MusicConfig, GetAudioFile( a_HashName ) );
		PlayAudioStream( m_NextMusicTrack );
		m_MusicFade = 0.0F;
		m_MusicFadeTime = Max( 0.1F, a_FadeTime );
	}
}

//=====================================================================================
void AudioManager::PlaySound( uint32_t a_AudioFileHashName, float a_Volume, float a_Pitch, float a_Pan )
{
	PROFILE;
	CHECK;

	AudioFile audfile = GetAudioFile( a_AudioFileHashName );
	if ( audfile )
	{
		AudioStreamConfiguration config{ true, false };
		AudioStream stream = OpenAudioStream( config, audfile );

		if ( stream )
		{
			SetAudioStreamParameter( stream, AudioManager::VOLUME, a_Volume );
			SetAudioStreamParameter( stream, AudioManager::PITCH, a_Pitch );
			SetAudioStreamParameter( stream, AudioManager::PAN, a_Pan );
			PlayAudioStream( stream );
		}
	}
}

#undef CHECK