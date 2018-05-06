#pragma once
#ifndef VFORMAT_H
#define VFORMAT_H

#include <stdarg.h>

//!	Non-threadsafe way to format a c-string
//!	\tparam CAPACITY How large to make the static buffer.
//!	\param a_Format The formatter string
//! \param ... The arguments
//! \return A formatted c-string
template< uint32_t CAPACITY >
const char * CFormatS( const char * a_Format, ... )
{
	static char buffer[ CAPACITY ];

	va_list argptr;
	va_start( argptr, a_Format );
	vsnprintf( buffer, sizeof( buffer ), a_Format, argptr );
	va_end( argptr );

	return buffer;
}

//!	Non-threadsafe way to format a wide c-string
//!	\tparam CAPACITY How large to make the static buffer.
//!	\param a_Format The formatter string
//! \param ... The arguments
//! \return A formatted wide c-string
template< uint32_t CAPACITY >
const wchar_t * WFormatS( const wchar_t * a_Format, ... )
{
	static wchar_t buffer[ CAPACITY ];

	va_list argptr;
	va_start( argptr, a_Format );
	vsnprintf( buffer, sizeof( buffer ), a_Format, argptr );
	va_end( argptr );

	return buffer;
}

#endif//VFORMAT_H