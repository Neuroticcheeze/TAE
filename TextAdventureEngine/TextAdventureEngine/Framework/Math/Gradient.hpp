#pragma once
#ifndef GRADIENT_H
#define GRADIENT_H

#include <Framework/Containers/Array.hpp>
#include <Framework/Utils/Tuple.hpp>

//=====================================================================================
template< typename T >
class Gradient final
{
	typedef Tuple2< float, T > KeyFrame;

public:

	void AddKeyFrame( float a_AtT, const T & a_Value )
	{
		m_KeyFrames.Append( KeyFrame( a_AtT, a_Value ) );
	}

	bool RemoveKeyFrame( uint32_t a_Index )
	{
		return m_KeyFrames.RemoveAt( a_Index );
	}

	bool RemoveKeyFrameNearest( float a_T )
	{
		float nearestToT = FLT_MAX;
		int32_t indexNearestToT = -1;

		for ( uint32_t t = 0; t < m_KeyFrames.Count(); ++t )
		{
			float f = Abs( m_KeyFrames[ t ].Value0 - a_T );

			if ( nearestToT > f )
			{
				nearestToT = f;
				indexNearestToT = t;
			}
		}

		if ( indexNearestToT >= 0 && indexNearestToT < m_KeyFrames.Count() )
		{
			return m_KeyFrames.RemoveAt( indexNearestToT );
		}

		return false;
	}

	uint32_t NumKeyFrames() const
	{
		return m_KeyFrames.Count();
	}
	
	T Evaluate( float a_T, float * o_LocalT = nullptr )
	{
		if ( m_KeyFrames.Count() == 0 )
		{
			return T();
		}

		if ( m_KeyFrames.Count() == 1 )
		{
			return ( T )( m_KeyFrames.First()->Value1 );
		}

		if ( ( *m_KeyFrames.First() ).Value0 >= a_T )
		{
			return ( T )( m_KeyFrames.First()->Value1 );
		}

		if ( ( *m_KeyFrames.Last() ).Value0 <= a_T )
		{
			return ( T )( m_KeyFrames.Last()->Value1 );
		}

		uint32_t i = 0;
		for ( uint32_t t = 0; t < m_KeyFrames.Count(); ++t )
		{
			const KeyFrame & kf = m_KeyFrames[ t ];

			if ( kf.Value0 > a_T )
			{
				i = t;
				break;
			}
		}

		if ( i > 0 && i < m_KeyFrames.Count() )
		{
			const KeyFrame & left = m_KeyFrames[ i - 1 ];
			const KeyFrame & right = m_KeyFrames[ i ];
			float k = ( a_T - left.Value0 ) / ( right.Value0 - left.Value0 );

			if ( o_LocalT )
			{
				*o_LocalT = k;
			}

			return ( ( T )left.Value1 ) * ( 1.0F - k ) + ( ( T )right.Value1 ) * k;
		}

		return T();
	}

private:
	Array< KeyFrame > m_KeyFrames;
};

#endif//GRADIENT_H