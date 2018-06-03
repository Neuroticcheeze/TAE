#include "XmlObject.hpp"

//=====================================================================================
const XmlObject XmlObject::NULL_XML_OBJ;

//=====================================================================================
const CString & XmlObject::GetName() const
{
	return this->m_Name;
}

//=====================================================================================
const CString & XmlObject::GetValue() const
{
	return this->m_Value;
}

//=====================================================================================
bool XmlObject::HasAttribute( const char * a_Attribute, const AttributeType * a_ExpectedTypeOf ) const
{
	int32_t ind = -1;
	if ( ( ind = m_Attributes.IndexOf( a_Attribute ) ) != -1 )
	{
		if ( !a_ExpectedTypeOf || m_Attributes[ ( uint32_t )ind ].Type == *a_ExpectedTypeOf )
		return true;
	}

	return false;
}

//=====================================================================================
XmlObject::AttributeValue XmlObject::GetAttribute( const char * a_Attribute, AttributeType * o_TypeOf ) const
{
	int32_t ind = -1;
	if ( ( ind = m_Attributes.IndexOf( a_Attribute ) ) != -1 )
	{
		( *o_TypeOf ) = m_Attributes[ ( uint32_t )ind ].Type;
		return m_Attributes[ ( uint32_t )ind ].Value;
	}

	return { "", 0.0F, false };
}

//=====================================================================================
uint32_t XmlObject::NumChildren() const
{
	return m_Children.Count();
}

//=====================================================================================
const XmlObject & XmlObject::operator[]( uint32_t a_Index ) const
{
	return m_Children[ a_Index ];
}

//=====================================================================================
const XmlObject & XmlObject::operator[]( const char * a_Name ) const
{
	int32_t ind = -1;
	if ( ( ind = m_Children.IndexOf( a_Name ) ) != -1 )
	{
		return static_cast< const XmlObject & >( m_Children[ ind ] );
	}

	return NULL_XML_OBJ;
}

//=====================================================================================
bool XmlObject::FindChildren( const InitialiserList< Filter > & a_FilterList, Array< XmlObject > & o_Result ) const
{
	bool result = false;
	for ( uint32_t ci = 0; ci < m_Children.Count(); ++ci )
	{
		const XmlObject & child = m_Children[ ci ];
		auto it = NBegin( a_FilterList );
		auto end = NEnd( a_FilterList );

		bool failed = false;
		while ( !failed && it < end )
		{
			const Filter & filter = ( *it );
			switch ( filter.FilterType )
			{
			case Filter::NAME:
				if (child.GetName() != filter.FilterValue)
					failed = true; break;

			case Filter::ATTRIBUTE:
				if ( !child.HasAttribute( filter.FilterValue.Get() ) ) 
					failed = true; break;

			case Filter::VALUE:
				if ( child.GetValue() != filter.FilterValue.Get() ) 
					failed = true; break;

			case Filter::HAS_CHILDREN:
				if ( child.NumChildren() == 0 ) 
					failed = true; break;
			}

			++it;
		}

		if ( !failed )
		{
			o_Result.Append( child );
			result = true;
		}
	}

	return result;
}