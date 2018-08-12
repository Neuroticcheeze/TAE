#include "AudioManager.hpp"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

//=====================================================================================
#define CHECK if ( ASSERT( m_InitialisedApi, "Failed to call \"%s\" - AudioManager has not successfully initialised!", __FUNCTION__ ) ) return

//=====================================================================================
//struct SFile final
//{
//	SNDFILE * File;
//	SF_INFO Info;
//};
//
//=====================================================================================
struct StreamRealtimeParameters final
{
	float Volume = 1.0F;
	float Pan = 0.0F;
	float Pitch = 1.0F;
	bool Reverse = false;
};




































































namespace AudioSpec
{
	int allocatedMixChannelsCount = 0;  // number of mix channels allocated

										// read-only!
	Uint16 format;  // current audio format constant
	int frequency;  // frequency rate of the current audio format
	int channelCount;  // number of channels of the current audio format
}

// this could be a macro as well
static inline Uint16 formatSampleSize(Uint16 format)
{
	return (format & 0xFF) / 8;
}

// Get chunk time length (in ms) given its size and current audio format
static int computeChunkLengthMillisec(int chunkSize)
{
	/* bytes / samplesize == sample points */
	const Uint32 points = chunkSize / formatSampleSize(AudioSpec::format);

	/* sample points / channels == sample frames */
	const Uint32 frames = (points / AudioSpec::channelCount);

	/* (sample frames * 1000) / frequency == play length, in ms */
	return ((frames * 1000) / AudioSpec::frequency);
}

// Custom handler object to control which part of the Mix_Chunk's audio data will be played, with which pitch-related modifications.
// This needed to be a template because the actual Mix_Chunk's data format may vary (AUDIO_U8, AUDIO_S16, etc) and the data type varies with it (Uint8, Sint16, etc)
// The AudioFormatType should be the data type that is compatible with the current SDL_mixer-initialized audio format.
template<typename AudioFormatType>
struct CustomSdlMixerPlaybackSpeedEffectHandler
{
	float& speed;
	Mix_Chunk* chunk;
	int position;  // current position of the sound, in ms
	bool touched;  // false if this playback has never been pitched.

				   // read-only!
	bool loop;
	int duration;  // the duration of the sound, in ms
	int chunkSize;  // the size of the sound, as a number of indexes (or sample points). thinks of this as a array size when using the proper array type (instead of just Uint8*).

	CustomSdlMixerPlaybackSpeedEffectHandler(float& speed, Mix_Chunk* chunk, bool loop)
		: speed(speed), chunk(chunk),
		position(0), touched(false), loop(loop), duration(0), chunkSize(0)
	{
		duration = computeChunkLengthMillisec(chunk->alen);
		chunkSize = chunk->alen / formatSampleSize(AudioSpec::format);
	}

	// processing function to be able to change chunk speed/pitch.
	void modifyStreamPlaybackSpeed(int mixChannel, void* stream, int length)
	{
		const float speedFactor = speed;
		const int channelCount = AudioSpec::channelCount, frequency = AudioSpec::frequency;

		const AudioFormatType* chunkData = reinterpret_cast<AudioFormatType*>(chunk->abuf);

		AudioFormatType* buffer = static_cast<AudioFormatType*>(stream);
		const int bufferSize = length / sizeof(AudioFormatType);  // buffer size (as array)
		const int bufferDuration = computeChunkLengthMillisec(length);  // buffer time duration

		if (not touched)  // if playback is still untouched
		{
			// if playback is still untouched and no pitch is requested this time, skip pitch routine and leave stream untouched.
			if (speedFactor == 1.0f)
			{
				// if there is still sound to be played
				if (position < duration or loop)
				{
					// just update position
					position += bufferDuration;

					// reset position if looping
					if (loop) while (position > duration)
						position -= duration;
				}
				else  // if we already played the whole sound, halt channel
				{
					// set silence on the buffer since Mix_HaltChannel() poops out some of it for a few ms.
					for (int i = 0; i < bufferSize; i++)
						buffer[i] = 0;

					Mix_HaltChannel(mixChannel);
				}

				return;  // skipping pitch routine
			}
			// if pitch is required for the first time
			else
				touched = true;  // mark as touched and proceed to the pitch routine.
		}

		// if there is still sound to be played
		if (position < duration or loop)
		{
			const float delta = 1000.0 / frequency,   // normal duration of each sample
				delta2 = delta * speedFactor; // virtual stretched duration, scaled by 'speedFactor'

			for (int i = 0; i < bufferSize; i += channelCount)
			{
				const int j = i / channelCount; // j goes from 0 to size/channelCount, incremented 1 by 1
				const float x = position + j * delta2;  // get "virtual" index. its corresponding value will be interpolated.
				const int k = floor(x / delta);  // get left index to interpolate from original chunk data (right index will be this plus 1)
				const float proportion = (x / delta) - k;  // get the proportion of the right value (left will be 1.0 minus this)

														   // usually just 2 channels: 0 (left) and 1 (right), but who knows...
				for (int c = 0; c < channelCount; c++)
				{
					// check if k will be within bounds
					if (k*channelCount + channelCount - 1 < chunkSize or loop)
					{
						AudioFormatType  leftValue = chunkData[(k   * channelCount + c) % chunkSize],
							rightValue = chunkData[((k + 1) * channelCount + c) % chunkSize];

						// put interpolated value on 'data' (linear interpolation)
						buffer[i + c] = (1 - proportion)*leftValue + proportion * rightValue;
					}
					else  // if k will be out of bounds (chunk bounds), it means we already finished; thus, we'll pass silence
					{
						buffer[i + c] = 0;
					}
				}
			}

			// update position
			position += bufferDuration * speedFactor; // this is not exact since a frame may play less than its duration when finished playing, but its simpler

													  // reset position if looping
			if (loop) while (position > duration)
				position -= duration;
		}

		else  // if we already played the whole sound but finished earlier than expected by SDL_mixer (due to faster playback speed)
		{
			// set silence on the buffer since Mix_HaltChannel() poops out some of it for a few ms.
			for (int i = 0; i < bufferSize; i++)
				buffer[i] = 0;

			Mix_HaltChannel(mixChannel);
		}
	}

	// a shorter name to improve readability
	typedef CustomSdlMixerPlaybackSpeedEffectHandler Handler;

	// Mix_EffectFunc_t callback that redirects to handler method (handler passed via userData)
	static void mixEffectFuncCallback(int channel, void* stream, int length, void* userData)
	{
		static_cast<Handler*>(userData)->modifyStreamPlaybackSpeed(channel, stream, length);
	}

	// Mix_EffectDone_t callback that deletes the handler at the end of the effect usage  (handler passed via userData)
	static void mixEffectDoneCallback(int channel, void *userData)
	{
		delete static_cast<Handler*>(userData);
	}

	// function to register a handler to this channel for the next playback.
	static void registerOnChannel(int channel, float& speed, Mix_Chunk* chunk, bool loop)
	{
		Mix_RegisterEffect(channel, Handler::mixEffectFuncCallback, Handler::mixEffectDoneCallback, new Handler(speed, chunk, loop));
	}
};

// register proper effect handler according to the current audio format. effect valid for the next playback only.
void setupForNextPlayback(float& speed, Mix_Chunk* chunk, int channel, bool loop)
{
	// select the register function for the current audio format and register the effect using the compatible handlers
	// xxx is it correct to behave the same way to all S16 and U16 formats? Should we create case statements for AUDIO_S16SYS, AUDIO_S16LSB, AUDIO_S16MSB, etc, individually?
	switch (AudioSpec::format)
	{
	case AUDIO_U8:  CustomSdlMixerPlaybackSpeedEffectHandler<Uint8 >::registerOnChannel(channel, speed, chunk, loop); break;
	case AUDIO_S8:  CustomSdlMixerPlaybackSpeedEffectHandler<Sint8 >::registerOnChannel(channel, speed, chunk, loop); break;
	case AUDIO_U16: CustomSdlMixerPlaybackSpeedEffectHandler<Uint16>::registerOnChannel(channel, speed, chunk, loop); break;
	default:
	case AUDIO_S16: CustomSdlMixerPlaybackSpeedEffectHandler<Sint16>::registerOnChannel(channel, speed, chunk, loop); break;
	case AUDIO_S32: CustomSdlMixerPlaybackSpeedEffectHandler<Sint32>::registerOnChannel(channel, speed, chunk, loop); break;
	case AUDIO_F32: CustomSdlMixerPlaybackSpeedEffectHandler<float >::registerOnChannel(channel, speed, chunk, loop); break;
	}
}
//
////=====================================================================================
//struct StreamTrackerData final
//{
//	int32_t LoopsRemaining;
//};
//
////=====================================================================================
//struct Stream final
//{
//	PaStream * StreamPtr;
//	AudioStreamConfiguration Config;
//	AudioFile SourceFileData;
//	StreamRealtimeParameters Params;
//	StreamTrackerData TrackerData;
//};
//
////=====================================================================================
//static int RealtimeCallback_StandardF32(
//	  const void * a_Input
//	, void * a_Output
//	, unsigned long a_FrameCount
//	, const PaStreamCallbackTimeInfo * a_TimeInfo
//	, PaStreamCallbackFlags StatusFlags
//	, void * a_UserData
//)
//{
//	float * out = ( float* ) a_Output;
//	Stream * streamData = ( Stream* ) a_UserData;
//	sf_count_t numRead;
//	const sf_count_t numChannels = streamData->SourceFileData->Info.channels;
//	const sf_count_t blockSize = a_FrameCount * numChannels;
//	const sf_count_t blockSizeBytes = a_FrameCount * numChannels * sizeof( float );
//
//	const float &	PARAM_vol = streamData->Params.Volume;
//	const float &	PARAM_pch = streamData->Params.Pitch;
//	const float &	PARAM_pan = streamData->Params.Pan;
//	const bool &	PARAM_rev = streamData->Params.Reverse;
//
//	// Clear the output buffer and read from loaded audiofile buffer
//	BSet( out, 0, static_cast< uint32_t >( blockSizeBytes ) );
//	numRead = sf_read_float( streamData->SourceFileData->File, out, blockSize );
//
//	// Iterate over block and apply runtime parameters to the frames
//	const float * end = out + numRead;
//	while ( out != end )
//	{
//		switch ( numChannels )
//		{
//		case 1:
//			( *out++ ) *= PARAM_vol;
//			break;
//		case 2:
//			( *out++ ) *= PARAM_vol * ( PARAM_pan <= 0.0F ? 1.0F : ( 1.0F - PARAM_pan ) );
//			( *out++ ) *= PARAM_vol * ( PARAM_pan >= 0.0F ? 1.0F : ( 0.0F - PARAM_pan ) );
//			break;
//		default:
//			out++;
//			break;
//		}
//	}
//
//	// We've reached the end of the stream if we couldn't read a full block
//	if ( numRead < blockSize )
//	{
//		int32_t & lr = streamData->TrackerData.LoopsRemaining;
//		if ( streamData->Config.LoopAudio && lr != 0 )
//		{
//			if ( lr != AudioStreamConfiguration::INF )
//			{
//				--lr;
//			}
//
//			sf_seek( streamData->SourceFileData->File, 0, SF_SEEK_SET );
//			sf_read_float( streamData->SourceFileData->File, out + numRead, blockSize - numRead );
//			
//			return paContinue;
//		}
//
//		return paComplete;
//	}
//
//	return paContinue;
//}
void myeffect(int chan, void *stream, int len, void *udata)
{

}

//=====================================================================================
void AudioManager::Init()
{
	PROFILE;

	if ( !ASSERT( !m_InitialisedApi, "AudioManager is already initialised, call Finalise() first" ) )
	{
		return;
	}

	int32_t opened = Mix_QuerySpec( &AudioSpec::frequency, &AudioSpec::format, &AudioSpec::channelCount );

	Mix_Music *gMusic = Mix_LoadMUS("res/audio/chimes.wav");
	Mix_Chunk * gScratch = Mix_LoadWAV("res/audio/chimes.wav");
	//Mix_RegisterEffect(0, myeffect, nullptr, nullptr);

	float speed = 0.5f;
	const int channel = Mix_PlayChannel(-1, gScratch, -1);
	setupForNextPlayback(speed, gScratch, channel, true);

	Mix_FreeChunk( gScratch );
	Mix_FreeMusic( gMusic );
	m_InitialisedApi = true;

	m_MusicFade = 0.0F;
	m_MusicFadeTime = 0.0F;
}

//=====================================================================================
void AudioManager::Finalise()
{
	PROFILE;
	CHECK;

	//PaError result;
	//ASSERT( ( result = Pa_Terminate() ) == paNoError, "PortAudio failed to terminate! %d [%s]", result, Pa_GetErrorText( result ) );
	m_InitialisedApi = false;

	// Release all AudioStreams
	Pointer< Stream > * audStrIt = m_AudioStreams.First();
	const Pointer< Stream > * audStrEnd = m_AudioStreams.End();
	while ( audStrIt != audStrEnd )
	{
		//Pa_CloseStream( ( *audStrIt++ )->StreamPtr );
	}
	m_AudioStreams.Clear();

	// Release all AudioFile
	auto audFileIt = m_AudioFileMap.GetEntries().First();
	auto audFileEnd = m_AudioFileMap.GetEntries().End();
	while ( audFileIt != audFileEnd )
	{
		//sf_close( audFileIt->Value->File );
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
		//PaError error;
		//if ( ( error = Pa_IsStreamActive( ( *it )->StreamPtr ) ) == 0 )
		{
			//Pa_CloseStream( ( *it )->StreamPtr );
			//m_AudioStreams.Remove( ( const Pointer< Stream > * )it );
		}

		//ASSERT( error >= paNoError, "AudioManager: Error occured while updating audiostream" );

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

	//SFile data;
	//data.File = sf_open( a_Path, SFM_READ, &data.Info );
	//if ( sf_error( data.File ) != SF_ERR_NO_ERROR )
	//{
	//	PRINT( "Failed to load audio file: \"%s\" [%s]", a_Path, sf_strerror( data.File ) );
	//	return AudioFile();
	//}
	//
	//Pointer< SFile > ptr( data );
	//m_AudioFileMap.Put( a_HashName, ptr );

	return AudioFile();// ptr );
}

//=====================================================================================
void AudioManager::UnloadAudioFile( uint32_t a_HashName )
{
	PROFILE;
	CHECK;

	if ( m_AudioFileMap.Contains( a_HashName ) )
	{
		auto & ptr = *m_AudioFileMap[ a_HashName ];
		
		//sf_close( ptr.Ptr()->File );

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

	//Pointer< Stream > ptr = Pointer< Stream >( Stream() );
	//ptr->Config = a_Config;
	//ptr->SourceFileData = a_SourceAudioData;

	//PaError error;

	/* Open PaStream with values read from the file */
	//error = Pa_OpenDefaultStream( &ptr->StreamPtr
	//	, 0										/* no input */
	//	, ptr->SourceFileData->Info.channels    /* stereo out */
	//	, paFloat32								/* floating point */
	//	, ptr->SourceFileData->Info.samplerate
	//	, a_Config.FrameBufferSize
	//	, RealtimeCallback_StandardF32
	//	, ptr.Ptr() );							/* our stream wrapper struct provide any and all info we need in realtime*/

	//if ( !ASSERT( error == paNoError, "'OpenAudioStream' - Failed to create AudioStream: [%s]", Pa_GetErrorText( error ) ) )
	//{
	//	return AudioStream();
	//}
	//
	//m_AudioStreams.Append( ptr );
	return AudioStream();// ptr );
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

		//if ( ptr && *ptr )
		//{
		//	PaError err = Pa_CloseStream( ptr->Ptr()->StreamPtr );
		//
		//	if ( ASSERT( err == paNoError, "'CloseAudioStream' - Failed to close AudioStream: [%s]", Pa_GetErrorText( err ) ) )
		//	{
		//		m_AudioStreams.Remove( ptr );
		//		return;
		//	}
		//}
	}
}

//=====================================================================================
void AudioManager::PlayAudioStream( const AudioStream & a_AudioStream )
{
	PROFILE;
	CHECK;

	if ( ASSERT( a_AudioStream, "'StopAudioStream' - AudioStream is NULL!" ) )
	{
		//a_AudioStream.Ptr()->TrackerData.LoopsRemaining = a_AudioStream.Ptr()->Config.LoopCount;
		//auto res = Pa_StartStream( a_AudioStream.Ptr()->StreamPtr );
		//ASSERT( res == paNoError, "'StopAudioStream' - ERROR: [%s]", Pa_GetErrorText( res ) );
	}
}

//=====================================================================================
void AudioManager::StopAudioStream( const AudioStream & a_AudioStream, bool a_ForceOperation )
{
	PROFILE;
	CHECK;

	if ( ASSERT( a_AudioStream, "'StopAudioStream' - AudioStream is NULL!" ) )
	{
		//PaError res;
		//
		//if ( a_ForceOperation )
		//{
		//	res = Pa_AbortStream( a_AudioStream.Ptr()->StreamPtr );
		//}
		//
		//else
		//{
		//	res = Pa_StopStream( a_AudioStream.Ptr()->StreamPtr );
		//}
		//
		//ASSERT( res == paNoError, "'StopAudioStream' - ERROR: [%s]", Pa_GetErrorText( res ) );
	}
}

//=====================================================================================
bool AudioManager::IsAudioStreamPlaying( const AudioStream & a_AudioStream ) const
{
	PROFILE;
	CHECK false;

	if ( ASSERT( a_AudioStream, "'IsAudioStreamPlaying' - AudioStream is NULL!" ) )
	{
		//auto res = Pa_IsStreamActive( a_AudioStream.Ptr()->StreamPtr );
		//
		//if ( res >= 0 )
		//{
		//	return res == ( PaError )1;
		//}
		//
		//ASSERT( false, "'IsAudioStreamPlaying' - ERROR: [%s]", Pa_GetErrorText( res ) );
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
		StreamRealtimeParameters params;//StreamRealtimeParameters & params = a_AudioStream->Params;

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