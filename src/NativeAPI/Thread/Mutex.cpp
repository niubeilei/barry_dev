////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Mutex.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Thread/Mutex.h"

#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h"
#include "Porting/Mutex.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/ThreadDef.h"
#include "Event/ThreadEvents.h"
#include "Thread/LockMonitor.h"
#include "Thread/ThreadMgr.h"


OmnMutex::OmnMutex()
:
mRecurSive(false),
mLockTime(0),
mLine(-1)
{
	AosSemanSet(mClassName, "OmnMutex");
	AosSemanSet(mInstName, "NoName");
	AosSemanSet(mSemanFlag, false);

	init();
}


OmnMutex::OmnMutex(bool recursive)
:
mRecurSive(recursive),
mLockTime(0),
mLine(-1)
{
	AosSemanSet(mClassName, "OmnMutex");
	AosSemanSet(mInstName, "NoName");
	AosSemanSet(mSemanFlag, false);
	init();
}


OmnMutex::OmnMutex(const OmnString &name)
:
mRecurSive(false),
mLockTime(0),
mLine(-1)
{
	AosSemanSet(mClassName, "OmnMutex");
	AosSemanSet(mInstName, name);
	AosSemanSet(mSemanFlag, false);

	init();
}


OmnMutex::~OmnMutex()
{
	OmnDestroyMutex(mLock);
	if (mRecurSive)
		OmnDestroyMutexAttr(mLockAttr);
}


bool
OmnMutex::init()
{
    //
    // Creating the mutex. This is platform dependent function call.
    //
	if (mRecurSive)
	{
		OmnInitMutexAttr(mLockAttr);
		OmnSetMutexAttrRecursive(mLockAttr);
		OmnInitMutex(mLock, &mLockAttr);
		return true;
	}
	OmnInitMutex(mLock);
	return true;
}


bool
OmnMutex::lock()
{
	aos_se_to_lock(mSemanFlag, this, "", mClassName, mInstName);

	if (OmnMutexLock(mLock))
	{
		aos_se_locked(mSemanFlag, this, "", mClassName, mInstName);
		return true;
	}
	else
	{
		//perror("Failed to lock mutex");
	}

	/*
	if (OmnAlarmMgr::isAlarmOn())
	{
		OmnAlarm << "Failed to lock mutex. Errno = " << OmnGetErrno() << enderr;
	}
	else
	{
		OmnScreen << "********** Failed to lock mutex. Errno = "
			<< OmnGetErrno() << endl;
	}
	*/
	return false;
}

bool
OmnMutex::trylock()
{
	return OmnMutexTryLock(mLock);
}

bool
OmnMutex::unlock()
{
	if (OmnMutexUnlock(mLock))
	{
		aos_se_unlocked(mSemanFlag, this, "", mClassName, mInstName);
		return true;
	}
	else
	{
		//perror("Failed to unlock mutex");
	}

	/*
	if (OmnAlarmMgr::isAlarmOn())
	{
		OmnAlarm << "Failed to unlock the mutex. Errno = " 
			<< OmnGetStrError(OmnErrType::eSystem) 
			<< ":" << (int)OmnGetCurrentThreadId() << " Lock: " << (unsigned long)&mLock << enderr;
	}
	else
	{
		OmnScreen << "********** Failed to lock mutex. The network has not started up" << endl;
	}
	*/

	return false;
}


void OmnMutex::setSemanFlag(bool f) 
{
	cout << "To turn semanflag on: " << this << endl;
//	mSemanFlag = f;
}
