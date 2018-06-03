#pragma once
#ifndef PRECIPITATIONSUSPENSION_H
#define PRECIPITATIONSUSPENSION_H

//=====================================================================================
struct Precipitation final
{
public:
	
	ENUMCLASS( Type, uint8_t )
		DRIZZLE,
		RAIN,
		SLEET,
		SNOW,
		GRAUPEL,
		HAIL,
	END_ENUMCLASS( Type, uint8_t )

	Type Type;
	Celcius Temperature;
	float Amount;
};

//=====================================================================================
struct Suspension final
{
public:
	
	ENUMCLASS( Type, uint8_t )
		FOG,
		CLOUD,
	END_ENUMCLASS( Type, uint8_t )

	Type Type;
	gM3	 VapourDensity;
	float Amount;
};

#endif//PRECIPITATIONSUSPENSION_H