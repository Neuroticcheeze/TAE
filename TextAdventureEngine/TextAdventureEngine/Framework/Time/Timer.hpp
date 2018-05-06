#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <Framework/Utils/Function.hpp>

//=====================================================================================
class Timer final
{
public:

	/* Create a timer. If it can sleep the thread between intervals, timing may be less stable, but allows for better cross-thread performance */
	Timer( bool a_CanSleepThread = true );

	void SetInterval( float a_Seconds );
	
	void SetIntervalFPS( int32_t a_TargetFPS )
	{
		SetInterval( 1.0F / static_cast< float >( a_TargetFPS ) );
	}
	
	void SetIntervalEvent( Consumer< float > a_Event );

	void Start();
	void Stop();

	bool Tick();
	
	float GetDeltaTime() const;
	float GetElapsed() const;
	float GetFramesPerSecond() const;
	uint64_t GetTotalTicks() const;

private:

	Consumer< float > m_Event;
	float m_Interval;
	bool m_IsRunning;
	bool m_CanSleep;
	int64_t m_StartTick;
	int64_t m_PrevTick;
	float m_Cumulative;
	float m_TotalCumulative;
	uint64_t m_TotalTicks;
};

#endif//TIMER_H