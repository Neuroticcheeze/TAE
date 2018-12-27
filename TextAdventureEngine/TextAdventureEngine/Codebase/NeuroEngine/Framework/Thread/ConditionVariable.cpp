#include "ConditionVariable.hpp"
#include <condition_variable>

#define CV static_cast< std::condition_variable* >( m_CV )
#define MUTEX static_cast< std::mutex* >( m_Mutex )

ConditionVariable::ConditionVariable()
	: m_CV( new std::condition_variable )
	, m_Mutex( new std::mutex )
{
}

ConditionVariable::~ConditionVariable()
{
	delete m_CV;
	delete m_Mutex;
}

void ConditionVariable::Wait(Predicate< void * > a_WakeCondition, void * a_Argument)
{
	std::unique_lock< std::mutex > lock( *MUTEX );
	CV->wait( lock, [&]() 
		{
			return a_WakeCondition ? a_WakeCondition( a_Argument ) : true; 
		} 
	);
}

void ConditionVariable::NotifyOne()
{
	CV->notify_one();
}

void ConditionVariable::NotifyAll()
{
	CV->notify_all();
}

#undef CV