#pragma once
#ifndef CLIMATOGRAPH_H
#define CLIMATOGRAPH_H

#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Math/Gradient.hpp>
#include <Framework/GameStructure/World/Nature/TimeFlow.hpp>

//=====================================================================================
struct Vector2;

//=====================================================================================
class Climatograph final
{
public:

	/* Annual temperature graph from Summer->Autumn->Winter->Spring */
	Climatograph( const float * a_YearlyDayTempsCelcius, 
				  const float * a_YearlyNightTempsCelcius,
				  uint32_t a_Entries );

	/* Temperature and Precipitation from Summer->Autumn->Winter->Spring */
	Climatograph( const InitialiserList< float > & a_YearlyDayTempsCelcius, 
				  const InitialiserList< float > & a_YearlyNightTempsCelcius );

	float Evaluate( uint32_t a_DayOfYear, TimeOfDay a_TimeOfDay );

private:

	Gradient< Vector2 > m_Entries;
};

#endif//CLIMATOGRAPH_H