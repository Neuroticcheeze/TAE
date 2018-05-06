#pragma once
#ifndef COMMANDLINEMANAGER_H
#define COMMANDLINEMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Containers/CString.hpp>

//=====================================================================================
struct CLArg final
{
	friend class CommandLineManager;

public:

	inline const CString & GetName() const
	{
		return m_Name;
	}

	bool GetParameterAsBool() const
	{
		bool r = false;
		CString::Parse( m_Parameter.Get(), r );
		return r;
	}

	int32_t GetParameterAsInt32() const
	{
		int32_t r = 0;
		CString::Parse( m_Parameter.Get(), r );
		return r;
	}

	const CString & GetParameterAsString() const
	{
		return m_Parameter;
	}

	float GetParameterAsFloat() const
	{
		float r = 0.0F;
		CString::Parse( m_Parameter.Get(), r );
		return r;
	}

private:
	CString m_Name;
	CString m_Parameter;
};

//=====================================================================================
class CommandLineManager final : public Manager< CommandLineManager, const char * >
{
public:

	void Init( const char * a_CommandLine );
	void Finalise();

	bool ArgumentExists( const char * a_Name ) const;
	CLArg GetArgument( const char * a_Name ) const;

private:

	const char * m_CommandLine;
	Array< CLArg > m_Arguments;
};

#endif//COMMANDLINEMANAGER_H