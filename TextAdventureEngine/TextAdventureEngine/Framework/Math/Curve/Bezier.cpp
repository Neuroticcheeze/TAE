#include "Bezier.hpp"

//=====================================================================================
const uint32_t Bezier::MAX_CONTROL_POINTS = 16;

//=====================================================================================
Bezier::Bezier()
{
}

//=====================================================================================
Bezier::Bezier( const Array< Vector2 > & a_ControlPointsIn )
	: m_ControlPoints( a_ControlPointsIn )
{
}

//=====================================================================================
Vector2 & Bezier::operator[]( uint32_t a_Index )
{
	return m_ControlPoints[ a_Index ];
}

//=====================================================================================
const Vector2 & Bezier::operator[]( uint32_t a_Index ) const
{
	return m_ControlPoints[ a_Index ];
}

//=====================================================================================
const Vector2 & Bezier::FirstControlPoint() const
{
	return m_ControlPoints[ 0 ];
}

//=====================================================================================
const Vector2 & Bezier::LastControlPoint() const
{
	return m_ControlPoints[ m_ControlPoints.Count() - 1 ];
}

//=====================================================================================
void Bezier::AddControlPoint( const Vector2 & a_NewControlPoint )
{
	if ( m_ControlPoints.Count() < MAX_CONTROL_POINTS )
	{
		m_ControlPoints.Append( a_NewControlPoint );
	}
}

//=====================================================================================
void Bezier::InsertControlPoint( const Vector2 & a_NewControlPoint, uint32_t a_LeftIndex )
{
	if ( m_ControlPoints.Count() == MAX_CONTROL_POINTS )
	{
		return;
	}

	if ( m_ControlPoints.Count() == 0 || a_LeftIndex >= m_ControlPoints.Count() )
	{
		m_ControlPoints.Append( a_NewControlPoint );
	}

	else if ( a_LeftIndex < m_ControlPoints.Count() )
	{
		// Insert after the last CP
		if ( m_ControlPoints.Count() == a_LeftIndex - 1 )
		{
			m_ControlPoints.Append( a_NewControlPoint );
		}

		// Insert before the first CP
		else if ( a_LeftIndex == 0 )
		{
			Array< Vector2 > temp;
			temp.Append( a_NewControlPoint );

			for ( uint32_t a = 0; a < m_ControlPoints.Count(); ++a )
			{
				temp.Append( m_ControlPoints[ a ] );
			}

			m_ControlPoints = temp;
		}

		// Insert between two CPs
		else
		{
			Array< Vector2 > temp;
			
			for ( uint32_t a = 0; a < a_LeftIndex + 1; ++a )
			{
				temp.Append( m_ControlPoints[ a ] );
			}
			
			temp.Append( a_NewControlPoint );

			for ( uint32_t a = a_LeftIndex + 1; a < m_ControlPoints.Count(); ++a )
			{
				temp.Append( m_ControlPoints[ a ] );
			}

			m_ControlPoints = temp;
		}
	}
}

//=====================================================================================
void Bezier::RemoveControlPoint( uint32_t a_Index )
{
	m_ControlPoints.RemoveAt( a_Index );
}

//=====================================================================================
bool Bezier::RemoveNearestControlPoint( const Vector2 & a_NearTo, float a_MaxSearchRadius )
{
	if ( m_ControlPoints.Count() == 0 )
	{
		return false;
	}

	int32_t indexOfNearest = -1;
	float mindSq = FLT_MAX;

	float maxdSq = a_MaxSearchRadius * a_MaxSearchRadius;
	float distSq = 0.0F;

	for ( uint32_t a = 0; a < m_ControlPoints.Count(); ++a )
	{
		const Vector2 & A = m_ControlPoints[ a ];
		distSq = ( a_NearTo - A ).LengthSqr();

		if ( distSq < maxdSq && distSq < mindSq )
		{
			indexOfNearest = static_cast< int32_t >( a );
			mindSq = distSq;
		}
	}

	if ( indexOfNearest >= 0 && indexOfNearest < static_cast< int32_t >( m_ControlPoints.Count() ) )
	{
		RemoveControlPoint( indexOfNearest );
		return true;
	}

	return false;
}

//=====================================================================================
uint32_t Bezier::NumControlPoints() const
{
	return m_ControlPoints.Count();
}

//=====================================================================================
Vector2 Bezier::Evaluate( float a_T ) const
{
	PROFILE;

	if ( m_ControlPoints.Count() == 0 )
	{
		return Vector2::ZERO;
	}

	if ( m_ControlPoints.Count() == 1 )
	{
		return FirstControlPoint();
	}

	uint32_t stackLength;
	Vector2 stackCPs[ MAX_CONTROL_POINTS ];

	// Fill with root CPs
	stackLength = m_ControlPoints.Count();
	BCopy( m_ControlPoints.First(), stackCPs, sizeof( Vector2 ) * stackLength );

	// Reduce to single linear interpolation
	while ( stackLength > 1 )
	{
		--stackLength;
		for ( uint32_t t = 0; t < stackLength; ++t )
		{
			Vector2 & A = stackCPs[ t + 0 ];
			const Vector2 & B = stackCPs[ t + 1 ];

			A = A * ( 1.0F - a_T ) + B * a_T;
		}
	}

	return stackCPs[ 0 ];
}

//=====================================================================================
void Bezier::Evaluate( Vector2 * a_SegmentPoints, uint32_t a_NumSegments ) const
{
	PROFILE;

	if ( !a_SegmentPoints )
	{
		return;
	}

	if ( m_ControlPoints.Count() == 0 )
	{
		BSet( a_SegmentPoints, 0, sizeof( Vector2 ) * a_NumSegments );
	}

	switch ( a_NumSegments )
	{
	case 0: 
	{
		return;
	}
	case 1:
	{
		a_SegmentPoints[ 0 ] = FirstControlPoint();
		return;
	}
	case 2:
	{
		a_SegmentPoints[ 0 ] = FirstControlPoint();
		a_SegmentPoints[ 1 ] = LastControlPoint();
		return;
	}
	}

	uint32_t stackLength;
	Vector2 stackCPs[ MAX_CONTROL_POINTS ];

	float m = 1.0F / static_cast< float >( a_NumSegments - 1 );
	float h1 = 0.0F;
	float h2 = 1.0F;

	for ( uint32_t q = 0; q < a_NumSegments; ++q )
	{
		if ( q + 1 == a_NumSegments )
		{
			h1 = 1.0F;
			h2 = 0.0F;
		}

		// Fill with root CPs
		stackLength = m_ControlPoints.Count();
		BCopy( m_ControlPoints.First(), stackCPs, sizeof( Vector2 ) * stackLength );

		// Reduce to single linear interpolation
		while ( stackLength > 1 )
		{
			--stackLength;
			for ( uint32_t t = 0; t < stackLength; ++t )
			{
				Vector2 & A = stackCPs[ t + 0 ];
				const Vector2 & B = stackCPs[ t + 1 ];

				A = A * h2 + B * h1;
			}
		}

		a_SegmentPoints[ q ] = stackCPs[ 0 ];

		h1 += m;
		h2 -= m;
	}
}

//=====================================================================================
Bezier Bezier::GetLowerOrder( float a_T ) const
{
	if ( m_ControlPoints.Count() < 2 )
	{
		return Bezier();
	}

	uint32_t stackLength;
	Vector2 stackCPs[ MAX_CONTROL_POINTS ];

	// Fill with root CPs
	stackLength = m_ControlPoints.Count();
	BCopy( m_ControlPoints.First(), stackCPs, sizeof( Vector2 ) * stackLength );
	
	// Reduce to single linear interpolation
	--stackLength;
	for ( uint32_t t = 0; t < stackLength; ++t )
	{
		Vector2 & A = stackCPs[ t + 0 ];
		const Vector2 & B = stackCPs[ t + 1 ];

		A = A * ( 1.0F - a_T ) + B * a_T;
	}

	Bezier result;

	for ( uint32_t j = 0; j < stackLength; ++j )
	{
		result.AddControlPoint( stackCPs[ j ] );
	}

	return result;
}

//=====================================================================================
Bezier Bezier::GetHigherOrder( float a_T ) const
{
	//TODO: implement

	if ( m_ControlPoints.Count() == MAX_CONTROL_POINTS )
	{
		return Bezier();
	}

	return Bezier();
}

//=====================================================================================
const Array< Vector2 > Bezier::GetControlPoints() const
{
	return m_ControlPoints;
}