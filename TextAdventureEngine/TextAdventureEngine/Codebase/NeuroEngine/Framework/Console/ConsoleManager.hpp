#pragma once
#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <Framework/Utils/TypeTraits.hpp>
#include <Framework/Template/Manager.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/UI/Pages/PageConsole.hpp>
#include "ICommand.hpp"

//=====================================================================================
class ConsoleManager final 
	: public Manager< ConsoleManager >
	, public InputManager::IKeyEventListener
{
public:

	void Init() override;
	void InitPost() override;
	void Finalise() override;
	void OnKeyPressed( InputManager::Key a_Key ) override;

	template< typename COMMAND >
	bool RegisterCommand()
	{
		STATIC_ASSERT( IS_BASE_OF( ICommand, COMMAND ), "Cannot register non-command. 'COMMAND' must inherit from ICommand" );

		ICommand * command = new COMMAND;
		m_RegisteredCommands.Put( CRC32( command->Get() ), command );
		return true;
	}

	void ProcessString( const char * a_InputStr );

	void Error( const CString & a_Text );
	void Warn( const CString & a_Text );
	void Praise( const CString & a_Text );
	void Whisper( const CString & a_Text );
	void Log( const CString & a_Text );
	
	const CString & GetLog() const { return m_LogConcat; }

private:

	void OutputToConsole( const char * a_String );

	bool m_ShowConsole = false;
	PageConsole * m_ConsolePage = nullptr;

	CString m_LogConcat;
	Queue< CString > m_Log;
	StaticDictionary< uint32_t, ICommand *, 128 > m_RegisteredCommands;
};

#endif//CONSOLEMANAGER_H