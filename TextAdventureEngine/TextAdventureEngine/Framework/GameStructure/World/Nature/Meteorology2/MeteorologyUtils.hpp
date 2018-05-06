#pragma once

#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Math/Math.hpp>

class Hygrology final
{
public:

	typedef float Pa;
	typedef float Meters;
	typedef float Celcius;
	typedef float gM3;

	static Pa AltitudeToAirPressure( Meters a_Altitude )
	{
		const float p0 = 101325.0F;
		const float T = 2.25577e-5F;
		const float k = 5.25588F;

		float p = p0 * Pow( 1.0F - T * a_Altitude, k );

		return p;
	}
	static Celcius AltitudeToTemperature( Meters a_SourceAltitude, Celcius a_SourceTemperature, Meters a_TargetAltitude )
	{
		return a_SourceTemperature + ( a_SourceAltitude - a_TargetAltitude ) * 0.0098F;
	}
	static gM3 GetAbsoluteHumidity( Celcius a_Temperature )
	{
		float Pa = 1000.0F * ( 0.61121F * Exp( ( 18.678F - a_Temperature * 0.00426439232F ) * ( a_Temperature / ( 257.14F + a_Temperature ) ) ) );
		return 2.16679F * Pa / ( a_Temperature + 273.15 );
	}
	static float GetRelativeHumidity( Celcius a_Temperature, float a_WaterToLandRatio )
	{
		gM3 ah = GetAbsoluteHumidity( a_Temperature );

		return Clamp( ( a_Temperature - 10.0F ) * ( 1.0F / 40.0F ) ) * a_WaterToLandRatio;
	}
};

class Anemology final
{
public:

	struct Wind
	{
		Vector2 Direction;
		float Strength;
	};

	struct Mountain
	{
		Vector2 RelativeLocation;
		float Size;
	};

	static float GetWindwardness( InitialiserList< Mountain > a_MountainSet, Wind a_Wind );
};