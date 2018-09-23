#pragma once
#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/UI/Pages/PageConsole.hpp>

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
};

#endif//CONSOLEMANAGER_H