#pragma once
#ifndef INITIALISERLIST_H
#define INITIALISERLIST_H

#include <initializer_list>

//=====================================================================================
template< typename T >
using InitialiserList = std::initializer_list< T >;

//=====================================================================================
template< typename T >
constexpr const T * Begin( const InitialiserList< T > & a_InitialiserList )
{
	return a_InitialiserList.begin();
}

//=====================================================================================
template< typename T >
constexpr const T * End( const InitialiserList< T > & a_InitialiserList )
{
	return a_InitialiserList.end();
}

//=====================================================================================
template< typename T >
constexpr uint32_t Size( const InitialiserList< T > & a_InitialiserList )
{
	return a_InitialiserList.size();
}

#endif//INITIALISERLIST_H