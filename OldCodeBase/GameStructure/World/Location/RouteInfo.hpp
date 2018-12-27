#pragma once
#ifndef ROUTEINFO_H
#define ROUTEINFO_H

#include <GameStructure/World/WorldManager.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
typedef uint32_t LocationID;

//=====================================================================================
class RouteFilter final
{
public:

	ENUM( Flags, uint8_t )
		DEPARTING		= 1 << 0,
		ARRIVING		= 1 << 1,
		WEIGHT_EQ		= 1 << 2,
		WEIGHT_NOT_EQ	= 1 << 3,
		ONE_WAY			= 1 << 6,
		TWO_WAY			= 1 << 7,
		NO_FILTER		= DEPARTING | ARRIVING | WEIGHT_EQ | WEIGHT_NOT_EQ | ONE_WAY | TWO_WAY,
	END_ENUM_F( Flags, uint8_t )
};

//=====================================================================================
struct RouteInfo final
{
	LocationID HeadLocation;
	LocationID TailLocation;
	Array< Vector2 > Spline;
};

#endif//ROUTEINFO_H