#pragma once
#ifndef REGISTRY_H
#define REGISTRY_H

#include <Framework/Template/Singleton.hpp>
#include <Framework/Containers/StaticDictionary.hpp>

//=====================================================================================
template< typename T, typename U , uint32_t CAPACITY >
class Registry : public Singleton< T >
{
public:

protected:

	typedef StaticDictionary< uint32_t, U, CAPACITY > Manifest;

	inline const Manifest & GetManifest() const
	{
		return m_Manifest;
	}

	bool Register( uint32_t a_Key, const U & a_Object )
	{
		if ( m_Manifest.Contains( a_Key ) || m_Manifest.Count() == m_Manifest.Capacity() )
		{
			return false;
		}

		m_Manifest.Put( a_Key, a_Object );
		return true;
	}
	
	bool DeRegister( const U & a_Object )
	{
		return m_Manifest.Remove( a_Object );
	}

	bool DeRegister( uint32_t a_Key )
	{
		return m_Manifest.Remove( a_Key );
	}

private:

	Manifest m_Manifest;
};

#endif//REGISTRY_H