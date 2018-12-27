#pragma once
#ifndef CLIMATEMANAGER_H
#define CLIMATEMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Utils/Function.hpp>
#include <Framework/Utils/Random.hpp>
#include <GameStructure/World/Nature/TimeFlow.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
class CString;

//=====================================================================================
class Geography final
{
public:
	ENUM( Flags, uint16_t )
		LOW_ALTITUDE = 1,
		MEDIUM_ALTITUDE = 2,
		HIGH_ALTITUDE = 4,
		COASTAL = 8,
		LAKEFRONT = 16,
		FLAT = 32,
		RUGGED = 64,
		TROPICAL = 128,
		TEMPERATE = 256,
		ARCTIC = 512,
		ARID_DESERT = 1024,
		// Helper
		ALL = 2048-1,
	END_ENUM_F2( Flags, uint16_t, 11 );
};

//=====================================================================================
ENUMCLASS( Temperature, uint8_t )
	FREEZING,
	ICY,
	COLD,
	CHILLY,
	COOL,
	PLEASANT,
	BALMY,
	WARM,
	HOT,
	SWELTERING,
	SCORCHING,	
END_ENUMCLASS( Temperature, uint8_t )
extern const char * const TemperatureDisplayNames[];

//=====================================================================================
ENUMCLASS( Humidity, uint8_t )
	ARID,
	PARCHED,
	DRY,
	PLEASANT,
	HUMID,
	WET,
	TROPICAL,
END_ENUMCLASS( Humidity, uint8_t )
extern const char * const HumidityDisplayNames[];

//=====================================================================================
struct WeatherEventOccurrence final
{
	WeatherEventOccurrence( float a_IdealTemperature, float a_IdealHumidity, float a_TemperatureChaos, float a_HumidityChaos, float a_AllRarity )
	{
		TempHumid = Vector2( a_IdealTemperature, a_IdealHumidity );
		Chaos = Vector2( a_TemperatureChaos, a_HumidityChaos );
		WithRarity( Geography::Flags::ALL, a_AllRarity );
	}

	WeatherEventOccurrence & WithRarity( Geography::Flags a_Flags, float a_Rarity )
	{
		for ( uint32_t p = 0; p < Geography::FlagsCount; ++p )
		{
			if ( ( a_Flags & ( Geography::Flags )p ) == ( Geography::Flags )p )
			{
				Rarity[ p ] = Clamp( a_Rarity );
			}
		}

		return *this;
	}

	float CalculateRarity( Geography::Flags a_Flags ) const
	{
		float minRar = 1.0F;
		for ( uint32_t p = 0; p < Geography::FlagsCount; ++p )
		{
			if ( ( a_Flags & ( Geography::Flags )p ) == ( Geography::Flags )p )
			{
				minRar = Min( minRar, Rarity[ p ] );
			}
		}

		return Clamp( minRar );
	}

	Vector2 TempHumid;
	Vector2 Chaos;

private:
	float Rarity[ Geography::FlagsCount ];
};

//=====================================================================================
struct WeatherEventInfluence final
{
	WeatherEventInfluence( int8_t a_TemperatureShiftMin, int8_t a_TemperatureShiftMax, int8_t a_HumidityShiftMin, int8_t a_HumidityShiftMax )
		: m_TemperatureShiftMin( a_HumidityShiftMin )
		, m_TemperatureShiftMax( a_TemperatureShiftMax )
		, m_HumidityShiftMin( a_HumidityShiftMin )
		, m_HumidityShiftMax( a_HumidityShiftMax )
	{
	}

	Temperature GetShiftedTemperature( const Random & a_Random, Temperature a_In ) const
	{
		int32_t in = ( int32_t )a_In;
		int32_t shift = a_Random.Range( m_TemperatureShiftMin, m_TemperatureShiftMax );

		return ( Temperature )Clamp( in + shift, 0, TemperatureCount - 1 );
	}

	Humidity GetShiftedHumidity( const Random & a_Random, Humidity a_In ) const
	{
		int32_t in = ( int32_t )a_In;
		int32_t shift = a_Random.Range( m_HumidityShiftMin, m_HumidityShiftMax );

		return ( Humidity )Clamp( in + shift, 0, HumidityCount - 1 );
	}

private:

	int8_t m_TemperatureShiftMin;
	int8_t m_TemperatureShiftMax;
	int8_t m_HumidityShiftMin; 
	int8_t m_HumidityShiftMax;
};

//=====================================================================================
ENUMCLASS( WeatherEvent, uint8_t )
	FAIR_SKIES,		// Clear skies, sun/moon/stars shining down, few/no clouds
	OVERCAST,		// Grey, dull, cloudy, gloomy skies
	FOGGY,			// Heavy fog settling over, visibility next-to-none
	DISTANT_STORM,	// All quiet except for the distant rumble of thunder off to the horizon
	WINDY_STORM,	// Strong winds blowing, large storm clouds brewing overhead
	DRIZZLE,		// Light rain, the tap of droplets hitting leaves and ferns
	DOWNPOUR,		// Strong drenching rain, loud rain storm, lightning
	HAIL,			// Big freezing downpour of ice pellets, covering the landscape in white like snow
	SLEET,			// Light downpour of icy rain and snow, the ground is covered in slushy snow
	LIGHT_FLURRY,	// Very light and brief snowfall, feint dusting of snow over the landscape, disappears quickly
	SNOW,			// Snowflakes settling down from the sky onto the white landscape
	BLIZZARD,		// Almost no visibility, absolutely freezing, fast, strong, howling winds, massive snow downpour.
	DIAMOND_DUST,	// Magical fine ice crystals floating through the clear air. Formed only in the coldest climates with very little humidity
	HURRICANE,		// Enormous storm cell with galeforce winds, gloomy low visibility, choppy waves, rain, dangerous, loud, destructive, dangerous
	SAND_STORM,		// Only present in arid deserts, massive red sandy front enshrouding the landscape in darkness and poor visibility as sand is blasted through the air
	CLEAR_SHOWER,	// Light rain falling during the middle of a sunny day, or with a bright moon out - generally caused by winds blowing rain off course over the region
END_ENUMCLASS( WeatherEvent, uint8_t )
extern const char * const WeatherEventDisplayNames[];
extern const WeatherEventOccurrence WeatherEventOccurrences[ WeatherEventCount ];
extern const WeatherEventInfluence WeatherEventInfluences[ WeatherEventCount ];

//=====================================================================================
struct Climate
{
	Climate() : Climate( Temperature::PLEASANT, Humidity::PLEASANT ) {}
	Climate( Temperature a_Temperature, Humidity a_Humidity )
		: Temperature( a_Temperature )
		, Humidity( a_Humidity )
	{
	}

	Temperature Temperature;
	Humidity	Humidity;
};

//=====================================================================================
struct DayNightClimate final
{
	DayNightClimate() : DayNightClimate( Temperature::PLEASANT, Temperature::PLEASANT, Humidity::PLEASANT )
	{
	}

	DayNightClimate( Temperature a_DayTemp, Temperature a_NightTemp, Humidity a_AllTimeHumidity )
		: DayClimate( a_DayTemp, a_AllTimeHumidity )
		, NightClimate( a_NightTemp, a_AllTimeHumidity )
	{
	}

	DayNightClimate( Temperature a_DayTemp, Temperature a_NightTemp, Humidity a_DayHumidity, Humidity a_NightHumidity )
		: DayClimate( a_DayTemp, a_DayHumidity )
		, NightClimate( a_NightTemp, a_NightHumidity )
	{
	}

	Climate NightClimate;
	Climate DayClimate;
};

//=====================================================================================
struct Climatograph final
{
	Climatograph()
	{

	}

	Climatograph( DayNightClimate a_EarlySummer, DayNightClimate a_MidSummer, 
				  DayNightClimate a_EarlyAutumn, DayNightClimate a_MidAutumn, 
				  DayNightClimate a_EarlyWinter, DayNightClimate a_MidWinter, 
				  DayNightClimate a_EarlySpring, DayNightClimate a_MidSpring )
	{
		m_ClimatographyData[ 0 ] = a_EarlySummer;
		m_ClimatographyData[ 1 ] = a_MidSummer;
		m_ClimatographyData[ 2 ] = a_EarlyAutumn;
		m_ClimatographyData[ 3 ] = a_MidAutumn;
		m_ClimatographyData[ 4 ] = a_EarlyWinter;
		m_ClimatographyData[ 5 ] = a_MidWinter;
		m_ClimatographyData[ 6 ] = a_EarlySpring;
		m_ClimatographyData[ 7 ] = a_MidSpring;
	}

	DayNightClimate GetDayNightClimateFor( Season a_Season, bool a_MiddleOf ) const
	{
		return m_ClimatographyData[ ( ( uint32_t )a_Season << 1 ) + ( a_MiddleOf ? 1 : 0 ) ];
	}

private:
	DayNightClimate m_ClimatographyData[ SeasonCount * 2 ];
};

//=====================================================================================
struct WeatherData final
{
	Temperature Temperature;
	Humidity Humidity;
	WeatherEvent WeatherEvent;
};

//=====================================================================================
class ClimateManager final : public Manager< ClimateManager >
{
public:

	void Init();
	void Finalise();

	WeatherData GenerateWeatherData( const Climatograph & a_Climatography, Geography::Flags a_Geography ) const;

private:

	Random m_Random;
};

#endif//CLIMATEMANAGER_H