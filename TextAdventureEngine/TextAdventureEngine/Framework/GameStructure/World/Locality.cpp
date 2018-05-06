#include "Locality.hpp"

//=====================================================================================
Locality::Locality( const char * a_LocalityRegionProvince, const Climatograph & a_Climatograph )
	: m_Climate( a_Climatograph )
{
	const auto & segs = CString( a_LocalityRegionProvince ).Split( "~" );

	switch ( segs.Count() )
	{

	case 0:
	default:
		return;

	case 1:
		m_Address[ 0 ] = segs[ 0 ];
	case 2:
		m_Address[ 1 ] = segs[ 1 ];
	case 3:
		m_Address[ 2 ] = segs[ 2 ];

	}
}

//=====================================================================================
Locality::~Locality()
{
}
