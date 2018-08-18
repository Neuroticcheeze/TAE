#pragma once
#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Thread/Mutex.hpp>
#include <Framework/Containers/Set.hpp>

//=====================================================================================
struct Value final
{
	ENUMCLASS( Type, int8_t )
		NIL = -1,
		BOOL,
		CHAR,
		INT32,
		FLOAT,
		STRING,
	END_ENUMCLASS( Type, int8_t );

private:

	friend class BlackBoard;

	union V
	{
		V() {}
		~V() {}

		bool		b = false;
		char		c;
		int32_t		si;
		float		f;
		CString		str;
	}
	m_Value;
	Type m_Type;

public:

#define GET( TYPE, ENUMV, FIELD, DEFAULT ) operator TYPE() const { return m_Type == Type::ENUMV ? m_Value.FIELD : DEFAULT; }
#define SET( TYPE, ENUMV, FIELD )\
	Value( TYPE a_Value ) { *this = a_Value; }\
	Value & operator=( const TYPE & a_Value ) { m_Type = Type::ENUMV; m_Value.FIELD = a_Value; return *this; }

	GET( bool, BOOL, b, false )
	GET( char, CHAR, c, 0 )
	GET( int32_t, INT32, si, 0 )
	GET( float, FLOAT, f, 0.0F )
	GET( const CString &, STRING, str, "" )

	SET( bool, BOOL, b )
	SET( char, CHAR, c )
	SET( int32_t, INT32, si )
	SET( float, FLOAT, f )
	SET( CString, STRING, str )

	Value();
	Value( const Value & a_Other );
	Value & operator=( const Value & a_Other );

	bool operator==( const Value & a_Other ) const;

#undef GET
#undef SET
};

//=====================================================================================
class BlackBoard final
{
public:

	ENUM( ValueEvent, uint8_t )
		EV_PUSH_VALUE_CHANGED,
		EV_PUSH_VALUE_ADDED,
	END_ENUM;

	void Push( uint32_t a_Id, Value a_Value );
	const Value & Query( uint32_t a_Id ) const;
	bool Contains( uint32_t a_Id ) const;

	class IBlackBoardListener abstract
	{
		friend class BlackBoard;

	private:

		virtual void OnValueEvent( ValueEvent a_ValueEvent, uint32_t a_ValueId, const Value & a_Value ) abstract;
	};

	void AddValueEventListener( IBlackBoardListener * a_Listener );
	void RemoveValueEventListener( IBlackBoardListener * a_Listener );

private:

	Set< IBlackBoardListener * > m_Listeners;
	Dictionary< uint32_t, Value > m_Values;
	mutable Mutex m_Lock;
	mutable Mutex m_ListenerLock;
};

#endif//BLACKBOARD_H