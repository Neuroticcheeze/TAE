#include "Blackboard.hpp"
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
const uint64_t Blackboard::Value::NIL = UINT64_MAX;

//=====================================================================================
void Blackboard::Value::SetType( Type a_ValueType )
{
	if ( m_ValueType != a_ValueType )
	{
		if ( m_ValueType != VT_NIL )
		{
			PRINT( "Blackboard: Value changed type, any data associated with previous type has been erased." );
		}

		switch ( m_ValueType )
		{
		case Blackboard::Value::VT_NIL:
			BSet( &m_VTypeData, 0, sizeof( decltype( m_VTypeData ) ) );
			m_StringData = "";
			m_CompoundData.GetEntries().Clear();
			break;
		case Blackboard::Value::VT_FLOAT:
			m_VTypeData.Float = 0.0F;
			break;
		case Blackboard::Value::VT_INT32:
			m_VTypeData.Int32 = 0;
			break;
		case Blackboard::Value::VT_BOOL:
			m_VTypeData.Bool = false;
			break;
		case Blackboard::Value::VT_STRING:
			m_StringData = "";
			break;
		case Blackboard::Value::VT_COMPOUND:
			m_CompoundData.GetEntries().Clear();
			break;
		default:
			break;
		}

		m_ValueType = a_ValueType;
	}
}

//=====================================================================================
Blackboard::Value::operator float &()
{
	SetType( VT_FLOAT );
	return m_VTypeData.Float;
}

//=====================================================================================
Blackboard::Value::operator const float &() const
{
	static const float def = 0.0F;
	return m_ValueType == VT_FLOAT ? m_VTypeData.Float : def;
}

//=====================================================================================
Blackboard::Value & Blackboard::Value::operator=( float a_Value )
{
	SetType( VT_FLOAT );
	m_VTypeData.Float = a_Value;
	return *this;
}

//=====================================================================================
Blackboard::Value::operator int32_t &()
{
	SetType( VT_INT32 );
	return m_VTypeData.Int32;
}

//=====================================================================================
Blackboard::Value::operator const int32_t &() const
{
	static const int32_t def = 0;
	return m_ValueType == VT_INT32 ? m_VTypeData.Int32 : def;
}

//=====================================================================================
Blackboard::Value & Blackboard::Value::operator=( int32_t a_Value )
{
	SetType( VT_INT32 );
	m_VTypeData.Int32 = a_Value;
	return *this;
}

//=====================================================================================
Blackboard::Value::operator bool &()
{
	SetType( VT_BOOL );
	return m_VTypeData.Bool;
}

//=====================================================================================
Blackboard::Value::operator const bool &() const
{
	static const bool def = false;
	return m_ValueType == VT_BOOL ? m_VTypeData.Bool : def;
}

//=====================================================================================
Blackboard::Value & Blackboard::Value::operator=( bool a_Value )
{
	SetType( VT_BOOL );
	m_VTypeData.Bool = a_Value;
	return *this;
}

//=====================================================================================
Blackboard::Value::operator CString &()
{
	SetType( VT_STRING );
	return m_StringData;
}

//=====================================================================================
Blackboard::Value::operator const CString &() const
{
	static const CString def = "";
	return m_ValueType == VT_STRING ? m_StringData : def;
}

//=====================================================================================
Blackboard::Value & Blackboard::Value::operator=( const CString & a_Value )
{
	SetType( VT_STRING );
	m_StringData = a_Value;
	return *this;
}

//=====================================================================================
bool Blackboard::Value::HasValue( const Name & a_Name )
{
	return m_ValueType == VT_COMPOUND && m_CompoundData.Contains( a_Name );
}

//=====================================================================================
Blackboard::Value & Blackboard::Value::PutValue( const char * a_Name, const Blackboard::Value & a_Value )
{
	Value val = a_Value;
	val.m_CompoundEntryName = a_Name;

	SetType( VT_COMPOUND );
	m_CompoundData.Put( WSID( a_Name ), val );

	auto it = m_CompoundData.GetEntries().Last();
	auto begin1n = m_CompoundData.GetEntries().First() - 1;

	while ( it != begin1n )
	{
		if ( it->Value.IsNil() )
		{
			m_CompoundData.Remove( it->Key );
		}

		--it;
	}

	return *this;
}

//=====================================================================================
Blackboard::Value & Blackboard::Value::GetValue( const Name & a_Name )
{
	if ( m_ValueType == VT_COMPOUND )
	{
		auto data = m_CompoundData[ a_Name ];

		if ( data != nullptr )
		{
			return *data;
		}

		throw OutOfRangeAccessException( CString().Format( __FUNCTION__ " -- Compound Value Ref Accessor FAILURE: Value [COMPOUND] does not contain {%d}", a_Name ).Get() );
	}

	throw OutOfRangeAccessException( __FUNCTION__ " -- Compound Value Ref Accessor FAILURE: Value is not of type [COMPOUND]" );
}

//=====================================================================================
const Blackboard::Value & Blackboard::Value::GetValue( const Name & a_Name ) const
{
	static const Value NilVal = Value( NIL );

	if ( m_ValueType == VT_COMPOUND )
	{
		auto data = m_CompoundData[ a_Name ];

		if ( data != nullptr )
		{
			return *data;
		}
	}

	return NilVal;
}

//=====================================================================================
uint32_t Blackboard::Value::Count() const
{
	if ( m_ValueType == VT_COMPOUND )
	{
		return m_CompoundData.Count();
	}

	return 0;
}

//=====================================================================================
Dictionary< Name, Blackboard::Value > & Blackboard::Value::GetData()
{
	SetType( VT_COMPOUND );
	return m_CompoundData;
}

//=====================================================================================
const Dictionary< Name, Blackboard::Value > & Blackboard::Value::GetData() const
{
	return m_CompoundData;
}

//=====================================================================================
Blackboard::Value & Blackboard::Value::operator=( uint64_t a_Value )
{
	if ( a_Value == NIL )
	{
		SetType( VT_NIL );
	}

	else
	{
		SetType( VT_INT32 );
		m_VTypeData.Int32 = static_cast< int32_t >( a_Value & 0x7FFFFFFF );
	}

	return *this;
}

//=====================================================================================
CString Blackboard::Value::ToString( const char * a_Parameter ) const
{
	return ToStringV( 0 );
}

//=====================================================================================
CString Blackboard::Value::ToStringV( uint32_t a_Level ) const
{
	CString string;

	switch ( m_ValueType )
	{
	case Blackboard::Value::VT_NIL:
		string = "[VT_NIL]";
		break;
	case Blackboard::Value::VT_FLOAT:
		string = CString().Format( "[VT_FLOAT]: %f", m_VTypeData.Float );
		break;
	case Blackboard::Value::VT_INT32:
		string = CString().Format( "[VT_INT32]: %i", m_VTypeData.Int32 );
		break;
	case Blackboard::Value::VT_BOOL:
		string = CString().Format( "[VT_BOOL]: %s", m_VTypeData.Bool ? "true" : "false" );
		break;
	case Blackboard::Value::VT_STRING:
		string = CString().Format( "[VT_STRING]: \"%s\"", m_StringData.Get() );
		break;
	case Blackboard::Value::VT_COMPOUND:
		string = CString().Format( "[VT_COMPOUND]: (%u values)", m_CompoundData.Count() );

		auto it = m_CompoundData.GetEntries().First();
		auto end = m_CompoundData.GetEntries().End();

		while ( it != end )
		{
			string += it->Value.ToStringV( a_Level + 1 );
			++it;
		}

		break;
	}

	CString pre = "\n";

	for ( uint32_t lvl = 0; lvl < a_Level; ++lvl )
	{
		pre += "              ";
	}

	pre += CString().Format( "|------ \"%s\" ---> ", m_CompoundEntryName.Get() );

	return pre + string;
}