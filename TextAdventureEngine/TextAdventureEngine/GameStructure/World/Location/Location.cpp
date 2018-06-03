#include "Location.hpp"

//=====================================================================================
Location::Location()
	: m_Type( LocationType::INVALID )
	, m_ID( 0 )
	, m_Name( 0 )
	, m_Description( 0 )
	, m_LatitudeLongitude( Vector2::ZERO )
	, m_VisitorWeight( 0 )
{
}

//=====================================================================================
Location::Location( LocationType a_Type, uint32_t a_ID, uint32_t a_NameStringID, uint32_t a_DescStringID, const Vector2 & a_LatitudeLongitude, uint32_t a_VisitorWeight )
	: m_Type( a_Type )
	, m_ID( a_ID )
	, m_Name( a_NameStringID )
	, m_Description( a_DescStringID )
	, m_LatitudeLongitude( a_LatitudeLongitude )
	, m_VisitorWeight( a_VisitorWeight )
{
}