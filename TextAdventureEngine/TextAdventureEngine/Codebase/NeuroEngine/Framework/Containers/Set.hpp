#pragma once
#ifndef SET_H
#define SET_H

//=====================================================================================
#include <Framework/Utils/MemoryOps.hpp>
#include <Framework/Math/Math.hpp>
#include <Framework/Utils/Function.hpp>
#include <Framework/Utils/Exception.hpp>
#include <Framework/Utils/InitialiserList.hpp>
#include <algorithm>

//=====================================================================================
//!	\brief An Set is used to wrap around dynamic allocations, affording functionality
//!		   to add, remove, find, sort, and change elements.
//!	\tparam T The type of element for this set.
template< typename T >
class Set final
{
public:

	//!	\brief Default Constructor.
	//!		   Construct an empty Set with no data, length, or capacity.
	Set()
		: m_Data( nullptr )
		, m_Length( 0 )
		, m_Capacity( 0 )
	{

	}

	//!	\brief Copy Constructor.
	//!		   Constructs this Set from another.
	//!	\param a_Other The Set to copy.
	//!	\sa Set::operator=
	Set( const Set< T > & a_Other )
		: m_Data( nullptr )
		, m_Length( 0 )
		, m_Capacity( 0 )
	{
		*this = a_Other;
	}

	//!	\brief Allocation Block Constructor.
	//!		   Constructs this set with a block of memory.
	//!		   Any duplicate entries are omitted.
	//!	\param a_Begin The start of the block.
	//!	\param a_End The end of the block.
	Set( const T * a_Begin, const T * a_End )
		: m_Data( nullptr )
		, m_Length( 0 )
		, m_Capacity( 0 )
	{
		if ( a_End <= a_Begin || a_End == nullptr || a_Begin == nullptr )
		{
			return;
		}

		auto it = a_Begin;
		while ( it != a_End )
		{
			Insert( *it );
			++it;
		}
	}

	//!	\brief Initialiser Constructor.
	//!		   Construct an Set from an InisialiserList< T >
	Set( const InitialiserList< T > & a_InitialiserList )
		: Set( NBegin( a_InitialiserList ), NEnd( a_InitialiserList ) )
	{

	}

	//!	\brief Destructor - deletes all elements and clears the capacity and length, before this object is deleted.
	~Set()
	{
		CFree( m_Data );
		m_Length = 0;
		m_Capacity = 0;
	}

	//!	\brief Fill out this set so as to be constructed from the InisialiserList< T >
	//!		   An allocation of the same size as another is made, and all element values are copied across.
	//!	\param a_Other The Set we are cloning into this.
	//!	\return A reference to this Set<T> object
	Set< T > & operator=( const InitialiserList< T > & a_InitialiserList )
	{
		return *this = Set< T >( a_InitialiserList );
	}

	//!	\brief Fill out this set so as to be an exact clone of another, with a separate allocation.
	//!		   An allocation of the same size as another is made, and all element values are copied across.
	//!	\param a_Other The Set we are cloning into this.
	//!	\return A reference to this Set<T> object
	Set< T > & operator=( const Set< T > & a_Other )
	{
		m_Length = 0;
		m_Capacity = 0;

		if ( m_Data )
		{
			CFree( m_Data );
		}

		if ( a_Other.m_Data )
		{
			m_Length = a_Other.m_Length;
			m_Capacity = a_Other.m_Capacity;

			m_Data = CAllocate< T >( m_Capacity );
			
			for ( uint32_t i = 0; i < m_Length; ++i )
			{
				m_Data[ i ] = a_Other.m_Data[ i ];
			}
		}

		return *this;
	}

	//!	\brief Get a pointer to the first element.
	//!	\return Pointer to the first element.
	inline T * First() const
	{
		return m_Data;
	}

	//!	\brief Get a pointer to the last element.
	//!	\return Pointer to the last element.
	T * Last() const
	{
		if ( m_Length == 0 )
		{
			return nullptr;
		}

		return m_Data + ( m_Length - 1 );
	}

	//! \brief Get the pointer that lies just after the domain of our Set.
	//!		   Used for iterating over this Set.
	//!	\return Const pointer to the [N+1]'th element.
	inline const T * End() const
	{
		return m_Data + m_Length;
	}

	//!	\brief Get the number of elements in the set.
	//! \return The count.
	uint32_t Count() const
	{
		return m_Length;
	}

	//!	\brief Cast this set to a raw const pointer - losing information about the set. Equivalent of accessing Set::First
	//! \sa Set::First
	//! \return The count.
	operator const T *() const
	{
		return First();
	}

	//!	\brief Add another element to the end of the set.
	//!	\param a_Entry The value to append.
	void Insert( const T & a_Entry )
	{
		if ( !Contains( a_Entry ) )
		{
			Resize( m_Length + 1 );
			m_Data[ m_Length - 1 ] = a_Entry;
		}
	}

	//! \brief Remove an element given the specified value.
	//! \param a_Entry A value to search for.
	//! \return True if an element with the specified value was found and removed, otherwise false.
	bool Remove( const T & a_Entry )
	{
		for ( uint32_t i = 0; i < m_Length; ++i )
		{
			if ( m_Data[ i ] == a_Entry )
			{
				BCopy( &m_Data[ m_Length - 1 ], &m_Data[ i ], sizeof( T ) );
				Resize( m_Length - 1 );

				return true;
			}
		}

		return false;
	}

	
	//!	\brief Query if the specified entry exists in this set.
	//!	\param a_Entry The entry to check for.
	//! \return Whether or not this set contains a_Entry.
	bool Contains( const T & a_Entry )
	{
		for ( uint32_t i = 0; i < m_Length; ++i )
		{
			if ( m_Data[ i ] == a_Entry )
			{
				return true;
			}
		}

		return false;
	}

	//!	\brief Clear this set.
	//! \remark Length and capacity is set to 0, the current allocation is freed.
	void Clear()
	{
		Resize( 0 );
	}

private:

	void Resize( uint32_t a_NewLength )
	{
		PROFILE;

		if ( a_NewLength == 0 )
		{
			m_Length = 0;
			m_Capacity = 0;
			CFree( m_Data );
			return;
		}

		uint32_t newCapacity = NextPowerOf2( a_NewLength );

		uint32_t oldLength = m_Length;

		m_Length = a_NewLength;
		
		if ( newCapacity != m_Capacity )
		{
			T* temp = m_Data;

			m_Capacity = newCapacity;

			m_Data = CAllocate< T >( m_Capacity );

			for ( uint32_t i = 0; i < Min( m_Length, oldLength ); ++i )
			{
				m_Data[ i ] = temp[ i ];
			}
			
			if ( temp )
			{
				CFree( temp );
			}
		}

		else if ( m_Length < oldLength )
		{
			for ( uint32_t p = m_Length; p < oldLength; ++p )
			{
				delete ( m_Data + p );
			}
		}
	}

	T * m_Data;				//!< Underlying set in memory.
	uint32_t m_Length;		//!< The number of all elements that this set extends to.
	uint32_t m_Capacity;	//!< The number of elements that our current allocation into the heap can hold. If we add/remove enough elements, this allocation will grow/shrink. The capacity is always a power-of-two number.
};

#endif//SET_H