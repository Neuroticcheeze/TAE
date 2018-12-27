#pragma once
#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Utils/MemoryExt.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
struct SoundParameters;

//=====================================================================================
class AudioManager final : public TickableManager< AudioManager >
{
public:

	typedef WeakPointer< void* > AudioBuffer;
	typedef WeakPointer< Tuple2< void*, SoundParameters > > SoundSource;
	
	void Init() override;
	void InitPost() override;
	void Finalise() override;
	void Tick( float a_DeltaTime = 0.0F ) override;

	AudioBuffer LoadAudioBuffer( uint32_t a_HashName, const char * a_Path );
	bool UnloadAudioBuffer( uint32_t a_HashName );
	bool UnloadAudioBuffer( const AudioBuffer & a_AudioBuffer );
	AudioBuffer GetAudioBuffer( uint32_t a_HashName ) const;
	float GetAudioBufferDuration( const AudioBuffer & a_AudioBuffer ) const;

	SoundSource CreateSoundSource( const SoundParameters & a_InitialParameters );
	void DestroySoundSource( const SoundSource & a_SoundSource );
	SoundParameters GetSoundParameters( const SoundSource & a_SoundSource ) const;
	void SetSoundParameters( const SoundSource & a_SoundSource, const SoundParameters & a_Params );

	void SfxPlaySound( const SoundParameters & a_Params );

	void Play( const SoundSource & a_SoundSource );
	void Pause( const SoundSource & a_SoundSource );
	void Stop( const SoundSource & a_SoundSource );
	bool IsPlaying( const SoundSource & a_SoundSource ) const;
	bool IsPaused( const SoundSource & a_SoundSource ) const;
	bool IsStopped( const SoundSource & a_SoundSource ) const;

private:

	typedef Pointer< void* > PAudioBuffer;
	typedef Pointer< SoundSource::PtrType > PSoundSource;

	Dictionary< uint32_t, PAudioBuffer > m_AudioBuffers;
	Array< PSoundSource > m_SoundSources;
	Array< SoundSource > m_ProcessedSoundSourceRefs;
};

//=====================================================================================
// The parameters of a sound.
struct SoundParameters
{
	float					  Volume		= 1.0F;
	float					  Pitch			= 1.0F;
	bool					  Loop			= false;
	Vector2					  Position		= Vector2::ZERO;
	float					  MinDistance   = 1.0F;		// Distance from sound source beyond which the volume will fall off.
	float					  Attenuation   = 1.0F;		// How quickly the volume falls off, 0 = constant volume.
	bool					  Relative		= true;		// Whether or not the Position is an offset from the listener instead of a world location.
	AudioManager::AudioBuffer Buffer		= nullptr;	// The buffer containing the sound data to play.
	float					  BufferOffset	= 0.0F;		// How many seconds into the audio buffer to play from.
	int32_t					  Priority		= 0;		// <UNUSED> The lower the number, the least likely that the SoundSource will be removed to make room for others.
};

//=====================================================================================
#define _GetAudioBuffer( HASHNAME ) AudioManager::Instance().GetAudioBuffer( CRC32( HASHNAME ) )

#endif//AUDIOMANAGER_H