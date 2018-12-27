#pragma once
#ifndef LOCATIONINFO_H
#define LOCATIONINFO_H

#include <GameStructure/World/WorldManager.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
struct LocationInfo final
{
	CString Name;
	CString Desc;
	Array< CString > Hierarchy;
	Vector2 LatitudeLongitude;
};

#endif//LOCATIONINFO_H