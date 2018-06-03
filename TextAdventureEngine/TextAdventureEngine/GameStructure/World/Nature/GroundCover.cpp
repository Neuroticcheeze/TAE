#include "GroundCover.hpp"

//=====================================================================================
const char * const GroundCoverDisplayNames[] = {
	"N/A", // NOTHING
	"Flower Blossoms", // FLOWER_PETALS
	"Autumn Leaves", // AUTUMN_LEAVES
	"Dew", // DEW
	"Frost", // FROST
	"Muddy Puddles", // MUD_AND_PUDDLES
	"Slushy Snow and Ice", // SLUSHY_SNOW_AND_RAIN
	"Feint Snowcover", // LIGHT_SNOW_DUSTING
	"Moderate Snowcover", // PATCHES_OF_SNOW
	"Thick Snow", // THICK_DEEP_SNOW
	"Hail", // ICE_PELLETS
};

//=====================================================================================
const GroundCoverManager::GroundCoverMixture GroundCoverManager::GroundCoverMixtures[] =
{
	GroundCoverMixture( GroundCover::PATCHES_OF_SNOW, GroundCover::LIGHT_SNOW_DUSTING, GroundCover::PATCHES_OF_SNOW ),
	GroundCoverMixture( GroundCover::PATCHES_OF_SNOW, GroundCover::PATCHES_OF_SNOW, GroundCover::THICK_DEEP_SNOW ),
	GroundCoverMixture( GroundCover::THICK_DEEP_SNOW, GroundCover::LIGHT_SNOW_DUSTING, GroundCover::THICK_DEEP_SNOW ),
	GroundCoverMixture( GroundCover::SLUSHY_SNOW_AND_RAIN, GroundCover::LIGHT_SNOW_DUSTING, GroundCover::SLUSHY_SNOW_AND_RAIN ),
	GroundCoverMixture( GroundCover::MUD_AND_PUDDLES, GroundCover::LIGHT_SNOW_DUSTING, GroundCover::MUD_AND_PUDDLES ),
	GroundCoverMixture( GroundCover::MUD_AND_PUDDLES, GroundCover::PATCHES_OF_SNOW, GroundCover::SLUSHY_SNOW_AND_RAIN ),
	GroundCoverMixture( GroundCover::MUD_AND_PUDDLES, GroundCover::THICK_DEEP_SNOW, GroundCover::THICK_DEEP_SNOW ),
	GroundCoverMixture( GroundCover::ICE_PELLETS, GroundCover::MUD_AND_PUDDLES, GroundCover::MUD_AND_PUDDLES ),
	GroundCoverMixture( GroundCover::SLUSHY_SNOW_AND_RAIN, GroundCover::MUD_AND_PUDDLES, GroundCover::MUD_AND_PUDDLES ),
	GroundCoverMixture( GroundCover::DEW, GroundCover::__SUNLIGHT, GroundCover::NOTHING, false ),
	GroundCoverMixture( GroundCover::FROST, GroundCover::__SUNLIGHT, GroundCover::NOTHING, false ),
	GroundCoverMixture( GroundCover::MUD_AND_PUDDLES, GroundCover::__SUNLIGHT, GroundCover::NOTHING, false ),
	GroundCoverMixture( GroundCover::LIGHT_SNOW_DUSTING, GroundCover::__SUNLIGHT, GroundCover::NOTHING, false ),
	GroundCoverMixture( GroundCover::ICE_PELLETS, GroundCover::__SUNLIGHT, GroundCover::MUD_AND_PUDDLES, false ),
	GroundCoverMixture( GroundCover::THICK_DEEP_SNOW, GroundCover::__SUNLIGHT, GroundCover::PATCHES_OF_SNOW, false ),
	GroundCoverMixture( GroundCover::PATCHES_OF_SNOW, GroundCover::__SUNLIGHT, GroundCover::SLUSHY_SNOW_AND_RAIN, false ),
	GroundCoverMixture( GroundCover::SLUSHY_SNOW_AND_RAIN, GroundCover::__SUNLIGHT, GroundCover::MUD_AND_PUDDLES, false ),
	GroundCoverMixture( GroundCover::FLOWER_PETALS, GroundCover::__WIND, GroundCover::NOTHING, false ),
	GroundCoverMixture( GroundCover::AUTUMN_LEAVES, GroundCover::__WIND, GroundCover::NOTHING, false ),
};

//=====================================================================================
void GroundCoverManager::Init()
{
}

//=====================================================================================
void GroundCoverManager::Finalise()
{
}

//=====================================================================================
GroundCover::Flags GroundCoverManager::GetGroundCover( GroundCover::Flags a_CurrentGroundCover, const WeatherData & a_LatestWeatherData, Flora::Flags a_Flora ) const
{
	auto time = TimeFlowManager::Instance().GetTime();
	auto season = TimeFlowManager::DayInYearToSeason( time.GetDay() );

	bool hasDeciduous = ( a_Flora & Flora::DECIDUOUS_TREES == Flora::DECIDUOUS_TREES );
	if ( season == Season::AUTUMN && hasDeciduous ) a_CurrentGroundCover = a_CurrentGroundCover | GroundCover::AUTUMN_LEAVES;
	
	bool hasBlossoms = ( a_Flora & Flora::BLOSSOMING_TREES == Flora::BLOSSOMING_TREES );
	if ( season == Season::SPRING && hasBlossoms ) a_CurrentGroundCover = a_CurrentGroundCover | GroundCover::FLOWER_PETALS;

	// Check for cover from current weather
	GroundCover::Flags overlay = GroundCover::NOTHING;
	switch ( a_LatestWeatherData.WeatherEvent )
	{
	case WeatherEvent::FAIR_SKIES:		overlay = GroundCover::__SUNLIGHT; break;
	case WeatherEvent::OVERCAST:		overlay = GroundCover::__SUNLIGHT; break;
	case WeatherEvent::WINDY_STORM:		overlay = GroundCover::__WIND; break;
	case WeatherEvent::DOWNPOUR:		overlay = GroundCover::MUD_AND_PUDDLES; break;
	case WeatherEvent::DRIZZLE:			overlay = GroundCover::MUD_AND_PUDDLES; break;
	case WeatherEvent::CLEAR_SHOWER:	overlay = GroundCover::MUD_AND_PUDDLES; break;
	case WeatherEvent::HURRICANE:		overlay = GroundCover::MUD_AND_PUDDLES | GroundCover::__WIND; break;
	case WeatherEvent::HAIL:			overlay = GroundCover::ICE_PELLETS; break;
	case WeatherEvent::SLEET:			overlay = GroundCover::SLUSHY_SNOW_AND_RAIN; break;
	case WeatherEvent::LIGHT_FLURRY:	overlay = GroundCover::LIGHT_SNOW_DUSTING | GroundCover::__WIND; break;
	case WeatherEvent::SNOW:			overlay = GroundCover::PATCHES_OF_SNOW; break;
	case WeatherEvent::BLIZZARD:		overlay = GroundCover::THICK_DEEP_SNOW | GroundCover::__WIND; break;
	}

	a_CurrentGroundCover = MixGroundCover( a_CurrentGroundCover, overlay );

	// Check for frost
	if ( ( a_CurrentGroundCover & ( 
		GroundCover::LIGHT_SNOW_DUSTING | 
		GroundCover::PATCHES_OF_SNOW | 
		GroundCover::SLUSHY_SNOW_AND_RAIN | 
		GroundCover::THICK_DEEP_SNOW |
		GroundCover::MUD_AND_PUDDLES ) ) == 0 )
	{
		if ( a_LatestWeatherData.Humidity > Humidity::DRY && a_LatestWeatherData.Temperature <= Temperature::CHILLY )
		{
			a_CurrentGroundCover = a_CurrentGroundCover | GroundCover::FROST;
		}
	}

	// Check for dew
	if ( ( a_CurrentGroundCover & ( 
		GroundCover::LIGHT_SNOW_DUSTING | 
		GroundCover::PATCHES_OF_SNOW | 
		GroundCover::SLUSHY_SNOW_AND_RAIN | 
		GroundCover::THICK_DEEP_SNOW |
		GroundCover::FROST |
		GroundCover::MUD_AND_PUDDLES ) ) == 0 )
	{
		if ( a_LatestWeatherData.Humidity >= Humidity::PLEASANT && InRange( a_LatestWeatherData.Temperature, Temperature::COOL, Temperature::WARM ) )
		{
			a_CurrentGroundCover = a_CurrentGroundCover | GroundCover::DEW;
		}
	}

	return a_CurrentGroundCover;
}

//=====================================================================================
GroundCover::Flags GroundCoverManager::MixGroundCover( GroundCover::Flags a_Destination, GroundCover::Flags a_Addition ) const
{
	uint32_t num = sizeof( GroundCoverMixtures ) / sizeof( GroundCoverMixture );
	
	for ( uint32_t p = 0; p < num; ++p )
	{
		GroundCoverMixture mix = GroundCoverMixtures[ p ];
		if ( ( ( mix.Left & a_Destination ) == mix.Left && ( mix.Right & a_Addition ) == mix.Right ) ||
			 ( mix.Indirect ? ( ( mix.Left & a_Addition ) == mix.Left && ( mix.Right & a_Destination ) == mix.Right ) : false ) )
		{
			a_Destination = a_Destination ^ ( mix.Left | mix.Right );
			a_Destination = a_Destination | mix.Result;
			return a_Destination;
		}
	}

	return a_Destination | a_Addition;
}

//=====================================================================================
Array< GroundCover::Flags > GroundCoverManager::SeparateGroundCoverFlag( GroundCover::Flags a_Value ) const
{
	Array< GroundCover::Flags > covers;

	for ( uint32_t k = 1; k < GroundCover::FlagsCount; ++k )
	{
		GroundCover::Flags a = a_Value & ( GroundCover::Flags )( 1 << ( k - 1 ) );

		if ( a != GroundCover::NOTHING )
		{
			covers.Append( a );
		}
	}

	return covers;
}