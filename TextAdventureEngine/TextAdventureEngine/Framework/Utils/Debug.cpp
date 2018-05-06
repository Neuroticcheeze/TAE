#include "Debug.hpp"

#ifdef _DEBUG

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

void VSOut( uint32_t a_Line, const char * a_FunctionName, const char * a_Msg, ... )
{
	char buff[2048];

	va_list args;
    va_start( args, a_Msg );
    vsprintf_s( buff, 2048, a_Msg, args );
    va_end( args );


	std::ostringstream oss;

	oss << buff;

	oss << "                   : [" << a_FunctionName << ", " << a_Line << "]";
	oss << "\n";

	OutputDebugString( oss.str().c_str() );
}


bool Assert( bool a_Expression, uint32_t a_Line, const char * a_File, const char * a_Msg, ... )
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

		_wassert( wbuff, wfile, a_Line );
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

std::map< const char *, avg > profiler;

using namespace std::chrono;
typedef time_point< steady_clock > tp;

_ScopedProfiler_::_ScopedProfiler_( const char * a_Name )
	: m_Name( a_Name )
	, m_Start( new tp( high_resolution_clock::now() ) )
{
}

_ScopedProfiler_::~_ScopedProfiler_()
{
	auto diff = high_resolution_clock::now() - *static_cast< tp* >( m_Start );
	delete m_Start;

	duration< double, std::milli > ms = duration< double, std::milli >( diff );

	profiler[ m_Name ] += ms.count();
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

	std::set< pair, cmp > profilerSet( profiler.begin(), profiler.end(), cmp_ );

	auto it = profilerSet.cbegin();

	while ( it != profilerSet.cend() )
	{
		std::ostringstream oss;

		oss << it->first << std::endl;
		oss << (double)it->second << "ms (avg) -- [" << it->second.calls() << " calls]" << std::endl;
		oss << "--------------------------------------" << std::endl;

		OutputDebugString( oss.str().c_str() );

		++it;
	}

	OutputDebugString("//---------------------- END PROFILER ----------------------\n");
}

#endif