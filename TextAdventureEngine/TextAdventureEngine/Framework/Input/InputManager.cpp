#include "InputManager.hpp"
#include <Framework/Utils/MemoryOps.hpp>
#include <SIGIL-0.9.0/sl.h>

//=====================================================================================
void InputManager::Init()
{
	BSet( m_KeyDownFlags, 0, sizeof( m_KeyDownFlags ) );
	BSet( m_KeyDownPrevFlags, 0, sizeof( m_KeyDownPrevFlags ) );

	BSet( m_MouseDownFlags, 0, sizeof( m_MouseDownFlags ) );
	BSet( m_MouseDownPrevFlags, 0, sizeof( m_MouseDownPrevFlags ) );

	m_IgnoreInput = false;
	m_InTextualMode = false;
}

//=====================================================================================
void InputManager::Finalise()
{
}

//=====================================================================================
void InputManager::Tick( float a_DeltaTime )
{
	for ( uint32_t i = 0; i < 512; ++i )
	{
		bool & f = m_KeyDownFlags[ i ];
		bool & p = m_KeyDownPrevFlags[ i ];
		f = slGetKey( i );

		if ( f != p )
		{
			if ( f )
			{
				m_KeyStates[ i ] = ButtonState::PRESSED;

				if ( !m_InTextualMode )
				{
					const StaticArray< IKeyEventListener *, MaxListenersPerEvent > & array = m_InputKeyEventListeners[ ( uint8_t)InputKeyEvent::ON_KEY_PRESSED ];
					for ( uint32_t k = 0; k < array.Count(); ++k )
					{
						array[ k ]->OnKeyPressed( (Key)i );
					}
				}
				else 
				{
					const char c = 'a';

					const StaticArray< ICharEventListener *, MaxListenersPerEvent > & array = m_InputCharEventListeners[ ( uint8_t)InputCharEvent::ON_TEXTUAL_CHAR_TYPED ];
					for ( uint32_t k = 0; k < array.Count(); ++k )
					{
						array[ k ]->OnCharTyped( c );
					}
				}
			}

			else
			{
				m_KeyStates[ i ] = ButtonState::RELEASED;

				if ( !m_InTextualMode )
				{
					const StaticArray< IKeyEventListener *, MaxListenersPerEvent > & array = m_InputKeyEventListeners[ ( uint8_t)InputKeyEvent::ON_KEY_RELEASED ];
					for ( uint32_t k = 0; k < array.Count(); ++k )
					{
						array[ k ]->OnKeyReleased( (Key)i );
					}
				}
			}
		}

		else
		{
			if ( f )
			{
				m_KeyStates[ i ] = ButtonState::DOWN;
			}

			else
			{
				m_KeyStates[ i ] = ButtonState::UP;
			}
		}

		p = f;
	}

	for ( uint32_t i = 0; i < MouseButtonCount; ++i )
	{
		bool & f = m_MouseDownFlags[ i ];
		bool & p = m_MouseDownPrevFlags[ i ];
		f = slGetMouseButton( i );

		if ( f != p )
		{
			if ( f )
			{
				m_MouseStates[ i ] = ButtonState::PRESSED;

				const StaticArray< IMouseEventListener *, MaxListenersPerEvent > & array = m_InputMouseEventListeners[ ( uint8_t)InputMouseEvent::ON_MOUSE_PRESSED ];
				for ( uint32_t k = 0; k < array.Count(); ++k )
				{
					array[ k ]->OnMousePressed( (MouseButton)i );
				}
			}

			else
			{
				m_MouseStates[ i ] = ButtonState::RELEASED;
				
				const StaticArray< IMouseEventListener *, MaxListenersPerEvent > & array = m_InputMouseEventListeners[ ( uint8_t)InputMouseEvent::ON_MOUSE_RELEASED ];
				for ( uint32_t k = 0; k < array.Count(); ++k )
				{
					array[ k ]->OnMouseReleased( (MouseButton)i );
				}
			}
		}

		else
		{
			if ( f )
			{
				m_MouseStates[ i ] = ButtonState::DOWN;
			}

			else
			{
				m_MouseStates[ i ] = ButtonState::UP;
			}
		}

		p = f;
	}
}

//=====================================================================================
InputManager::ButtonState InputManager::GetKeyState( KeyCode a_Key )
{
	return m_IgnoreInput ? ButtonState::UP : ( m_InTextualMode ? ButtonState::UP : m_KeyStates[ a_Key ] );
}

//=====================================================================================
InputManager::ButtonState InputManager::GetMouseButtonState( MouseButton a_MouseButton )
{
	return m_IgnoreInput ? ButtonState::UP : m_MouseStates[ (uint8_t)a_MouseButton ];
}

//=====================================================================================
Vector2 InputManager::GetMousePosition() const
{
	return Vector2( ( float )slGetMouseX(), ( float )slGetMouseY() );
}

//=====================================================================================
void InputManager::SetTextualMode( bool a_Flag )
{
	m_InTextualMode = a_Flag;
}

//=====================================================================================
bool InputManager::InTextualMode() const
{
	return m_InTextualMode;
}

//=====================================================================================
void InputManager::AttachListener( InputKeyEvent a_Event, IKeyEventListener * a_EventListener )
{
	if ( m_InputKeyEventListeners[ ( uint8_t )a_Event ].Find< IKeyEventListener * >( []( IKeyEventListener * a_In, IKeyEventListener * const & a_Elem ){ return a_Elem == a_In; }, a_EventListener ) )
	{
		return;
	}

	m_InputKeyEventListeners[ ( uint8_t )a_Event ].Append( a_EventListener );
}

//=====================================================================================
void InputManager::AttachListener( InputMouseEvent a_Event, IMouseEventListener * a_EventListener )
{
	if ( m_InputMouseEventListeners[ ( uint8_t )a_Event ].Find< IMouseEventListener * >( []( IMouseEventListener * a_In, IMouseEventListener * const & a_Elem ){ return a_Elem == a_In; }, a_EventListener ) )
	{
		return;
	}

	m_InputMouseEventListeners[ ( uint8_t )a_Event ].Append( a_EventListener );
}

//=====================================================================================
void InputManager::AttachListener( InputCharEvent a_Event, ICharEventListener * a_EventListener )
{
	if ( m_InputCharEventListeners[ ( uint8_t )a_Event ].Find< ICharEventListener * >( []( ICharEventListener * a_In, ICharEventListener * const & a_Elem ){ return a_Elem == a_In; }, a_EventListener ) )
	{
		return;
	}

	m_InputCharEventListeners[ ( uint8_t )a_Event ].Append( a_EventListener );
}

//=====================================================================================
void InputManager::DetatchListener( InputKeyEvent a_Event, IKeyEventListener * a_EventListener )
{
	m_InputKeyEventListeners[ ( uint8_t )a_Event ].Remove( a_EventListener );
}

//=====================================================================================
void InputManager::DetatchListener( InputMouseEvent a_Event, IMouseEventListener * a_EventListener )
{
	m_InputMouseEventListeners[ ( uint8_t )a_Event ].Remove( a_EventListener );
}

//=====================================================================================
void InputManager::DetatchListener( InputCharEvent a_Event, ICharEventListener * a_EventListener )
{
	m_InputCharEventListeners[ ( uint8_t )a_Event ].Remove( a_EventListener );
}