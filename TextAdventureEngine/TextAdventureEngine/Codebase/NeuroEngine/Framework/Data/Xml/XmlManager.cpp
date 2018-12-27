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
	PROFILE;

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

CString XmlManager::GenerateXmlImpl( XmlObject & a_XmlObject, uint32_t a_Indentation ) const
{
	PROFILE;

	CString attribs;
	auto it = a_XmlObject.m_Attributes.First();
	while ( it != a_XmlObject.m_Attributes.End() )
	{
		CString attrVal;
		switch ( it->Type )
		{
		case XmlObject::AttributeType::STRING:
			attrVal = it->Value.String;
			break;
		case XmlObject::AttributeType::FLOAT:
			attrVal = CString::ToString( it->Value.Float );
			break;
		case XmlObject::AttributeType::BOOLEAN:
			attrVal = CString::ToString( it->Value.Boolean );
			break;
		default:
			break;
		}
		attribs += CString::Format( attrVal.Length() > 0 ? " %s=\"%s\"" : " %s", it->Name.Get(), attrVal.Get() );
		++it;
	}


	if ( a_XmlObject.m_Value.Length() > 0 )
	{
		return a_XmlObject.m_Value;
	}

	if ( a_XmlObject.m_Children.Count() == 0 )
	{
		return CString::Format( "<%s%s>%s</%s>", a_XmlObject.m_Name.Get(), attribs.Get(), a_XmlObject.m_Value.Get(), a_XmlObject.m_Name.Get() );
	}

	if ( a_XmlObject.m_Children.Count() == 1 && a_XmlObject.m_Children[ 0U ].m_Value.Length() > 0 )
	{
		return CString::Format( "<%s%s>%s</%s>", a_XmlObject.m_Name.Get(), attribs.Get(), a_XmlObject.m_Children[ 0U ].m_Value.Get(), a_XmlObject.m_Name.Get() );
	}

	CString whitespaces = CString( " ", ( a_Indentation + 1 ) * 4 );
	CString whitespacesm1 = CString( " ", a_Indentation * 4 );

	CString children;
	auto childIt = a_XmlObject.m_Children.First();
	while ( childIt != a_XmlObject.m_Children.End() )
	{
		children += CString::Format( childIt == a_XmlObject.m_Children.Last() ? "%s%s" : "%s%s\n", whitespaces.Get(), GenerateXmlImpl( *childIt, a_Indentation + 1 ).Get() );
		++childIt;
	}

	CString inner = a_XmlObject.m_Children.Count() > 0 ? CString::Format( "\n%s\n", children.Get() ) : a_XmlObject.m_Value;
	return CString::Format( "<%s%s>%s%s</%s>", a_XmlObject.m_Name.Get(), attribs.Get(), inner.Get(), whitespacesm1.Get(), a_XmlObject.m_Name.Get() );
}