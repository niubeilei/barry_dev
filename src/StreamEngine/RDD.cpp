////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 05/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StreamEngine/RDD.h"

#include "SEInterfaces/SysInfo.h"
#include "Debug/Debug.h"
#include "JQLStatement/JqlStmtInsertItem.h"
#include "JQLStatement/JqlStmtUpdateItem.h"
#include "TransClient/Ptrs.h" 
#include "StreamEngine/SendStreamDataTrans.h"
#include "StreamEngine/Service.h"
#include "DataRecord/RecordContainer.h"
#include "DataRecord/StreamRecordset.h"
#include "API/AosApi.h" 

static bool sgStat = false;

////////////////////////////////////////////////////
//   constructors/destructors
////////////////////////////////////////////////////
AosRDD::AosRDD()
:
mStartNum(0),
mFinishNum(0),
mTotalNum(0),
mStartTime(0),
mFinishTime(0),
mRDDId(0),
mRecvTimeStamp(0),
mInUseTimeStamp(0),
mProcessedTimeStamp(0),
mDocId(0),
mProcTime(0),
mDataId(""),
mServiceId(""),
mSendDataProcName(""),
mRecvDataProcName(""),
mLock(OmnNew OmnMutex()),
mDoc(0),
mRecordset(0),
mRecordsetRaw(0)
{
}


AosRDD::AosRDD(
		const OmnString &data_id,
		const OmnString &send_dp_name,
		const OmnString &recv_dp_name,
		const OmnString &service_id,
		const u64 &rdd_id,
		const AosRecordsetObjPtr &rs,
		const AosRundataPtr &rdata)
:
mStartNum(0),
mFinishNum(0),
mTotalNum(0),
mStartTime(0),
mFinishTime(0),
mRDDId(rdd_id),
mRecvTimeStamp(0),
mInUseTimeStamp(0),
mProcessedTimeStamp(0),
mDocId(0),
mProcTime(0),
mDataId(data_id),
mServiceId(service_id),
mSendDataProcName(send_dp_name),
mRecvDataProcName(recv_dp_name),
mLock(OmnNew OmnMutex()),
mDoc(0),
mRecordset(rs),
mRecordsetRaw(mRecordset.getPtr())
{
	//OmnScreen << "**********************new RDD, rddid: " << mRDDId << endl;
}

AosRDD::AosRDD(const AosRDD &rdd, const AosRundataPtr &rdata)
:
mStartNum(0),
mFinishNum(0),
mTotalNum(0),
mStartTime(0),
mFinishTime(0),
mRDDId(rdd.mRDDId),
mRecvTimeStamp(rdd.mRecvTimeStamp),
mInUseTimeStamp(rdd.mInUseTimeStamp),
mProcessedTimeStamp(rdd.mProcessedTimeStamp),
mDocId(rdd.mDocId),
mProcTime(rdd.mProcTime),
mDataId(rdd.mDataId),
mServiceId(rdd.mServiceId),
mSendDataProcName(rdd.mSendDataProcName),
mRecvDataProcName(rdd.mRecvDataProcName),
mLock(OmnNew OmnMutex()),
mDoc(0),
mRecordset(rdd.mRecordset->clone(rdata.getPtr())),
mRecordsetRaw(mRecordset.getPtr())
{
	//OmnScreen << "**********************new RDD, rddid: " << mRDDId << endl;
}

AosRDD::~AosRDD()
{
	//OmnScreen << "**********************release RDD, rddid: " << mRDDId << endl;
}

//
//clone a new RDD from current RDD
//
AosRDDPtr
AosRDD::clone(const AosRundataPtr &rdata)
{
	return OmnNew AosRDD(*this, rdata);
}

bool 
AosRDD::isSameRDD(AosRDD *rdd)
{
	return (mDataId == rdd->mDataId &&
			mRDDId == rdd->mRDDId);
}

bool
AosRDD::serializeTo(const AosRundataPtr &rdata, const AosBuffPtr &buff)
{
	buff->setOmnStr(mDataId);
	buff->setU64(mRDDId);
	buff->setU64(mDocId);
	buff->setOmnStr(mSendDataProcName);
	buff->setOmnStr(mRecvDataProcName);
	buff->setOmnStr(mServiceId);
	buff->setU8(mIsStreamRecordset);

	bool rslt = mRecordset->serializeTo(rdata.getPtr(), buff.getPtr());
	aos_assert_r(rslt, false);
	mRecordsetRaw = mRecordset.getPtr();
	return true;
}

bool
AosRDD::serializeFrom(const AosRundataPtr &rdata, const AosBuffPtr &buff)
{
	mDataId = buff->getOmnStr("");
	mRDDId = buff->getU64(0);
	mDocId = buff->getU64(0);
	mSendDataProcName = buff->getOmnStr("");
	mRecvDataProcName = buff->getOmnStr("");
	mServiceId = buff->getOmnStr("");
	mIsStreamRecordset =  buff->getU8(0);

	if (mIsStreamRecordset)
	{
		mRecordset = OmnNew AosStreamRecordset();
	}
	else
	{
		mRecordset = OmnNew AosRecordset();
	}
	mRecordset->serializeFrom(rdata.getPtr(), buff.getPtr());
	mRecordsetRaw = mRecordset.getPtr();
	return true;
}

////////////////////////////////////////////////////////
//      Counters/logging methods
////////////////////////////////////////////////////////
void
AosRDD::buildFieldMap()
{
	OmnString str;
	mFieldMap[JOBDATA_DATAID] = mDataId;
	//mFieldMap[JOBDATA_STATUS] = mStatus;

	str = "";
	str << mRDDId;
	mFieldMap[JOBDATA_RDDID] = str;

	str = "";
	str << mSendDataProcName;
	mFieldMap[JOBDATA_SENDDATAPROC] = str;
	
	str = "";
	str << mRecvDataProcName;
	mFieldMap[JOBDATA_RECVDATAPROC] = str;
	
	str = "";
	str << mProcTime;
	mFieldMap[JOBDATA_PROCTIME] = str;
}

bool
AosRDD::insertJobData(const AosRundataPtr &rdata)
{
	buildFieldMap();

	mDoc = AosJqlStmtInsertItem::insertJobData(rdata, &mFieldMap);
	mDocId = mDoc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(mDoc, false);

	return true;
}

bool
AosRDD::updateJobData(const AosRundataPtr &rdata)
{
	//insert a new entry in jobData system table if 
	//not existing
	if(sgStat)
	{
		if (!mDoc) return insertJobData(rdata);  

		bool rslt = true;
		buildFieldMap();

		//to avoid "read only" doc modification
		AosXmlTagPtr doc = mDoc->clone(AosMemoryCheckerArgsBegin);
		rslt = AosJqlStmtUpdateItem::updateData(rdata, doc, &mFieldMap);
		return rslt;
	}
	return true;
}


/////////////////////////////////////////////////
//    getterst/setters
/////////////////////////////////////////////////
i64
AosRDD::getDataLen()
{
	AosBuffPtr buff = mRecordset->getDataBuff();
	return buff->dataLen();
}

i64
AosRDD::getEntryLen()
{
	AosBuffPtr buff = mRecordset->getEntryBuff();
	return buff->dataLen();
}

bool
AosRDD::start()
{
	mLock->lock();
	mStartNum++;
	mLock->unlock();
	return true;
}

bool
AosRDD::finish(const int remain)
{
	mLock->lock();
	mFinishNum++;
	mTotalNum -= remain;
	mLock->unlock();
	return true;
}

bool
AosRDD::isFinished()
{
	mLock->lock();
	if (mStartNum > 0 && 
		mFinishNum > 0 && 
		mStartNum == mFinishNum &&
		mStartNum == mTotalNum)
	{
		mLock->unlock();
		return true;
	}
	mLock->unlock();
	return false;
}

bool
AosRDD::setStartTime()
{
	mStartTime = OmnGetSecond();
	return true;
}

bool
AosRDD::setFinishTime()
{
	mFinishTime = OmnGetSecond();
	return true;
}

u64
AosRDD::getStartTime()
{
	return mStartTime;
}

u64
AosRDD::getFinishTime()
{
	return mFinishTime;
}

bool 
AosRDD::setTotalNum(const int total)
{
	mTotalNum = total;
	return true;
}


bool 
AosRDD::sort(AosRundata* rdata, const AosCompareFunPtr &comp)
{
	aos_assert_r(mRecordset && comp, false);
	mRecordset->sort(rdata, comp);
	return true;
}

bool
AosRDD::appendRecord(AosRundata* rdata, AosDataRecordObjPtr &rcd)
{
	AosBuffDataPtr buff;
	return mRecordsetRaw->appendRecord(rdata, rcd, buff);
}

bool
AosRDD::isEmpty()
{
	if (mRecordset->size() == 0)
	{
		return true;
	}
	return false;
}
