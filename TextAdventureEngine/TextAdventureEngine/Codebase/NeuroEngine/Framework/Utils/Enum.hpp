#pragma once
#ifndef ENUM_H
#define ENUM_H

#define ENUM_CONSTANT( VNAME, TYPE, VALUE ) enum : TYPE { VNAME = VALUE }

#define ENUM( NAME, UNDERTYPE ) enum NAME : UNDERTYPE {
#define END_ENUM };

// Adds an enum value count constant
#define END_ENUM_C( NAME, UNDERTYPE ) NAME##_NUM };\
	static const UNDERTYPE NAME##Count = NAME::NAME##_NUM;\

// Used to specify custom enum value count
#define END_ENUM_C2( NAME, UNDERTYPE, COUNT ) NAME##_NUM = COUNT };\
	static const UNDERTYPE NAME##Count = static_cast< UNDERTYPE >( NAME::NAME##_NUM );\

// Used for flag enums
#define END_ENUM_F( NAME, UNDERTYPE ) };\
	inline friend NAME operator|( NAME a, NAME b )\
	{\
		return static_cast< NAME >( static_cast< UNDERTYPE >( a ) | static_cast< UNDERTYPE >( b ) );\
	}\
	inline friend NAME operator&( NAME a, NAME b )\
	{\
		return static_cast< NAME >( static_cast< UNDERTYPE >( a ) & static_cast< UNDERTYPE >( b ) );\
	}\
	inline friend NAME operator^( NAME a, NAME b )\
	{\
		return static_cast< NAME >( static_cast< UNDERTYPE >( a ) ^ static_cast< UNDERTYPE >( b ) );\
	}

#define END_ENUM_F2( NAME, UNDERTYPE, COUNT ) NAME##_NUM = COUNT };\
	static const UNDERTYPE NAME##Count = static_cast< UNDERTYPE >( NAME::NAME##_NUM );\
	inline friend NAME operator|( NAME a, NAME b )\
	{\
		return static_cast< NAME >( static_cast< UNDERTYPE >( a ) | static_cast< UNDERTYPE >( b ) );\
	}\
	inline friend NAME operator&( NAME a, NAME b )\
	{\
		return static_cast< NAME >( static_cast< UNDERTYPE >( a ) & static_cast< UNDERTYPE >( b ) );\
	}\
	inline friend NAME operator^( NAME a, NAME b )\
	{\
		return static_cast< NAME >( static_cast< UNDERTYPE >( a ) ^ static_cast< UNDERTYPE >( b ) );\
	}

#define ENUMCLASS( NAME, UNDERTYPE ) enum class NAME : UNDERTYPE {
#define END_ENUMCLASS( NAME, UNDERTYPE ) NAME##_NUM };\
	static const UNDERTYPE NAME##Count = static_cast< UNDERTYPE >( NAME::NAME##_NUM );

#endif//ENUM_H