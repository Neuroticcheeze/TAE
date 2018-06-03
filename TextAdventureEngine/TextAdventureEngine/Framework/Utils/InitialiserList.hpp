#pragma once
#ifndef INITIALISERLIST_H
#define INITIALISERLIST_H

#include <initializer_list>

//=====================================================================================
template< typename T >
using InitialiserList = std::initializer_list< T >;

//=====================================================================================
template< typename T >
constexpr const T * NBegin( const InitialiserList< T > & a_InitialiserList )
{
	return a_InitialiserList.begin();
}

//=====================================================================================
template< typename T >
constexpr const T * NEnd( const InitialiserList< T > & a_InitialiserList )
{
	return a_InitialiserList.end();
}

//=====================================================================================
template< typename T >
constexpr uint32_t NSize( const InitialiserList< T > & a_InitialiserList )
{
	return a_InitialiserList.size();
}

#endif//INITIALISERLIST_H