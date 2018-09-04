#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Containers/CString.hpp>

//=====================================================================================
class Exposer final
{
public:

	Exposer * Declaration( const char * a_Name, const Colour & a_Tint );
	Exposer * Field( const char * a_Name );
	Exposer * Separator( const char * a_Name );
	Exposer * Output( const char * a_Name );
	Exposer * Input( const char * a_Name );

private:

	CString m_Name = "";
	uint32_t m_NameHash = 0;
	Colour	m_Tint = Colour::GRAY;
};

//=====================================================================================
class Object abstract
{
public:

protected:

	//static void Expose( ... );
	//void Save( ... );
	//void Load( ... );

private:


};

#endif//OBJECT_H