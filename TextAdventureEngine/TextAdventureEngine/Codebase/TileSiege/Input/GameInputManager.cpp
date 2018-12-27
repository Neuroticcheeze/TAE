#include "GameInputManager.hpp"
#include <NeuroEngine/Framework/File/FileManager.hpp>
#include <NeuroEngine/Framework/Data/Xml/XmlManager.hpp>
#include <NeuroEngine/Framework/Utils/Hash.hpp>

//=====================================================================================
void GameInputManager::Init( const char * a_InputConfigPath )
{
	XmlObject inputCfgXml = XmlManager::Instance().ParseXml( FileManager::Instance().ReadAll( a_InputConfigPath ).Get() );

	Array< XmlObject > inputs;
	inputCfgXml.FindChildren( { { XmlObject::Filter::NAME, "input" }, { XmlObject::Filter::HAS_CHILDREN } }, inputs );

	auto it = inputs.First();
	while ( it != inputs.End() )
	{
		CString name = it->GetAttribute( "name" ).String;

		InputEvent ie;
		ie.m_Name = CRC32( name.Get() );
		
		Array< XmlObject > sources;
		it->FindChildren( { { XmlObject::Filter::NAME, "source" }, { XmlObject::Filter::HAS_CHILDREN } }, sources );
		
		auto it2 = sources.First();
		while ( it2 != sources.End() )
		{
			CString src = ( *it2 )[ 0U ].GetValue();

			auto match = src.Regex( R"((\w+)\$(\d+)\s+([+-]?[0-9]*.[0-9]*))" );
		
#define ILLEGAL_FORMAT_MSG "Illegally formatted Input-Source inside '%s'", a_InputConfigPath

			if ( ASSERT_WARN( match.Count() == 1, ILLEGAL_FORMAT_MSG ) )
			{
				auto captures = match.First()->GroupCaptures;
				if ( ASSERT_WARN( captures.Count() == ( 1+3 ), ILLEGAL_FORMAT_MSG ) )
				{
					bool failedParsing = false;

					CString device = captures[ 1U ];
					int32_t button = -1;
					failedParsing |= !CString::Parse( captures[ 2U ].Get(), button );
					float value = 0.0F;
					failedParsing |= !CString::Parse( captures[ 3U ].Get(), value );

					if ( ASSERT_WARN( !failedParsing, ILLEGAL_FORMAT_MSG ) )
					{
						if ( device == "kb" ) // Keyboard
						{
							InputManager::KeyCode keyCode = ( InputManager::KeyCode )button;
							PRINT( "Registering keyboard key %d as source for input event '%s' with driving scalar value of %f", button, name.Get(), value );

							InputSource is;
							is.m_Device = Device::ISD_KEYBOARD;
							is.m_SubIdx = ( uint32_t )keyCode;
							is.m_Value = value;
							ie.m_Sources.Insert( is );
						}
						else if ( device == "mb" ) // Mouse
						{
							InputManager::MouseButton mouseButton = ( InputManager::MouseButton )button;
							PRINT( "Registering mouse %d as source for input event '%s' with driving scalar value of %f", button, name.Get(), value );

							InputSource is;
							is.m_Device = Device::ISD_MOUSE;
							is.m_SubIdx = ( uint32_t )mouseButton;
							is.m_Value = value;
							ie.m_Sources.Insert( is );

							// left
							// middle
							// right
							// scroll_delta
							// x_delta
							// y_delta
						}
						else
						{
							ASSERT_WARN( false, "Unsupported input device '%s' as source for input event '%s'", device.Get(), name.Get() );
						}
					}
				}
			}

#undef ILLEGAL_FORMAT_MSG

			++it2;
		}

		m_InputEvents.Insert( ie );

		++it;
	}

	// Calculate mappings
	auto ieIt = m_InputEvents.First();
	while ( ieIt != m_InputEvents.End() )
	{
		auto isIt = ieIt->m_Sources.First();
		while ( isIt != ieIt->m_Sources.End() )
		{
			switch ( isIt->m_Device )
			{
			case ISD_KEYBOARD:
				m_KeyCodeToEventMappings[ isIt->m_SubIdx ].Value1.Insert( ieIt );
				break;
			case ISD_MOUSE:
				if ( InRange( isIt->m_SubIdx, 0U, 2U ) ) // Lmb, Mmb, Rmb
				{
					m_MouseButtonToEventMappings[ isIt->m_SubIdx ].Value1.Insert( ieIt );
				}
				else
				{
					//TODO: mouse-delta mappings
				}
				break;
			default:
				break;
			}
			++isIt;
		}

		++ieIt;
	}

	InputManager::Instance().AttachListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
	InputManager::Instance().AttachListener( InputManager::InputKeyEvent::ON_KEY_RELEASED, this );
	InputManager::Instance().AttachListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
	InputManager::Instance().AttachListener( InputManager::InputMouseEvent::ON_MOUSE_RELEASED, this );
}

//=====================================================================================
void GameInputManager::InitPost()
{
}

//=====================================================================================
void GameInputManager::Tick( float a_DeltaTime )
{
	PROFILE;

	for ( uint32_t i = 0; i < InputManager::KeyCount; ++i )
	{
		if ( m_KeyCodeToEventMappings[ i ].Value0 )
		{
			Set< InputEvent* > & mappedIEs = m_KeyCodeToEventMappings[ i ].Value1;
		
			auto it = mappedIEs.First();
			while ( it != mappedIEs.End() )
			{
				auto listenerIt = ( *it )->m_Listeners.First();
				while ( listenerIt != ( *it )->m_Listeners.End() )
				{
					// Calculate the value
					float value = 0.0F;
					auto & sources = ( *it )->m_Sources;
					auto sourceIt = sources.First();
					while ( sourceIt != sources.End() )
					{
						if ( sourceIt->m_SubIdx == i )
						{
							value += sourceIt->m_Value;
						}
						++sourceIt;
					}//
		
					( *listenerIt )->OnInputUpdate( ( *it )->m_Name, value );
		
					++listenerIt;
				}
		
				++it;
			}
		}
	}

	for ( uint32_t i = 0; i < InputManager::MouseButtonCount; ++i )
	{
		if ( m_MouseButtonToEventMappings[ i ].Value0 )
		{
			Set< InputEvent* > & mappedIEs = m_MouseButtonToEventMappings[ i ].Value1;
		
			auto it = mappedIEs.First();
			while ( it != mappedIEs.End() )
			{
				auto listenerIt = ( *it )->m_Listeners.First();
				while ( listenerIt != ( *it )->m_Listeners.End() )
				{
					// Calculate the value
					float value = 0.0F;
					auto & sources = ( *it )->m_Sources;
					auto sourceIt = sources.First();
					while ( sourceIt != sources.End() )
					{
						if ( sourceIt->m_SubIdx == i )
						{
							value += sourceIt->m_Value;
						}
						++sourceIt;
					}//
		
					( *listenerIt )->OnInputUpdate( ( *it )->m_Name, value );
		
					++listenerIt;
				}
		
				++it;
			}
		}
	}
}

//=====================================================================================
void GameInputManager::Finalise()
{
	InputManager::Instance().DetatchListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
	InputManager::Instance().DetatchListener( InputManager::InputKeyEvent::ON_KEY_RELEASED, this );
	InputManager::Instance().DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
	InputManager::Instance().DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_RELEASED, this );
}

//=====================================================================================
void GameInputManager::OnKeyPressed( InputManager::Key a_Key )
{
	m_KeyCodeToEventMappings[ a_Key ].Value0 = true;
	Set< InputEvent* > & mappedIEs = m_KeyCodeToEventMappings[ a_Key ].Value1;

	auto it = mappedIEs.First();
	while ( it != mappedIEs.End() )
	{
		auto listenerIt = ( *it )->m_Listeners.First();
		while ( listenerIt != ( *it )->m_Listeners.End() )
		{
			// Calculate the value
			float value = 0.0F;
			auto & sources = ( *it )->m_Sources;
			auto sourceIt = sources.First();
			while ( sourceIt != sources.End() )
			{
				if ( sourceIt->m_SubIdx == a_Key )
				{
					value += sourceIt->m_Value;
				}
				++sourceIt;
			}//

			( *listenerIt )->OnInputBegin( ( *it )->m_Name, value );

			++listenerIt;
		}

		++it;
	}
}

//=====================================================================================
void GameInputManager::OnKeyReleased( InputManager::Key a_Key )
{
	m_KeyCodeToEventMappings[ a_Key ].Value0 = false;
	Set< InputEvent* > & mappedIEs = m_KeyCodeToEventMappings[ a_Key ].Value1;

	auto it = mappedIEs.First();
	while ( it != mappedIEs.End() )
	{
		auto listenerIt = ( *it )->m_Listeners.First();
		while ( listenerIt != ( *it )->m_Listeners.End() )
		{
			( *listenerIt )->OnInputEnd( ( *it )->m_Name, 0.0F );

			++listenerIt;
		}

		++it;
	}
}

//=====================================================================================
void GameInputManager::OnMousePressed( InputManager::MouseButton a_MouseButton )
{
	m_MouseButtonToEventMappings[ ( uint32_t )a_MouseButton ].Value0 = true;
	Set< InputEvent* > & mappedIEs = m_MouseButtonToEventMappings[ ( uint32_t )a_MouseButton ].Value1;

	auto it = mappedIEs.First();
	while ( it != mappedIEs.End() )
	{
		auto listenerIt = ( *it )->m_Listeners.First();
		while ( listenerIt != ( *it )->m_Listeners.End() )
		{
			// Calculate the value
			float value = 0.0F;
			auto & sources = ( *it )->m_Sources;
			auto sourceIt = sources.First();
			while ( sourceIt != sources.End() )
			{
				if ( sourceIt->m_SubIdx == ( uint32_t )a_MouseButton )
				{
					value += sourceIt->m_Value;
				}
				++sourceIt;
			}//

			( *listenerIt )->OnInputBegin( ( *it )->m_Name, value );

			++listenerIt;
		}

		++it;
	}
}

//=====================================================================================
void GameInputManager::OnMouseReleased( InputManager::MouseButton a_MouseButton )
{
	m_MouseButtonToEventMappings[ ( uint32_t )a_MouseButton ].Value0 = false;
	Set< InputEvent* > & mappedIEs = m_MouseButtonToEventMappings[ ( uint32_t )a_MouseButton ].Value1;

	auto it = mappedIEs.First();
	while ( it != mappedIEs.End() )
	{
		auto listenerIt = ( *it )->m_Listeners.First();
		while ( listenerIt != ( *it )->m_Listeners.End() )
		{
			( *listenerIt )->OnInputEnd( ( *it )->m_Name, 0.0F );

			++listenerIt;
		}

		++it;
	}
}

//=====================================================================================
void GameInputManager::RegisterListener( uint32_t a_InputEvent, IListener * a_Listener )
{
	ASSERT( a_Listener, "Listener is NULL!" );

	auto it = m_InputEvents.First();
	while ( it != m_InputEvents.End() )
	{
		if ( it->m_Name == a_InputEvent )
		{
#ifdef _DEBUG
			ASSERT( !it->m_Listeners.Contains( a_Listener ), "Trying to register listener twice!" );
#endif//DEBUG

			it->m_Listeners.Insert( a_Listener );
			return;
		}
		++it;
	}

	ASSERT( false, "Found no input event with hashed name %u", a_InputEvent );
}

//=====================================================================================
void GameInputManager::DeregisterListener( uint32_t a_InputEvent, IListener * a_Listener )
{
	ASSERT( a_Listener, "Listener is NULL!" );

	auto it = m_InputEvents.First();
	while ( it != m_InputEvents.End() )
	{
		if ( it->m_Name == a_InputEvent )
		{
			ASSERT( it->m_Listeners.Remove( a_Listener ), "Trying to deregister unregistered listener, either was never registered or was deregistered twice." );
			return;
		}
		++it;
	}

	ASSERT( false, "Found no input event with hashed name %u", a_InputEvent );
}
