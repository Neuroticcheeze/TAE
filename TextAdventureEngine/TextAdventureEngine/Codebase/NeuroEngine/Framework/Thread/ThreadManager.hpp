#pragma once
#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Utils/Function.hpp>
#include <Framework/Utils/BitField.hpp>
#include <Framework/Containers/Queue.hpp>

#include "Mutex.hpp"
#include "ConditionVariable.hpp"

//=====================================================================================
class ThreadManager final : public TickableManager< ThreadManager, uint32_t >
{
public:

	struct AsyncTicket
	{
		bool Expired() const
		{
			return m_Expired;
		}

		/* Was the task successfully enqueued? Is this a valid ticket? */
		operator bool() const
		{
			return m_Valid;
		}

	private:
		friend class ThreadManager;

		uint8_t m_TaskID;
		bool m_Expired;
		bool m_Valid;
	};

	void Init( uint32_t a_WorkerCount = 3 );
	void Tick( float a_DeltaTime = 0.0F );
	void Finalise();

	AsyncTicket EnqueueTask( Consumer< void* > a_Task, void * a_TaskResources = nullptr );

	/* Will stop a thread's execution until the task is complete. */
	void FlushTask( AsyncTicket & a_AsyncTicket );

	/* Will only return true if the task is complete, or the ticket has expired. */
	bool ConditionalFlushTask( AsyncTicket & a_AsyncTicket );

	inline bool IsTerminating() const
	{
		return m_TerminateRequested;
	}

	static void TerminateThread();
	static void SleepThread( float a_Seconds );
	
private:

	struct AsyncTask
	{
		Consumer< void* > m_Task;
		void * m_Resource;
		uint8_t m_TaskUUID;
	};

	void WorkerEntryPoint( ThreadManager * a_ThreadManager, uint32_t a_WorkerIndex );
	bool SetGetIdleFlag( bool a_Set, uint32_t a_Index, bool a_Value = false );
	bool SetGetTaskUUIDFlag( bool a_Set, uint32_t a_Index, bool a_Value = false );

	uint32_t m_WorkerPoolSize;
	void * m_WorkerPool;
	bool m_TerminateRequested;
	uint8_t m_TaskGlobalUUIDCounter;
	BitField< uint64_t, 64 > m_TaskUUIDFlags; //track task completion (biggest bitfiels possible = 64 bits, so we can only have 64 tasks queued up / computing at a time.
	Mutex m_IdleFlagsMutex, m_TaskUUIDFlagsMutex;
	BitField< uint64_t, 64 > m_IdleFlags; //track which workers are idle
	AsyncTask m_AssignedTasks[ 64 ];
	Queue< AsyncTask > m_TaskQueue;
	ConditionVariable m_MainFlushCV;
	ConditionVariable m_WorkerCV;
};

#endif//THREADMANAGER_H