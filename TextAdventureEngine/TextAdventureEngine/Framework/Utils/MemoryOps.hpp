#pragma once
#ifndef MEMORYOPS_H
#define MEMORYOPS_H

#include <memory>
#include <Framework/Template/Singleton.hpp>

//=====================================================================================
inline void BSet( void * a_Target, uint32_t a_Value, uint32_t a_Count )
{
	memset( a_Target, a_Value, a_Count );
}

//=====================================================================================
inline void BCopy( const void * a_From, void * a_To, uint32_t a_Count )
{
	memcpy_s( a_To, a_Count, a_From, a_Count );
}

//=====================================================================================
template< typename T >
inline T * BAllocate( uint32_t a_Count )
{
	return reinterpret_cast< T* >( malloc( a_Count ) );
}

//=====================================================================================
template< typename T >
inline void BFree( T & a_Target )
{
	free( a_Target );
	a_Target = nullptr;
}

//=====================================================================================
inline bool BCompare( const void * a_LHS, const void * a_RHS, uint32_t a_Count )
{
	return memcmp( a_LHS, a_RHS, a_Count ) == 0;
}

//=====================================================================================
template< typename T >
inline T * CAllocate( uint32_t a_Count )
{
	return new T[ a_Count ];
}

//=====================================================================================
template< typename T >
inline void CFree( T & a_Target )
{
	delete[] a_Target;
	a_Target = nullptr;
}

//=====================================================================================
template< typename T >
inline T * Allocate()
{
	return new T;
}

//=====================================================================================
template< typename T >
inline void Free( T *& a_Target )
{
	delete a_Target;
	a_Target = nullptr;
}

#endif//MEMORYOPS_H