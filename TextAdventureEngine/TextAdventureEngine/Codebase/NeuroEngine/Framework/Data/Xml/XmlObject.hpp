#pragma once
#ifndef XMLOBJECT_H
#define XMLOBJECT_H

#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/CString.hpp>

//=====================================================================================
class XmlObject final
{
	friend class XmlManager;

public:

	ENUMCLASS( AttributeType, int8_t )
		INVALID = -1,
		STRING,
		FLOAT,
		BOOLEAN,
	END_ENUMCLASS( AttributeType, int8_t )

	struct AttributeValue
	{
		CString			String;
		float			Float;
		bool			Boolean;
	};

	struct Filter
	{
		ENUM( Type, uint8_t )
			NAME,
			ATTRIBUTE,
			VALUE,
			HAS_CHILDREN,
		END_ENUM;

		Type FilterType;
		CString FilterValue;
	};

	const CString & GetName() const;
	const CString & GetValue() const;
	bool HasAttribute( const char * a_Attribute, const AttributeType * a_ExpectedTypeOf = nullptr ) const;
	AttributeValue GetAttribute( const char * a_Attribute, AttributeType * o_TypeOf = nullptr ) const;
	uint32_t NumChildren() const;
	const XmlObject & operator[]( uint32_t a_Index ) const;
	const XmlObject & operator[]( const char * a_Name ) const;
	bool FindChildren( const InitialiserList< Filter > & a_FilterList, Array< XmlObject > & o_Result ) const;

	inline bool operator==( const char * a_Name ) const { return m_Name == a_Name; }
	inline bool operator!=( const char * a_Name ) const { return m_Name != a_Name; }
	inline bool operator==( const XmlObject & a_Other ) const { return m_Name == a_Other.m_Name; }
	inline bool operator!=( const XmlObject & a_Other ) const { return m_Name != a_Other.m_Name; }

	static const XmlObject NULL_XML_OBJ;

private:

	struct Attribute
	{
		CString Name;
		AttributeValue Value;
		AttributeType Type;
		
		inline bool operator==( const char * a_Name ) const { return Name == a_Name; }
		inline bool operator!=( const char * a_Name ) const { return Name != a_Name; }
		inline bool operator==( const Attribute & a_Other ) const { return Name == a_Other.Name; }
		inline bool operator!=( const Attribute & a_Other ) const { return Name != a_Other.Name; }
	};

	Array< XmlObject > m_Children;
	Array< Attribute > m_Attributes;
	CString m_Name, m_Value;
};

#endif//XMLOBJECT_H