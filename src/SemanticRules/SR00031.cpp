////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// For the definition of SR00031 that checks double locking. Please 
// refer to the document for more details.  
//
// Modification History:
// 12/07/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticRules/SR00031.h"

#include "Alarm/Alarm.h"
#include "alarm/Alarm.h"
#include "Debug/ErrorMgr.h"
#include "Event/Event.h"
#include "SemanticObj/Ptrs.h"
#include "SemanticObj/LockSO.h"
#include "SemanticObj/ThreadSO.h"
#include "Semantics/Semantics.h"
#include "Semantics/SemanticsRuntime.h"



AosSR00031::AosSR00031() 
:
AosSemanticRuleCommon(AosRuleId::eSR00031, "SR00031", 0, 0)
{
}


AosSR00031::~AosSR00031()
{
}


int	
AosSR00031::evaluate(const AosVarContainerPtr &data, 
					 std::string &errmsg)
{
	return 0;
}


bool 
AosSR00031::procEvent(const AosEventPtr &event, 
					  bool &cont)
{
	cont = true;
	switch (event->getEventId())
	{
	case AosEventId::eToLock:
		 return checkToLockEvent(event, cont);

	case AosEventId::eToUnlock:
		 return checkToUnlockEvent(event, cont);

	case AosEventId::eLocked:
		 return checkLockedEvent(event, cont);

	case AosEventId::eUnlocked:
		 return checkUnlockedEvent(event, cont);

	default:
		 OmnAlarm << "This is an unsolicited event: " 
			 << event->toString() << enderr;
		 return true;
	}

	aos_should_never_come_here;
	return false;
}


// 
// Someone tries to lock a lock. It checks whether the lock 
// was locked. If not, it does nothing. If yes, it traces
// the thread that locked the lock. If that thread is not
// trying to acquire another lock, no deadlock is detected. 
// If yes, it continues the back trace until it is certain
// whether a deadlock is present.
//
bool
AosSR00031::checkToLockEvent(const AosEventPtr &event, bool &cont)
{
	// 
	// Someone wants to lock a lock. We need to check whether it is 
	// the lock this rule is managing. 
	//
	cont = true;
	aos_assert_r(event->getEventId() == AosEventId::eToLock, false);
	void *lock = event->getEventData();
	aos_assert_r(lock, false);
	AosLockSOPtr lockSo = AosSemanticsRuntimeSelf->getLock(lock);
	aos_assert_r(lockSo, false);

	std::list<AosThreadSOPtr> threads;
	std::list<AosLockSOPtr> locks;
	AosThreadSOPtr threadSo;

	aos_decl_loop_safety_guard(guard);
	while (1)
	{
		aos_sr_loop_safety_guard_r(guard, 100, true);

		locks.push_back(lockSo);
		if (!lockSo->isLocked()) 
		{
			// 
			// The lock is not locked. No problem.
			//
			return true;
		}

		// 
		// The lock is locked. Retrieve the thread that owns the lock.
		//
		threadSo = lockSo->getOwner();
		aos_assert_r(threadSo, false);

		if (!threadSo->isTryingToLock())
		{
			// 
			// No deadlock is detected. 
			//
			return true;
		}

		// 
		// The thread is trying to acquire another lock. 
		// Time to check whether a deadlock is already formed
		//
		std::list<AosThreadSOPtr>::iterator itr;
		for (itr = threads.begin(); itr != threads.end(); itr++)
		{
			if (threadSo == (*itr))
			{
				// 
				// A deadlock is found!!!
				//
				generateError(threads, locks, event);
				return true;
			}
		}

		// 
		// No deadlock found yet.
		//
		threads.push_back(threadSo);
		lockSo = threadSo->getPendingLock();
		if (!lockSo)
		{
			// 
			// Note that this may happen because it may be the case
			// that when we ask "isTryingToLock()", it answered yes, 
			// but when we call "getPendingLock()", it got the lock, 
			// so it would return 0.
			//
			// If this happens, it means no deadlock is possible.
			//
			return true;
		}
	}

	aos_should_never_come_here;
	return true;
}


// 
// Someone tries to unlock a lock. If the lock was not locked, this
// is an error.
//
bool
AosSR00031::checkToUnlockEvent(const AosEventPtr &event, bool &cont)
{
	return true;
}


// 
// A lock was locked. 
//
bool
AosSR00031::checkLockedEvent(const AosEventPtr &event, bool &cont)
{
	return true;
}


// 
// A lock was unlocked
//
bool
AosSR00031::checkUnlockedEvent(const AosEventPtr &event, bool &cont)
{
	return true;
}

