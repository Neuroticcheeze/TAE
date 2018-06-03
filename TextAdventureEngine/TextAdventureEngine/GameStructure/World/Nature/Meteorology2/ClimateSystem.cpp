#include "ClimateSystem.hpp"
#include "MeteorologyUtils.hpp"

//=====================================================================================
ClimateSystem::ClimateSystem( const ClimateSystemConfiguration & a_ClimateSystemConfiguration )
	: m_Config( a_ClimateSystemConfiguration )
	, m_WindSystem( m_Config.Obstructions )
{

}

//=====================================================================================
WeatherData ClimateSystem::GenerateNextWeatherEvent( int a_TimeInterval )
{
	static const float sunMixer[ TimeOfDayCount ] = {
		0.0F,		//	THE_WITCHING_HOUR
		0.2F,		//	THE_WEE_HOURS,
		0.7F,		//	DAWN,
		0.95F,		//	MORNING,
		1.0F,		//	NOON,
		0.8F,		//	AFTERNOON,
		0.6F,		//	DUSK,
		0.1F,		//	EVENING,
		0.0F,		//	THE_DEAD_OF_NIGHT
	};

	// generate the next wind event
	m_LatestWindEvent = m_WindSystem.GenerateNextWindEvent();

	// query base temperature for time-of-day + time-of-year
	Celcius sourceTemp = m_Config.AnnualTemperatures.Evaluate( a_TimeInterval / TimeOfDayCount, ( TimeOfDay )( a_TimeInterval % TimeOfDayCount ) );

	// calculate base humidities
	gM3 sourceAbsHumid = GetAbsoluteHumidity( sourceTemp );
	gM3 sourceSpecHumid = m_LatestWeatherData.Humidity;

	// modify sourcetemp from wind
	sourceTemp = GetTempFromWindChill( sourceTemp, m_LatestWindEvent.WindSpeed, sourceSpecHumid / sourceAbsHumid );

	// after temperature is finalised, recalculate abs humidity.
	sourceAbsHumid = GetAbsoluteHumidity( sourceTemp );

	// calculate sunlight
	float cloudAmount = m_LatestWeatherData.SuspensionLevels[ ( uint32_t )Suspension::Type::CLOUD ].Amount;
	gM3 fogDensity = m_LatestWeatherData.SuspensionLevels[ ( uint32_t )Suspension::Type::FOG ].VapourDensity;

	float sunlightPower = sunMixer[ a_TimeInterval % TimeOfDayCount ] * 
							( 1.0F - cloudAmount ) * 
							( 1.0F - Clamp( ReMap( fogDensity, 0.0F, 100.0F, 0.0F, 1.0F ) ) );

	// calculate waterbody evaporation
	float evap = sunlightPower * m_Config.LandToWaterRatio * 0.05432F;//grams of water evaporated per m2 of surface every 1/9th of a day at full sunlight
	m_LatestWeatherData.Humidity += evap;

	if ( m_LatestWeatherData.Humidity > sourceAbsHumid )
	{

	}

	return m_LatestWeatherData;
}