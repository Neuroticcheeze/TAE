#include "Timer.hpp"

#include <Framework/Math/Math.hpp>

#include <time.h>
#include <limits>
#include <thread>
#include <chrono>

const float g_InvClocksPerSecond = 1.0F / static_cast< float >( CLOCKS_PER_SEC );

//=====================================================================================
Timer::Timer( bool a_CanSleepThread )
	: m_Event( nullptr )
	, m_Interval( 0.0F )
	, m_IsRunning( false )
	, m_CanSleep( a_CanSleepThread )
	, m_StartTick( 0 )
	, m_PrevTick( 0 )
	, m_Cumulative( 0.0F )
	, m_TotalCumulative( 0.0F )
	, m_TotalTicks( 0 )
{

}

//=====================================================================================
void Timer::SetInterval( float a_Seconds )
{
	m_Interval = Max( a_Seconds, 0.0F );
}

//=====================================================================================
void Timer::SetIntervalEvent( Consumer< float > a_Event )
{
	m_Event = a_Event;
}

//=====================================================================================
void Timer::Start()
{
	Stop();

	m_IsRunning = true;
	m_PrevTick = m_StartTick = clock();
}

//=====================================================================================
void Timer::Stop()
{
	if ( m_IsRunning )
	{
		m_IsRunning = false;
		m_StartTick = 0;
		m_PrevTick = 0;
		m_Cumulative = 0.0F;
		m_TotalCumulative = 0.0F;
		m_TotalTicks = 0;
	}
}

//=====================================================================================
bool Timer::Tick()
{
	if ( m_IsRunning )
	{
		int64_t curr = clock();

		m_TotalCumulative = static_cast< float >( curr - m_StartTick ) * g_InvClocksPerSecond;

		int64_t diff = curr - m_PrevTick;

		float timeSinceLast = static_cast< float >( diff ) * g_InvClocksPerSecond;

		m_Cumulative += timeSinceLast;

		if ( m_CanSleep )
		{
			PROFILE2( "CanSleep" );

			if ( m_Event != nullptr )
			{
				++m_TotalTicks;

				m_Event( m_Cumulative );
				m_Cumulative = 0.0F;
			}

			std::this_thread::sleep_for( std::chrono::milliseconds( static_cast< int32_t >( m_Interval * 1000 ) ) );
		}

		else
		{
			PROFILE2( "Runaway" );

			if ( m_Cumulative >= m_Interval )
			{
				if ( m_Event != nullptr )
				{
					++m_TotalTicks;

					m_Event( m_Cumulative );
					m_Cumulative = 0.0F;
				}
			}
		}

		m_PrevTick = curr;
	}

	return m_IsRunning;
}

//=====================================================================================
float Timer::GetDeltaTime() const
{
	return m_Cumulative;
}

//=====================================================================================
float Timer::GetElapsed() const
{
	return m_TotalCumulative;
}

//=====================================================================================
float Timer::GetFramesPerSecond() const
{
	return 1.0F / ( m_Cumulative == 0.0F ? std::numeric_limits< float >::min() : m_Cumulative );
}

//=====================================================================================
uint64_t Timer::GetTotalTicks() const
{
	return m_TotalTicks;
}