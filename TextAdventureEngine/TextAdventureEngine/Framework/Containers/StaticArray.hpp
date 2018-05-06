#pragma once
#ifndef STATICARRAY_H
#define STATICARRAY_H

//=====================================================================================
#include <Framework/Utils/MemoryOps.hpp>
#include <Framework/Math/Math.hpp>
#include <Framework/Utils/Function.hpp>

//=====================================================================================
template< typename T, uint32_t CAPACITY >
class StaticArray final
{
public:

	StaticArray()
		: m_Length( 0 )
	{

	}

	StaticArray( const StaticArray< T, CAPACITY > & a_Other )
		: m_Length( 0 )
	{
		*this = a_Other;
	}

	StaticArray( const T & a_DefaultValue, uint32_t a_Count )
		: m_Length( 0 )
	{
		m_Length = Min( c_Capacity, a_Count );

		for ( uint32_t t = 0; t < a_Count; ++t )
		{
			*( m_Data + t ) = a_DefaultValue;
		}
	}

	~StaticArray()
	{
		m_Length = 0;
	}

	StaticArray< T, CAPACITY > & operator=( const StaticArray< T, CAPACITY > & a_Other )
	{
		m_Length = a_Other.m_Length;
		
		for ( uint32_t i = 0; i < m_Length; ++i )
		{
			m_Data[ i ] = a_Other.m_Data[ i ];
		}

		return *this;
	}

	T & operator[]( uint32_t a_Index )
	{
		return *( m_Data + a_Index );
	}

	inline T * First()
	{
		return m_Data;
	}

	T * Last()
	{
		if ( m_Length == 0 )
		{
			return nullptr;
		}

		return m_Data + ( m_Length - 1 );
	}

	inline const T * First() const
	{
		return m_Data;
	}

	T * const Last() const
	{
		if ( m_Length == 0 )
		{
			return nullptr;
		}

		return m_Data + ( m_Length - 1 );
	}

	inline const T * End() const
	{
		return m_Data + m_Length;
	}

	/* Get the number of elements in the array. */
	uint32_t Count() const
	{
		return m_Length;
	}

	operator const T *() const
	{
		return First();
	}

	void Append( const T & a_Value )
	{
		if ( !ASSERT( m_Length < c_Capacity, "StaticArray<> is at capacity!" ) )
		{
			return;
		}

		m_Data[ m_Length++ ] = a_Value;
	}

	bool RemoveAt( uint32_t a_Index )
	{
		if ( a_Index >= m_Length )
		{
			return false;
		}

		m_Data[ a_Index ] = m_Data[ m_Length - 1 ];
		--m_Length;

		return true;
	}

	bool Remove( const T * a_Element )
	{
		for ( uint32_t i = 0; i < m_Length; ++i )
		{
			if ( & m_Data[ i ] == a_Element )
			{
				m_Data[ i ] = m_Data[ m_Length - 1 ];
				--m_Length;

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
				BCopy( &m_Data[ m_Length - 1 ], &m_Data[ i ], sizeof( T ) );
				--m_Length;
				return true;
			}
		}

		return false;
	}

	template< typename USER >
	T * Find( UserPredicate< USER, const T & > a_Predicate, USER a_UserData )
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

	T * Find( Predicate< const T & > a_Predicate )
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

	template< typename USER >
	const T * Find( UserPredicate< USER, const T & > a_Predicate, USER a_UserData ) const
	{
		const T * curr = First();

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

	const T * Find( Predicate< const T & > a_Predicate ) const
	{
		const T * curr = First();

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

	void Sort( Comparer< const T & > a_Comparer )
	{
		T i, j, first, temp;
		T numLength = m_Length;

		for ( i = numLength - 1; i > 0; --i )
		{
			first = 0;              
			for ( j = 1; j <= i; ++j )
			{
				if ( a_Comparer( m_Data[ first ], m_Data[ j ] ) )
				{
					first = j;
				}
			}
			temp = m_Data[ first ];   
			m_Data[ first ] = m_Data[ i ];
			m_Data[ i ] = temp;
		}
	}

	void Clear()
	{
		for ( uint32_t t = 0; t < m_Length; ++t )
		{
			~( *( m_Data + t ) );
		}

		m_Length = 0;
	}

private:

	T m_Data[ CAPACITY ];
	uint32_t m_Length;
	uint32_t c_Capacity = CAPACITY;
};

#endif//STATICARRAY_H