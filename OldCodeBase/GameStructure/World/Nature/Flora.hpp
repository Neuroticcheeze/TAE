#pragma once
#ifndef FLORA_H
#define FLORA_H

//=====================================================================================
class Flora final
{
public:
	ENUM( Flags, uint8_t )
		NOTHING					= 0,
		BLOSSOMING_TREES		= 1,
		DECIDUOUS_TREES			= 2,
		EVERGREEN_TREES			= 4,
	END_ENUM_F( Flags, uint8_t )
};

#endif//FLORA_H