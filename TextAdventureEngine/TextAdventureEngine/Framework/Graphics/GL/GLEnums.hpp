#pragma once
#ifndef GLENUMS_H
#define GLENUMS_H

//=====================================================================================
ENUMCLASS( TextureType, uint8_t )
	TT_1D,
	TT_2D,
	TT_RT,
END_ENUMCLASS( TextureType, uint8_t );

//=====================================================================================
ENUMCLASS( TextureWrapMode, uint32_t )
	REPEAT = 0x2901,
	MIRROR = 0x8370,
	CLAMP = 0x2900
END_ENUM;

//=====================================================================================
ENUMCLASS( TextureFilterMode, uint32_t )
	LINEAR = 0x2601,
	NEAREST = 0x2600,
END_ENUM;

#endif//GLENUMS_H