#include "ConsoleManager.hpp"
#include <Framework/UI/PageManager.hpp>

// Commands
#include "Commands/CommandQuit.hpp"
#include "Commands/CommandDebugInfo.hpp"
#define REG_COMMANDS RegisterCommand< CommandQuit >();\
					 RegisterCommand< CommandDebugInfo >();\

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

	REG_COMMANDS;
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
	CString text = CString( FONTCOLOUR_TAG( FF3344FF ) ) + a_Text + FONTCOLOUR_POP;
	OutputToConsole( text.Get() );
}

//=====================================================================================
void ConsoleManager::Warn( const CString & a_Text )
{
	CString text = CString( FONTCOLOUR_TAG( FFEE22FF ) ) + a_Text + FONTCOLOUR_POP;
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
	static const uint32_t MAX_LOG = 28;

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

//=====================================================================================
void ConsoleManager::ProcessString( const char * a_InputStr )
{
	PROFILE;

	Array< CString > args;

	CString input = a_InputStr;
	auto pattern = input.Regex( R"([^\s]+|\".*\"|'.*')" );
	for ( uint32_t p = 0; p < pattern.Count(); ++p )
	{
		auto rm = pattern[ (uint32_t)p ];
		CString piece = input.SubString( rm.Start, rm.Length );
		if ( piece.Length() >= 2 &&
			 ( piece[ 0 ] == '\'' && piece[ piece.Length() - 1 ] == '\'' ) ||
			 ( piece[ 0 ] == '\"' && piece[ piece.Length() - 1 ] == '\"' ) )
		{
			piece = piece.SubString( 1, piece.Length() - 2 );
		}

		args.Append( piece );
	}

	if ( args.Count() == 0 )
	{
		return;
	}

	ICommand ** com = m_RegisteredCommands[ CRC32( args.First()->Get() ) ];

	if ( !com )
	{
		Error( CString::Format( "unknown command: \"%s\"", args.First()->Get() ) );
		return;
	}

	decltype( args ) args2;
	for ( uint32_t i = 1; i < args.Count(); ++i )
	{
		args2.Append( args[ i ] );
	}
	ICommand::Result execRes = ( *com )->Execute( args2 );

	if ( execRes == ICommand::Result::CR_FAILURE_USAGE )
	{
		Error( CString::Format( "invalid command usage:\n%s", ( *com )->GetUsage() ) );
	}

	else if ( execRes == ICommand::Result::CR_FAILURE_EXECUTION )
	{
		Error( "command execution failed" );
	}
}
