#pragma once
#ifndef LOCALITY_H
#define LOCALITY_H

#include <Framework/Containers/CString.hpp>
#include <GameStructure/World/Nature/Climate.hpp>

//=====================================================================================
ENUMCLASS( AddressType, int8_t )
	STRUCTURE,
	DISTRICT,
	LOCALITY,
	REGION,	
	PROVINCE,
END_ENUMCLASS( AddressType, int8_t )

//=====================================================================================
class Locality
{
public:

	Locality() {}
	Locality( const char * a_LocalityRegionProvince, const Climatograph & a_Climatograph );
	~Locality();

private:

	CString m_Address[ AddressTypeCount - ( uint32_t )AddressType::LOCALITY ];
	Climatograph m_Climate;
};

#endif//LOCALITY_H