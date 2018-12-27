#pragma once
#ifndef CLIMATOGRAPH_H
#define CLIMATOGRAPH_H

#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Math/Gradient.hpp>
#include <GameStructure/World/Nature/TimeFlow.hpp>

//=====================================================================================
struct Vector2;

//=====================================================================================
class Climatograph final
{
public:

	/* Annual temperature graph from Summer->Autumn->Winter->Spring */
	Climatograph( const Celcius * a_YearlyDayTemps, 
				  const Celcius * a_YearlyNightTemps,
				  uint32_t a_Entries );

	/* Temperature and Precipitation from Summer->Autumn->Winter->Spring */
	Climatograph( const InitialiserList< Celcius > & a_YearlyDayTemps, 
				  const InitialiserList< Celcius > & a_YearlyNightTemps );

	Celcius Evaluate( uint32_t a_DayOfYear, TimeOfDay a_TimeOfDay );

private:

	Gradient< Vector2 > m_Entries;
};

#endif//CLIMATOGRAPH_H