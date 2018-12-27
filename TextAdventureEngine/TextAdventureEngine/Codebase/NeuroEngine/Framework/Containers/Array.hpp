#pragma once
#ifndef ARRAY_H
#define ARRAY_H

//=====================================================================================
#include <Framework/Utils/MemoryOps.hpp>
#include <Framework/Math/Math.hpp>
#include <Framework/Utils/Function.hpp>
#include <Framework/Utils/Exception.hpp>
#include <Framework/Utils/InitialiserList.hpp>
#include <algorithm>

//=====================================================================================
//!	\brief An Array is used to wrap around dynamic allocations, affording functionality
//!		   to add, remove, find, sort, and change elements.
//!	\tparam T The type of element for this array.
template< typename T >
class Array final
{
public:

	//!	\brief Default Constructor.
	//!		   Construct an empty Array with no data, length, or capacity.
	Array()
		: m_Data( nullptr )
		, m_Length( 0 )
		, m_Capacity( 0 )
	{

	}

	//!	\brief Copy Constructor.
	//!		   Constructs this Array from another.
	//!	\param a_Other The Array to copy.
	//!	\sa Array::operator=
	Array( const Array< T > & a_Other )
		: m_Data( nullptr )
		, m_Length( 0 )
		, m_Capacity( 0 )
	{
		*this = a_Other;
	}

	//!	\brief Parameterised Constructor.
	//!		   Constructs this array with a size and fills all elements with the same value.
	//!	\param a_DefaultValue The value that all elements will take on.
	//!	\param a_Count The number of elements we will create, the capacity will be the next highest power-of-two sized allocation.
	Array( const T & a_DefaultValue, uint32_t a_Count )
		: m_Data( nullptr )
		, m_Length( 0 )
		, m_Capacity( 0 )
	{
		m_Length = a_Count;
		m_Capacity = NextPowerOf2( m_Length );

		m_Data = CAllocate< T >( m_Capacity );

		for ( uint32_t t = 0; t < a_Count; ++t )
		{
			*( m_Data + t ) = a_DefaultValue;
		}
	}

	//!	\brief Allocation Block Constructor.
	//!		   Constructs this array with a block of memory.
	//!	\param a_Begin The start of the block.
	//!	\param a_End The end of the block.
	Array( const T * a_Begin, const T * a_End )
		: m_Data( nullptr )
		, m_Length( 0 )
		, m_Capacity( 0 )
	{
		if ( a_End <= a_Begin || a_End == nullptr || a_Begin == nullptr )
		{
			return;
		}

		m_Length = a_End - a_Begin;
		m_Capacity = NextPowerOf2( m_Length );

		m_Data = CAllocate< T >( m_Capacity );
		BCopy( a_Begin, m_Data, sizeof( T ) * m_Length );
	}

	//!	\brief Initialiser Constructor.
	//!		   Construct an Array from an InisialiserList< T >
	Array( const InitialiserList< T > & a_InitialiserList )
		: Array( NBegin( a_InitialiserList ), NEnd( a_InitialiserList ) )
	{

	}

	//!	\brief Destructor - deletes all elements and clears the capacity and length, before this object is deleted.
	~Array()
	{
		CFree( m_Data );
		m_Length = 0;
		m_Capacity = 0;
	}

	//!	\brief Fill out this array so as to be constructed from the InisialiserList< T >
	//!		   An allocation of the same size as another is made, and all element values are copied across.
	//!	\param a_Other The Array we are cloning into this.
	//!	\return A reference to this Array<T> object
	Array< T > & operator=( const InitialiserList< T > & a_InitialiserList )
	{
		return *this = Array< T >( a_InitialiserList );
	}

	//!	\brief Fill out this array so as to be an exact clone of another, with a separate allocation.
	//!		   An allocation of the same size as another is made, and all element values are copied across.
	//!	\param a_Other The Array we are cloning into this.
	//!	\return A reference to this Array<T> object
	Array< T > & operator=( const Array< T > & a_Other )
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

	//! \brief Access an element by index.
	//! \remark If the index is outside this Array's range, an OutOfRangeAccessException is thrown
	//! \param a_Index The index of the element we wish to get.
	//! \return A reference to the element value - from which we can read and/or write to.
	T & operator[]( uint32_t a_Index )
	{
		if ( a_Index >= m_Length )
		{
			throw OutOfRangeAccessException( CFormatS< 4096 >( "a_Index is out of range! Was [%u], Range is [0,%u]", a_Index, m_Length ) );
		}

		return *( m_Data + a_Index );
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

	//! \brief Get the pointer that lies just after the domain of our Array.
	//!		   Used for iterating over this Array.
	//!	\return Const pointer to the [N+1]'th element.
	inline const T * End() const
	{
		return m_Data + m_Length;
	}

	//!	\brief Get the number of elements in the array.
	//! \return The count.
	uint32_t Count() const
	{
		return m_Length;
	}

	//!	\brief Cast this array to a raw const pointer - losing information about the array. Equivalent of accessing Array::First
	//! \sa Array::First
	//! \return The count.
	operator const T *() const
	{
		return First();
	}

	//!	\brief Resize the array.
	//! \param a_NewLength The new length to resize to.
	//! \remark Only if a_NewLength is less than or equal to the previous pow-of-2 value of Capacity
	//!			or greater than the current Capacity will a new allocation be performed.
	//! \remark If a_NewLength is 0, the allocation is deleted and the array is cleared out entirely.
	void Resize( uint32_t a_NewLength, bool a_ExplicitDelete = true )
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

		else if ( m_Length < oldLength && a_ExplicitDelete )
		{
			for ( uint32_t p = m_Length; p < oldLength; ++p )
			{
				delete ( m_Data + p );
			}
		}
	}

	//!	\brief Resize the array; and fill any new spaces with a default value.
	//! \param a_NewLength The new length to resize to.
	//! \param a_DefaultValue THe value to set newly created elements with.
	//! \remark Only if a_NewLength is less than or equal to the previous pow-of-2 value of Capacity
	//!			or greater than the current Capacity will a new allocation be performed.
	//! \remark If a_NewLength is 0, the allocation is deleted and the array is cleared out entirely.
	void Resize( uint32_t a_NewLength, const T & a_DefaultValue )
	{
		uint32_t oldLength = m_Length;

		Resize( a_NewLength );

		for ( uint32_t p = oldLength; p < m_Length; ++p )
		{
			m_Data[ p ] = a_DefaultValue;
		}
	}

	//!	\brief Add another element to the end of the array.
	//!	\param a_Value The value to append.
	void Append( const T & a_Value )
	{
		Resize( m_Length + 1 );
		m_Data[ m_Length - 1 ] = a_Value;
	}

	//!	\brief Get a subarray of this one.
	//!	\param a_StartingIndex The index of the element to set as the sub array's first element.
	//!	\param a_Length How many consecutive elements after a_StartingIndex to copy to the sub array.
	//!	\return The sub array.
	Array< T > SubArray( uint32_t a_StartingIndex, uint32_t a_Length ) const
	{
		return Array< T >( Clamp( m_Data + a_StartingIndex, m_Data, End() ), 
						   Clamp( m_Data + a_StartingIndex + a_Length, m_Data, End() ) );
	}

	//! \brief Append another array at the end of this one.
	//! \param a_Array The array to append.
	void Concat( const Array< T > & a_Array )
	{
		if ( a_Array.m_Length == 0 )
		{
			return;
		}

		uint32_t oldLen;
		Resize( ( oldLen = m_Length ) + a_Array.m_Length );
		BCopy( a_Array.First(), m_Data + oldLen, sizeof( T ) * a_Array.m_Length );
	}

	//! \brief Remove an element at the specified index.
	//! \remark If the index is outside this Array's range, an OutOfRangeAccessException is thrown.
	//! \param a_Index The index of the element we wish to remove.
	void RemoveAt( uint32_t a_Index )
	{
		if ( a_Index >= m_Length )
		{
			throw OutOfRangeAccessException( CFormatS< 4096 >( "a_Index is out of range! Was [%u], Range is [0,%u]", a_Index, m_Length ) );
		}

		m_Data[ a_Index ] = m_Data[ m_Length - 1 ];
		Resize( m_Length - 1, false );
	}

	//! \brief Remove an element given the specified value.
	//! \param a_Element A value to search for.
	//! \return True if an element with the specified value was found and removed, otherwise false.
	bool Remove( const T * a_Element )
	{
		for ( uint32_t i = 0; i < m_Length; ++i )
		{
			if ( & m_Data[ i ] == a_Element )
			{
				m_Data[ i ] = m_Data[ m_Length - 1 ];
				Resize( m_Length - 1, false );

				return true;
			}
		}

		return false;
	}

	//! \brief Remove an element given the specified value.
	//!	\tparam U The value to compare an element in the array with.
	//! \param a_Value The value to compare an element in the array with.
	//! \return True if an element with the specified value was found and removed, otherwise false.
	template< typename U >
	bool Remove( U a_Value )
	{
		for ( uint32_t i = 0; i < m_Length; ++i )
		{
			if ( m_Data[ i ] == a_Value)
			{
				m_Data[ i ] = m_Data[ m_Length - 1 ];
				Resize( m_Length - 1, false );

				return true;
			}
		}

		return false;
	}

	//!	\brief Find an element given a specified predicate.
	//!	\tparam USER Data to pass along to the predicate to be used in evaluation.
	//! \param a_Predicate The predicate function which takes in any one element in the array, along with specified user data.
	//! \param a_UserData Data to be used in the evaluation. This affords the user flexibility.
	//! \return Returns the element that the predicate successfully evaluated.
	template< typename USER >
	T * Find( UserPredicate< USER, const T & > a_Predicate, USER a_UserData ) const
	{
		T * curr = First();

		while ( curr != End() )
		{
			if ( a_Predicate( a_UserData, *curr ) )
			{
				return curr;
			}

			++curr;
		}

		return nullptr;
	}

	//!	\brief Find an element given a specified predicate.
	//! \param a_Predicate The predicate function which takes in any one element in the array.
	//! \return Returns the element that the predicate successfully evaluated.
	T * Find( Predicate< const T & > a_Predicate ) const
	{
		T * curr = First();

		while ( curr != End() )
		{
			if ( a_Predicate( *curr ) )
			{
				return curr;
			}

			++curr;
		}

		return nullptr;
	}

	//! \brief Fetch the index of the first element in the array encountered that is equal to a specified value.
	//! \param a_Value The value to look for.
	//!	\return The index of the first element found.
	int32_t IndexOf( const T & a_Value ) const
	{
		const T * curr = First();

		while ( curr != End() )
		{
			if ( *curr == a_Value )
			{
				return static_cast< int32_t >( curr - First() );
			}

			++curr;
		}

		return -1;
	}

	//! \brief Fetch the index of the first element in the array encountered that is equal to a specified value.
	//! \param a_Value The value to look for.
	//!	\return The index of the first element found.
	template< typename U >
	int32_t IndexOf( const U & a_Value ) const
	{
		T * curr = First();

		while ( curr != End() )
		{
			if ( *curr == a_Value )
			{
				return static_cast< int32_t >( curr - First() );
			}

			++curr;
		}

		return -1;
	}

	//! \brief Sorts the array using Bubble-Sort Algorithm given a comparor.
	//!	\param a_Comparer Defines the behaviour of this sort.
	void Sort( Comparer< const T & > a_Comparer )
	{
		std::sort( m_Data, m_Data + m_Length, a_Comparer );
	}

	//!	\brief Clear this array.
	//! \remark Length and capacity is set to 0, the current allocation is freed.
	void Clear()
	{
		Resize( 0 );
	}

private:

	T * m_Data;				//!< Underlying array in memory.
	uint32_t m_Length;		//!< The number of all elements that this array extends to.
	uint32_t m_Capacity;	//!< The number of elements that our current allocation into the heap can hold. If we add/remove enough elements, this allocation will grow/shrink. The capacity is always a power-of-two number.
};

#endif//ARRAY_H