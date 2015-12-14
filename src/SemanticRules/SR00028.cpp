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
// For the definition of SR00028 that checks double locking. Please 
// refer to the document for more details.  
//
// Modification History:
// 12/05/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticRules/SR00028.h"

#include "Alarm/Alarm.h"
#include "alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/ErrorMgr.h"
#include "Event/Event.h"
#include "Util/string.h"



AosSR00028::AosSR00028(const std::string &thread_name, 
			   		   const std::string &lockname, 
			   		   void *lock)
:
AosSemanticRuleCommon(AosRuleId::eSR00028, "SR00028", lock, 4),
mThreadName(thread_name),
mLockName(lockname)
{
	addEvent(AosEventId::eToLock, lock, 4);
	addEvent(AosEventId::eToUnlock, lock, 4);
	addEvent(AosEventId::eLocked, lock, 4);
	addEvent(AosEventId::eUnlocked, lock, 4);
}


AosSR00028::~AosSR00028()
{
}


int	
AosSR00028::evaluate(const AosVarContainerPtr &data, 
						 std::string &errmsg)
{
	return 0;
}


bool 
AosSR00028::procEvent(const AosEventPtr &event, 
					  bool &cont)
{
	OmnTrace << "Received event: " << event->toString() << endl;

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
// was already locked. If yes, an error is detected. 
//
bool
AosSR00028::checkToLockEvent(const AosEventPtr &event, bool &cont)
{
	// 
	// Someone wants to lock a lock. We need to check whether it is 
	// the lock this rule is managing. 
	//
	cont = true;
	aos_assert_r(event->getEventId() == AosEventId::eToLock, false);
	void *ptr = event->getEventData();
	aos_assert_r(ptr == mUserData, false);
	std::string errmsg;

	// 
	// Check whether the lock is already locked. If yes, 
	// it is an error.
	//
	if (mActivities.checkActivity(AosEventId::eLocked, mUserData))
	{
		// 
		// This means the lock has been locked. This is a double 
		// locking attempt. 
		//
		errmsg << "Rule-00028: Error Detected!!! Thread: "
			<< mThreadName << " tries to double lock on: '" 
			<< mLockName
			<< "'.\nActivity Track: \n"
			<< mActivities.toString();
		AosRaiseError(errmsg);
	}

	// 
	// Log the activity. Note that the activity tracker 
	// also checks for errors. 
	//
	logActivity(AosEventId::eToLock, event);
	return true;
}


// 
// Someone tries to unlock a lock. If the lock was not locked, this
// is an error.
//
bool
AosSR00028::checkToUnlockEvent(const AosEventPtr &event, bool &cont)
{
	cont = true;
	aos_assert_r(event->getEventId() == AosEventId::eToUnlock, false);
	void *ptr = event->getEventData();
	aos_assert_r(ptr == mUserData, false);

	// 
	// Check whether the lock was already locked.
	//
	if (!mActivities.checkActivity(AosEventId::eLocked, mUserData))
	{
		// 
		// This means the lock was not locked by this thread. This is 
		// an error. 
		//
		std::string errmsg = "Rule-00028: Error Detected!!! Thread: ";
		errmsg << mThreadName << " tries to unlock the lock: '" 
			<< mLockName 
			<< "' that was not locked by this thread!";
		AosRaiseError(errmsg);
		return true;
	}

	logActivity(AosEventId::eToLock, event);
	return true;
}


// 
// A lock was locked. 
//
bool
AosSR00028::checkLockedEvent(const AosEventPtr &event, bool &cont)
{
	cont = true;
	aos_assert_r(event->getEventId() == AosEventId::eLocked, false);
	void *ptr = event->getEventData();
	aos_assert_r(ptr == mUserData, false);

	logActivity(AosEventId::eToLock, event);
	return true;
}


// 
// A lock was unlocked
//
bool
AosSR00028::checkUnlockedEvent(const AosEventPtr &event, bool &cont)
{
	logActivity(AosEventId::eToLock, event);
	return true;
}

