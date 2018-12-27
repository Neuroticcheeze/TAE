#pragma once
#ifndef TIMEPOINT_H
#define TIMEPOINT_H

#include <Framework/Template/Validatable.hpp>

//=====================================================================================
struct TimeSpan final : public Validatable
{
public:

	TimeSpan() 
		: m_Seconds( 0 )
	{
	}

	TimeSpan( int16_t a_Days, int8_t a_Hours, int8_t a_Minutes, int8_t a_Seconds )
	{
		m_Seconds = a_Seconds;
		m_Seconds += a_Minutes * 60;
		m_Seconds += a_Hours * 60 * 60;
		m_Seconds += a_Days * 24 * 60 * 60;
	}
	
	TimeSpan( int64_t a_Seconds )
	{
		m_Seconds = a_Seconds;
	}

	bool IsValid() const { return false; }

	int64_t GetSeconds() const { return m_Seconds; }

private:

	int64_t m_Seconds;
};

//=====================================================================================
struct TimePoint final : public Validatable
{
public:

	ENUMCLASS( TimeZone, int32_t )
		UTC_N12		= -12	*60,
		UTC_N11		= -11	*60,
		UTC_N10		= -10	*60,
		UTC_N09_30	= -9	*60-30,
		UTC_N09		= -9	*60,
		UTC_N08		= -8	*60,
		UTC_N07		= -7	*60,
		UTC_N06		= -6	*60,
		UTC_N05		= -5	*60,
		UTC_N04		= -4	*60,
		UTC_N03_30	= -3	*60-30,
		UTC_N03		= -3	*60,
		UTC_N02		= -2	*60,
		UTC_N01		= -1	*60,
		UTC_00		= 0,
		UTC_P01		= 1		*60,
		UTC_P02		= 2		*60,
		UTC_P03		= 3		*60,
		UTC_P03_30	= 3		*60+30,
		UTC_P04		= 4		*60,
		UTC_P04_30	= 4		*60+30,
		UTC_P05		= 5		*60,
		UTC_P05_30	= 5		*60+30,
		UTC_P05_45	= 5		*60+45,
		UTC_P06		= 6		*60,
		UTC_P06_30	= 6		*60+30,
		UTC_P07		= 7		*60,
		UTC_P08		= 8		*60,
		UTC_P08_30	= 8		*60+30,
		UTC_P08_45	= 8		*60+45,
		UTC_P09		= 9		*60,
		UTC_P09_30	= 9		*60+30,
		UTC_P10		= 10	*60,
		UTC_P10_30	= 10	*60+30,
		UTC_P11		= 11	*60,
		UTC_P12		= 12	*60,
		UTC_P12_45	= 12	*60+45,
		UTC_P13		= 13	*60,
		UTC_P14		= 14	*60,
	END_ENUM_C2( TimeZone, int32_t, 39 )

	TimePoint();

	static TimePoint Now();
	static TimePoint Now( TimeZone a_RelativeTimeZone );

	TimePoint operator+( const TimeSpan & a_TimeSpan ) const;
	TimePoint operator-( const TimeSpan & a_TimeSpan ) const;
	TimeSpan operator+( const TimePoint & a_Other ) const;
	TimeSpan operator-( const TimePoint & a_Other ) const;
	bool operator>( const TimePoint & a_Other ) const{return false;}
	bool operator<( const TimePoint & a_Other ) const;
	bool operator>=( const TimePoint & a_Other ) const;
	bool operator<=(const TimePoint & a_Other) const;
	bool operator==( const TimePoint & a_Other ) const;
	bool operator!=( const TimePoint & a_Other ) const;

	TimeZone GetTimeZone() const;
	void	 SetTimeZone( TimeZone a_TimeZone );

	bool IsValid() const;

private:

	TimeZone m_TimeZone;
	uint16_t m_Year		: 12;
	uint8_t m_Month		: 4;
	uint8_t m_Day		: 5;
	uint8_t m_Hour		: 5;
	uint8_t m_Minute	: 6;
	uint8_t m_Second	: 6;
};

#endif//TIMEPOINT_H