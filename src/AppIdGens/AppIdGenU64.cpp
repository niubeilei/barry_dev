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
// An ID is defined by a doc. 
//
// Modification History:
// 01/06/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "AppIdGens/AppIdGenU64.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "UtilData/JPID.h"
#include "UtilData/JSID.h"
#include "XmlUtil/XmlTag.h"


OmnThreadPoolPtr AosAppIdGenU64::smThreadPool = OmnNew OmnThreadPool("app_id_gen_u64", __FILE__, __LINE__);
	
AosAppIdGenU64::AosAppIdGenU64(
		const AosRundataPtr &rdata,
		const OmnString &objid)
:
mLock(OmnNew OmnMutex()),
mObjid(objid),
mNoRegionCrtId(0),
mNoRegionNextId(0),
mBlockSize(eDftBlockSize),
mInitValue(eDftInitValue),
mMaxId(0),
mPrefix(0),
mThreadRunning(false)
{
	mIILName = getIILName();
	bool rslt = start(rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosAppIdGenU64::AosAppIdGenU64(
		const AosRundataPtr &rdata,
		const OmnString &objid, 
		const u64 &block_size,
		const u64 &init_value,
		const u64 &max_id,
		const u64 &prefix)
:
mLock(OmnNew OmnMutex()),
mObjid(objid),
mNoRegionCrtId(0),
mNoRegionNextId(0),
mBlockSize(block_size),
mInitValue(init_value),
mMaxId(max_id),
mPrefix(prefix),
mThreadRunning(false)
{
	mIILName = getIILName();
	bool rslt = start(rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosAppIdGenU64::~AosAppIdGenU64()
{
}


OmnString
AosAppIdGenU64::getIILName() const
{
	return AosIILName::composeAppIdGenIILName();
}


bool
AosAppIdGenU64::start(const AosRundataPtr &rdata)
{
	aos_assert_r(rdata, false);

// TMP
rdata->setJPID(AOSJPID_SYSTEM);
rdata->setJSID(AOSJSID_SYSTEM);

	// It retrieves the definition from the database and
	// initializes itself.
	AosXmlTagPtr doc = AosGetDocByObjid(mObjid, rdata);
	if (doc)
	{
		mBlockSize = doc->getAttrU64(AOSTAG_BLOCKSIZE, eDftBlockSize);
		if (mBlockSize <= 0) mBlockSize = eDftBlockSize;

		mInitValue = doc->getAttrU64(AOSTAG_INITVALUE, eDftInitValue);
		mMaxId = doc->getAttrU64(AOSTAG_MAX_ID, 0);
		mPrefix = doc->getAttrU64(AOSTAG_PREFIX, 0);
	}

	mLock->lock();
	if (mNoRegionCrtId >= mNoRegionNextId)
	{
		// Need to load new IILIDs
		aos_assert_rl(mBlockSize > 0, mLock, 0);
		u64 new_id = 0;
		bool rslt = AosIncrementKeyedValue(mIILName, mObjid, new_id, 
				true, mBlockSize, mInitValue, true, rdata);
		aos_assert_rl(rslt, mLock, false);

		mNoRegionNextId = new_id + mBlockSize;
		mNoRegionCrtId = new_id;
	}
	mLock->unlock();
	// Retrieve the value from the database
	// u64 new_id = 0;
	// bool rslt = AosIncrementKeyedValue(mIILName, mObjid, 
	// 		new_id, true, mBlockSize, mInitValue, true, rdata);
	// aos_assert_rr(rslt, rdata, false);
	// 
	// mNextId = new_id + mBlockSize;
	// mCrtId = new_id;
	// mNumRegions = AosGetNumRegions();
	// aos_assert_rr(mNumRegions > 0, rdata, false);

	// Chen Ding, 2013/05/07
	// This is not used anymore.
	// for (u32 i=0; i<(u32)mNumRegions; i++)
	// {
	// 	mCrtId[i] = 0;
	// 	mNextId[i] = 0;
	// }
	return true;
}


/*
u64
AosAppIdGenU64::getNextId(
		const AosRundataPtr &rdata, 
		const u32 &region_id)
{
	aos_assert_r(rdata, 0);
	aos_assert_rr(rdata->getSiteid() > 0, rdata, 0);
	aos_assert_rr(region_id < (u32)mNumRegions, rdata, 0);
	mLock->lock();
	if (mCrtId[region_id] >= mNextId[region_id])
	{
		// Need to load new IILIDs
		aos_assert_rl(mBlockSize > 0, mLock, 0);
		u64 new_id = 0;
		OmnString key = mObjid;
		key << "_" << region_id;
		bool rslt = AosIncrementKeyedValue(mIILName, key, new_id, 
				true, mBlockSize, mInitValue, true, rdata);
		aos_assert_rl(rslt, mLock, false);

		mNextId[region_id] = new_id + mBlockSize;
		mCrtId[region_id] = new_id;
	}
	u64 id = (mNumRegions * mCrtId[region_id]++) + region_id;
	if (mMaxId > 0 && id > mMaxId)
	{
		OmnString key = mObjid;
		key << "_" << region_id;
		u64 new_id = 0;
		bool rslt = AosResetKeyedValue(rdata, mIILName, key, new_id, true, 
				mBlockSize);
		aos_assert_rl(rslt, mLock, false);
		mNextId[region_id] = new_id + mBlockSize;
		mCrtId[region_id] = new_id;
		id = (mNumRegions * mCrtId[region_id]++) + region_id;
	}
	mLock->unlock();
	return mPrefix + id;
}
*/


u64
AosAppIdGenU64::getNextId(const AosRundataPtr &rdata) 
{
	aos_assert_r(rdata, 0);
	aos_assert_rr(rdata->getSiteid() > 0, rdata, 0);
	mLock->lock();
	i64 id_left = mNoRegionNextId - mNoRegionCrtId;
	
	// temp way for thread lock
	if(id_left > 0 && id_left < (int)(mBlockSize*4/5) && !mThreadRunning )
	{
		mThreadRunning = true;
		mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
		// start the thread to get some id for prepare
		AosAppIdGenU64Ptr thisPtr(this, false);
		AosAppIdGenU64ThrdPtr runner = OmnNew AosAppIdGenU64Thrd(thisPtr,rdata);
		vector<OmnThrdShellProcPtr> runners;
		runners.push_back(runner);
		smThreadPool->procAsync(runners);
		

//		mThread = OmnNew OmnThread(thisPtr, "AppIdGenU64", 0, true, true, __FILE__, __LINE__);
//		mThread->start();
	}
	
	
	if (mNoRegionCrtId >= mNoRegionNextId)
	{
		// Need to load new IILIDs
		aos_assert_rl(mBlockSize > 0, mLock, 0);
		u64 new_id = 0;
		bool rslt = AosIncrementKeyedValue(mIILName, mObjid, new_id, 
				true, mBlockSize, mInitValue, true, rdata);
		aos_assert_rl(rslt, mLock, false);

		mNoRegionNextId = new_id + mBlockSize;
		mNoRegionCrtId = new_id;
	}
	u64 id = mNoRegionCrtId++;
	if (mMaxId > 0 && id > mMaxId)
	{
		u64 new_id = 0;
		bool rslt = AosResetKeyedValue(rdata, mIILName, mObjid, new_id, true, 
				mBlockSize);
		aos_assert_rl(rslt, mLock, false);
		mNoRegionNextId = new_id + mBlockSize;
		mNoRegionCrtId = new_id;
		id = mNoRegionCrtId++;
	}
	mLock->unlock();
	return mPrefix + id;
}

bool    
AosAppIdGenU64::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	aos_assert_r(mBlockSize > 0, 0);
	u64 new_id = 0;
	mLock->lock();
	AosRundataPtr rdata = mRundata->clone(AosMemoryCheckerArgsBegin);
	mLock->unlock();
	AosIncrementKeyedValue(mIILName, mObjid, new_id, 
			true, mBlockSize, mInitValue, true, rdata);

	OmnScreen << "New ID Group:" << new_id << ":" << mBlockSize << ":" << mIILName << endl; 
	mLock->lock();
	mNoRegionNextId = new_id + mBlockSize;
	mNoRegionCrtId = new_id;
	mThreadRunning = false;
	mLock->unlock();
	return true;
}

bool    
AosAppIdGenU64::proc()
{
	aos_assert_r(mBlockSize > 0, 0);
	u64 new_id = 0;
	mLock->lock();
	AosRundataPtr rdata = mRundata->clone(AosMemoryCheckerArgsBegin);
	mLock->unlock();
	bool rslt = AosIncrementKeyedValue(mIILName, mObjid, new_id, 
			true, mBlockSize, mInitValue, true, rdata);
	aos_assert_rl(rslt,mLock,false);
	OmnScreen << "New ID Group:" << new_id << ":" << mBlockSize << ":" << mIILName << endl; 
	mLock->lock();
	mNoRegionNextId = new_id + mBlockSize;
	mNoRegionCrtId = new_id;
	mThreadRunning = false;
	mLock->unlock();
	return true;
}

bool    
AosAppIdGenU64::signal(const int threadLogicId)
{
	return true;
}

AosAppIdGenU64Thrd::AosAppIdGenU64Thrd(
		const AosAppIdGenU64Ptr   idgen,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("AppIdGenU64Thrd"),
mIDGen(idgen),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
}

AosAppIdGenU64Thrd::~AosAppIdGenU64Thrd()
{
}

bool
AosAppIdGenU64Thrd::run()
{
	aos_assert_r(mIDGen,false);
	mIDGen->proc();
	return true;
}
