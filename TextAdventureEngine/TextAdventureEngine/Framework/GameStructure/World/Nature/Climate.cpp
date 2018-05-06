#include "Climate.hpp"
#include <Framework/Containers/CString.hpp>

//=====================================================================================
const float ALL = -1.0F;
const WeatherEventOccurrence WeatherEventOccurrences[ WeatherEventCount ] =
{
	// FAIR_SKIES,		- Clear skies, sun/moon/stars shining down, few/no clouds
	WeatherEventOccurrence( ALL, ALL, 0.42F, 0.42F, 0.3F ),			

	// OVERCAST,		- Grey, dull, cloudy, gloomy skies
	WeatherEventOccurrence( ALL, 0.65F, 1.0F, 0.5F, 0.25F )
	.WithRarity( Geography::HIGH_ALTITUDE | Geography::ARID_DESERT, 0.475F ),	

	// FOGGY,			- Heavy humid fog settling over, visibility next-to-none, happens after the rains or in the morning
	WeatherEventOccurrence( ALL, 1.0F, 1.0F, 0.5F, 0.4F)
	.WithRarity( Geography::ARID_DESERT | Geography::TROPICAL | Geography::ARCTIC, 0.9F )
	.WithRarity( Geography::MEDIUM_ALTITUDE, 0.525F )
	.WithRarity( Geography::HIGH_ALTITUDE, 0.65F )
	.WithRarity( Geography::LAKEFRONT, 0.275F )
	.WithRarity( Geography::RUGGED, 0.325F ),

	// DISTANT_STORM,	- All quiet except for the distant rumble of thunder off to the horizon
	WeatherEventOccurrence( 0.75F, 0.6F, 0.25F, 0.4F, 0.6F )
	.WithRarity( Geography::LOW_ALTITUDE | Geography::FLAT | Geography::COASTAL, 0.425F ),

	// WINDY_STORM,		- Strong winds blowing, large storm clouds brewing overhead
	WeatherEventOccurrence( 0.6F, 0.22F, 0.3F, 0.275F, 0.4F ),		

	// DRIZZLE,			- Light rain, the tap of droplets hitting leaves and ferns
	WeatherEventOccurrence( 0.615F, 0.65F, 0.15F, 0.4F, 0.425F )
	.WithRarity( Geography::COASTAL, 0.275F )
	.WithRarity( Geography::ARCTIC, 0.65F )
	.WithRarity( Geography::TROPICAL, 0.35F ),

	// DOWNPOUR,		- Strong drenching rain, loud rain storm, lightning
	WeatherEventOccurrence( 0.615F, 0.65F, 0.15F, 0.4F, 0.45F )
	.WithRarity( Geography::COASTAL, 0.275F )
	.WithRarity( Geography::ARCTIC, 0.65F )
	.WithRarity( Geography::TROPICAL, 0.35F ),

	// HAIL,			- Big freezing downpour of ice pellets, covering the landscape in white like snow
	WeatherEventOccurrence( 0.15F, 0.5F, 0.15F, 0.45F, 0.78F )
	.WithRarity( Geography::LOW_ALTITUDE, 0.675F )
	.WithRarity( Geography::ARID_DESERT, 0.95F ),

	// SLEET,			- Light downpour of icy rain and snow, the ground is covered in slushy snow
	WeatherEventOccurrence( 0.1F, 0.6F, 0.05F, 0.3F, 0.85F )
	.WithRarity( Geography::ARID_DESERT, 0.985F ),		

	// LIGHT_FLURRY		- Very light and brief snowfall, feint dusting of snow over the landscape, disappears quickly
	WeatherEventOccurrence( 0.175F, ALL, 0.1F, 0.5F, 0.9F )
	.WithRarity( Geography::ARID_DESERT, 0.95F ),		

	// SNOW,			- Snowflakes settling down from the sky onto the white landscape
	WeatherEventOccurrence( 0.0F, ALL, 0.15F, 0.4F, 0.9F )
	.WithRarity( Geography::ARID_DESERT, 1.0F ),			

	// BLIZZARD,		- Almost no visibility, absolutely freezing, fast, strong, howling winds, massive snow downpour.
	WeatherEventOccurrence( 0.0F, ALL, 0.1F, 0.4F, 0.94F )
	.WithRarity( Geography::ARID_DESERT, 1.0F ),			

	// DIAMOND_DUST,	- Magical fine ice crystals floating through the clear air. Formed only in the coldest climates with very little humidity
	WeatherEventOccurrence( 0.0F, 0.0F, 0.1F, 0.375F, 0.995F )
	.WithRarity( Geography::ARID_DESERT, 1.0F ),

	// HURRICANE,		- Enormous storm cell with galeforce winds, gloomy low visibility, choppy waves, rain, dangerous, loud, destructive, dangerous
	WeatherEventOccurrence( 0.9F, 0.9F, 0.3F, 0.2F, 1.0F )
	.WithRarity( Geography::COASTAL | Geography::TROPICAL, 0.9995F ),

	// SAND_STORM,		- Only present in arid deserts, massive red sandy front enshrouding the landscape in darkness and poor visibility as sand is blasted through the air
	WeatherEventOccurrence( 0.9F, 0.0F, 0.1F, 0.1F, 1.0F )
	.WithRarity( Geography::ARID_DESERT, 0.9F ),

	// CLEAR_SHOWER,	- Light rain falling during the middle of a sunny day, or with a bright moon out - generally caused by winds blowing rain off course over the region
	WeatherEventOccurrence( 0.625F, 0.4F, 0.15F, 0.4F, 0.5F )
	.WithRarity( Geography::HIGH_ALTITUDE, 0.75F )
	.WithRarity( Geography::MEDIUM_ALTITUDE, 0.625F )
	.WithRarity( Geography::ARCTIC, 0.9F )
};

//=====================================================================================
const WeatherEventInfluence WeatherEventInfluences[ WeatherEventCount ] =
{
	WeatherEventInfluence( 0, 0, 0, 0 ),	// FAIR_SKIES,		
	WeatherEventInfluence( -1, 0, 0, 1 ),	// OVERCAST,		
	WeatherEventInfluence( 0, 0, 0, 2 ),	// FOGGY,			
	WeatherEventInfluence( 0, 0, 0, 0 ),	// DISTANT_STORM,	
	WeatherEventInfluence( -2, 0, -1, 0 ),	// WINDY_STORM,	
	WeatherEventInfluence( -1, 0, 0, 2 ),	// DRIZZLE,		
	WeatherEventInfluence( -2, 0, 0, 3 ),	// DOWNPOUR,		
	WeatherEventInfluence( -3, 0, 0, 0 ),	// HAIL,			
	WeatherEventInfluence( -2, 0, 0, 0 ),	// SLEET,			
	WeatherEventInfluence( -1, 0, 0, 0 ),	// LIGHT_FLURRY,	
	WeatherEventInfluence( -2, 0, 0, 0 ),	// SNOW,			
	WeatherEventInfluence( -4, 0, -1, 0 ),	// BLIZZARD,		
	WeatherEventInfluence( 0, 0, 0, 0 ),	// DIAMOND_DUST,	
	WeatherEventInfluence( -3, 0, 0, 10 ),	// HURRICANE,		
	WeatherEventInfluence( 0, 1, -1, 0 ),	// SAND_STORM,		
	WeatherEventInfluence( 0, 0, 0, 1 )		// CLEAR_SHOWER,	
};

//=====================================================================================
const char * const TemperatureDisplayNames[] = {
	"Freezing",
	"Icy",
	"Cold",
	"Chilly",
	"Cool",
	"Pleasant",
	"Balmy",
	"Warm",
	"Hot",
	"Sweltering",
	"Scorching",
};

//=====================================================================================
const char * const HumidityDisplayNames[] = {
	"Arid",
	"Parched",
	"Dry",
	"Pleasant",
	"Humid",
	"Wet",
	"Tropical",
};

//=====================================================================================
const char * const WeatherEventDisplayNames[] = {
	"Fair Skies",
	"Overcast",
	"Foggy",
	"Distant Storms",
	"Wind Storm",
	"Drizzle",
	"Downpour",
	"Hail Shower",
	"Sleet Shower",
	"Light Flurry",
	"Snowfall",
	"Blizzard",
	"Diamond Dust",
	"Hurricane",
	"Sandstorm",
	"Clear Showers",
};

//=====================================================================================
void ClimateManager::Init()
{
}

//=====================================================================================
void ClimateManager::Finalise()
{
}

//=====================================================================================
WeatherData ClimateManager::GenerateWeatherData(  const Climatograph & a_Climatography, Geography::Flags a_Geography ) const
{
	const auto time = TimeFlowManager::Instance().GetTime();
	const TimeOfDay tod = time.GetTime();
	const Season season = TimeFlowManager::DayInYearToSeason( time.GetDay() );
	const bool isMidSeason = TimeFlowManager::DayInYearToDayInSeason( time.GetDay() ) / ( TimeFlowManager::DaysPerSeason >> 1 );
	const DayNightClimate dnclimate = a_Climatography.GetDayNightClimateFor( season, isMidSeason );
	
	Vector2 dncDay( ( float )dnclimate.DayClimate.Temperature / ( TemperatureCount - 1 ),
					( float )dnclimate.DayClimate.Humidity / ( HumidityCount - 1 ) );
	Vector2 dncNight( ( float )dnclimate.NightClimate.Temperature / ( TemperatureCount - 1 ),
					  ( float )dnclimate.NightClimate.Humidity / ( HumidityCount - 1 ) );

	static const float dncClimateMixer[ TimeOfDayCount ] = {
		0.0F,		//	THE_WITCHING_HOUR
		0.0F,		//	THE_WEE_HOURS,
		0.1F,		//	DAWN,
		0.45F,		//	MORNING,
		0.8F,		//	NOON,
		1.0F,		//	AFTERNOON,
		0.8F,		//	DUSK,
		0.45F,		//	EVENING,
		0.1F,		//	THE_DEAD_OF_NIGHT
	};

	const float dncT = Clamp( dncClimateMixer[ ( uint32_t )tod ] );
	Vector2 p = dncNight * ( 1.0F - dncT ) + dncDay * ( dncT );

	// Remap temperature to feel more natural
	p.x = Pow( p.x, 0.875F );

	// Remap humidity to feel more natural
	p.y = Pow( p.y, 1.15F );

	float weDists[ WeatherEventCount ];
	for ( uint32_t o = 0; o < WeatherEventCount; ++o )
	{
		WeatherEventOccurrence occ = WeatherEventOccurrences[ o ];

		float rarity = occ.CalculateRarity( a_Geography );

		if ( rarity == 1.0F )
		{
			continue;
		}

		Vector2 q = occ.TempHumid;

		if ( q.x == ALL ) q.x = p.x;
		if ( q.y == ALL ) q.y = p.y;

		q.x += m_Random.Range( -occ.Chaos.x, occ.Chaos.x );
		q.y += m_Random.Range( -0.5F * occ.Chaos.y, occ.Chaos.y );

		weDists[ o ] = 1.0F - ( ( q - p ).Length() * rarity );
	}

	WeatherEvent result = WeatherEvent::FAIR_SKIES;
	float max2 = 0.0F;
	for ( uint32_t o = 0; o < WeatherEventCount; ++o )
	{
		const float & f = weDists[ o ];
		
		if ( f > max2 )
		{
			max2 = f;
			result = ( WeatherEvent )o;
		}
	}

	Temperature rtemp = ( Temperature )( ( uint32_t )( p.x * ( TemperatureCount - 1 ) ) );
	Humidity rhumid = ( Humidity )( ( uint32_t )( p.y * ( HumidityCount - 1 ) ) );

	const WeatherEventInfluence & influence = WeatherEventInfluences[ ( uint32_t )result ];
	rtemp = influence.GetShiftedTemperature( m_Random, rtemp );
	rhumid = influence.GetShiftedHumidity( m_Random, rhumid );

	return WeatherData 
	{ 
		rtemp,
		rhumid,
		result
	};
}