#pragma once
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <Framework/Utils/Function.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Utils/Tuple.hpp>

//=====================================================================================
template< typename T, uint32_t STATES >
class StateMachine
{
public:

	template< typename E >
	E GetState() const
	{
		return static_cast< E >( m_CurrentState );
	}

protected:

	template< typename E >
	StateMachine( E a_InitialState )
		: m_CurrentState( a_InitialState )
		, m_TargetState( a_InitialState )
	{
	}

	template< typename E >
	void RegisterTransition( E a_SrcState, E a_DestState, Predicate< T * > a_Evaluator = nullptr, 
														  Consumer< T * > a_Event = nullptr )
	{
		Transition & transition = m_Transitions[ static_cast< uint32_t >( a_SrcState ) ][ static_cast< uint32_t >( a_DestState ) ];
		transition.m_Evaluator = a_Evaluator;
		transition.m_Event = a_Event;
	}

	void PollStateMachine()
	{
		if ( m_CurrentState != m_TargetState )
		{
			const Transition & transition = m_Transitions[ m_CurrentState ][ m_TargetState ];
				
			if ( transition.m_Evaluator == nullptr || transition.m_Evaluator( static_cast< T* >( this ) ) )
			{
				m_CurrentState = m_TargetState;

				if ( transition.m_Event != nullptr )
				{
					transition.m_Event( static_cast< T* >( this ) );
				}
			}
		}
	}

	template< typename E >
	void SetState( E a_NewState )
	{
		m_TargetState = static_cast< uint32_t >( a_NewState );
	}

private:
	struct Transition
	{
		Predicate< T * > m_Evaluator = nullptr;
		Consumer< T * > m_Event = nullptr;
	};

	Transition m_Transitions[ STATES ][ STATES ];
	uint32_t m_CurrentState;
	uint32_t m_TargetState;
};

#endif//STATEMACHINE_H