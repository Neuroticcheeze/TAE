#pragma once
#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Containers/StaticArray.hpp>
#include <Framework/Utils/MemoryExt.hpp>

//=====================================================================================
struct SFile;
struct Stream;
typedef WeakPointer< SFile > AudioFile;
typedef WeakPointer< Stream > AudioStream;

//=====================================================================================
struct AudioStreamConfiguration final
{
	enum { INF = -1 };

	bool CloseOnFinish = false;
	bool LoopAudio = false;
	int32_t LoopCount = INF;
	uint32_t FrameBufferSize = 512;
};

//=====================================================================================
class AudioManager final : public TickableManager< AudioManager >
{
public:

	ENUM( ParameterType, uint8_t )
		VOLUME,
		PITCH,
		PAN,
	END_ENUM;

	void Init();
	void Finalise();
	void Tick( float a_DeltaTime = 0.0F );
	
	AudioFile LoadAudioFile( uint32_t a_HashName, const char * a_Path );
	void UnloadAudioFile( uint32_t a_HashName );
	AudioFile GetAudioFile( uint32_t a_HashName ) const;

	AudioStream OpenAudioStream( AudioStreamConfiguration a_Config, const AudioFile & a_SourceAudioData );
	void CloseAudioStream( const AudioStream & a_AudioStream );
	void PlayAudioStream( const AudioStream & a_AudioStream );
	void StopAudioStream( const AudioStream & a_AudioStream, bool a_ForceOperation = false );
	bool IsAudioStreamPlaying( const AudioStream & a_AudioStream ) const;
	void SetAudioStreamParameter( const AudioStream & a_AudioStream, ParameterType a_ParameterType, float a_Value );

	void SetMusicTrack( uint32_t a_HashName, float a_FadeTime = 2.0F );
	void PlaySound( uint32_t a_AudioFileHashName, float a_Volume = 1.0F, float a_Pitch = 1.0F, float a_Pan = 0.0F );

private:

	Dictionary< uint32_t, Pointer< SFile > > m_AudioFileMap;
	StaticArray< Pointer< Stream >, 64 > m_AudioStreams;
	
	AudioStream m_CurrentMusicTrack;
	AudioStream m_NextMusicTrack;

	float m_MusicFade;
	float m_MusicFadeTime;

	bool m_InitialisedApi = false;
};

//=====================================================================================
#define _GetAudioFile( HASHNAME ) AudioManager::Instance().GetAudioFile( WSID( HASHNAME ) )

#endif//AUDIOMANAGER_H