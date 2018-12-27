#pragma once
#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/Tree.hpp>
#include <Framework/Math/Relationship/Graph.hpp>
#include <GameStructure/World/Location/Location.hpp>
#include <Framework/Data/Xml/XmlObject.hpp>
#include <GameStructure/World/Location/RouteInfo.hpp>

//=====================================================================================
struct LocationInfo;

//=====================================================================================
typedef uint32_t LocationID;

//=====================================================================================
class WorldManager final : public Manager< WorldManager >
{
public:

	void Init();
	void InitPost() override;
	void Finalise();

	Array< RouteInfo > GetAdjacentRoutes( LocationID a_LocationID, RouteFilter::Flags a_Filter = RouteFilter::NO_FILTER, uint32_t a_FilterCompareWeight = 0 ) const;
	Array< RouteInfo > CalculateFastestRoute( LocationID a_LocationID1, LocationID a_LocationID2 );
	LocationID GetCurrentLocation() const;
	LocationInfo GetLocationInfoFromID( LocationID a_LocationID ) const;
	void CloseRoute( LocationID a_LocationID1, LocationID a_LocationID2 );
	void CloseRoute( const RouteInfo & a_Route );
	void OpenRoute( LocationID a_LocationID1, LocationID a_LocationID2 );
	void OpenRoute( const RouteInfo & a_Route );
	bool IsRouteOpen( LocationID a_LocationID1, LocationID a_LocationID2 ) const;
	bool IsRouteOpen( const RouteInfo & a_Route ) const;

private:

	class RouteComparator : public IWeightComparator< RouteComparator >
	{
	public:

		RouteComparator( uint32_t a_Weight = 0 )
			: m_Weight( a_Weight )
		{
		}

		operator bool() const
		{
			return true;
		}

		bool operator>( const RouteComparator & a_Other ) const
		{
			return m_Weight > a_Other.m_Weight;
		}

	private:

		uint32_t m_Weight;
	};

	typedef Tree< Location, uint16_t, 256, LocationTypeCount > LocationHierarchy;
	typedef Graph< uint32_t, RouteComparator, void *, Array< Vector2 > > RouteGraph;

	void LoadLocationData();
	Location ExtractLocationInfo( const XmlObject & a_LocationXml, Array< XmlObject > & a_Children );
	void BuildRecursively( const XmlObject & a_LocationXml, LocationHierarchy::Node * a_Node );

	RouteGraph m_RouteGraph;
	LocationHierarchy m_LocationHierarchy;
};

#endif//WORLDMANAGER_H