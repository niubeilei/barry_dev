////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LockMonitor.h
// Description:
//   
//
// Modification History:
// 10/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Thread/LockMonitor.h"
#include "Util/OmnNew.h"
#include "Thread/Mutex.h"
#include "Porting/ThreadDef.h"
#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Porting/TimeOfDay.h"


AosLockMonitor::AosLockMonitor(const OmnMutexPtr &lock)
:
mLock(OmnNew OmnMutex()),
mTargetLock(lock),
mNumRecords(0)
{
}


AosLockMonitor::~AosLockMonitor()
{
}


bool	
AosLockMonitor::enter(
		const OmnString &fname, 
		const int line)
{
	//using recursive lock
	return true;

	int thrdid = OmnGetCurrentThreadId();
	mLock->lock();
	int idx = -1;
	for (int i=0; i<mNumRecords; i++)
	{
		if (mRecords[i].mThreadId == thrdid)
		{
			switch (mRecords[i].mStatus)
			{
			case eIdle:
			case eEntered:
			case eUnlocked:
			case eFinished:
				 idx = i;
				 break;

			case eWait:
			case eLocked:
				 OmnAlarm << "Status incorrect: " << fname << ":" << line
					 << mRecords[i].toString() << enderr;
				 idx = i;
				 break;

			default:
				 OmnShouldNeverComeHere;
				 idx = i;
				 break;
			}
		}
	}

	if (idx == -1)
	{
		// Did not find the record. Add one, if possible
		aos_assert_rl(mNumRecords < eMaxRecords, mLock, false);
		idx = mNumRecords++;
	}

	mRecords[idx].mThreadId 	= thrdid;
	mRecords[idx].mStatus 		= eEntered;
	mRecords[idx].mEnterFname 	= fname;
	mRecords[idx].mEnterLine  	= line;
	mRecords[idx].mLastFname 	= fname;
	mRecords[idx].mLastLine  	= line;
	mRecords[idx].mAttemptFname = "";
	mLock->unlock();
	return true;
}


bool 	
AosLockMonitor::lock(const OmnString &fname ,const int line)
{
	mTargetLock->lock();
	return true;
	int thrdid = OmnGetCurrentThreadId();
	mLock->lock();
	int idx = -1;
	for (int i=0; i<mNumRecords; i++)
	{
		if (mRecords[i].mThreadId == thrdid)
		{
			switch (mRecords[i].mStatus)
			{
			case eIdle:
				 OmnAlarm << "Status incorrect: " << fname << ":" << line
					 << mRecords[i].toString() << enderr;
				 idx = i;
				 break;
				 
			case eEntered:
			case eUnlocked:
			case eFinished:
				 // These are the correct cases
				 idx = i;
				 break;

			case eWait:
				 OmnAlarm << "Incorrect. Lock ignored: " << fname << ":" << line
					 << mRecords[i].toString() << enderr;
				 mLock->unlock();
				 return false;

			case eLocked:
				 // We will not double lock it.
				 OmnAlarm << "To double lock: \n" << fname << ":" << line
					 << mRecords[i].toString() << enderr;
				 mLock->unlock();
				 return false;

			default:
				 OmnShouldNeverComeHere;
				 mLock->unlock();
				 return false;
			}
		}
	}

	if (idx == -1)
	{
		// Did not find the record. Add one, if possible
		OmnAlarm << "Did not find the record: " << fname << ":"  << line << enderr;
		aos_assert_rl(mNumRecords < eMaxRecords, mLock, false);
		idx = mNumRecords++;
	}

	mRecords[idx].mThreadId 		= thrdid;
	mRecords[idx].mStatus 			= eTryLock;
	mRecords[idx].mTryingLockFname  = fname;
	mRecords[idx].mTryingLockLine  	= line;
	mRecords[idx].mLastFname 		= fname;
	mRecords[idx].mLastLine  		= line;
	mRecords[idx].mTryingLockTime 	= OmnGetSecond();
	mRecords[idx].mAttemptFname = "";

	mLock->unlock();
	mTargetLock->lock();
	mLock->lock();
	mRecords[idx].mThreadId 	= thrdid;
	mRecords[idx].mStatus 		= eLocked;
	mRecords[idx].mLockedFname  = fname;
	mRecords[idx].mLockedLine  	= line;
	mRecords[idx].mLastFname 	= fname;
	mRecords[idx].mLastLine  	= line;
	mRecords[idx].mAttemptFname = "";
	mRecords[idx].mLockedTime = OmnGetSecond();
	mLock->unlock();
	return true;
}


bool 	
AosLockMonitor::wait(const OmnString &fname ,const int line)
{
	return true;
	int thrdid = OmnGetCurrentThreadId();
	mLock->lock();
	int idx = -1;
	for (int i=0; i<mNumRecords; i++)
	{
		if (mRecords[i].mThreadId == thrdid)
		{
			switch (mRecords[i].mStatus)
			{
			case eIdle:
			case eEntered:
			case eUnlocked:
			case eFinished:
			case eWait:
				 OmnAlarm << "Status incorrect: " << fname << ":" << line
					 << mRecords[i].toString() << enderr;
				 idx = i;
				 mLock->unlock();
				 return false;
				 
			case eLocked:
				 idx = i;
				 break;

			default:
				 OmnShouldNeverComeHere;
				 mLock->unlock();
				 return false;
			}
		}
	}

	if (idx == -1)
	{
		// Did not find the record. Add one, if possible
		OmnAlarm << "Did not find the record: " << fname << ":"  << line << enderr;
		aos_assert_rl(mNumRecords < eMaxRecords, mLock, false);
		idx = mNumRecords++;
	}

	mRecords[idx].mThreadId 	= thrdid;
	mRecords[idx].mStatus 		= eWait;
	mRecords[idx].mWaitFname  	= fname;
	mRecords[idx].mWaitLine  	= line;
	mRecords[idx].mLastFname 	= fname;
	mRecords[idx].mLastLine  	= line;
	mLock->unlock();
	return true;
}


bool	
AosLockMonitor::unlock(const OmnString &fname ,const int line)
{
	mTargetLock->unlock();
	return true;
	int thrdid = OmnGetCurrentThreadId();
	mLock->lock();
	int idx = -1;
	for (int i=0; i<mNumRecords; i++)
	{
		if (mRecords[i].mThreadId == thrdid)
		{
			switch (mRecords[i].mStatus)
			{
			case eIdle:
			case eEntered:
			case eUnlocked:
			case eFinished:
				 OmnAlarm << "Inlegal Unlock: " << fname << ":" << line 
					 << mRecords[i].toString() << enderr;
				 // We will not unlock it.
				 // These are the correct cases
				 mLock->unlock();
				 return false;

			case eWait:
			case eLocked:
				 idx = i;
				 break;

			default:
				 OmnShouldNeverComeHere;
				 mLock->unlock();
				 return false;
			}
		}
	}

	if (idx == -1)
	{
		OmnAlarm << "Record not found: " << fname << ":" << line << thrdid << enderr;
		aos_assert_rl(mNumRecords < eMaxRecords, mLock, false);
		idx = mNumRecords++;
	}

	mRecords[idx].mThreadId 		= thrdid;
	mRecords[idx].mStatus 			= eUnlocked;
	mRecords[idx].mUnlockedFname 	= fname;
	mRecords[idx].mUnlockedLine  	= line;
	mRecords[idx].mLastFname 		= fname;
	mRecords[idx].mLastLine  		= line;
	mLock->unlock();
	mTargetLock->unlock();
	return true;
}


bool	
AosLockMonitor::finish(const OmnString &fname ,const int line)
{
	return true;
	int thrdid = OmnGetCurrentThreadId();
	mLock->lock();
	int idx = -1;
	for (int i=0; i<mNumRecords; i++)
	{
		if (mRecords[i].mThreadId == thrdid)
		{
			switch (mRecords[i].mStatus)
			{
			case eIdle:
			case eUnlocked:
			case eFinished:
			case eEntered:
				 idx = i;
				 break;

			case eWait:
			case eLocked:
				 OmnAlarm << "Forgot unlocking: " << fname << ":" << line
					 << mRecords[i].toString() << enderr;
				 mLock->unlock();
				 mTargetLock->unlock();
				 idx = i;
				 break;
	
			default:
				 OmnShouldNeverComeHere;
				 OmnAlarm << "Unknown: " << fname << ":" << line
					 << mRecords[i].toString() << enderr;
				 mLock->unlock();
				 return false;
			}
		}
	}

	if (idx == -1)
	{
		OmnAlarm << "Record not found: " << fname << ":" << line << thrdid << enderr;
		aos_assert_rl(mNumRecords < eMaxRecords, mLock, false);
		idx = mNumRecords++;
	}

	mRecords[idx].mThreadId 		= thrdid;
	mRecords[idx].mStatus 			= eFinished;
	mRecords[idx].mFinishedFname 	= fname;
	mRecords[idx].mFinishedLine  	= line;
	mRecords[idx].mLastFname 		= fname;
	mRecords[idx].mLastLine  		= line;
	mLock->unlock();
	return true;
}


bool
AosLockMonitor::checkLock()
{
	return true;
	// This function checks whether any lock has been locked for too long.
	// If yes, it means the lock was either dead locked or managed 
	// incorrectly. We will raise alarms and force unlock the lock.
	u32 crttime = OmnGetSecond();
	for (int idx=0; idx<mNumRecords; idx++)
	{
		Status status = mRecords[idx].mStatus;
		switch (status)
		{
		case eLocked:
			 if (crttime - mRecords[idx].mLockedTime > eMaxLockedTime)
			 {
				 OmnAlarm << "Lock too long: " << crttime << ":" 
					 << mRecords[idx].mLockedTime << ":" 
					 << (crttime - mRecords[idx].mLockedTime)
					 << ". " << mRecords[idx].toString() << enderr;
				 OmnScreen << "\nWill force unlock!" << endl;
				 mTargetLock->unlock();
				 return false;
			 }
			 break;

		default:
			 return true;
		}
	}
	return true;
}


OmnString
AosLockMonitor::toString() const
{
	OmnString contents;
	for (int i=0; i<mNumRecords; i++)
	{
		contents << mRecords[i].toString();
	}
	return contents;
}


bool	
AosLockMonitor::attempt(const OmnString &fname, const int line)
{
	return true;
	int thrdid = OmnGetCurrentThreadId();
	mLock->lock();
	int idx = -1;
	for (int i=0; i<mNumRecords; i++)
	{
		if (mRecords[i].mThreadId == thrdid)
		{
			idx = i;
			break;
		}
	}

	if (idx == -1)
	{
		OmnAlarm << "Record not found: " << fname << ":" << line << thrdid << enderr;
		mLock->unlock();
		return false;
	}

	mRecords[idx].mAttemptFname << ":" << fname << ":" << line;
	mLock->unlock();
	return true;
}

