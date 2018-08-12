#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

namespace Globals
{

//=====================================================================================
#define BF_LETTERS_AND_DIGITS_DefaultBody		WSID( "BF_LETTERS_AND_DIGITS_DefaultBody" )
#define BF_LETTERS_AND_DIGITS_DefaultHeading	WSID( "BF_LETTERS_AND_DIGITS_DefaultHeading" )
#define BF_LETTERS_AND_DIGITS_DefaultTiny		WSID( "BF_LETTERS_AND_DIGITS_DefaultTiny" )

//=====================================================================================
void CreateBitmapFonts();

//=====================================================================================
void ReleaseBitmapFonts();

}

#endif//GLOBALS_H