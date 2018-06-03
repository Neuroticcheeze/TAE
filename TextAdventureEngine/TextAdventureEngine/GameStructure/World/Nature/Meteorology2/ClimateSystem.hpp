#pragma once
#ifndef CLIMATESYSTEM_H
#define CLIMATESYSTEM_H

#include <Framework/Template/System.hpp>
#include <GameStructure/World/Nature/Flora.hpp>
#include <Framework/Utils/InitialiserList.hpp>
#include "WindSystem.hpp"
#include "Climatograph.hpp"
#include "WeatherData.hpp"

//=====================================================================================
struct ClimateSystemConfiguration final
{
	Array< WindObstruction > Obstructions = {};
	M				Altitude			= 500.0F;
	float			LandToWaterRatio	= 0.5F;
	Flora::Flags	Flora				= Flora::DECIDUOUS_TREES;
	Climatograph	AnnualTemperatures	= Climatograph( { 30.0F, 15.0F, 10.0F, 20.0F }, { 18.0F, 5.0F, -5.0F, 15.0F } );
};

//=====================================================================================
class ClimateSystem final : public System< ClimateSystem >
{
public:

	ClimateSystem( const ClimateSystemConfiguration & a_ClimateSystemConfiguration );

	inline const ClimateSystemConfiguration & GetConfiguration() const
	{
		return m_Config;
	}

	WeatherData GenerateNextWeatherEvent( int a_TimeInterval );

private:

	WindEvent					m_LatestWindEvent;
	WeatherData					m_LatestWeatherData;
	WeatherData					m_PrevCachedData;
	WindSystem					m_WindSystem;
	ClimateSystemConfiguration	m_Config;
};

#endif//CLIMATESYSTEM_H