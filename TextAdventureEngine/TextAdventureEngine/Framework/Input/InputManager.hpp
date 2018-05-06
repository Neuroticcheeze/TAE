#pragma once
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Utils/Function.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Containers/StaticArray.hpp>

//=====================================================================================
class InputManager final : public TickableManager< InputManager >
{
public:

	ENUMCLASS( ButtonState, uint8_t )
		UP,
		DOWN,
		PRESSED,
		RELEASED,
	END_ENUMCLASS( ButtonState, uint8_t )

	ENUMCLASS( MouseButton, uint8_t )
		LEFT,
		RIGHT,
		MIDDLE,
	END_ENUMCLASS( MouseButton, uint8_t )

	typedef uint16_t KeyCode;
	ENUM( Key, KeyCode )
		ESCAPE = 256,
		ENTER = 257,
		TAB = 258,
		BACKSPACE = 259,
		INSERT = 260,
		DELETE = 261,
		RIGHT = 262,
		LEFT = 263,
		DOWN = 264,
		UP = 265,
		PAGE_UP = 266,
		PAGE_DOWN = 267,
		HOME = 268,
		END = 269,
		CAPS_LOCK = 280,
		SCROLL_LOCK = 281,
		NUM_LOCK = 282,
		PRINT_SCREEN = 283,
		PAUSE = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		KEYPAD_0 = 320,
		KEYPAD_1 = 321,
		KEYPAD_2 = 322,
		KEYPAD_3 = 323,
		KEYPAD_4 = 324,
		KEYPAD_5 = 325,
		KEYPAD_6 = 326,
		KEYPAD_7 = 327,
		KEYPAD_8 = 328,
		KEYPAD_9 = 329,
		KEYPAD_DECIMAL = 330,
		KEYPAD_DIVIDE = 331,
		KEYPAD_MULTIPLY = 332,
		KEYPAD_SUBTRACT = 333,
		KEYPAD_ADD = 334,
		KEYPAD_ENTER = 335,
		KEYPAD_EQUAL = 336,
		LEFT_SHIFT = 340,
		LEFT_CONTROL = 341,
		LEFT_ALT = 342,
		LEFT_SUPER = 343,
		RIGHT_SHIFT = 344,
		RIGHT_CONTROL = 345,
		RIGHT_ALT = 346,
		RIGHT_SUPER = 347
	END_ENUM;

	ENUMCLASS( InputKeyEvent, uint8_t )
		ON_KEY_PRESSED,
		ON_KEY_RELEASED,
	END_ENUMCLASS( InputKeyEvent, uint8_t )

	ENUMCLASS( InputMouseEvent, uint8_t )
		ON_MOUSE_PRESSED,
		ON_MOUSE_RELEASED,
	END_ENUMCLASS( InputMouseEvent, uint8_t )

	ENUMCLASS( InputCharEvent, uint8_t )
		ON_TEXTUAL_CHAR_TYPED,
	END_ENUMCLASS( InputCharEvent, uint8_t )

	class IKeyEventListener abstract
	{
		friend class InputManager;

	protected:

		virtual void OnKeyPressed( Key a_Key ) {}
		virtual void OnKeyReleased( Key a_Key ) {}
	};

	class IMouseEventListener abstract
	{
		friend class InputManager;

	protected:

		virtual void OnMousePressed( MouseButton a_MouseButton ) {}
		virtual void OnMouseReleased( MouseButton a_MouseButton ) {}
	};

	class ICharEventListener abstract
	{
		friend class InputManager;

	protected:

		virtual void OnCharTyped( char a_Char ) abstract;
	};

	void Init();
	void Finalise();
	void Tick( float a_DeltaTime = 0.0F );

	ButtonState GetKeyState( KeyCode a_Key );
	ButtonState GetMouseButtonState( MouseButton a_MouseButton = MouseButton::LEFT );
	
	Vector2 GetMousePosition() const;
	
	void SetTextualMode( bool a_Flag = true );
	bool InTextualMode() const;

	void AttachListener( InputKeyEvent a_Event, IKeyEventListener * a_EventListener );
	void AttachListener( InputMouseEvent a_Event, IMouseEventListener * a_EventListener );
	void AttachListener( InputCharEvent a_Event, ICharEventListener * a_EventListener );

	void DetatchListener( InputKeyEvent a_Event, IKeyEventListener * a_EventListener );
	void DetatchListener( InputMouseEvent a_Event, IMouseEventListener * a_EventListener );
	void DetatchListener( InputCharEvent a_Event, ICharEventListener * a_EventListener );

	static const uint32_t MaxListenersPerEvent = 256;

	inline void IgnoreInput( bool a_Flag )
	{
		m_IgnoreInput = a_Flag;
	}

	inline bool IsIgnoringInput() const
	{
		return m_IgnoreInput;
	}

private:
	bool m_IgnoreInput;
	bool m_InTextualMode;
	
	bool m_KeyDownFlags[ 512 ];
	bool m_KeyDownPrevFlags[ 512 ];
	ButtonState m_KeyStates[ 512 ];

	bool m_MouseDownFlags[ MouseButtonCount ];
	bool m_MouseDownPrevFlags[ MouseButtonCount ];
	ButtonState m_MouseStates[ MouseButtonCount ];

	StaticArray< IKeyEventListener *, MaxListenersPerEvent >	m_InputKeyEventListeners	[ InputKeyEventCount ];
	StaticArray< IMouseEventListener *, MaxListenersPerEvent >	m_InputMouseEventListeners	[ InputMouseEventCount ];
	StaticArray< ICharEventListener *, MaxListenersPerEvent >	m_InputCharEventListeners	[ InputCharEventCount ];
};

#endif//INPUTMANAGER_H