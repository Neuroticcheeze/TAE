#pragma once
#ifndef METEOROLOGYUTILS_H
#define METEOROLOGYUTILS_H

#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Math/Math.hpp>

//=====================================================================================
extern Pa AltitudeToAirPressure( M a_Altitude );

//=====================================================================================
extern Celcius AltitudeToTemperature( M a_SourceAltitude, Celcius a_SourceTemperature, M a_TargetAltitude );

//=====================================================================================
extern gM3 GetAbsoluteHumidity( Celcius a_Temperature );

//=====================================================================================
WpM2 CalculateNetRadiation( nfloat_t a_IncomingSolarFrac, nfloat_t a_CloudCoverFrac, nfloat_t a_LandscapeAlbedo, Celcius a_Temperature, Pa a_VapourPressure );

//=====================================================================================
#ifdef WINDCHILL_CONSIDER_NETRADIATION
extern Celcius GetTempFromWindChill( Celcius a_Temperature, Kmph a_WindSpeed, nfloat_t a_RelativeHumidity, WpM2 a_NetRadiation );
#else
extern Celcius GetTempFromWindChill( Celcius a_Temperature, Kmph a_WindSpeed, nfloat_t a_RelativeHumidity );
#endif

#endif//METEOROLOGYUTILS_H