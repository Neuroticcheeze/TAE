#include "Uuid.hpp"
#include <Framework/Utils/Random.hpp>
#include <Framework/Containers/StaticArray.hpp>

#if PLATFORM_OS == WINDOWS
#pragma comment( lib, "rpcrt4.lib" )
#include <windows.h>
#endif


//=====================================================================================
Uuid GenUuid()
{
	UUID uuid;
	UuidCreate( &uuid );

	Uuid resuuid;
	BCopy( &uuid.Data1, &resuuid.Data1, sizeof( UUID ) );

	return resuuid;
}

//=====================================================================================
uint32_t GenUniqueRND()
{
	static StaticArray< uint32_t, 65536 > sReserved;
	static Random sRandom;
	
	uint32_t r = 0;
	do 
	{
		r = sRandom.Range( 0U, UINT_MAX );
	}
	while ( sReserved.IndexOf( r ) != -1 );

	return r;
}