#include "XmlManager.hpp"
#include <Framework/Containers/CString.hpp>
#include <tinyxml/tinyxml2.h>
#include <tinyxml/tinyxml2.cpp>

using namespace tinyxml2;

//=====================================================================================
void XmlManager::Init()
{
}

//=====================================================================================
void XmlManager::Finalise()
{
}

//=====================================================================================
XmlObject XmlManager::ParseXml( const char * a_XmlString ) const
{
	XmlObject outTree;

	CString xmlstr = a_XmlString;

	XMLDocument doc;
	XMLError err = doc.Parse( xmlstr.Get(), xmlstr.Length() );

	if ( err == XML_SUCCESS )
	{
		XMLNode * node = doc.FirstChild();
		Recurse( node, outTree );
	}

	return outTree;
}

//=====================================================================================
void XmlManager::Recurse( void * a_Xml, XmlObject & o_Xml ) const
{
	XMLNode * xml = static_cast< XMLNode * >( a_Xml );
	if ( !xml )
	{
		return;
	}

	Marshal( a_Xml, o_Xml );

	auto it = xml->FirstChild();
	while ( it )
	{
		XmlObject child;
		Recurse( it, child );
		o_Xml.m_Children.Append( child );
		it = it->NextSibling();
	}
}

//=====================================================================================
void XmlManager::Marshal( void * a_Xml, XmlObject & o_Xml ) const
{
	XMLNode * xml = static_cast< XMLNode * >( a_Xml );
	if ( !xml )
	{
		return;
	}

	if ( xml->ToText() )
	{
		o_Xml.m_Name = "";
		o_Xml.m_Value = xml->ToText()->Value();
		return;
	}

	if ( !xml->ToElement() )
	{
		return;
	}

	XMLElement * xmle = static_cast< XMLElement * >( xml );
	
	const XMLAttribute * attr = xmle->FirstAttribute();

	o_Xml.m_Name = xmle->Name();
	o_Xml.m_Value = "";

	if ( attr )
	{
		do
		{
			XmlObject::Attribute attrib_;
			attrib_.Name = attr->Name();
			
			const char * strval = attr->Value();
			bool b = false;
			float f = 0.0F;

			if ( CString::Parse( strval, f ) )
			{
				attrib_.Type = XmlObject::AttributeType::FLOAT;
				attrib_.Value.Float = f;
			}

			else if ( CString::Parse( strval, b ) )
			{
				attrib_.Type = XmlObject::AttributeType::BOOLEAN;
				attrib_.Value.Boolean = b;
			}
			
			else
			{
				attrib_.Type = XmlObject::AttributeType::STRING;
				attrib_.Value.String = strval;
			}

			o_Xml.m_Attributes.Append( attrib_ );
		}
		while ( attr = attr->Next() );
	}
}