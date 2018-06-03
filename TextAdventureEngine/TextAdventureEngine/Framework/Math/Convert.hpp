#pragma once
#ifndef CONVERT_H
#define CONVERT_H

#include "Math.hpp"


//=====================================================================================
#define TD_UNITF32( NAME ) typedef float NAME

// MISC
TD_UNITF32( nfloat_t );
TD_UNITF32( perc );

// ENERGY
TD_UNITF32( Watts );

// ENERGY - ABSORPTION
TD_UNITF32( WpM2 ); // watts/m^2

// TIME
TD_UNITF32( Milisecond );
TD_UNITF32( Second );
TD_UNITF32( Minute );
TD_UNITF32( Hour );
TD_UNITF32( Day );
TD_UNITF32( Week );
TD_UNITF32( Month );
TD_UNITF32( Year );
TD_UNITF32( Decade );
TD_UNITF32( Century );

// DISTANCE - METRIC
TD_UNITF32( Mm );	// Milimeters
TD_UNITF32( Cm );	// Centimeters
TD_UNITF32( M );	// Meters
TD_UNITF32( Km );	// Kilometers
// AREA - METRIC	
TD_UNITF32( Mm2 );	// Milimeters Squared
TD_UNITF32( Cm2 );	// Centimeters Squared
TD_UNITF32( M2 );	// Meters Squared
TD_UNITF32( Km2 );	// Kilometers Squared
// VOLUME - METRIC	
TD_UNITF32( Mm3 );	// Milimeters Cubed
TD_UNITF32( Cm3 );	// Centimeters Cubed
TD_UNITF32( M3 );	// Meters Cubed
TD_UNITF32( Km3 );	// Kilometers Cubed
// SPEED - METRIC
TD_UNITF32( Mps );	// m/s
TD_UNITF32( Kmps );	// km/s
TD_UNITF32( Kmph );	// km/h
// WEIGHT - METRIC
TD_UNITF32( Gram );
TD_UNITF32( Kg );	// Kilogram
TD_UNITF32( Tonne );
// DENSITY - METRIC
TD_UNITF32( gM3 );	// g/m^3
TD_UNITF32( KgM3 );	// kg/m^3
TD_UNITF32( gCm3 );	// g/cm^3
TD_UNITF32( KgCm3 );// kg/cm^3
// FORCE - METRIC
TD_UNITF32( N );	// Newton
// PRESSURE - METRIC
TD_UNITF32( Pa );	// N/m^2

// DISTANCE - IMPERIAL
TD_UNITF32( In );	// Inches
TD_UNITF32( Ft );	// Feet
TD_UNITF32( Yd );	// Yards
TD_UNITF32( Mi );	// Miles
// AREA - IMPERIAL
TD_UNITF32( In2 );	// Inches Squared
TD_UNITF32( Ft2 );	// Feet Squared
TD_UNITF32( Yd2 );	// Yards Squared
TD_UNITF32( Mi2 );	// Miles Squared
// VOLUME - IMPERIAL
TD_UNITF32( In3 );	// Inches Cubed
TD_UNITF32( Ft3 );	// Feet Cubed
TD_UNITF32( Yd3 );	// Yards Cubed
TD_UNITF32( Mi3 );	// Miles Cubed
// SPEED - IMPERIAL
TD_UNITF32( Ftps );	// feet/sec
TD_UNITF32( Mps );	// miles/sec
TD_UNITF32( Mph );	// miles/hour
// WEIGHT - IMPERIAL
TD_UNITF32( Oz );	// Ounce
TD_UNITF32( Pound );
TD_UNITF32( Stone );
TD_UNITF32( Ton );
// DENSITY/PRESSURE - IMPERIAL
TD_UNITF32( Psi );	// pounds/inch^2

// TEMPERATURE
TD_UNITF32( Celcius );		
TD_UNITF32( Centigrade );
TD_UNITF32( Farenheit );
TD_UNITF32( Kelvin );

#undef TD_UNITF32

//=====================================================================================
//	Misc
#define Conv_NFloatToPerc( X ) ( X * 100.0F )
#define Conv_PercToNFloat( X ) ( X * 0.01F )


//=====================================================================================
//	Angles
#define Conv_RadiansToDegrees( X ) ( X * RAD2DEG )
#define Conv_DegreesToRadians( X ) ( X * DEG2RAD )

//=====================================================================================
//	Memory
#define Conv_KilobyteToByte( X ) ( X * 1024 )
#define Conv_MegabyteToByte( X ) ( X * 1024 * 1024 )
#define Conv_GigabyteToByte( X ) ( X * 1024 * 1024 * 1024 )
#define Conv_TerabyteToByte( X ) ( X * 1024 * 1024 * 1024 * 1024 )
#define Conv_ByteToKilobyte( X ) ( X * 0.0009765625 )
#define Conv_ByteToMegabyte( X ) ( X * 0.0009765625 * 0.0009765625 )
#define Conv_ByteToGigabyte( X ) ( X * 0.0009765625 * 0.0009765625 * 0.0009765625 )
#define Conv_ByteToTerabyte( X ) ( X * 0.0009765625 * 0.0009765625 * 0.0009765625 * 0.0009765625 )

//=====================================================================================
//	Temperature
#define Conv_CelciusToFarenheit( X ) ( 32.0F + X * 1.8F )
#define Conv_FarenheitToCelcius( X ) ( X * 0.5555555F - 32.0F )
#define Conv_CelciusToKelvin( X ) ( 273.15F + X )
#define Conv_KelvinToCelcius( X ) ( X - 273.15F )

//=====================================================================================
//	Distance
#define Conv_MilesToKilometers( X ) ( X * 1.6093440F )
#define Conv_KilometersToMiles( X ) ( X * 0.6213712F )

//=====================================================================================
//	Speed
#define Conv_KphToMps( X ) ( X * 0.2777777F )
#define Conv_MpsToKph( X ) ( X * 3.6F )

#endif//CONVERT_H