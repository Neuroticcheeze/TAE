#include "MeteorologyUtils.hpp"
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Math/Math.hpp>

//=====================================================================================
Pa AltitudeToAirPressure( M a_Altitude )
{
	const float p0 = 101325.0F;
	const float T = 2.25577e-5F;
	const float k = 5.25588F;

	float p = p0 * Pow( 1.0F - T * a_Altitude, k );

	return p;
}

//=====================================================================================
Celcius AltitudeToTemperature( M a_SourceAltitude, Celcius a_SourceTemperature, M a_TargetAltitude )
{
	return a_SourceTemperature + ( a_SourceAltitude - a_TargetAltitude ) * 0.0098F;
}

//=====================================================================================
gM3 GetAbsoluteHumidity( Celcius a_Temperature )
{
	float Pa = 1000.0F * ( 0.61121F * Exp( ( 18.678F - a_Temperature * 0.00426439232F ) * ( a_Temperature / ( 257.14F + a_Temperature ) ) ) );
	return 2.16679F * Pa / ( a_Temperature + 273.15F );
}

//=====================================================================================
WpM2 CalculateNetRadiation( nfloat_t a_IncomingSolarFrac, nfloat_t a_CloudCoverFrac, nfloat_t a_LandscapeAlbedo, Celcius a_Temperature, Pa a_VapourPressure )
{
/*=====================================================
	http://www.seao2.info/_TMP/longwave.pdf
			Konzelmann et al. (1994)
**=====================================================*/

	static const WpM2 SOLAR_CONSTANT = 1361.0F;
	static const long double sb = 5.670367e-8;

	Kelvin f = Conv_CelciusToKelvin( a_Temperature );
	nfloat_t n2 = a_CloudCoverFrac * a_CloudCoverFrac;

	long double eCS = 0.23 + 0.433 * PowL( a_VapourPressure / f, 0.125 );
 	long double emissivity = eCS * ( 1.0 - n2 ) + 0.976 * n2;
	long double ilr = emissivity * sb * n2 * n2;

	long double isr = 0.7;

	long double olr = a_LandscapeAlbedo;

	long double netRadiation = 1.0 - a_LandscapeAlbedo * isr + ( ilr - olr );

	return static_cast< WpM2 >( netRadiation * SOLAR_CONSTANT * a_IncomingSolarFrac );
}

//=====================================================================================
#ifdef WINDCHILL_CONSIDER_NETRADIATION
Celcius GetTempFromWindChill( Celcius a_Temperature, Kmph a_WindSpeed, nfloat_t a_RelativeHumidity, WpM2 a_NetRadiation )
#else
Celcius GetTempFromWindChill( Celcius a_Temperature, Kmph a_WindSpeed, nfloat_t a_RelativeHumidity )
#endif
{
	double e = a_RelativeHumidity * 6.105 * ExpD( 17.27 * a_Temperature / ( 237.7 + a_Temperature ) );
	double ws = Conv_KphToMps( a_WindSpeed );
#ifdef WINDCHILL_CONSIDER_NETRADIATION
	double out = a_Temperature + 0.348 * e - 0.7 * ws + 0.7 * ( a_NetRadiation / ( ws + 10.0 ) ) - 4.25;
#else
	double out = a_Temperature + 0.348 * e - 0.7 * ws + 0.7 - 4.0;
#endif
	return static_cast< Celcius >( out );
}

