#pragma once
#ifndef GAMEINPUTMANAGER_H
#define GAMEINPUTMANAGER_H

#include <NeuroEngine/Framework/Template/Manager.hpp>
#include <NeuroEngine/Framework/Containers/Set.hpp>
#include <NeuroEngine/Framework/Input/InputManager.hpp>
#include <NeuroEngine/Framework/Utils/Tuple.hpp>

//=====================================================================================
class GameInputManager final
	: public TickableManager< GameInputManager, const char * >
	, private InputManager::IKeyEventListener
	, private InputManager::IMouseEventListener
{
public:

	class IListener abstract
	{
		friend class GameInputManager;

		virtual void OnInputBegin( uint32_t a_InputEvent, float a_Value ) abstract;
		virtual void OnInputUpdate( uint32_t a_InputEvent, float a_Value ) abstract;
		virtual void OnInputEnd( uint32_t a_InputEvent, float a_Value ) abstract;
	};

public:

	void Init( const char * a_InputConfigPath ) override;
	void InitPost() override;
	void Tick( float a_DeltaTime = 0.0F ) override;
	void Finalise() override;

	void RegisterListener( uint32_t a_InputEvent, IListener * a_Listener );
	void DeregisterListener( uint32_t a_InputEvent, IListener * a_Listener );

private:

	void OnKeyPressed( InputManager::Key a_Key ) override;
	void OnKeyReleased( InputManager::Key a_Key ) override;
	void OnMousePressed( InputManager::MouseButton a_MouseButton ) override;
	void OnMouseReleased( InputManager::MouseButton a_MouseButton ) override;

private:

	ENUM( Device, uint8_t )
		ISD_INVALID = -1,
		ISD_KEYBOARD,
		ISD_MOUSE,
	END_ENUM;

	struct InputSource
	{
		Device m_Device = ISD_INVALID;
		uint32_t m_SubIdx = 0xFFFFFFFF;
		float m_Value = 0.0F;

		bool operator==( const InputSource & a_Other )
		{
			return m_Device == a_Other.m_Device && 
				   m_SubIdx == a_Other.m_SubIdx;
		}

		bool operator!=( const InputSource & a_Other )
		{
			return !( *this == a_Other );
		}
	};

	struct InputEvent
	{
		uint32_t m_Name;
		Set< InputSource > m_Sources;
		Set< IListener* > m_Listeners;

		bool operator==( const InputEvent & a_Other )
		{
			return m_Name == a_Other.m_Name;
		}

		bool operator!=( const InputEvent & a_Other )
		{
			return !( *this == a_Other );
		}
	};

	Set< InputEvent > m_InputEvents;

	// Mappings of game input-events to direct framework input-events.
	Tuple2< bool, Set< InputEvent* > > m_KeyCodeToEventMappings[ InputManager::KeyCount ];
	Tuple2< bool, Set< InputEvent* > > m_MouseButtonToEventMappings[ InputManager::MouseButtonCount ];
	//TODO: mapping for mouse delta & scroll delta
};


#endif//GAMEINPUTMANAGER_H