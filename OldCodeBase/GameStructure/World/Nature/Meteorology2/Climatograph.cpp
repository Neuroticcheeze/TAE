#include "Climatograph.hpp"
#include <Framework/Math/Math.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
Climatograph::Climatograph( const Celcius * a_YearlyDayTemps,
							const Celcius * a_YearlyNightTemps,
							uint32_t a_Entries )
{
	for ( uint32_t k = 0; k < a_Entries; ++k )
	{
		const float t = ( a_Entries == 1 ) ? 0.0F : ( static_cast< Celcius >( k ) / ( a_Entries - 1 ) );
		m_Entries.AddKeyFrame( t, Vector2( a_YearlyDayTemps[ k ], a_YearlyNightTemps[ k ] ) );
	}
}

//=====================================================================================
Climatograph::Climatograph( const InitialiserList< Celcius > & a_YearlyDayTemps,
							const InitialiserList< Celcius > & a_YearlyNightTemps )
	: Climatograph( NBegin( a_YearlyDayTemps ), 
					NBegin( a_YearlyNightTemps ),
					Min( NSize( a_YearlyDayTemps ), NSize( a_YearlyNightTemps ) ) )
{
}

//=====================================================================================
float Climatograph::Evaluate( uint32_t a_DayOfYear, TimeOfDay a_TimeOfDay )
{
	const uint32_t N = TimeFlowManager::DaysPerYear;
	int32_t doy = Min( a_DayOfYear, N );
	float a;
	Vector2 entries[ 3 ];

	{
		a = Clamp( ( float )doy / N );
		entries[ 1 ] = m_Entries.Evaluate( a );

		a = Clamp( ( float )Wrap( doy - 1, 0, ( int32_t )N ) / N );
		entries[ 0 ] = m_Entries.Evaluate( a );

		a = Clamp( ( float )Wrap( doy + 1, 0, ( int32_t )N ) / N );
		entries[ 2 ] = m_Entries.Evaluate( a );
	}

	static const float adjacentDayMixer[ TimeOfDayCount ] = {
		0.0F,		//	THE_WITCHING_HOUR
		0.1F,		//	THE_WEE_HOURS,
		0.4F,		//	DAWN,
		0.9F,		//	MORNING,
		1.0F,		//	NOON,
		1.0F,		//	AFTERNOON,
		1.1F,		//	DUSK,
		1.4F,		//	EVENING,
		1.9F,		//	THE_DEAD_OF_NIGHT
	};

	static const float tempMixer[ TimeOfDayCount ] = {
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

	const float adm = adjacentDayMixer[ ( uint32_t )a_TimeOfDay ];
	const float tm = tempMixer[ ( uint32_t )a_TimeOfDay ];
	

	float day = Lerp( entries[ 0 ].x, entries[ 1 ].x, entries[ 2 ].x, adm );
	float night = Lerp( entries[ 0 ].y, entries[ 1 ].y, entries[ 2 ].y, adm );

	return Lerp( night, day, tm );
}