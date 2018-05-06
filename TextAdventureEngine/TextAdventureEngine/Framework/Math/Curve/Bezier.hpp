#pragma once
#ifndef BEZIER_H
#define BEZIER_H

#include <Framework/Containers/Array.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
class Bezier final
{
public:
	Bezier();
	Bezier( const Array< Vector2 > & a_ControlPointsIn );

	Vector2 & operator[]( uint32_t a_Index );
	const Vector2 & operator[]( uint32_t a_Index ) const;
	const Vector2 & FirstControlPoint() const;
	const Vector2 & LastControlPoint() const;

	void AddControlPoint( const Vector2 & a_NewControlPoint );
	void InsertControlPoint( const Vector2 & a_NewControlPoint, uint32_t a_LeftIndex );
	void RemoveControlPoint( uint32_t a_Index );
	bool RemoveNearestControlPoint( const Vector2 & a_NearTo, float a_MaxSearchRadius );
	uint32_t NumControlPoints() const;

	Vector2 Evaluate( float a_T ) const;
	void Evaluate( Vector2 * a_SegmentPoints, uint32_t a_NumSegments ) const;

	Bezier GetLowerOrder( float a_T ) const;
	Bezier GetHigherOrder( float a_T ) const;

	const Array< Vector2 > GetControlPoints() const;

	static const uint32_t MAX_CONTROL_POINTS;
	
private:
	Array< Vector2 > m_ControlPoints;
};

#endif//BEZIER_H