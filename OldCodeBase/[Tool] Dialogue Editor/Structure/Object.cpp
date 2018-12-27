#include "Object.hpp"
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
Exposer * Exposer::Declaration( const char * a_Name, const Colour & a_Tint )
{
	m_Name = a_Name;
	m_NameHash = WSID( a_Name );
	m_Tint = a_Tint;
    return this;
}