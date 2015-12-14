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
// 1. This class maintaines the fixed numbers of buckets to cache iiltrans 
// that is a sorted  array of AosIILEntry. 
// 2. Every bucket has boundary of low and hight, the entries will append
// to the matched bucket by the range. 
// 3. The bucket can automaticly ajust range.  The mechanism of mergering 
// and spliting bucket by the aging and total entries  of the bucket. 
// If the bucket is too old. it will be merged to it's left bucket or the
// right bucket, and the bucket will free. If the bucket has full, 
// the entries will be proccesed, and the bucket will free. when the entrie 
// has not in all the buckets range .it will get the  free buckets 
// to split the range.
// 
// Modification History:
// 11/17/2015 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "DataCollector/DataCollectorBuff.h"

AosDataCollectorBuff::AosDataCollectorBuff(const u64& job_id)
:
AosDataCollector(job_id)
{
}


AosDataCollectorBuff::~AosDataCollectorBuff()
{
}


bool
AosDataCollectorBuff::config(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	mDataColId = conf->getAttrStr(AOSTAG_NAME, "");	
	aos_assert_r(mDataColId != "", false);

	mLogicId = mDataColId;
	mType = eBuff;

	mDataColTag = conf->clone(AosMemoryCheckerArgsBegin);
	mIsInited = true;
	return true;
}


bool 
AosDataCollectorBuff::addOutput(
		const AosXmlTagPtr &output, 
		const AosRundataPtr &rdata)
{
	return true;
}


bool 
AosDataCollectorBuff::finishDataCollector(const AosRundataPtr &rdata)
{
	aos_assert_r(mStartNum > 0, false);
	mStartNum--;
	if (mStartNum > 0)
	{
		return true;
	}

	mFinished = true;

	mLock->lock();
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);  
	if (!job)
	{
		OmnScreen << "jozhi job restart, may be not started : " << mJobDocid << endl;
		mLock->unlock();
		return true;
	}

	bool rslt = job->datacollectorFinished(rdata, mLogicId, mTaskIds);		
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();

	return true;
}
