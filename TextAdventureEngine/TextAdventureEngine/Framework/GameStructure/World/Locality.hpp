#pragma once
#ifndef LOCALITY_H
#define LOCALITY_H

#include <Framework/Containers/CString.hpp>
#include <Framework/GameStructure/World/Nature/Climate.hpp>

//=====================================================================================
ENUMCLASS( AddressType, int8_t )
	STRUCTURE,	// Mother's Cottage
	DISTRICT,	// Raven Outskirts
	LOCALITY,	// Town of Ravensbrook
	REGION,		// Ravensbrook
	PROVINCE,	// Southern Avalon
END_ENUMCLASS( AddressType, int8_t )

//=====================================================================================
class Locality
{
public:

	Locality( const char * a_LocalityRegionProvince, const Climatograph & a_Climatograph );
	~Locality();

private:

	CString m_Address[ AddressTypeCount - ( uint32_t )AddressType::LOCALITY ];
	Climatograph m_Climate;
};

#endif//LOCALITY_H