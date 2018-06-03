#include "TimePoint.hpp"
#include <chrono>
#ifdef _WIN32

#include <windows.h>

#endif

//=====================================================================================
TimePoint::TimePoint()
	: m_TimeZone( TimeZone::UTC_00 )
	, m_Year( 0 )
	, m_Month( 0 )
	, m_Day( 0 )
	, m_Hour( 0 )
	, m_Minute( 0 )
	, m_Second( 0 )
{
}

//=====================================================================================
TimePoint TimePoint::Now()
{
	TimePoint tp;

#ifdef _WIN32

	TIME_ZONE_INFORMATION tzi;
	ZeroMemory( &tzi, sizeof( tzi ) );
	GetTimeZoneInformation( &tzi );

	tp.m_TimeZone = (TimeZone)tzi.Bias;

	time_t t = time( 0 );
	struct tm * now = localtime(&t);

	tp.m_Year = now->tm_year + 1900;
	tp.m_Month = now->tm_mon;
	tp.m_Day = now->tm_mday - 1;
	tp.m_Hour = now->tm_hour;
	tp.m_Minute = now->tm_min;
	tp.m_Second = now->tm_sec;

#endif

	return tp;
}

//=====================================================================================
TimePoint TimePoint::Now( TimeZone a_RelativeTimeZone )
{
	struct tm _tm;
	TimePoint tp = Now();

	_tm.tm_year = tp.m_Year - 1900;
	_tm.tm_mon = tp.m_Month;
	_tm.tm_mday = tp.m_Day + 1;
	_tm.tm_hour = tp.m_Hour;
	_tm.tm_min = tp.m_Minute;
	_tm.tm_sec = tp.m_Second;

	_tm.tm_min += (int32_t)a_RelativeTimeZone - (int32_t)tp.m_TimeZone;

	mktime(&_tm);

	tp.m_TimeZone = a_RelativeTimeZone;
	tp.m_Year = _tm.tm_year + 1900;
	tp.m_Month = _tm.tm_mon;
	tp.m_Day = _tm.tm_mday - 1;
	tp.m_Hour = _tm.tm_hour;
	tp.m_Minute = _tm.tm_min;
	tp.m_Second = _tm.tm_sec;

	return tp;
}

//=====================================================================================
TimePoint TimePoint::operator+( const TimeSpan & a_TimeSpan ) const
{
	if ( !a_TimeSpan.IsValid() )
	{
		return TimePoint( *this );
	}

	struct tm _tm;
	TimePoint tp = Now();

	_tm.tm_year = tp.m_Year - 1900;
	_tm.tm_mon = tp.m_Month;
	_tm.tm_mday = tp.m_Day + 1;
	_tm.tm_hour = tp.m_Hour;
	_tm.tm_min = tp.m_Minute;
	_tm.tm_sec = tp.m_Second;

	_tm.tm_sec += static_cast< int32_t >( a_TimeSpan.GetSeconds() % ( 60 * 60 * 24 ) );
	//_tm.tm_min += static_cast< int32_t >( a_TimeSpan.GetSeconds() % ( 60 * 24 ) );

	mktime(&_tm);

	tp.m_Year = _tm.tm_year + 1900;
	tp.m_Month = _tm.tm_mon;
	tp.m_Day = _tm.tm_mday - 1;
	tp.m_Hour = _tm.tm_hour;
	tp.m_Minute = _tm.tm_min;
	tp.m_Second = _tm.tm_sec;

	return tp;
}

//=====================================================================================
TimePoint TimePoint::operator-( const TimeSpan & a_TimeSpan ) const
{
	if ( !a_TimeSpan.IsValid() )
	{
		return TimePoint( *this );
	}

	struct tm _tm;
	TimePoint tp = Now();

	_tm.tm_year = tp.m_Year - 1900;
	_tm.tm_mon = tp.m_Month;
	_tm.tm_mday = tp.m_Day + 1;
	_tm.tm_hour = tp.m_Hour;
	_tm.tm_min = tp.m_Minute;
	_tm.tm_sec = tp.m_Second;

	_tm.tm_sec -= static_cast< int32_t >( a_TimeSpan.GetSeconds() % ( 60 * 60 * 24 ) );
	//_tm.tm_sec -= a_TimeSpan.GetSeconds();

	mktime(&_tm);

	tp.m_Year = _tm.tm_year + 1900;
	tp.m_Month = _tm.tm_mon;
	tp.m_Day = _tm.tm_mday - 1;
	tp.m_Hour = _tm.tm_hour;
	tp.m_Minute = _tm.tm_min;
	tp.m_Second = _tm.tm_sec;

	return tp;
}

//=====================================================================================
TimeSpan TimePoint::operator+( const TimePoint & a_Other ) const
{
	return TimeSpan();
}

//=====================================================================================
TimeSpan TimePoint::operator-( const TimePoint & a_Other ) const
{
	return TimeSpan();
}

bool TimePoint::IsValid() const
{
	return m_Year	> 0 && 
		   m_Year	<= 2999 &&
		   m_Month	< 12 &&
		   m_Day	< 31 &&
		   m_Hour	< 24 &&
		   m_Minute < 60 &&
		   m_Second < 60;
}