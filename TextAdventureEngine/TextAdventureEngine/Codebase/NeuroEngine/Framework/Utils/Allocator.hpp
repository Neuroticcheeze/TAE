#pragma once
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <Framework/Utils/TypeTraits.hpp>
#include <Framework/Containers/Array.hpp>
#include <type_traits>

//=====================================================================================
template< typename T, T CAPACITY, T WORDSIZE >
class Allocator final
{
	STATIC_ASSERT( IS_INTEGRAL( T ), "T must be integral" );

public:

	typedef T Pointer;

	struct Block
	{
		Pointer Ptr;
		T NumWords;
	};

	const T Capacity = CAPACITY;
	const T WordSize = WORDSIZE;
	
	Pointer Allocate( const T & a_NumWords )
	{
		return 0;
	}

	void DeAllocate( const Pointer & a_Pointer )
	{

	}

	// Defragment the code into a continuous block for better allocation performance.
	// A callback can be specified which will be fired whenever a block of memory is copied somewhere.
	template< typename USER >
	void DeFragment( Action< void, USER, const Block &, const Pointer & > a_Predicate, USER a_UserData )
	{
		
	}

private:
	
	void Sort();

	Array< Block > m_Blocks;
};

#endif//ALLOCATOR_H