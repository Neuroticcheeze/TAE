#pragma once
#ifndef LOCATION_H
#define LOCATION_H

#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
ENUMCLASS( LocationType, int8_t )
	INVALID = -1,
	WORLD,
	EXPANSE,
	CONTINENT,
	SUBCONTINENT,
	REGION,
	SUBREGION,
	LOCALITY,
END_ENUMCLASS( LocationType, int8_t )

//=====================================================================================
class Location
{
public:

	Location();
	Location( LocationType a_Type, uint32_t a_ID, uint32_t a_NameStringID, uint32_t a_DescStringID, const Vector2 & a_LatitudeLongitude = Vector2::ZERO, uint32_t a_VisitorWeight = 0 );

	inline uint32_t GetID() const
	{
		return m_ID;
	}

	inline LocationType GetType() const
	{
		return m_Type;
	}

	inline uint32_t GetNameStrID() const
	{
		return m_Name;
	}

	inline uint32_t GetDescStrID() const
	{
		return m_Description;
	}

	inline const Vector2 & GetLatitudeLongitude() const
	{
		return m_LatitudeLongitude;
	}

	inline uint32_t GetVisitorWeight() const
	{
		return m_VisitorWeight;
	}

private:

	LocationType m_Type;
	uint32_t m_Name;
	uint32_t m_Description;
	uint32_t m_ID;
	Vector2 m_LatitudeLongitude;
	uint32_t m_VisitorWeight;
};

#endif//LOCATION_H










//Eta -> Central Eta -> Ardia -> Southern Ardia -> Southernlands -> Bluepond -> Eastern Mikwood Forest -> Orven Swamp