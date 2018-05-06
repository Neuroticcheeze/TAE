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
	float WindSpeed;
	Vector2 WindDirection;
	float AirmassUpwardPush;
};

//=====================================================================================
struct WindObstruction final
{
	Vector2 RelativePosition;
	float Height; // Meters from base of the mountain (or altitude of locality, to the mountain-peak altitude)
};

//=====================================================================================
class WindSystem final : public System< WindSystem >
{
public:

	WindSystem( const InitialiserList< WindObstruction > & a_WindObstructions );
	void Tick( float a_DeltaTime ) {}
	
	WindEvent GenerateNextWindEvent();

private:

	Array< WindObstruction > m_WindObstructionSet;
	Vector2 m_CurrentWindDirection;
	Random m_Random;
};

#endif//WINDSYSTEM_H