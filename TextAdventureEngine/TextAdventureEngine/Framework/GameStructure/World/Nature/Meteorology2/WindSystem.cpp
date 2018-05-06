#include "WindSystem.hpp"

//=====================================================================================
WindSystem::WindSystem( const InitialiserList< WindObstruction > & a_WindObstructions )
{
	m_WindObstructionSet = Array< WindObstruction >( Begin( a_WindObstructions ), End( a_WindObstructions ) );
}

//=====================================================================================
WindEvent WindSystem::GenerateNextWindEvent()
{
	m_CurrentWindDirection = ( m_CurrentWindDirection * 0.33F + m_Random.RandomVector2() * 0.67F ).Normalised();
	float windSpeed = Pow( m_Random.Range( 0.0F, 1.0F ), 4.0F ) * 350.0F;

	WindEvent evt;
	evt.WindDirection = m_CurrentWindDirection;
	evt.WindSpeed = windSpeed;

	auto it = m_WindObstructionSet.First();
	auto end = m_WindObstructionSet.End();

	float w = 0.0F;
	float h = 0.0F;

	while ( it != end )
	{
		Vector2 p = it->RelativePosition;
		float h2 = it->Height;

		w = Max( w, m_CurrentWindDirection.Dot( p.Normalised() ) / p.LengthSqr() );
		h = Max( h, h2 );

		++it;
	}

	evt.AirmassUpwardPush = Min( h * 1.1F, w * h * 1000000.0F );

	return evt;
}