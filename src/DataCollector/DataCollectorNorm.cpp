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
// 12/29/2014 Created by Andy zhang 
////////////////////////////////////////////////////////////////////////////
#include "DataCollector/DataCollectorNorm.h"

#include "DataCollector/DataCollectorType.h"
#include "DataAssembler/DataAssembler.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/JobMgrObj.h"
#include "Thread/ThreadPool.h"
#include "Thread/Sem.h"
#include "Porting/Sleep.h"


//AosDataCollectorNorm::AosDataCollectorNorm(
//		const AosXmlTagPtr &config, 
//		const AosRundataPtr &rdata)
//:
//AosDataCollector(config, rdata),
//mLock(OmnNew OmnMutex())
//{
//}
//


AosDataCollectorNorm::AosDataCollectorNorm(const u64& job_id)
:
AosDataCollector(job_id)
{
}


AosDataCollectorNorm::~AosDataCollectorNorm()
{
}


//bool
//AosDataCollectorNorm::config(
//		const AosXmlTagPtr &conf,
//		const AosRundataPtr &rdata)
//{
//	OmnString type = conf->getAttrStr(AOSTAG_ZKY_TYPE);
//	aos_assert_r(type != AOSDATACOLLECTOR_NORM, false);
//	
//	mDataType = conf->getAttrStr(AOSTAG_DATA_TYPE);
//	aos_assert_r(type != AOSDATACOLLECTOR_NORM, false);
//
//	mPhyNum = AosGetNumPhysicals();
//	aos_assert_r(mPhyNum >= 0, false);
//
//	mFileList.resize(mPhyNum);
//	mDataColTag = conf->clone(AosMemoryCheckerArgsBegin);
//	return true;
//}


bool
AosDataCollectorNorm::config(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	//not implement
	//bool rslt = AosDataAssembler::checkConfigStatic(conf, rdata.getPtrNoLock());
	//aos_assert_r(rslt, false);

	mDataColId = conf->getAttrStr(AOSTAG_NAME, "");	
	aos_assert_r(mDataColId != "", false);

	mLogicId = mDataColId;
	mType = eNorm;

	AosXmlTagPtr iilAsmConfig = conf->getFirstChild("asm");
	aos_assert_r(iilAsmConfig, 0);

	mDataColTag = conf->clone(AosMemoryCheckerArgsBegin);
	mIsInited = true;

	return true;
}


bool 
AosDataCollectorNorm::addOutput(
		const AosXmlTagPtr &output, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(output, false);

	u64 fileid = output->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(fileid, false);

	AosXmlTagPtr info = output->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(info, false);

	mLock->lock();

	mFileList.resize(1);
	mFileList[0].push_back(info);

	info->setAttr(AOSTAG_SHUFFLE_ID, mShuffleId);
	OmnScreen << "jozhi add last outputfiles: " << info->toString() << endl;
	mLastOutputFiles.push_back(info);

	mLock->unlock();

	return true;
}


bool 
AosDataCollectorNorm::finishDataCollector(const AosRundataPtr &rdata)
{
	aos_assert_r(mStartNum > 0, false);
	mStartNum--;
	if (mStartNum > 0)
	{
		return true;
	}

	if (mFileList.empty())
	{
		OmnAlarm << "============================================ No Output." << enderr;
		mFinished = true;
	}

	mFinished = true;
	OmnScreen << "DataCollectorNorm Finished." << endl;

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

