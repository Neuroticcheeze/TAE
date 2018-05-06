#include "Mutex.hpp"

#include <mutex>

//=====================================================================================
Mutex::Mutex()
	: m_Mutex( new std::mutex )
{
}

//=====================================================================================
Mutex::~Mutex()
{
	delete static_cast< std::mutex * >( m_Mutex );
	m_Mutex = nullptr;
}

//=====================================================================================
ScopedLock Mutex::Lock()
{
	ScopedLock lock;
	lock.m_OwningMutex = this;
	return lock;
}

//=====================================================================================
ScopedLock::ScopedLock()
	: m_Lock( nullptr )
{

}

//=====================================================================================
ScopedLock::ScopedLock( const ScopedLock & a_Other )
	: m_Lock( new std::lock_guard< std::mutex >( *static_cast< std::mutex* >( a_Other.m_OwningMutex->m_Mutex ) ) )
{
}

//=====================================================================================
ScopedLock::~ScopedLock()
{
	if ( m_Lock )
	{
		delete static_cast< std::lock_guard< std::mutex >* >( m_Lock );
		m_Lock = nullptr;
	}
}