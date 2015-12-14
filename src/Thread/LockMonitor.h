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
#ifndef Omn_Thread_LockMonitor_h 
#define Omn_Thread_LockMonitor_h

#include "Semantics/Semantics.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"


#ifndef AOSLMTR_ENTER
#define AOSLMTR_ENTER(x) (x)->enter(__FILE__, __LINE__)
#endif

#ifndef AOSLMTR_FINISH
#define AOSLMTR_FINISH(x) (x)->finish(__FILE__, __LINE__)
#endif

#ifndef AOSLMTR_LOCK
#define AOSLMTR_LOCK(x) (x)->lock(__FILE__, __LINE__)
#endif

#ifndef AOSLMTR_UNLOCK
#define AOSLMTR_UNLOCK(x) (x)->unlock(__FILE__, __LINE__)
#endif

#ifndef AOSLMTR_ENTER1
#define AOSLMTR_ENTER1(x,y) if(!y){(x)->enter(__FILE__, __LINE__);}
#endif

#ifndef AOSLMTR_FINISH1
#define AOSLMTR_FINISH1(x,y) if(!y){(x)->finish(__FILE__, __LINE__);}
#endif

#ifndef AOSLMTR_LOCK1
#define AOSLMTR_LOCK1(x,y) if(!y){(x)->lock(__FILE__, __LINE__);}
#endif

#ifndef AOSLMTR_UNLOCK1
#define AOSLMTR_UNLOCK1(x,y) if(!y){(x)->unlock(__FILE__, __LINE__);}
#endif

#ifndef AOSLMTR_WAIT
#define AOSLMTR_WAIT(x) (x)->wait(__FILE__, __LINE__)
#endif

#ifndef AOSLMTR_ATTEMPT
#define AOSLMTR_ATTEMPT(x) (x)->attempt(__FILE__, __LINE__)
#endif

class AosLockMonitor : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxRecords		= 50,
		eMaxLockedTime 	= 10
	};

	enum Status
	{
		eIdle,
		eEntered,
		eLocked,
		eUnlocked,
		eWait,
		eFinished,
		eTryLock
	};

	struct Record
	{
		int			mThreadId;
		OmnString	mEnterFname;
		int			mEnterLine;
		OmnString	mTryingLockFname;
		int			mTryingLockLine;
		u32			mTryingLockTime;
		OmnString	mLockedFname;
		int			mLockedLine;
		u32			mLockedTime;
		OmnString	mUnlockedFname;
		int			mUnlockedLine;
		OmnString	mWaitFname;
		int			mWaitLine;
		OmnString	mFinishedFname;
		int			mFinishedLine;
		OmnString	mAttemptFname;

		OmnString	mLastFname;
		int			mLastLine;

		Status		mStatus;

		Record()
		:
		mThreadId(-1),
		mEnterLine(-1),
		mTryingLockLine(-1),
		mTryingLockTime(0),
		mLockedLine(-1),
		mLockedTime(0),
		mUnlockedLine(-1),
		mWaitLine(-1),
		mLastLine(-1),
		mStatus(eIdle)
		{
		}

		void reset()
		{
			mThreadId = -1;
			mEnterFname = "";
			mEnterLine = -1;
			mTryingLockFname = "";
			mTryingLockLine = -1;
			mTryingLockTime = 0;
			mLockedFname = "";
			mLockedLine = -1;
			mLockedTime = 0;
			mUnlockedFname = "";
			mUnlockedLine = -1;
			mWaitFname = "";
			mWaitLine = -1;
			mLastFname = "";
			mLastLine = -1;
			mStatus = eIdle;
		}

		OmnString toString() const
		{
			OmnString str;
			str << "Thread:" << mThreadId << "\n"
					  << "\tEnterFname:      " << mEnterFname << "\n"
					  << "\tEnterLine:       " << mEnterLine << "\n"
					  << "\tTryingLockFname: " << mTryingLockFname << "\n"
					  << "\tTryingLockLine:  " << mTryingLockLine << "\n"
					  << "\tTryingLockTime:  " << mTryingLockTime << "\n"
					  << "\tLockFname:       " << mLockedFname << "\n"
					  << "\tLockLine:        " << mLockedLine << "\n"
					  << "\tLockTime:        " << mLockedTime << "\n"
					  << "\tUnlockFname:     " << mUnlockedFname << "\n"
					  << "\tUnlockLine:      " << mUnlockedLine << "\n"
					  << "\tWaitFname:       " << mWaitFname << "\n"
					  << "\tWaitLine:        " << mWaitLine << "\n"
					  << "\tFinishedFname:   " << mFinishedFname << "\n"
					  << "\tFinishedLine:    " << mFinishedLine << "\n"
					  << "\tLastFname:       " << mLastFname << "\n"
					  << "\tLastLine:        " << mLastLine << "\n"
					  << "\tAttempt          " << mAttemptFname << "\n"
					  << "\tStatus:          " << (int)mStatus << "\n";
			return str;
		}
	};

	OmnMutexPtr		mLock;
	OmnMutexPtr		mTargetLock;
	Record			mRecords[eMaxRecords];
	int				mNumRecords;

public:
	AosLockMonitor(const OmnMutexPtr &lock);
    ~AosLockMonitor();

	bool	enter(const OmnString &fname, const int line);
    bool 	lock(const OmnString &fname, const int line);
    bool	unlock(const OmnString &fname, const int line);
	bool	wait(const OmnString &fname, const int line);
	bool	finish(const OmnString &fname, const int line);
	bool	attempt(const OmnString &fname, const int line);
	bool	checkLock();
	OmnString toString() const;

private:
	void	setRecord(Record *record, 
				const OmnString &fname, 
				const int line, 
				const Status status);
};

#endif

