#pragma once
#ifndef COMMANDDEBUGINFO_H
#define COMMANDDEBUGINFO_H

#include <Framework/Console/ConsoleManager.hpp>
#include <Framework/Console/ICommand.hpp>
#include <Framework/Engine.hpp>
#include <Framework/UI/View.hpp>

//=====================================================================================
class CommandDebugInfo final : public ICommand
{
	const char * Get() const override
	{
		return "dbg";
	}
	
	Result Execute( const Array< CString > a_Args ) override
	{
		if ( a_Args.Count() < 2 )
		{
			return Result::CR_FAILURE_USAGE;
		}

		CString categ = a_Args[ 0U ];
		if ( categ != "views" &&
			 categ != "profiler" &&
			 categ != "geometry" )
		{
			return Result::CR_FAILURE_USAGE;
		}

		categ.ToLower();

		int32_t flag = 0;
		if ( !CString::Parse( a_Args[ 1U ].Get(), flag ) )
		{
			return Result::CR_FAILURE_USAGE;
		}

		// View Debugging
		if ( categ == "views" )
		{
#ifdef _DEBUG
			View::g_DebugViews = flag != 0;
			ConsoleManager::Instance().Log( View::g_DebugViews ? "View Debugging enabled" : "View Debugging disabled" );
#else
			ConsoleManager::Instance().Warn( "View Debugging is not supported in Release" );
#endif//_DEBUG
		}

		return Result::CR_SUCCESS;
	}

	const char * GetUsage() const override
	{
		return 
			"dbg <category> <integer>\n"
			"categories:\n"
			" views	   -> 0=off 1=on\n"
			" profiler -> 0=off 1=\n"
			" geometry ->";
	}
};

#endif//COMMANDDEBUGINFO_H