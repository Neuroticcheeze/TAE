#include "Debug.hpp"

#if defined( _DEBUG ) || defined( ALWAYS_DEBUG )

#include <Framework/Console/ConsoleManager.hpp>

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <chrono>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <comdef.h>

bool g_StartProfiling = false;
bool g_PrintToConsole = true;

void VSOut( uint32_t a_Line, const char * a_FunctionName, const char * a_Msg, ... )
{
	char buff[2048];

	va_list args;
    va_start( args, a_Msg );
    vsprintf_s( buff, 2048, a_Msg, args );
    va_end( args );


	std::ostringstream oss, oss2;

	oss << buff;
	oss << "                   : [" << a_FunctionName << ", " << a_Line << "]";
	oss << "\n";

	oss2 << buff;

	if ( g_PrintToConsole )
	{
		std::string str1 = oss2.str();
		std::string str2;
		for ( char c : str1 )
		{
			if ( c == '\t' ) str2 += "    ";
			else if ( c == '[' ) str2 += "\\[";
			else str2 += c;
		}
		ConsoleManager::Instance().Log( str2.c_str() );
	}

	oss2 << "                   : \[" << a_FunctionName << ", " << a_Line << "\]";

	OutputDebugString( oss.str().c_str() );
}


bool Assert( bool a_Fatal, bool a_Expression, uint32_t a_Line, const char * a_File, const char * a_Msg, ... )
{
	char buff[ 4096 ];
	wchar_t wbuff[ 4096 ];
	wchar_t wfile[ 4096 ];

	if ( !a_Expression )
	{
		va_list args;
		va_start( args, a_Msg );
		vsprintf_s( buff, 2048, a_Msg, args );
		va_end( args );

		uint32_t len = strlen( buff );
		for ( uint32_t a = 0; a < len; ++a )
		{
			wbuff[ a ] = static_cast< wchar_t >( buff[ a ] );
		}
		wbuff[ len ] = L'\0';

		uint32_t flen = strlen( a_File );
		for ( uint32_t a = 0; a < flen; ++a )
		{
			wfile[ a ] = static_cast< wchar_t >( a_File[ a ] );
		}
		wfile[ flen ] = L'\0';

#ifdef USE_WASSERT
		_wassert( wbuff, wfile, a_Line );
#else

		{
			std::ostringstream oss, oss2;
			oss << "\n\n\n\n======== ASSERT FAILED [" << a_File << ":" << a_Line << "] ========\n" << buff << "\n===============================\n";
			OutputDebugString(oss.str().c_str());

			oss2 << "======== ASSERT FAILED \[" << a_File << ":" << a_Line << "\] ========\n" << buff << "\n===============================";
			std::string str1 = oss2.str();
			std::string str2;
			for ( char c : str1 )
			{
				if ( c == '[' ) str2 += "\\[";
				else str2 += c;
			}
			ConsoleManager::Instance().Error( str2.c_str() );

			if ( a_Fatal ) exit( 1 );
		}
#endif
	}

	return a_Expression;
}

bool AssertHResult( long a_HResult, uint32_t a_Line, const char * a_File, const char * a_Msg, ... )
{
	char buff[ 2048 ];
	wchar_t wbuff[ 2048 ];
	wchar_t wfile[ 1024 ];

	if ( FAILED( a_HResult ) )
	{
		va_list args;
		va_start( args, a_Msg );
		vsprintf_s( buff, 2048, a_Msg, args );
		va_end( args );

		_com_error err( a_HResult );

		std::string strbuff = buff;
		strbuff += " -- HRESULT = [";
		strbuff += err.ErrorMessage();
		strbuff += "]";
		memcpy_s( buff, 2048, strbuff.c_str(), strbuff.length() + 1 );

		uint32_t len = strlen( buff );
		for ( uint32_t a = 0; a < len; ++a )
		{
			wbuff[ a ] = static_cast< wchar_t >( buff[ a ] );
		}
		wbuff[ len ] = L'\0';

		uint32_t flen = strlen( a_File );
		for ( uint32_t a = 0; a < flen; ++a )
		{
			wfile[ a ] = static_cast< wchar_t >( a_File[ a ] );
		}
		wfile[ flen ] = L'\0';

		PRINT( "!!!ASSERT_HRESULT FAILED!!!\n%s\n[%s]:%d\n!!!!!!!!!!!!!!!!!!!!!!!!!!!", buff, a_File, a_Line );
		//_wassert( wbuff, wfile, a_Line );

		return false;
	}

	return true;
}

struct avg
{
	avg()
		: total( 0. )
		, count( 0 )
	{

	}

	inline int32_t calls() const
	{
		return count;
	}

	inline operator double() const
	{
		return total;
	}

	avg & operator+=(double d)
	{
		total *= count;
		total += d;
		total /= ++count;

		return *this;
	}

private:
	double total;
	int32_t count;
};

std::map< const char *, avg > gProfiler;
bool gProfilerComplete = false;

using namespace std::chrono;
typedef time_point< steady_clock > tp;

_ScopedProfiler_::_ScopedProfiler_( const char * a_Name )
	: m_Name( a_Name )
	, m_Start( new tp( high_resolution_clock::now() ) )
{
}

_ScopedProfiler_::~_ScopedProfiler_()
{
	if ( !g_StartProfiling )
	{
		return;
	}

	auto diff = high_resolution_clock::now() - *static_cast< tp* >( m_Start );
	delete m_Start;

	// We're too late, profiler has finished. 
	// This can fail if our scope is global or within the same block containing a call to PDump.
	if ( gProfilerComplete )
	{
		return;
	}

	duration< double, std::milli > ms = duration< double, std::milli >( diff );
	gProfiler[ m_Name ] += ms.count();
}


typedef std::pair< const char *, avg > pair;
typedef std::function< bool( pair, pair ) > cmp;

bool cmp_( const pair & a, const pair & b )
{
	return a.second > b.second;
}

void PDump()
{
	OutputDebugString("\n//------------------------ PROFILER ------------------------\n");

	std::set< pair, cmp > gProfilerSet( gProfiler.begin(), gProfiler.end(), cmp_ );

	auto it = gProfilerSet.cbegin();

	while ( it != gProfilerSet.cend() )
	{
		std::ostringstream oss;

		oss << it->first << std::endl;
		oss << (double)it->second << "ms (avg) -- [" << it->second.calls() << " calls]" << std::endl;
		oss << "--------------------------------------" << std::endl;

		OutputDebugString( oss.str().c_str() );

		++it;
	}

	OutputDebugString("//---------------------- END PROFILER ----------------------\n");
	gProfilerComplete = true;
}

#else

//void Print(  )
//{
//	ConsoleManager::Instance().Log( CString::Format( FMT, __VA_ARGS__ ) );
//}

#endif