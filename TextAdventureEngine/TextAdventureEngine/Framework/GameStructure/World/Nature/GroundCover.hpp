#pragma once
#ifndef GROUNDCOVER_H
#define GROUNDCOVER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Array.hpp>
#include "Climate.hpp"
#include "TimeFlow.hpp"
#include "Flora.hpp"

//=====================================================================================
class GroundCover final
{
public:
	ENUM( Flags, uint16_t )
		NOTHING					= 0,
		FLOWER_PETALS			= 1,
		AUTUMN_LEAVES			= 2,
		DEW						= 4,
		FROST					= 8,
		MUD_AND_PUDDLES			= 16,
		SLUSHY_SNOW_AND_RAIN	= 32,
		LIGHT_SNOW_DUSTING		= 64,
		PATCHES_OF_SNOW			= 128,
		THICK_DEEP_SNOW			= 256,
		ICE_PELLETS				= 512,

		__SUNLIGHT				= 1024,
		__WIND					= 2048,
	END_ENUM_F2( Flags, uint16_t, 11 )
};
extern const char * const GroundCoverDisplayNames[];

//=====================================================================================
class GroundCoverManager final : public Manager< GroundCoverManager >
{
private:

	struct GroundCoverMixture final
	{
		GroundCoverMixture() {}
		GroundCoverMixture( GroundCover::Flags a_Left, GroundCover::Flags a_Right, GroundCover::Flags a_Result, bool a_IndirectMixing = true )
			: Left( a_Left )
			, Right( a_Right )
			, Result( a_Result )
			, Indirect( a_IndirectMixing )
		{
		}
	
		GroundCover::Flags Left;
		GroundCover::Flags Right;
		GroundCover::Flags Result;
		bool Indirect;
	};

	static const GroundCoverMixture GroundCoverMixtures[];

public:

	void Init();
	void Finalise();

	GroundCover::Flags GetGroundCover( GroundCover::Flags a_CurrentGroundCover, const WeatherData & a_LatestWeatherData, Flora::Flags a_Flora ) const;
	GroundCover::Flags MixGroundCover( GroundCover::Flags a_Destination, GroundCover::Flags a_Addition ) const;
	Array< GroundCover::Flags > SeparateGroundCoverFlag( GroundCover::Flags a_Value ) const;

private:
};

#endif//GROUNDCOVER_H