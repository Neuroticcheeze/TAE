#pragma once
#ifndef TIMEFLOWMANAGER_H
#define TIMEFLOWMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Math/Math.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/Dictionary.hpp>

//=====================================================================================
ENUMCLASS( TimeUnit, uint8_t )
	TIME_OF_DAY,
	DAY,
	SEASON,
	YEAR,
END_ENUMCLASS( TimeUnit, uint8_t );
extern const char * const TimeUnitDisplayNames[];

//=====================================================================================
ENUMCLASS( TimeOfDay, uint8_t )
	THE_WITCHING_HOUR,	// 12am			: "Witching Hour"
	THE_WEE_HOURS,		// 12am - 5am	: "Wee Hours"
	DAWN,				// 5am - 7am	: "Dawn"
	MORNING,			// 7am - 12pm	: "Morning"
	NOON,				// 12pm			: "Noon"
	AFTERNOON,			// 12pm - 5pm	: "Afternoon"
	DUSK,				// 5pm - 7pm	: "Dusk"
	EVENING,			// 7pm - 9pm	: "Evening"
	THE_DEAD_OF_NIGHT,	// 9pm - 12am	: "Dead of Night"
END_ENUMCLASS( TimeOfDay, uint8_t );
extern const char * const TimeOfDayDisplayNames[];

//=====================================================================================
ENUMCLASS( Season, uint8_t )
	AUTUMN,				// Days = 16
	WINTER,				// Days = 16
	SPRING,				// Days = 16
	SUMMER,				// Days = 16
END_ENUMCLASS( Season, uint8_t );
extern const char * const SeasonDisplayNames[];


//=====================================================================================
struct TimePeriod
{
	friend class TimeFlowManager;
	friend struct TimePoint;

	TimePeriod( uint32_t a_TimePeriod )
		: m_TimePeriod( a_TimePeriod )
	{
	}

	TimePeriod( uint32_t a_TimeOfDayIntervals, uint32_t a_Days, uint32_t a_Years );

	uint32_t GetYears() const;
	uint32_t GetDays() const;
	uint32_t GetTimeOfDayIntervals() const;

private:

	uint32_t m_TimePeriod;
};

//=====================================================================================
struct TimePoint
{
	friend class TimeFlowManager;
	friend struct TimePeriod;

	TimePoint( TimeOfDay a_TimeOfDay, uint32_t a_DayOfYear, uint32_t a_Year );

	inline uint32_t GetYear() const
	{
		return m_Year;
	}

	inline uint32_t GetDay() const
	{
		return m_DayOfYear;
	}

	inline TimeOfDay GetTime() const
	{
		return m_TimeOfDay;
	}

	bool operator==( const TimePoint & a_Other ) const
	{
		return m_TimePoint == a_Other.m_TimePoint;
	}

	bool operator!=( const TimePoint & a_Other ) const
	{
		return m_TimePoint != a_Other.m_TimePoint;
	}

	bool operator>( const TimePoint & a_Other ) const
	{
		return m_TimePoint > a_Other.m_TimePoint;
	}

	bool operator<( const TimePoint & a_Other ) const
	{
		return m_TimePoint < a_Other.m_TimePoint;
	}

	bool operator>=( const TimePoint & a_Other ) const
	{
		return m_TimePoint >= a_Other.m_TimePoint;
	}

	bool operator<=( const TimePoint & a_Other ) const
	{
		return m_TimePoint <= a_Other.m_TimePoint;
	}

	TimePoint operator+( const TimePeriod & a_TP ) const;

	TimePoint & operator+=( const TimePeriod & a_TP );

	TimePoint operator-( const TimePeriod & a_TP ) const;

	TimePoint & operator-=( const TimePeriod & a_TP );

	TimePeriod operator-( const TimePoint & a_Other ) const;

private:

	TimeOfDay m_TimeOfDay = TimeOfDay::THE_WITCHING_HOUR;
	uint32_t  m_DayOfYear = 0;
	uint32_t  m_Year = 0;
	uint32_t  m_TimePoint = 0;
};

//=====================================================================================
class TimeFlowManager final : public Manager< TimeFlowManager >
{
public:

	void Init();
	void Finalise();

	static const uint32_t SeasonOffset = 4;
	static const uint32_t DaysPerSeason = 16;
	static const uint32_t DaysPerYear = DaysPerSeason * 4;

	class IListener
	{
		friend class TimeFlowManager;

	protected:

		virtual void OnTimePointReached( const TimePoint & a_TimePoint, const TimePeriod & a_Elapsed ) {}
		virtual void OnTimeUnitChanged( TimeUnit a_TimeUnit, uint32_t a_CountElapsed ) {}
	};

	void SetupInitialTime( const TimePoint & a_TimePoint );
	void ProgressTo( const TimePoint & a_TimePoint );
	void ProgressBy( const TimePeriod & a_TimePeriod );
	void SubscribeToTimePoint( const TimePoint & a_TimePoint, IListener & a_Listener );
	void UnSubscribeToTimePoint( const TimePoint & a_TimePoint, IListener & a_Listener );
	void SubscribeToTimeUnit( TimeUnit a_TimeUnit, IListener & a_Listener );
	void UnSubscribeToTimeUnit( TimeUnit a_TimeUnit, IListener & a_Listener );

	static TimePoint UInt32ToTimePoint( uint32_t a_UInt32 );
	static uint32_t TimePointToUInt32( const TimePoint & a_TimePoint );
	static Season DayInYearToSeason( uint32_t a_DayInYear );
	static uint32_t DayInYearToDayInSeason( uint32_t a_DayInYear );
	static uint32_t DayInSeasonToDayInYear( Season a_Season, uint32_t a_DayInSeason );

	inline TimePoint GetTime() const
	{
		return UInt32ToTimePoint( m_CurrentTime );
	}

private:

	bool	  m_InitialTimeSet		= false;
	uint32_t  m_CurrentTime			= 0;
	Dictionary< uint32_t, Array< IListener * > > m_ListenersTP;
	Array< IListener * > m_ListenersTU[ TimeUnitCount ];
};

#endif//TIMEFLOWMANAGER_H