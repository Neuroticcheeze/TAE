#pragma once
#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <Framework/Containers/CString.hpp>

//=====================================================================================
class ICommand abstract
{
public:

	ENUMCLASS( Result, uint8_t )
		CR_SUCCESS,
		CR_FAILURE_USAGE,
		CR_FAILURE_EXECUTION,
	END_ENUMCLASS( Result, uint8_t );

	virtual const char * Get() const abstract;
	virtual Result Execute( const Array< CString > a_Args ) abstract;
	virtual const char * GetUsage() const abstract;

protected:

private:
};

#endif//ICOMMAND_H