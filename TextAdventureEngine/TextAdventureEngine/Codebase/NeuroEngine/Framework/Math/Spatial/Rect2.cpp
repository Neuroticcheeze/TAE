#include "Rect2.hpp"

//=====================================================================================
Rect2 Rect2::CreateFromMinMax( const Vector2 & a_Min, const Vector2 & a_Max )
{
	Rect2 rect2;
	rect2.m_Min = Vector2( Min( a_Min.x, a_Max.x ), Min( a_Min.y, a_Max.y ) );
	rect2.m_Max = Vector2( Max( a_Min.x, a_Max.x ), Max( a_Min.y, a_Max.y ) );
	return rect2;
}

//=====================================================================================
Rect2 Rect2::CreateFromPosSize(const Vector2 & a_Position, const Vector2 & a_Size )
{
	return CreateFromMinMax( a_Position, a_Position + a_Size );
}

//=====================================================================================
Rect2::Rect2()
{
}

//=====================================================================================
Rect2::Rect2( const Rect2 & a_Other )
{
	*this = a_Other;
}

//=====================================================================================
Rect2 & Rect2::operator=( const Rect2 & a_Other )
{
	m_Min = a_Other.m_Min;
	m_Max = a_Other.m_Max;

	return *this;
}

//=====================================================================================
Vector2 Rect2::GetMin() const
{
	return m_Min;
}

//=====================================================================================
Vector2 Rect2::GetMax() const
{
	return m_Max;
}

//=====================================================================================
Vector2 Rect2::GetSize() const
{
	return m_Max - m_Min;
}

//=====================================================================================
bool Rect2::operator==(const Rect2 & a_Other) const
{
	return m_Min == a_Other.m_Min && m_Max == a_Other.m_Max;
}

//=====================================================================================
bool Rect2::operator!=(const Rect2 & a_Other) const
{
	return !( *this == a_Other );
}

//=====================================================================================
Rect2 & Rect2::Move( const Vector2 & a_Amount )
{
	m_Min += a_Amount;
	m_Max += a_Amount;

	return *this;
}