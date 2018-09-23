#include "ConsoleManager.hpp"
#include <Framework/UI/PageManager.hpp>

//=====================================================================================
void ConsoleManager::Init()
{
	m_ShowConsole = false;
}

//=====================================================================================
void ConsoleManager::InitPost()
{
	InputManager::Instance().AttachListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
	m_ConsolePage = new PageConsole();

	Warn("abc");
	Error("abc");
	Log("abc");
	Whisper("abc");
	Praise( "Successfully loaded" );
}

//=====================================================================================
void ConsoleManager::Finalise()
{
	InputManager::Instance().DetatchListener( InputManager::InputKeyEvent::ON_KEY_PRESSED, this );
	Free( m_ConsolePage );
}

//=====================================================================================
void ConsoleManager::OnKeyPressed( InputManager::Key a_Key )
{
	if ( a_Key == InputManager::Key::KEYCODE_GRAVE )
	{
		m_ShowConsole = !m_ShowConsole;
		if ( m_ShowConsole )
		{
			PageManager::Instance().Push( m_ConsolePage );
		}
		else if ( PageManager::Instance().Peek() == m_ConsolePage )
		{
			PageManager::Instance().Pop();
		}
	}
}

//=====================================================================================
void ConsoleManager::Error( const CString & a_Text )
{
	CString text = CString( FONTCOLOUR_TAG( FF3344FF ) ) + "Error: " + a_Text + FONTCOLOUR_POP;
	OutputToConsole( text.Get() );
}

//=====================================================================================
void ConsoleManager::Warn( const CString & a_Text )
{
	CString text = CString( FONTCOLOUR_TAG( FFEE22FF ) ) + "Warning: " + a_Text + FONTCOLOUR_POP;
	OutputToConsole( text.Get() );
}

//=====================================================================================
void ConsoleManager::Praise( const CString & a_Text )
{
	CString text = CString( FONTCOLOUR_TAG( 4488FFFF ) ) + a_Text + FONTCOLOUR_POP;
	OutputToConsole( text.Get() );
}

//=====================================================================================
void ConsoleManager::Whisper( const CString & a_Text )
{
	CString text = CString( FONTCOLOUR_TAG( 888888FF ) ) + a_Text + FONTCOLOUR_POP;
	OutputToConsole( text.Get() );
}

//=====================================================================================
void ConsoleManager::Log( const CString & a_Text )
{
	CString text = CString( FONTCOLOUR_TAG( EEEEEEFF ) ) + a_Text + FONTCOLOUR_POP;
	OutputToConsole( text.Get() );
}

//=====================================================================================
void ConsoleManager::OutputToConsole( const char * a_String )
{
	// Max number of messages on the log (Not necessarily the number of lines).
	static const uint32_t MAX_LOG = 12;

	// Push new message to the front of the queue
	m_Log.Push( a_String );

	// Make sure queue isn't too big.
	while ( m_Log.Count() > MAX_LOG )
	{
		m_Log.Pop();
	}

	// Calculate concatenated string from queue
	m_LogConcat = "";
	Queue< CString > logTmp = m_Log;
	while ( logTmp.Count() > 0 )
	{
		m_LogConcat += *logTmp.Peek() + "\n";// Newline after so there's a gap between log and input
		logTmp.Pop();
	}
}
