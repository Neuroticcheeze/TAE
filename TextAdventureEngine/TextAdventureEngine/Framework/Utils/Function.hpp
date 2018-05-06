#pragma once
#ifndef FUNCTION_H
#define FUNCTION_H

//=====================================================================================
typedef void ( *Event )();			/*A function that just fires without any inputs or outputs.*/

//=====================================================================================
template< typename R, typename ... T >
using Action = R ( * )( T ... );	/*A function that can accept any number of arguments and must return a type*/

//=====================================================================================
template< typename ... T >
using Predicate = bool ( * )( T ... );	/*A function that evaluates an argument*/

//=====================================================================================
template< typename USER, typename ... T >
using UserPredicate = bool( * )( USER, T ... );/*A function that evaluates an argument and can accept a user pointer*/

//=====================================================================================
template< typename T >
using Comparer = bool( * )( T, T );		/*A function that forms an evaluation between two arguments of equal type*/

//=====================================================================================
template< typename T >
using Consumer = void ( * )( T );	/*A function that consumes an argument and returns nothing*/

//=====================================================================================
template< uint32_t CAPACITY, typename R, typename ... T >
class Delegate final
{
public:

	Delegate() 
		: m_Count( 0 )
	{}

	~Delegate()
	{
		m_Count = 0;
	}

	Delegate< CAPACITY, R, T ... > & operator+=( Action< R, T ... > a_Functor )
	{
		if ( a_Functor && m_Count < ( CAPACITY - 1 ) )
		{
			m_Functors[ m_Count ] = a_Functor;
			++m_Count;
		}

		return *this;
	}

	Delegate< CAPACITY, R, T ... > & operator-=( Action< R, T ... > a_Functor )
	{
		if ( m_Count == 0 )
		{
			return *this;
		}

		for ( uint32_t t = m_Count - 1; t >= 0; --t )
		{
			if ( t >= m_Count )
				break;

			if ( a_Functor == m_Functors[ t ] )
			{
				m_Functors[ t ] = m_Functors[ m_Count ];
				--m_Count;
			}
		}

		return *this;
	}

	R Invoke( T ... a_Args )
	{
		for ( uint32_t t = 0; t < m_Count; ++t )
		{
			m_Functors[ t ]( a_Args... );
		}
	}

private:
	uint32_t m_Count;
	Action< R, T ... > m_Functors[ CAPACITY ];
};

#endif//FUNCTION_H