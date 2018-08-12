#include "Random.hpp"
#include <random>
#include <chrono>
#include <Framework/Math/Linear/Vector2.hpp>

#define GENERATOR static_cast< std::mt19937_64* >( m_Generator )

//=====================================================================================
Random::Random() : Random( static_cast< uint64_t >( std::chrono::system_clock::now().time_since_epoch().count() ) )
{
}

//=====================================================================================
Random::Random( const uint64_t & a_Seed )
	: m_Seed( a_Seed )
{
	std::random_device rd;
	m_Generator = new std::mt19937_64( rd() );
	GENERATOR->seed( m_Seed );
}

//=====================================================================================
Random::~Random()
{
	if ( m_Generator )
	{
		delete GENERATOR;
		m_Generator = nullptr;
	}
}

//=====================================================================================
Random & Random::operator=( const Random & a_Other )
{
	m_Seed = a_Other.m_Seed;

	if ( m_Generator )
	{
		delete GENERATOR;
		m_Generator = nullptr;
	}

	std::random_device rd;
	m_Generator = new std::mt19937_64( rd() );
	GENERATOR->seed( m_Seed );

	return *this;
}

//=====================================================================================
int32_t Random::Range( const int32_t & a_Min, const int32_t & a_Max ) const
{
	std::uniform_int_distribution< int32_t >  distr( a_Min, a_Max );

	return distr( *GENERATOR );
}

//=====================================================================================
float Random::Range( const float & a_Min, const float & a_Max ) const
{
	std::uniform_real_distribution< float >  distr( a_Min, a_Max );

	return distr( *GENERATOR );
}

//=====================================================================================
Vector2 Random::RandomVector2() const
{
	return Vector2( Range( -1.0F, 1.0F ), Range( -1.0F, 1.0F ) ).Normalised();
}

//=====================================================================================
Vector2 Random::RandomVector2( float a_MinRadius, float a_MaxRadius ) const
{
	return RandomVector2() * Range( a_MinRadius, a_MaxRadius );
}

#undef GENERATOR