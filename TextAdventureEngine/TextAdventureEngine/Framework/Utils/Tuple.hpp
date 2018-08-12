#pragma once
#ifndef TUPLE_H
#define TUPLE_H

//=====================================================================================
template< typename T0, typename T1 >
struct Tuple2
{
	Tuple2() 
		: Value0( T0() )
		, Value1( T1() )
	{}

	Tuple2( T0 a_Value0, T1 a_Value1 )
		: Value0( a_Value0 )
		, Value1( a_Value1 )
	{
	}

	bool operator==( const Tuple2< T0, T1 > & a_Other ) const
	{
		return Value0 == a_Other.Value0 &&
			   Value1 == a_Other.Value1;
	}

	bool operator!=( const Tuple2< T0, T1 > & a_Other ) const
	{
		return !( *this == a_Other );
	}

	T0 Value0;
	T1 Value1;
};

//=====================================================================================
template< typename T0, typename T1, typename T2 >
struct Tuple3
{
	Tuple3() 
		: Value0( T0() )
		, Value1( T1() )
		, Value2( T2() )
	{}

	Tuple3( T0 a_Value0, T1 a_Value1, T2 a_Value2 )
		: Value0( a_Value0 )
		, Value1( a_Value1 )
		, Value2( a_Value2 )
	{
	}

	bool operator==( const Tuple3< T0, T1, T2 > & a_Other ) const
	{
		return Value0 == a_Other.Value0 &&
			   Value1 == a_Other.Value1 &&
			   Value2 == a_Other.Value2;
	}

	bool operator!=( const Tuple3< T0, T1, T2 > & a_Other ) const
	{
		return !( *this == a_Other );
	}

	T0 Value0;
	T1 Value1;
	T2 Value2;
};

//=====================================================================================
template< typename T0, typename T1, typename T2, typename T3 >
struct Tuple4
{
	Tuple4() 
		: Value0( T0() )
		, Value1( T1() )
		, Value2( T2() )
		, Value3( T3() )
	{}

	Tuple4( T0 a_Value0, T1 a_Value1, T2 a_Value2, T3 a_Value3 )
		: Value0( a_Value0 )
		, Value1( a_Value1 )
		, Value2( a_Value2 )
		, Value3( a_Value3 )
	{
	}

	bool operator==( const Tuple3< T0, T1, T2 > & a_Other ) const
	{
		return Value0 == a_Other.Value0 &&
			   Value1 == a_Other.Value1 &&
			   Value2 == a_Other.Value2 &&
			   Value3 == a_Other.Value3;
	}

	bool operator!=( const Tuple3< T0, T1, T2 > & a_Other ) const
	{
		return !( *this == a_Other );
	}

	T0 Value0;
	T1 Value1;
	T2 Value2;
	T3 Value3;
};

#endif//TUPLE_H