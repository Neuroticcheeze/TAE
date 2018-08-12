#pragma once
#ifndef RANDOM_H
#define RANDOM_H

//=====================================================================================
struct Vector2;

//=====================================================================================
class Random final
{
public:

	Random();
	Random( const uint64_t & a_Seed );
	~Random();

	Random( const Random & a_Other )
	{
		*this = a_Other;
	}

	Random & operator=( const Random & a_Other );

	int32_t Range( const int32_t & a_Min = 0, const int32_t & a_Max = 9999 ) const;
	float Range( const float & a_Min = 0, const float & a_Max = 9999.0F ) const;
	Vector2 RandomVector2() const;
	Vector2 RandomVector2( float a_MinRadius, float a_MaxRadius ) const;
	inline uint64_t GetSeed() const
	{
		return m_Seed;
	}

private:

	void * m_Generator;
	uint64_t m_Seed;
};

#endif//RANDOM_H