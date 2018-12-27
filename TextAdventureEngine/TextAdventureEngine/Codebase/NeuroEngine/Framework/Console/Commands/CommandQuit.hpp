#pragma once
#ifndef COMMANDQUIT_H
#define COMMANDQUIT_H

#include <Framework/Console/ConsoleManager.hpp>
#include <Framework/Console/ICommand.hpp>
#include <Framework/Engine.hpp>

//=====================================================================================
class CommandQuit final : public ICommand
{
	const char * Get() const override
	{
		return "quit";
	}
	
	Result Execute( const Array< CString > a_Args ) override
	{
		Engine::Instance().Quit();
		return Result::CR_SUCCESS;
	}

	const char * GetUsage() const override
	{
		return "quit";
	}
};

#endif//COMMANDQUIT_H