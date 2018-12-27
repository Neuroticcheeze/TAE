#include "ThreadManager.hpp"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>

#define WORKER( I ) static_cast< std::thread* >( m_WorkerPool )[ I ]

//=====================================================================================
void ThreadManager::Init( uint32_t a_WorkerCount )
{
	if ( m_WorkerPool )
	{
		delete[] static_cast< std::thread* >( m_WorkerPool );
		m_WorkerPool = nullptr;
	}

	m_TaskGlobalUUIDCounter = 0;
	m_TerminateRequested = false;
	m_WorkerPoolSize = a_WorkerCount;
	m_WorkerPool = new std::thread[ m_WorkerPoolSize ];

	for ( uint32_t i = 0; i < m_WorkerPoolSize; ++i )
	{
		SetGetIdleFlag( true, i, true );

		WORKER( i ) = std::thread( []( ThreadManager * a_ThreadManager, uint32_t a_WorkerIndex )
			{
				a_ThreadManager->WorkerEntryPoint( a_ThreadManager, a_WorkerIndex );
			}, 
		this, i );
	}
}

//=====================================================================================
void ThreadManager::Tick( float a_DeltaTime )
{
	uint32_t queuedTasks;
	uint32_t idleThreads;

	bool workersRequireWakeUp = false;

	for ( uint32_t i = 0; i < m_WorkerPoolSize; ++i )
	{
		queuedTasks = m_TaskQueue.Count();
		idleThreads = m_IdleFlags.Count();

		if ( queuedTasks == 0 || idleThreads == 0 )
		{
			if ( queuedTasks == 0 && idleThreads == m_WorkerPoolSize )
			{
				//no tasks in the system, we can safely reset the uuid counter again.
				m_TaskGlobalUUIDCounter = 0;
			}

			break;
		}

		// we found an idle worker, assign the next task to them
		if ( SetGetIdleFlag( false, i ) )
		{
			SetGetIdleFlag( true, i, false );

			AsyncTask task = *m_TaskQueue.Peek();
			m_AssignedTasks[ i ] = task;
			m_TaskQueue.Pop();

			workersRequireWakeUp = true;
		}
	}


	if ( workersRequireWakeUp )
	{
		// wake up any sleeping workers because we have tasks for them to do now.
		m_WorkerCV.NotifyAll();
	}
}

//=====================================================================================
void ThreadManager::Finalise()
{
	// set the global termination flag so any workers that are cycling will receive the command this way.
	m_TerminateRequested = true;
	m_WorkerCV.NotifyAll();

	for ( uint32_t i = 0; i < m_WorkerPoolSize; ++i )
	{
		if ( WORKER( i ).joinable() )
		{
			WORKER( i ).join();
		}
	}

	delete[] static_cast< std::thread* >( m_WorkerPool );
	m_WorkerPool = nullptr;
}

//=====================================================================================
ThreadManager::AsyncTicket ThreadManager::EnqueueTask( Consumer< void* > a_Task, void * a_TaskResources )
{
	if ( m_TaskGlobalUUIDCounter >= 63 )
	{
		AsyncTicket invalidat;
		invalidat.m_Valid = false;
		return invalidat; // Return an invalid async ticket
	}

	AsyncTask asyncTask;
	asyncTask.m_Task = a_Task;
	asyncTask.m_Resource = a_TaskResources;
	asyncTask.m_TaskUUID = m_TaskGlobalUUIDCounter++;

	SetGetTaskUUIDFlag( true, asyncTask.m_TaskUUID, true );

	m_TaskQueue.Push( asyncTask );

	AsyncTicket ticket;
	ticket.m_Valid = true;
	ticket.m_Expired = false;
	ticket.m_TaskID = asyncTask.m_TaskUUID;

	return ticket;
}

//=====================================================================================
void ThreadManager::FlushTask( AsyncTicket & a_AsyncTicket )
{
	if ( !a_AsyncTicket )
	{
		return;
	}

	if ( !a_AsyncTicket.m_Expired )
	{
		ThreadManager & tm = ThreadManager::Instance();
		tm.Tick();

		m_MainFlushCV.Wait( []( void * a_Result )
		{
			ThreadManager & tm = ThreadManager::Instance();

			tm.Tick();

			// true = no task currently being computed or in the queue
			return !tm.SetGetTaskUUIDFlag( false, (uint8_t)a_Result );
		},
		(void*)a_AsyncTicket.m_TaskID );

		// Wait until a worker sends a completion signal with the task id stored in the supplied ticket.
	}

	a_AsyncTicket.m_Expired = true;
}

//=====================================================================================
bool ThreadManager::ConditionalFlushTask( AsyncTicket & a_AsyncTicket )
{
	// a ticket which has expired could still hold a task id which is being 
	// reused and the ticket will refer to the wrong task but only if the 
	// ticket expired, in this case, we want to filter the ticket out
	if ( a_AsyncTicket.m_Expired ) 
	{
		return true;
	}

	// true = no task currently being computed or in the queue
	bool result = SetGetTaskUUIDFlag( false, a_AsyncTicket.m_TaskID );

	if ( result )
	{
		return false;
	}

	a_AsyncTicket.m_Expired = true;
	return true;
}

//=====================================================================================
void ThreadManager::WorkerEntryPoint( ThreadManager * a_ThreadManager, uint32_t a_WorkerIndex )
{
	//this code runs for each thread
	while ( !a_ThreadManager->m_TerminateRequested )
	{
		AsyncTask & task = m_AssignedTasks[ a_WorkerIndex ];

		// We have been assigned something to do
		if ( task.m_Task != nullptr )
		{
			task.m_Task( task.m_Resource );
			task.m_Task = nullptr;

			// We have finished our task, mark us as idle & mark the task as complete
			a_ThreadManager->SetGetIdleFlag( true, a_WorkerIndex, true );
			a_ThreadManager->SetGetTaskUUIDFlag( true, task.m_TaskUUID, false ); // false = not a pending task; no task to do

			m_MainFlushCV.NotifyAll();// tell any threads waiting for us to complete the task that we're done.
		}

		struct Info
		{
			AsyncTask * _AsyncTaskPtr;
		} info;
		
		info._AsyncTaskPtr = &task;

		// time to go dormant until: we get worken up and we find a task ready at the door or we've been terminated.
		m_WorkerCV.Wait( []( void * a_Info )
		{
			AsyncTask * task_ = static_cast< Info* >( a_Info )->_AsyncTaskPtr;
			return ThreadManager::Instance().IsTerminating() || task_->m_Task != nullptr;
		},
		&info );
	}
}

//=====================================================================================
void ThreadManager::TerminateThread()
{
	abort();
}

//=====================================================================================
void ThreadManager::SleepThread( float a_Seconds )
{
	std::this_thread::sleep_for( std::chrono::milliseconds( static_cast< uint64_t >( a_Seconds * 1000.0F ) ) );
}

//=====================================================================================
bool ThreadManager::SetGetIdleFlag( bool a_Set, uint32_t a_Index, bool a_Value )
{
	ScopedLock lock = m_IdleFlagsMutex.Lock();
	
	if ( a_Set )
	{
		m_IdleFlags.Set( a_Index, a_Value );
	}

	return m_IdleFlags.Get( a_Index );
}

//=====================================================================================
bool ThreadManager::SetGetTaskUUIDFlag( bool a_Set, uint32_t a_Index, bool a_Value )
{
	ScopedLock lock = m_TaskUUIDFlagsMutex.Lock();
	
	if ( a_Set )
	{
		m_TaskUUIDFlags.Set( a_Index, a_Value );
	}
	
	return m_TaskUUIDFlags.Get( a_Index );
}

#undef WORKER