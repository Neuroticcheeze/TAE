#pragma once
#ifndef CONDITIONVARIABLE_H
#define CONDITIONVARIABLE_H

#include <Framework/Utils/Function.hpp>

//=====================================================================================
class ConditionVariable final
{
public:

	ConditionVariable();
	~ConditionVariable();

	void Wait( Predicate< void * > a_WakeCondition, void * a_Argument );
	void NotifyOne();
	void NotifyAll();

private:

	void * m_Mutex;
	void * m_CV;
};

#endif//CONDITIONVARIABLE_H