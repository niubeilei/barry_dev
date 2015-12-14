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
#if 0
#include "TaskMgr/IDU.h"

#include "SEInterfaces/SysInfo.h"
#include "Debug/Debug.h"
#include "JQLStatement/JqlStmtInsertItem.h"
#include "JQLStatement/JqlStmtUpdateItem.h"
#include "TransClient/Ptrs.h" 
#include "JobTrans/SendTaskDataTrans.h"
#include "JobTrans/GetLogicPidTrans.h"
#include "API/AosApi.h" 

AosIDU::AosIDU()
{
	mStatus = IDU_STATUS_SENT;
	mProcTime = 0;
	mSendTimeStamp = 0;
	mRecvTimeStamp = 0;
	mInUseTimeStamp = 0;
	mProcessedTimeStamp = 0;
}

AosIDU::~AosIDU()
{
}

bool 
AosIDU::isSameIDU(AosIDU *idu)
{
	return (mDataId == idu->mDataId &&
			mIDUId == idu->mIDUId &&
			mSegId == idu->mSegId &&
			mSendPhyId == idu->mSendPhyId &&
			mSendTaskId == idu->mSendTaskId &&
			mRecvPhyId == idu->mRecvPhyId &&
			mRecvTaskId == idu->mRecvTaskId);
}

bool 
AosIDU::serializeTo(
		AosBuffPtr &buff)
{
	buff->reset();
	buff->setOmnStr(mDataId);
	buff->setU64(mIDUId);
	buff->setInt(mSegId);
	buff->setInt(mSendPhyId);
	buff->setU64(mSendTaskId);
	buff->setInt(mRecvPhyId);
	buff->setU64(mRecvTaskId);
	buff->setOmnStr(mStatus);
	buff->setU64(mDocId);
	buff->setU64(mSendTimeStamp);
	buff->setOmnStr(mDoc->toString());

	return true;
}

bool 
AosIDU::serializeFrom(
		AosBuffPtr &buff)
{
	OmnString str;
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);

	buff->reset();
	mDataId = buff->getOmnStr("");
	mIDUId = buff->getU64(0);
	mSegId = buff->getInt(0);
	mSendPhyId = buff->getInt(0);
	mSendTaskId = buff->getU64(0);
	mRecvPhyId = buff->getInt(0);
	mRecvTaskId = buff->getU64(0);
	mStatus = buff->getOmnStr("");
	mDocId = buff->getU64(0);
	mSendTimeStamp = buff->getU64(0);
	str = buff->getOmnStr("");
	mDoc = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs);

	return true;
}

void
AosIDU::buildFieldMap()
{
	OmnString str;

	mFieldMap[JOBDATA_DATAID] = mDataId;
	mFieldMap[JOBDATA_STATUS] = mStatus;

	str = "";
	str << mIDUId;
	mFieldMap[JOBDATA_IDUID] = str;

	str = "";
	str << mSegId;
	mFieldMap[JOBDATA_SEGID] = str;

	str = "";
	str << mSendPhyId;
	mFieldMap[JOBDATA_SENDPHYID] = str;

	str = "";
	str << mSendTaskId;
	mFieldMap[JOBDATA_SENDTASKID] = str;

	str = "";
	str << mRecvPhyId;
	mFieldMap[JOBDATA_RECVPHYID] = str;

	str = "";
	str << mRecvTaskId;
	mFieldMap[JOBDATA_RECVTASKID] = str;

	str = "";
	str << mProcTime;
	mFieldMap[JOBDATA_PROCTIME] = str;
}

bool
AosIDU::insertJobData(const AosRundataPtr &rdata)
{
	buildFieldMap();

	//insert an entry to sys job table
	mDoc = AosJqlStmtInsertItem::insertJobData(rdata, &mFieldMap);
	mDocId = mDoc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(mDoc, false);

	return true;
}

bool
AosIDU::updateJobData(const AosRundataPtr &rdata)
{
	buildFieldMap();
	return AosJqlStmtUpdateItem::updateData(rdata, mDoc, &mFieldMap);
}

bool 
AosIDU::send(AosRundataPtr rdata)
{
	OmnTagFuncInfo << "start to transfer an IDU to downstream. " << endl;

	//get the logic pid firstly
	bool timeout = false;
	AosBuffPtr resp; 
	u32 logicPid = 0;
	bool rslt;
	AosTransPtr trans;

	//sometimes we need to wait for downstream tasks started
	for (u32 i = 0; i < 5; i++)
	{
		trans = OmnNew AosGetLogicPidTrans(mRecvTaskId, mRecvPhyId);
		rslt = AosSendTrans(rdata, trans, timeout, resp);
		if (timeout)
		{
			//AosSetErrorU(rdata, "Faild to add the trans, timeout");
			//OmnAlarm << rdata->getErrmsg() << enderr;
			//return false;
			OmnTagFuncInfo << "sendtrans timeout once! " << endl;
			timeout = false;
		}
		else
		{
			OmnTagFuncInfo << "try to get logic pid " << endl;
			logicPid = resp->getU32(0);
			if (logicPid > 0)
				break;
		}

		sleep(1);
		OmnTagFuncInfo << "resend trans to downstream" << endl;
	}

	if (logicPid == 0)
		OmnTagFuncInfo << "Sorry, downstream is not up" << endl;

	aos_assert_r(logicPid, false);
	OmnTagFuncInfo << "downstream logicPid is: " << logicPid << " physical id is: " 
		<< mSendPhyId << " taskDocId is: " << mRecvTaskId;

	//insert a new entry in jobData system table
	mStatus = IDU_STATUS_SENT;
	mSendTimeStamp = OmnGetSystemTime();
	rslt = insertJobData(rdata);

	//compose the conf buffer
	u32 len = (mDoc->toString()).length() + 200;
	AosBuffPtr confBuff = OmnNew AosBuff(len AosMemoryCheckerArgs);
	serializeTo(confBuff);
	OmnTagFuncInfo << "IDU create conf buff len is: " << confBuff->dataLen() << endl;
	OmnTagFuncInfo << "data buff len is: " << mBuff->dataLen() << endl;

	//send data to logic pid running the task
	trans = OmnNew AosSendTaskDataTrans(mRecvPhyId, logicPid,
			mRecvTaskId, mDataId, confBuff, mBuff);

	rslt = AosSendTrans(rdata, trans);

	//update the data info sys job table through taskDataBuff
	//addDataCounter(dataId, 1, buff->dataLen());
	//
	
	return rslt;
}

u64 
AosIDU::recv(
		AosRundataPtr rdata,
		AosBuffPtr confBuff,
		AosBuffPtr dataBuff)
{
	bool rslt;

	rslt = serializeFrom(confBuff);
	mBuff = dataBuff;
	mRecvTimeStamp = OmnGetSystemTime();
	mStatus = IDU_STATUS_RECVED;

	updateJobData(rdata);
	if (rslt)
		return mIDUId;

	//0 is an invalid IDU id
	return 0;
}

bool
AosIDU::inProcess(AosRundataPtr rdata)
{
	mStatus = IDU_STATUS_INPROCESS;
	return updateJobData(rdata);
}

bool
AosIDU::processed(AosRundataPtr rdata)
{
	mStatus = IDU_STATUS_PROCESSED;
	mProcessedTimeStamp = OmnGetSystemTime();
	mProcTime = mProcessedTimeStamp - mRecvTimeStamp;
	//the time is in ms, translated to second
	mProcTime /= 1000; 
	return updateJobData(rdata);
}

#endif
