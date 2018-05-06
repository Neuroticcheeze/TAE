#pragma once
#ifndef QUEUE_H
#define QUEUE_H

//=====================================================================================
#include <Framework/Utils/MemoryOps.hpp>
#include <Framework/Math/Math.hpp>

//=====================================================================================
template< typename T >
class Queue final
{
public:

	Queue()
		: m_Data( nullptr )
		, m_Length( 0 )
		, m_Capacity( 0 )
	{

	}

	Queue( const Queue< T > & a_Other )
		: m_Data( nullptr )
		, m_Length( 0 )
		, m_Capacity( 0 )
	{
		*this = a_Other;
	}

	~Queue()
	{
		BFree( m_Data );
		m_Length = 0;
		m_Capacity = 0;
	}

	Queue< T > & operator=( const Queue< T > & a_Other )
	{
		m_Length = 0;
		m_Capacity = 0;

		if ( m_Data )
		{
			BFree( m_Data );
		}

		if ( a_Other.m_Data )
		{
			m_Length = a_Other.m_Length;
			m_Capacity = a_Other.m_Capacity;

			m_Data = BAllocate< T >( m_Capacity * sizeof(T) );
			BCopy( a_Other.m_Data, m_Data, m_Length );
		}
	}

	T * Peek() const
	{
		if ( m_Length == 0 )
		{
			return nullptr;
		}

		return m_Data;
	}

	T * Last() const
	{
		if ( m_Length == 0 )
		{
			return nullptr;
		}

		return m_Data + ( m_Length - 1 );
	}

	/* Get the number of elements in the stack. */
	uint32_t Count() const
	{
		return m_Length;
	}

	operator const T *() const
	{
		return Peek();
	}

	void Push( const T & a_Value )
	{
		Resize( m_Length + 1 );
		m_Data[ m_Length - 1 ] = a_Value;
	}

	void Pop()
	{
		if ( m_Length == 0 )
		{
			return;
		}

		if ( m_Length > 1 )
		{
			BCopy( m_Data + 1, m_Data, ( m_Length - 1 ) * sizeof( T ) );
		}

		Resize( m_Length - 1 );
	}

private:

	void Resize( uint32_t a_NewLength )
	{
		if ( a_NewLength == 0 )
		{
			m_Length = 0;
			m_Capacity = 0;
			CFree( m_Data );
			return;
		}

		T* temp = m_Data;
		uint32_t oldBLength = m_Length * sizeof( T );

		m_Length = a_NewLength;
		m_Capacity = NextPowerOf2( m_Length );

		m_Data = CAllocate< T >( m_Capacity * sizeof( T ) );

		BCopy( temp, m_Data, Min( oldBLength, m_Length * static_cast< uint32_t >( sizeof( T ) ) ) );

		if ( temp )
		{
			BFree( temp );
		}
	}

	T * m_Data;
	uint32_t m_Length;
	uint32_t m_Capacity;
};

#endif//QUEUE_H