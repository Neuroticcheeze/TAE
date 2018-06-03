#pragma once
#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include "PrecipitationSuspension.hpp"
#include "WindSystem.hpp"

//=====================================================================================
struct WeatherData final
{
	Kmps			WindSpeed;
	Vector2			WindDirection;

	Celcius			Temperature;
	gM3				Humidity; // grams of water-vapour in a meter^3 of dry air
	
	Precipitation	PrecipitationLevels[ Precipitation::TypeCount ]; // How much rain, snow, drizzle, etc exists in this weather event?
	Suspension		SuspensionLevels[ Suspension::TypeCount ];		 // How much fog, mist, clouds, etc exist in this weather event?
};

#endif//WEATHERDATA_H