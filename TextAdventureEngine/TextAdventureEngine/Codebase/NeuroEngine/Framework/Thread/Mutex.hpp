#pragma once
#ifndef MUTEX_H
#define MUTEX_H

//=====================================================================================
class Mutex final
{
	friend class ScopedLock;

public:

	Mutex();
	~Mutex();

	ScopedLock Lock();

private:

	void * m_Mutex;
};

//=====================================================================================
class ScopedLock final
{
	friend class Mutex;

private:

	ScopedLock();

public:
	
	ScopedLock( const ScopedLock & a_Other );
	~ScopedLock();

private:

	void * m_Lock;
	Mutex * m_OwningMutex;
};


#endif//MUTEX_H