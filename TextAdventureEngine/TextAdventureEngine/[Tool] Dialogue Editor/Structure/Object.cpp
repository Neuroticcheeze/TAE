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

//=====================================================================================
Exposer * Exposer::Field( const char * a_Name )
{
    return this;
}

//=====================================================================================
Exposer * Exposer::Separator( const char * a_Name )
{
    return this;
}

//=====================================================================================
Exposer * Exposer::Output( const char * a_Name )
{
    return this;
}

//=====================================================================================
Exposer * Exposer::Input( const char * a_Name )
{
    return this;
}
