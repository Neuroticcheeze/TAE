#include "TimeFlow.hpp"

//=====================================================================================
const char * const TimeUnitDisplayNames[] = {
	"Time of Day",
	"Day",
	"Season"
	"Year"
};

//=====================================================================================
const char * const TimeOfDayDisplayNames[] = {
	"Witching Hour",
	"Wee Hours",
	"Dawn",
	"Morning",
	"Noon",
	"Afternoon",
	"Dusk",
	"Evening",
	"Dead of Night",
};

//=====================================================================================
const char * const SeasonDisplayNames[] = {
	"Summer",
	"Autumn",
	"Winter",
	"Spring",
};

//=====================================================================================
void TimeFlowManager::Init()
{
	m_InitialTimeSet = false;
	m_CurrentTime = 0;
}

//=====================================================================================
void TimeFlowManager::Finalise()
{
	Init();
}

//=====================================================================================
void TimeFlowManager::SetupInitialTime( const TimePoint & a_TimePoint )
{
	if ( !m_InitialTimeSet )
	{
		m_InitialTimeSet = true;
		m_CurrentTime = TimePointToUInt32( a_TimePoint );
	}
}

//=====================================================================================
void TimeFlowManager::ProgressTo( const TimePoint & a_TimePoint )
{
	const TimePoint & tpCurrent = GetTime();
	
	if ( a_TimePoint > tpCurrent )
	{
		TimePeriod elapsed = a_TimePoint - tpCurrent;
		
		uint32_t pre = m_CurrentTime;
		uint32_t post = a_TimePoint.m_TimePoint;
		
		m_CurrentTime = a_TimePoint.m_TimePoint;

#pragma region Listen to TimeUnit
		if ( elapsed.m_TimePeriod > 0 )
		{
			auto & list0 = m_ListenersTU[ ( int32_t )TimeUnit::TIME_OF_DAY ];
			for ( uint32_t k = 0; k < list0.Count(); ++k )
			{
				list0[ k ]->OnTimeUnitChanged( TimeUnit::TIME_OF_DAY, elapsed.m_TimePeriod );
			}
		}
		if ( elapsed.GetYears() > 0 )
		{
			auto & list0 = m_ListenersTU[ ( int32_t )TimeUnit::YEAR ];
			for ( uint32_t k = 0; k < list0.Count(); ++k )
			{
				list0[ k ]->OnTimeUnitChanged( TimeUnit::YEAR, elapsed.GetYears() );
			}
		}
		if ( elapsed.GetDays() > 0 || elapsed.GetYears() > 0 )
		{
			auto & list0 = m_ListenersTU[ ( int32_t )TimeUnit::DAY ];
			for ( uint32_t k = 0; k < list0.Count(); ++k )
			{
				list0[ k ]->OnTimeUnitChanged( TimeUnit::DAY, elapsed.GetDays() + elapsed.GetYears() * DaysPerYear );
			}
		}

		uint32_t numSeasons = 0;
		for ( int32_t l = pre / TimeOfDayCount; l < post / TimeOfDayCount; ++l )
		{
			if ( ( ( l + SeasonOffset ) % DaysPerYear ) % DaysPerSeason == ( DaysPerSeason - 1 ) )
			{
				++numSeasons;
			}
		}
		if ( numSeasons > 0 )
		{
			auto & list0 = m_ListenersTU[ ( int32_t )TimeUnit::SEASON ];
			for ( uint32_t k = 0; k < list0.Count(); ++k )
			{
				list0[ k ]->OnTimeUnitChanged( TimeUnit::SEASON, numSeasons );
			}
		}
#pragma endregion//Listen to TimeUnit

		//fire any listeners
		auto & entries = m_ListenersTP.GetEntries();
		for ( uint32_t w = 0; w < entries.Count(); ++w )
		{
			auto & pair = entries[ w ];
			if ( InRange( pair.Key, pre, post ) )
			{
				Array< IListener * > & list = pair.Value;
				for ( uint32_t a = 0; a < list.Count(); ++a )
				{
					list[ a ]->OnTimePointReached( UInt32ToTimePoint( pair.Key ), elapsed );
				}

				list.Clear();
			}
		}
	}
}

//=====================================================================================
void TimeFlowManager::ProgressBy( const TimePeriod & a_TimePeriod )
{
	TimePoint tpCurrent = GetTime();
	tpCurrent += a_TimePeriod;

	uint32_t pre = m_CurrentTime;
	uint32_t post = tpCurrent.m_TimePoint;

	m_CurrentTime = tpCurrent.m_TimePoint;

	
#pragma region Listen to TimeUnit
		if ( a_TimePeriod.m_TimePeriod > 0 )
		{
			auto & list0 = m_ListenersTU[ ( int32_t )TimeUnit::TIME_OF_DAY ];
			for ( uint32_t k = 0; k < list0.Count(); ++k )
			{
				list0[ k ]->OnTimeUnitChanged( TimeUnit::TIME_OF_DAY, a_TimePeriod.m_TimePeriod );
			}
		}
		if ( a_TimePeriod.GetYears() > 0 )
		{
			auto & list0 = m_ListenersTU[ ( int32_t )TimeUnit::YEAR ];
			for ( uint32_t k = 0; k < list0.Count(); ++k )
			{
				list0[ k ]->OnTimeUnitChanged( TimeUnit::YEAR, a_TimePeriod.GetYears() );
			}
		}
		if ( a_TimePeriod.GetDays() > 0 || a_TimePeriod.GetYears() > 0 )
		{
			auto & list0 = m_ListenersTU[ ( int32_t )TimeUnit::DAY ];
			for ( uint32_t k = 0; k < list0.Count(); ++k )
			{
				list0[ k ]->OnTimeUnitChanged( TimeUnit::DAY, a_TimePeriod.GetDays() + a_TimePeriod.GetYears() * DaysPerYear );
			}
		}

		uint32_t numSeasons = 0;
		for ( int32_t l = pre / TimeOfDayCount; l < post / TimeOfDayCount; ++l )
		{
			if ( ( ( l + SeasonOffset ) % DaysPerYear ) % DaysPerSeason == ( DaysPerSeason - 1 ) )
			{
				++numSeasons;
			}
		}
		if ( numSeasons > 0 )
		{
			auto & list0 = m_ListenersTU[ ( int32_t )TimeUnit::SEASON ];
			for ( uint32_t k = 0; k < list0.Count(); ++k )
			{
				list0[ k ]->OnTimeUnitChanged( TimeUnit::SEASON, numSeasons );
			}
		}
#pragma endregion//Listen to TimeUnit

	//fire any listeners
	auto & entries = m_ListenersTP.GetEntries();
	for ( uint32_t w = 0; w < entries.Count(); ++w )
	{
		auto & pair = entries[ w ];
		if ( InRange( pair.Key, pre, post ) )
		{
			Array< IListener * > & list = pair.Value;
			for ( uint32_t a = 0; a < list.Count(); ++a )
			{
				list[ a ]->OnTimePointReached( UInt32ToTimePoint( pair.Key ), a_TimePeriod );
			}

			list.Clear();
		}
	}
}

//=====================================================================================
void TimeFlowManager::SubscribeToTimePoint( const TimePoint & a_TimePoint, IListener & a_Listener )
{
	if ( !m_ListenersTP.Contains( a_TimePoint.m_TimePoint ) )
	{
		m_ListenersTP.Put( a_TimePoint.m_TimePoint, Array< IListener * >() );
	}

	Array< IListener * > * list = m_ListenersTP[ a_TimePoint.m_TimePoint ];

	if ( list && list->IndexOf( &a_Listener ) == -1 )
	{
		list->Append( &a_Listener );
	}
}

//=====================================================================================
void TimeFlowManager::UnSubscribeToTimePoint( const TimePoint & a_TimePoint, IListener & a_Listener )
{
	if ( m_ListenersTP.Contains( a_TimePoint.m_TimePoint ) )
	{
		Array< IListener * > * list = m_ListenersTP[ a_TimePoint.m_TimePoint ];

		if ( list )
		{
			list->Remove( &a_Listener );
		}
	}
}

//=====================================================================================
void TimeFlowManager::SubscribeToTimeUnit( TimeUnit a_TimeUnit, IListener & a_Listener )
{
	Array< IListener * > & list = m_ListenersTU[ ( int32_t )a_TimeUnit ];

	if ( list.IndexOf( &a_Listener ) == -1 )
	{
		list.Append( &a_Listener );
	}
}

//=====================================================================================
void TimeFlowManager::UnSubscribeToTimeUnit( TimeUnit a_TimeUnit, IListener & a_Listener )
{
	m_ListenersTU[ ( int32_t )a_TimeUnit ].Remove( &a_Listener );
}

//=====================================================================================
TimePoint TimeFlowManager::UInt32ToTimePoint( uint32_t a_UInt32 )
{
	uint32_t tod = a_UInt32 % TimeOfDayCount;
	uint32_t years = a_UInt32 / ( TimeOfDayCount * DaysPerYear );
	uint32_t doy = a_UInt32 / TimeOfDayCount - years * DaysPerYear;

	return TimePoint( ( TimeOfDay )tod, doy, years );
}

//=====================================================================================
uint32_t TimeFlowManager::TimePointToUInt32( const TimePoint & a_TimePoint )
{
	return 
		( uint32_t )a_TimePoint.m_TimeOfDay + 
		a_TimePoint.m_DayOfYear * TimeOfDayCount + 
		a_TimePoint.m_Year * TimeOfDayCount * DaysPerYear;
}

//=====================================================================================
Season TimeFlowManager::DayInYearToSeason( uint32_t a_DayInYear )
{
	int32_t l = Min( DaysPerYear, a_DayInYear ) - SeasonOffset;

	while ( l < 0 )
	{
		l += DaysPerYear;
	}

	return ( Season )( l / DaysPerSeason );
}

//=====================================================================================
uint32_t TimeFlowManager::DayInYearToDayInSeason( uint32_t a_DayInYear )
{
	int32_t l = Min( DaysPerYear, a_DayInYear ) - SeasonOffset;

	while ( l < 0 )
	{
		l += DaysPerYear;
	}

	return l - ( ( l / DaysPerSeason ) * DaysPerSeason );
}

//=====================================================================================
uint32_t TimeFlowManager::DayInSeasonToDayInYear( Season a_Season, uint32_t a_DayInSeason )
{
	int32_t l = ( int32_t )a_Season * DaysPerSeason + ( int32_t )Min( DaysPerSeason, a_DayInSeason ) + SeasonOffset;

	while ( l < 0 )
	{
		l += DaysPerYear;
	}

	return l;
}

//=====================================================================================
TimePeriod::TimePeriod( uint32_t a_TimeOfDayIntervals, uint32_t a_Days, uint32_t a_Years )
{
	// Time-of-day
	uint32_t tod = a_TimeOfDayIntervals % TimeOfDayCount;
	uint32_t todExcessDays = a_TimeOfDayIntervals / TimeOfDayCount;

	// Days
	uint32_t doy = ( a_Days % TimeFlowManager::DaysPerYear ) + todExcessDays;
	uint32_t doyExcessYears = doy / TimeFlowManager::DaysPerYear;

	// Years
	uint32_t years = a_Years + doyExcessYears;

	m_TimePeriod = TimeFlowManager::TimePointToUInt32( TimePoint( ( TimeOfDay )tod, doy, years ) );
}

//=====================================================================================
uint32_t TimePeriod::GetYears() const
{
	return m_TimePeriod / ( TimeOfDayCount * TimeFlowManager::DaysPerYear );
}

//=====================================================================================
uint32_t TimePeriod::GetDays() const
{
	return ( m_TimePeriod / TimeOfDayCount ) - GetYears() * TimeFlowManager::DaysPerYear;
}

//=====================================================================================
uint32_t TimePeriod::GetTimeOfDayIntervals() const
{
	return m_TimePeriod % TimeOfDayCount;
}

//=====================================================================================
TimePoint::TimePoint( TimeOfDay a_TimeOfDay, uint32_t a_DayOfYear, uint32_t a_Year )
		: m_TimeOfDay( a_TimeOfDay )
		, m_DayOfYear( a_DayOfYear )
		, m_Year( a_Year )
{
	m_TimePoint = TimeFlowManager::TimePointToUInt32( *this );
}

//=====================================================================================
TimePoint TimePoint::operator+( const TimePeriod & a_TP ) const
{
	uint32_t tp = m_TimePoint + a_TP.m_TimePeriod;
	return TimeFlowManager::UInt32ToTimePoint( tp );
}

//=====================================================================================
TimePoint & TimePoint::operator+=( const TimePeriod & a_TP )
{
	uint32_t tp = m_TimePoint + a_TP.m_TimePeriod;
	*this = TimeFlowManager::UInt32ToTimePoint( tp );
	m_TimePoint = tp;
	return *this;
}

//=====================================================================================
TimePoint TimePoint::operator-( const TimePeriod & a_TP ) const
{
	uint32_t tp = ( uint32_t )Max( 0, ( int32_t )m_TimePoint - ( int32_t )a_TP.m_TimePeriod );
	return TimeFlowManager::UInt32ToTimePoint( tp );
}

//=====================================================================================
TimePoint & TimePoint::operator-=( const TimePeriod & a_TP )
{
	uint32_t tp = ( uint32_t )Max( 0, ( int32_t )m_TimePoint - ( int32_t )a_TP.m_TimePeriod );
	*this = TimeFlowManager::UInt32ToTimePoint( tp );
	m_TimePoint = tp;
	return *this;
}

//=====================================================================================
TimePeriod TimePoint::operator-( const TimePoint & a_Other ) const
{
	if ( a_Other >= *this )
	{
		return TimePeriod( 0 );
	}

	uint32_t tp = ( uint32_t )Max( 0, ( int32_t )m_TimePoint - ( int32_t )a_Other.m_TimePoint );
	return TimePeriod( tp );
}