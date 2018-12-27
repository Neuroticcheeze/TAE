#pragma once
#ifndef RECT2_H
#define RECT2_H

#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
struct Rect2 final
{
public:

	static Rect2 CreateFromMinMax( const Vector2 & a_Min, const Vector2 & a_Max );
	static Rect2 CreateFromPosSize(const Vector2 & a_Position, const Vector2 & a_Size );

	Rect2();
	Rect2( const Rect2 & a_Other );

	Rect2 & operator=( const Rect2 & a_Other );

	Vector2 GetMin() const;
	Vector2 GetMax() const;
	Vector2 GetSize() const;

	Rect2 & Move( const Vector2 & a_Amount );
	
	bool operator==(const Rect2 & a_Other) const;
	bool operator!=(const Rect2 & a_Other) const;

private:

	Vector2 m_Min;
	Vector2 m_Max;
};

#endif//RECT2_H