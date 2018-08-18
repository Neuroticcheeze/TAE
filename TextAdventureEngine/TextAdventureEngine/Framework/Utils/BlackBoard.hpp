#pragma once
#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Thread/Mutex.hpp>
#include <Framework/Containers/Set.hpp>
#include <Framework/Template/StringRepresentable.hpp>

//=====================================================================================
typedef uint32_t Name;

//=====================================================================================
union PackedVTypes
{
	float	Float;
	int32_t Int32;
	bool	Bool;
};

//=====================================================================================
class Blackboard final
{
public:

	struct Value final : public StringRepresentableStruct< Value >
	{
		friend class Blackboard;
	
	public:
	
		static const uint64_t NIL;
	
		ENUM( Type, uint8_t )
			VT_NIL,
			VT_FLOAT,
			VT_INT32,
			VT_BOOL,
			VT_STRING,
			VT_COMPOUND,
		END_ENUM;
	
	public:
	
		Value() : m_ValueType( VT_NIL ) {}
	
		Type GetType() const { return m_ValueType; }
		bool IsNil() const { return m_ValueType == VT_NIL; }
	
		// Generic Converters
		operator float &();
		operator const float &() const;
		Value & operator=( float a_Value );
		Value( float a_Value ) { *this = a_Value; }
		operator int32_t &();
		operator const int32_t &() const;
		Value & operator=( int32_t a_Value );
		Value( int32_t a_Value ) { *this = a_Value; }
		operator bool &();
		operator const bool &() const;
		Value & operator=( bool a_Value );
		Value( bool a_Value ) { *this = a_Value; }
		operator CString &();
		operator const CString &() const;
		Value & operator=( const CString & a_Value );
		Value( const CString & a_Value ) { *this = a_Value; }
	
		Value & operator=( uint64_t a_Value );
		Value( uint64_t a_Value ) { *this = a_Value; }
	
		// Compound
		bool HasValue( const Name & a_Name );
		Value & PutValue( const char * a_Name, const Value & a_Value ); // The compound returns itself for chaining
		Value & GetValue( const Name & a_Name );
		const Value & GetValue( const Name & a_Name ) const;
		uint32_t Count() const;
		Dictionary< Name, Value > & GetData();
		const Dictionary< Name, Value > & GetData() const;
	
		CString ToString( const char * a_Parameter = "" ) const override;
	
	private:
	
		CString ToStringV( uint32_t a_Level ) const;
	
		// Value Type
		Type m_ValueType = VT_NIL;
		void SetType( Type a_ValueType );
		
		// Data Storage
		PackedVTypes			  m_VTypeData;
		CString					  m_StringData;
		Dictionary< Name, Value > m_CompoundData;
	
		CString m_CompoundEntryName;
	};
};

#endif//Blackboard_H