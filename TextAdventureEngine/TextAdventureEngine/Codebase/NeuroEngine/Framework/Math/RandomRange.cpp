#include "RandomRange.hpp"

//=====================================================================================
Random RandomRange::s_Random;

//=====================================================================================
RandomRange::RandomRange( float a_Min, float a_Max )
	: m_Min( a_Min )
	, m_Max( a_Max )
{
}

//=====================================================================================
float RandomRange::GetValue() const
{
	return s_Random.Range( m_Min, m_Max );
}