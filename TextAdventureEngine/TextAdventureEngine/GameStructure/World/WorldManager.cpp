#include "WorldManager.hpp"
#include <Framework/Utils/Hash.hpp>
#include <Framework/File/FileManager.hpp>
#include <Framework/File/FileStream.hpp>
#include <Framework/Data/Xml/XmlManager.hpp>
#include <Framework/Data/String/StringManager.hpp>
#include <GameStructure/World/Location/LocationInfo.hpp>

//=====================================================================================
void WorldManager::Init()
{
}

//=====================================================================================
void WorldManager::InitPost()
{
	LoadLocationData();
	StringManager::Instance().LoadStringTable( WSID( "WORLD_LOCATIONS" ) );
}

//=====================================================================================
void WorldManager::Finalise()
{
	m_LocationHierarchy.Clear();
	StringManager::Instance().UnLoadStringTable( WSID( "WORLD_LOCATIONS" ) );
}

//=====================================================================================
Array< RouteInfo > WorldManager::GetAdjacentRoutes( LocationID a_LocationID, RouteFilter::Flags a_Filter, uint32_t a_FilterCompareWeight ) const
{
	Array< RouteInfo > routes;

	const bool arriving = a_Filter & RouteFilter::ARRIVING;
	const bool departing = a_Filter & RouteFilter::DEPARTING;
	Array< RouteGraph::Edge > edges;
	m_RouteGraph.GetEdges( a_LocationID, edges, departing, arriving );

	auto it = edges.First();
	const auto end = edges.End();
	while ( it != end )
	{
		const uint32_t fw = it->ForwardWeight;
		const uint32_t bw = it->BackwardWeight;

		if ( ( a_Filter & RouteFilter::ONE_WAY && it->Type == EdgeType::ONE_WAY ) ||
			 ( a_Filter & RouteFilter::TWO_WAY && it->Type == EdgeType::TWO_WAY ) ||
			 ( a_Filter & RouteFilter::WEIGHT_EQ && ( fw == a_FilterCompareWeight || bw == a_FilterCompareWeight ) ) || 
			 ( a_Filter & RouteFilter::WEIGHT_NOT_EQ && ( fw != a_FilterCompareWeight && bw != a_FilterCompareWeight ) ) )
		{
			RouteInfo routeInf;
			routeInf.Spline = it->UserData;
			routeInf.HeadLocation = it->GetHeadNode();
			routeInf.TailLocation = it->GetTailNode();
			routes.Append( routeInf );
		}

		++it;
	}

	return routes;
}

//=====================================================================================
Array< RouteInfo > WorldManager::CalculateFastestRoute( LocationID a_LocationID1, LocationID a_LocationID2 )
{
	return Array< RouteInfo >();
}

//=====================================================================================
LocationID WorldManager::GetCurrentLocation() const
{
	return 0;
}

//=====================================================================================
LocationInfo WorldManager::GetLocationInfoFromID( LocationID a_LocationID ) const
{
	const LocationHierarchy::Node * locNode = m_LocationHierarchy.Find< LocationID >( []( const Location & a_Location, LocationID a_LocationID )
	{
		return a_Location.GetID() == a_LocationID;
	}, a_LocationID );

	LocationInfo out;
	const Location & location = locNode->Value;
	out.Name = StringManager::Instance().GetString( location.GetNameStrID() ).RawString;
	out.Desc = StringManager::Instance().GetString( location.GetDescStrID() ).RawString;
	out.LatitudeLongitude = location.GetLatitudeLongitude();
	
	auto curr = locNode->GetParent();
	while ( curr != nullptr )
	{
		out.Hierarchy.Append( StringManager::Instance().GetString( curr->Value.GetNameStrID() ).RawString );
		curr = curr->GetParent();
	}

	return out;
}

//=====================================================================================
void WorldManager::CloseRoute( LocationID a_LocationID1, LocationID a_LocationID2 )
{
}

//=====================================================================================
void WorldManager::CloseRoute( const RouteInfo & a_Route )
{
	CloseRoute( a_Route.TailLocation, a_Route.HeadLocation );
}

//=====================================================================================
void WorldManager::OpenRoute( LocationID a_LocationID1, LocationID a_LocationID2 )
{
}

//=====================================================================================
void WorldManager::OpenRoute( const RouteInfo & a_Route )
{
	OpenRoute( a_Route.TailLocation, a_Route.HeadLocation );
}

//=====================================================================================
bool WorldManager::IsRouteOpen( LocationID a_LocationID1, LocationID a_LocationID2 ) const
{
	return false;
}

//=====================================================================================
bool WorldManager::IsRouteOpen( const RouteInfo & a_Route ) const
{
	return IsRouteOpen( a_Route.TailLocation, a_Route.HeadLocation );
}

//=====================================================================================
void WorldManager::LoadLocationData()
{
	FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE );
	FileStream xmlfile = FileManager::Instance().Open( "res/world/locations.xml", FileManager::READ );
	XmlObject xmlRootObj;

	if ( xmlfile.IsOpen() )
	{
		xmlRootObj = XmlManager::Instance().ParseXml( xmlfile.ReadText().Get() );
		FileManager::Instance().Close( xmlfile );
	}
	
	m_RouteGraph.Clear();
	m_LocationHierarchy = LocationHierarchy( Location() );// re-initialise the tree with an empty root node
	BuildRecursively( xmlRootObj[ "location" ], nullptr ); // pass nullptr to force usage of Root node

	Array< XmlObject > routesXml;
	xmlRootObj[ "route_set" ].FindChildren( { { XmlObject::Filter::NAME, "route" } }, routesXml );

	auto it = routesXml.First();
	const auto end = routesXml.End();
	while ( it != end )
	{
		CString rtype;

		XmlObject::AttributeType type;
		XmlObject::AttributeValue attrib;
		
		bool valid = true;

		attrib = it->GetAttribute( "type", &type );
		if ( type == XmlObject::AttributeType::STRING )
		{
			rtype = attrib.String;
			rtype.ToUpper();
		}
		else valid = false;

		uint32_t tail, head;
		tail = WSID( ( ( *it )[ "tail" ] )[ 0U ].GetValue().Get() );
		head = WSID( ( ( *it )[ "head" ] )[ 0U ].GetValue().Get() );

		uint32_t fwdWeight = 0, bckWeight = 0;
		CString::Parse( ( ( *it )[ "weight_fwd" ] )[ 0U ].GetValue().Get(), fwdWeight );
		CString::Parse( ( ( *it )[ "weight_bwd" ] )[ 0U ].GetValue().Get(), bckWeight );

		// handle spline info
		CString splineDataString = ( *it )[ "spline" ][ 0U ].GetValue();
		Array< CString > splineControlPoints = splineDataString.Split( "|" );
		Array< Vector2 > splineControlPointsVec2;
		auto splineIt = splineControlPoints.First();
		const auto splineEnd = splineControlPoints.End();
		while ( splineIt != splineEnd )
		{
			*splineIt = splineIt->Trim();

			Vector2 splineCP;
			if ( Vector2::Parse( splineIt->Get(), splineCP ) )
			{
				splineControlPointsVec2.Append( splineCP );
			}

			++splineIt;
		}

		EdgeType edgeType = EdgeType::TWO_WAY;

#define IF_DO_ENUM_ELSE( ENUMVAL ) if ( rtype == #ENUMVAL ) edgeType = EdgeType::ENUMVAL; else

		IF_DO_ENUM_ELSE( ONE_WAY )
		IF_DO_ENUM_ELSE( TWO_WAY ) {}

#undef IF_DO_ENUM_ELSE

		if ( ! m_RouteGraph.HasNode( tail ) || 
			 ! m_RouteGraph.HasNode( head ) )
		{
			valid = false;
		}

		if ( valid )
		{
			m_RouteGraph.AddEdge( edgeType, tail, head, fwdWeight, bckWeight, splineControlPointsVec2 );
		}

		++it;
	}
}

//=====================================================================================
Location WorldManager::ExtractLocationInfo( const XmlObject & a_LocationXml, Array< XmlObject > & a_Children )
{
	if ( a_LocationXml.GetName() != "location" )
	{
		return Location();
	}

	uint32_t id;
	CString name, desc, ltype;

	XmlObject::AttributeType type;
	XmlObject::AttributeValue attrib;
	
	attrib = a_LocationXml.GetAttribute( "type", &type );
	if ( type == XmlObject::AttributeType::STRING )
	{
		ltype = attrib.String;
		ltype.ToUpper();
	}
	
	else
	{
		return Location();
	}

	attrib = a_LocationXml.GetAttribute( "id", &type );
	if ( type == XmlObject::AttributeType::STRING )
	{
		id = WSID( attrib.String.Get() );
	}
	
	else
	{
		return Location();
	}

	attrib = a_LocationXml.GetAttribute( "name", &type );
	if ( type == XmlObject::AttributeType::STRING )
	{
		name = attrib.String;
	}
	
	else
	{
		return Location();
	}

	attrib = a_LocationXml.GetAttribute( "desc", &type );
	if ( type == XmlObject::AttributeType::STRING )
	{
		desc = attrib.String;
	}

	LocationType locationType;

#define IF_DO_ENUM_ELSE( ENUMVAL ) if ( ltype == #ENUMVAL ) locationType = LocationType::ENUMVAL; else

	IF_DO_ENUM_ELSE( WORLD )
	IF_DO_ENUM_ELSE( EXPANSE )
	IF_DO_ENUM_ELSE( CONTINENT )
	IF_DO_ENUM_ELSE( SUBCONTINENT )
	IF_DO_ENUM_ELSE( REGION )
	IF_DO_ENUM_ELSE( SUBREGION )
	IF_DO_ENUM_ELSE( LOCALITY ) {}

#undef IF_DO_ENUM_ELSE

	// handle children
	bool hasChildren = a_LocationXml.FindChildren( { { XmlObject::Filter::NAME, "location" } }, a_Children );
	Vector2 longlat;
	uint32_t weight = 0;

	if ( !hasChildren )
	{
		bool result = false;
		XmlObject xml;
		if ( ( xml = a_LocationXml[ "long_latd" ] ) != XmlObject::NULL_XML_OBJ )
		{
			if ( Vector2::Parse( xml[ 0U ].GetValue().Get(), longlat ) )
			{
				result = true;
			}
		}

		if ( ( xml = a_LocationXml[ "weight" ] ) != XmlObject::NULL_XML_OBJ )
		{
			if ( !CString::Parse( xml[ 0U ].GetValue().Get(), weight ) )
			{
				weight = 0;
			}
		}

		if ( !result )
		{
			return Location();
		}
	}

	return Location( locationType, id, WSID( name.Get() ), desc.Length() == 0 ? 0 : WSID( desc.Get() ), longlat, weight );
}

void WorldManager::BuildRecursively( const XmlObject & a_LocationXml, LocationHierarchy::Node * a_Node )
{
	Array< XmlObject > children;
	Location location = ExtractLocationInfo( a_LocationXml, children );
	
	if ( location.GetType() != LocationType::INVALID )
	{
		if ( a_Node )
		{
			*a_Node += location;
			a_Node = ( *a_Node )[ a_Node->GetChildCount() - 1 ];
		}
		else // null-root = initial call to BuildRecursively
		{
			m_LocationHierarchy.GetRoot()->Value = location;
			a_Node = m_LocationHierarchy.GetRoot();
		}

		if ( children.Count() > 0 )
		{
			auto it = children.First();
			const auto end = children.End();
			while ( it != end )
			{
				BuildRecursively( *it, a_Node );
				++it;
			}
		}

		else
		{
			m_RouteGraph.AddNode( location.GetID() );
		}
	}
}