#pragma once
#ifndef XMLMANAGER_H
#define XMLMANAGER_H

#include <Framework/Template/Manager.hpp>
#include "XmlObject.hpp"

//=====================================================================================
class XmlManager final : public Manager< XmlManager >
{
public:

	void Init();
	void Finalise();

	XmlObject ParseXml( const char * a_XmlString ) const;

private:

	void Recurse( void * a_Xml, XmlObject & o_Xml ) const;
	void Marshal( void * a_Xml, XmlObject & o_Xml ) const;
};

#endif//XMLMANAGER_H