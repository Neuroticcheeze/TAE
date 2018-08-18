#include "BlackBoard.hpp"

//=====================================================================================
Value::Value()
	: m_Type( Value::Type::NIL )
{}

//=====================================================================================
Value::Value( const Value & a_Other )
{
	*this = a_Other;
}

//=====================================================================================
Value & Value::operator=( const Value & a_Other )
{ 
	m_Type = a_Other.m_Type;

	switch ( m_Type )
	{
#define ASSIGN_UNION( K ) m_Value.K = a_Other.m_Value.K

	case Value::Type::BOOL:
		ASSIGN_UNION( b );
		break;
	case Value::Type::CHAR:
		ASSIGN_UNION( c );
		break;
	case Value::Type::INT32:
		ASSIGN_UNION( si );
		break;
	case Value::Type::FLOAT:
		ASSIGN_UNION( f );
		break;
	case Value::Type::STRING:
		ASSIGN_UNION( str );
		break;

#undef ASSIGN_UNION
	}
	
	return *this; 
}

bool Value::operator==( const Value & a_Other ) const
{
	if ( m_Type == a_Other.m_Type )
	{
		switch ( m_Type )
		{
#define CHECK_UNION( K ) if ( m_Value.K == a_Other.m_Value.K ) return true;
	
		case Value::Type::BOOL:
			CHECK_UNION( b );
			break;
		case Value::Type::CHAR:
			CHECK_UNION( c );
			break;
		case Value::Type::INT32:
			CHECK_UNION( si );
			break;
		case Value::Type::FLOAT:
			CHECK_UNION( f );
			break;
		case Value::Type::STRING:
			CHECK_UNION( str );
			break;
	
#undef CHECK_UNION
		}
	}

	return false;
}

//=====================================================================================
void BlackBoard::Push( uint32_t a_Id, Value a_Value )
{
	m_Lock.Lock();

	const bool contains = m_Values.Contains( a_Id );
	const bool newval = contains ? ( *m_Values[ a_Id ] == a_Value ) : false;
	m_Values.Put( a_Id, a_Value );

	if ( !contains || newval )
	{
		auto it = m_Listeners.First();
		const auto end = m_Listeners.End();
		while ( it != end )
		{
			( *it )->OnValueEvent( newval ? ValueEvent::EV_PUSH_VALUE_CHANGED 
										  : ValueEvent::EV_PUSH_VALUE_ADDED, 
								   a_Id, 
								   a_Value );
			++it;
		}
	}
}

//=====================================================================================
const Value & BlackBoard::Query( uint32_t a_Id ) const
{
	static const Value NIL = Value();

	m_Lock.Lock();

	const Value * val = m_Values[ a_Id ];

	if ( val )
	{
		return *val;
	}

	return NIL;
}

//=====================================================================================
bool BlackBoard::Contains( uint32_t a_Id ) const
{
	m_Lock.Lock();
	
	return m_Values.Contains( a_Id );
}

//=====================================================================================
void BlackBoard::AddValueEventListener( IBlackBoardListener * a_Listener )
{
	if ( !a_Listener )
	{
		return;
	}

	m_ListenerLock.Lock();
	m_Listeners.Insert( a_Listener );
}

//=====================================================================================
void BlackBoard::RemoveValueEventListener( IBlackBoardListener * a_Listener )
{
	m_ListenerLock.Lock();
	m_Listeners.Remove( a_Listener );
}