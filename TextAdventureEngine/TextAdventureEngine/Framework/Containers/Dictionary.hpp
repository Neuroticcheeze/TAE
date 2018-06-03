#pragma once
#ifndef DICTIONARY_H
#define DICTIONARY_H

//=====================================================================================
#include <Framework/Containers/Array.hpp>

//=====================================================================================
template< typename K, typename V >
class Dictionary final
{
public:

	Dictionary()
	{

	}

	Dictionary( const Dictionary< K, V > & a_Other )
	{
		*this = a_Other;
	}

	Dictionary< K, V > & operator=( const Dictionary< K, V > & a_Other )
	{
		m_Entries = a_Other.m_Entries;
		return *this;
	}

	V * operator[]( K a_Key ) const
	{
		Pair * entry = m_Entries.Find< const K & >( []( const K & a_Key, const Pair & a_PairEntry )
			{
				return a_PairEntry.Key == a_Key;
			},
			a_Key
		);

		if ( entry != nullptr )
		{
			return &entry->Value;
		}

		return nullptr;
	}

	const K * operator[]( V a_Value ) const
	{
		Pair * entry = m_Entries.Find< const V & >( []( const V & a_Value, const Pair & a_PairEntry )
			{
				return a_PairEntry.Value == a_Value;
			},
			a_Value
		);

		if ( entry != nullptr )
		{
			return &entry->Key;
		}

		return nullptr;
	}

	/* Get the number of elements in the array. */
	uint32_t Count() const
	{
		return m_Entries.Length();
	}

	void Put( const K & a_Key, const V & a_Value )
	{
		Pair * entry = m_Entries.Find< const K & >( []( const K & a_Key, const Pair & a_PairEntry )
			{
				return a_PairEntry.Key == a_Key;
			},
			a_Key
		);

		if ( entry != nullptr )
		{
			entry->Value = a_Value;
		}

		else
		{
			Pair pair;
			pair.Key = a_Key;
			pair.Value = a_Value;
			m_Entries.Append( pair );
		}
	}

	bool Contains( const K & a_Key ) const
	{
		const Pair * entry = m_Entries.Find< const K & >( []( const K & a_Key, const Pair & a_PairEntry )
			{
				return a_PairEntry.Key == a_Key;
			},
			a_Key
		);

		return entry != nullptr;
	}

	bool Contains( const V & a_Value ) const
	{
		const Pair * entry = m_Entries.Find< const V & >( []( const V & a_Value, const Pair & a_PairEntry )
			{
				return a_PairEntry.Value == a_Value;
			},
			a_Value
		);

		if ( entry != nullptr )
		{
			return &entry->Key;
		}

		return entry != nullptr;
	}

	bool Remove( const K & a_Key )
	{
		Pair * entry = m_Entries.Find< const K & >( []( const K & a_Key, const Pair & a_PairEntry )
			{
				return a_PairEntry.Key == a_Key;
			},
			a_Key
		);

		if ( entry != nullptr )
		{
			m_Entries.RemoveAt( m_Entries.IndexOf( *entry ) );
			return true;
		}

		return false;
	}

	bool Remove( const V & a_Value )
	{
		Pair * entry = m_Entries.Find< const V & >( []( const V & a_Value, const Pair & a_PairEntry )
			{
				return a_PairEntry.Value == a_Value;
			},
			a_Value
		);

		if ( entry != nullptr )
		{
			m_Entries.RemoveAt( m_Entries.IndexOf( *entry ) );
			return true;
		}

		return false;
	}

	struct Pair
	{
		Pair() {}

		K Key;
		V Value;

		bool operator==( const Pair & a_Other ) const
		{
			return Key == a_Other.Key;
		}
	};

	inline Array< Pair > & GetEntries()
	{
		return m_Entries;
	}

private:
	Array< Pair > m_Entries;
};

#endif//DICTIONARY_H