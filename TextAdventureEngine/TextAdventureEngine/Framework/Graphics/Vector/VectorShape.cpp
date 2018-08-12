#include "VectorShape.hpp"

//=====================================================================================
VectorShape::VectorShape( const Colour & a_Colour, const float * a_Transform )
	: m_Colour( a_Colour )
{
	if ( a_Transform )
	{
		BCopy( a_Transform, m_Transform, sizeof( float ) * 9 );
	}
}