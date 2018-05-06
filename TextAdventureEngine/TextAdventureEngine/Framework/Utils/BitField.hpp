#pragma once
#ifndef BITFIELD_H
#define BITFIELD_H

#include <Framework/Template/Serialisable.hpp>
#include <Framework/Containers/DataStream.hpp>
#include <type_traits>

//=====================================================================================
template< typename T, uint32_t CAPACITY >
class BitField final : public Serialisable< BitField< T, CAPACITY > >
{
	static_assert( std::is_integral< T >::value, "BitField<T, CAPACITY> T must be an integral type" );
	static_assert( ( sizeof( T ) << 3 ) >= CAPACITY, "BitField<T, CAPACITY> CAPACITY must be smaller than or equal to the number of bits in the integral datatype T" );

public:
	
	BitField( T a_DefaultValue = 0 )
		: m_Store( a_DefaultValue )
		, m_Capacity( CAPACITY )
	{}

	void Write( DataStream & a_Writer ) const
	{
		a_Writer.Write( m_Store );
	}
	
	void Read( DataStream & a_Reader )
	{
		a_Reader.Read( m_Store );
	}

	void Set( uint32_t a_Index, bool a_Flag )
	{
		if ( a_Index < m_Capacity )
		{
			if ( a_Flag )
			{
				m_Store |= ( static_cast< T >( 1 ) << a_Index );
			}

			else
			{
				m_Store &= ~( static_cast< T >( 1 ) << a_Index );
			}
		}
	}

	bool Get( uint32_t a_Index ) const
	{
		if ( a_Index < m_Capacity )
		{
			return ( m_Store >> a_Index ) & static_cast< T >( 1 );
		}

		return false;
	}

	void Flip( uint32_t a_Index )
	{
		if ( a_Index < m_Capacity )
		{
			m_Store ^= static_cast< T >( 1 ) << a_Index;
		}
	}

	void Flip()
	{
		m_Store = ~m_Store;
	}

	uint32_t Count() const
	{
		uint32_t count = 0;

		for ( uint32_t i = 0; i < m_Capacity; ++i )
		{
			count += ( m_Store >> i ) & static_cast< T >( 1 );
		}

		return count;
	}

private:

	T m_Store;
	uint32_t m_Capacity;
};

//=====================================================================================
template< typename T >
bool FlagsHas( const T & a_Flags, const T & a_Bits )
{
	return ( a_Flags & a_Bits ) != 0;
}

#endif//BITFIELD_H