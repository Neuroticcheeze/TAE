#pragma once
#ifndef STATICDICTIONARY_H
#define STATICDICTIONARY_H

#include <Framework/Utils/MemoryOps.hpp>

//=====================================================================================
template< typename K, typename V , uint32_t CAPACITY >
class StaticDictionary final
{
public:

	StaticDictionary()
	{

	}

	StaticDictionary( const StaticDictionary< K, V, CAPACITY > & a_Other )
	{
		*this = a_Other;
	}

	StaticDictionary< K, V, CAPACITY > & operator=( const StaticDictionary< K, V, CAPACITY > & a_Other )
	{
		m_Count = a_Other.m_Count;
		BCopy( a_Other.m_Keys, m_Keys, m_Count );
		BCopy( a_Other.m_Values, m_Values, m_Count );
	}

	const V * operator[]( K a_Key ) const
	{
		for ( uint32_t t = 0; t < m_Count; ++t )
		{
			if ( m_Keys[ t ] == a_Key )
			{
				return m_Values + t;
			}
		}

		return nullptr;
	}

	const K * operator[]( V a_Value ) const
	{
		for ( uint32_t t = 0; t < m_Count; ++t )
		{
			if ( m_Values[ t ] == a_Value )
			{
				return m_Keys + t;
			}
		}

		return nullptr;
	}

	V * operator[]( K a_Key )
	{
		for ( uint32_t t = 0; t < m_Count; ++t )
		{
			if ( m_Keys[ t ] == a_Key )
			{
				return m_Values + t;
			}
		}

		return nullptr;
	}

	K * operator[]( V a_Value )
	{
		for ( uint32_t t = 0; t < m_Count; ++t )
		{
			if ( m_Values[ t ] == a_Value )
			{
				return m_Keys + t;
			}
		}

		return nullptr;
	}

	uint32_t Count() const
	{
		return m_Count;
	}

	void Put( const K & a_Key, const V & a_Value )
	{
		if ( m_Count < c_Capacity )
		{
			m_Keys[ m_Count ] = a_Key;
			m_Values[ m_Count ] = a_Value;
			++m_Count;
		}
	}

	bool Contains( const K & a_Key ) const
	{
		for ( uint32_t t = 0; t < m_Count; ++t )
		{
			if ( m_Keys[ t ] == a_Key )
			{
				return true;
			}
		}

		return false;
	}

	bool Contains( const V & a_Value ) const
	{
		for ( uint32_t t = 0; t < m_Count; ++t )
		{
			if ( m_Values[ t ] == a_Value )
			{
				return true;
			}
		}

		return false;
	}

	bool Remove( const K & a_Key )
	{
		for ( uint32_t t = 0; t < m_Count; ++t )
		{
			if ( m_Keys[ t ] == a_Key )
			{
				m_Keys[ t ] = m_Keys[ m_Count - 1 ];
				m_Values[ t ] = m_Values[ m_Count - 1 ];
				--m_Count;
				return true;
			}
		}

		return false;
	}

	bool Remove( const V & a_Value )
	{
		for ( uint32_t t = 0; t < m_Count; ++t )
		{
			if ( m_Values[ t ] == a_Value )
			{
				m_Keys[ t ] = m_Keys[ m_Count - 1 ];
				m_Values[ t ] = m_Values[ m_Count - 1 ];
				--m_Count;
				return true;
			}
		}

		return false;
	}

	uint32_t Capacity() const
	{
		return c_Capacity;
	}

	inline K * GetKeys()
	{
		return m_Keys;
	}

	inline const K * GetKeys() const
	{
		return m_Keys;
	}

	inline V * GetValues()
	{
		return m_Values;
	}

	inline const V * GetValues() const
	{
		return m_Values;
	}

	void Clear()
	{
		m_Count = 0;
	}

private:
	K m_Keys[ CAPACITY ];
	V m_Values[ CAPACITY ];
	uint32_t c_Capacity = CAPACITY;
	uint32_t m_Count;
};

#endif//STATICDICTIONARY_H