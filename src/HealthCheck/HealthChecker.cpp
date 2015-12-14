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
// The class maintains a list of all the outstanding requests. When a new
// request is created, it should call 'addEntry(...)' to add the request
// to this class. When the request finishes, it should call 'removeEntry(...)'
// to remove from this class. 
//
// The class assumes that requests are identified by transids. It is the
// caller's responsibility to ensure that transids are unique and sequential.
//
// Entries are separated into buckets. Entries in each bucket is in the 
// same age. The class will grow the age. Among all the ages, there is one
// that is considered 'too old'. 
//
// Memory Usage Analysis:
// The class uses two array:
// 		int	mCounters[num_counters];
// 		u8	mAgeIdx[num_entries];
// The max number of counters is small (max = 255). The memory is determined
// by mAgeIdx[num_entries]. If the number of entries is 1,000,000, the
// memory usage is about 1M. 
//
// Modification History:
// 09/06/2010	by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HealthCheck/HealthChecker.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "HealthCheck/HealthCheckObj.h"
#include "HealthCheck/HealthCheckReq.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"


AosHealthChecker::AosHealthChecker(
		const AosHealthCheckObjPtr &caller, 
		const int freq,
		const int maxAges, 
		const int maxEntries)
:
mLock(OmnNew OmnMutex()),
mCaller(caller),
mNumAges(maxAges),
mNumEntries(maxEntries),
mCrtAgeIdx(1),
mFreq(freq)
{
	if (mFreq <= 0) mFreq = eDftFreq;

	// Note that Age 0 is reserved for error checking
	aos_assert(mNumAges > 0);
	if (mNumAges > eMaxAges) mNumAges = eMaxAges;

	aos_assert(mNumEntries > 0);
	if (mNumEntries > eMaxEntries) mNumEntries = eMaxEntries;

	// Create mCounters
	mCounters = OmnNew int[mNumAges];
	aos_assert(mCounters);
	memset(mCounters, 0, 4 * mNumAges);

	// Create Counter Idx
	mAgeIdx = OmnNew u8[mNumEntries];
	aos_assert(mAgeIdx);
	memset(mAgeIdx, 0, mNumEntries);

	// Create mRequests
	mRequests = OmnNew vector<AosHealthCheckReqPtr>[mNumAges];

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "HealthChecker", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosHealthChecker::~AosHealthChecker()
{
}


bool	
AosHealthChecker::addEntry(const u64 &transid)
{
	// It adds a request to the class. 'transid' is converted into 
	// entry idx. This means that the max number of entries should
	// be set big enough not to override the outstanding transactions. 
	// In addition, it increments the corresponding counter by one.
	mLock->lock();
	u64 idx = transid % mNumEntries;
	mAgeIdx[idx] = mCrtAgeIdx;
	mCounters[mCrtAgeIdx]++;
	mLock->unlock();
	return true;
}


bool
AosHealthChecker::addEntry(const AosHealthCheckReqPtr &request)
{
	// It adds a request to the class. 'transid' is converted into 
	// entry idx. This means that the max number of entries should
	// be set big enough not to override the outstanding transactions. 
	// In addition, it increments the corresponding counter by one.
	mLock->lock();
	u64 trans_id = request->getTransId();
	u64 idx = trans_id % mNumEntries;
	mAgeIdx[idx] = mCrtAgeIdx;
	mCounters[mCrtAgeIdx]++;
	int size = mRequests[mCrtAgeIdx].size();
	mRequests[mCrtAgeIdx].push_back(request);
	request->setEntryIdx((u64)size);
	mLock->unlock();
	return true;
}


bool	
AosHealthChecker::removeEntry(const u64 &transid)
{
	// It removes a request from the class. 
	mLock->lock();
	u64 idx = transid % mNumEntries;
	u8 age_idx = mAgeIdx[idx];
	aos_assert_r(age_idx > 0, false);
	aos_assert_r(mCounters[age_idx] > 0, false);
	mCounters[age_idx]--;
	mAgeIdx[idx] = 0;
	mLock->unlock();
	return true;
}


bool	
AosHealthChecker::removeEntry(const AosHealthCheckReqPtr &request)
{
	// It removes a request from the class. 
	mLock->lock();
	u64 entry_idx = request->getEntryIdx();
	u64 transid = request->getTransId();
	u64 idx = transid % mNumEntries;
	u8 age_idx = mAgeIdx[idx];
	aos_assert_r(age_idx > 0, false);
	aos_assert_r(mCounters[age_idx] > 0, false);
	mCounters[age_idx]--;
	mAgeIdx[idx] = 0;
	aos_assert_rl(entry_idx < mRequests[mCrtAgeIdx].size(), mLock, false);
	mRequests[mCrtAgeIdx][entry_idx] = 0;
	mLock->unlock();
	return true;
}


bool    
AosHealthChecker::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		//OmnWait::getSelf()->wait(mFreq, 0);
		//felicia, 2013/06/19
		//OmnSleep(mFreq);
		int index = mFreq;
		while(index--)
		{
			OmnSleep(1);
			mThreadStatus = true;
		}

		checkEntries();
	}
	return true;
}


bool    
AosHealthChecker::signal(const int threadLogicId)
{
	return true;
}

/*
bool    
AosHealthChecker::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}
*/

bool
AosHealthChecker::checkEntries()
{
	// If the oldest counter is not 0, it means something is wrong.
	mLock->lock();
	mCrtAgeIdx++;
	if (mCrtAgeIdx >= mNumAges) mCrtAgeIdx = 1;

	// 1. Find the oldest counter
	if (mCounters[mCrtAgeIdx] > 0)
	{
		//OmnAlarm << "System hangs: " << mCounters[mCrtAgeIdx] << enderr;
		OmnScreen << "System hangs: " << mCounters[mCrtAgeIdx] << endl;

		if (mCaller)
		{
			for (u64 i=0; i<mRequests[mCrtAgeIdx].size(); i++)
			{
				if (mRequests[mCrtAgeIdx][i]) 
				{
					mCaller->healthCheckFailed(mRequests[mCrtAgeIdx][i]);
				}
			}
		}
		mLock->unlock();
		systemFails();
	}

	mRequests[mCrtAgeIdx].clear();
	mLock->unlock();
	return true;
}


bool
AosHealthChecker::systemFails()
{
	if (!mCaller) return true;
	mCaller->healthCheckFailed(0);
	return true;
}

