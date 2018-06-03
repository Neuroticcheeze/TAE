#pragma once
#ifndef WINDSYSTEM_H
#define WINDSYSTEM_H

#include <Framework/Template/System.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Utils/Random.hpp>

//=====================================================================================
struct WindEvent final
{
	Kmps WindSpeed;
	Vector2 WindDirection;
	M AirmassUpwardPush;
};

//=====================================================================================
struct WindObstruction final
{
	Vector2 RelativePosition;
	M Height; // Meters from base of the mountain (or altitude of locality, to the mountain-peak altitude)
};

//=====================================================================================
class WindSystem final : public System< WindSystem >
{
public:

	WindSystem() {}
	
	WindSystem( const InitialiserList< WindObstruction > & a_WindObstructions )
		: WindSystem( ( const Array< WindObstruction > & ) a_WindObstructions )
	{
	}

	WindSystem( const Array< WindObstruction > & a_WindObstructions );

	WindEvent GenerateNextWindEvent();

private:

	Array< WindObstruction > m_WindObstructionSet;
	Vector2 m_CurrentWindDirection;
	Random m_Random;
};

#endif//WINDSYSTEM_H