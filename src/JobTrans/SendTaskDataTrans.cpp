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
//   
//
// Modification History:
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "JobTrans/SendTaskDataTrans.h"

#include "XmlUtil/XmlTag.h"
#include "TaskMgr/Task.h"

AosSendTaskDataTrans::AosSendTaskDataTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSendTaskData, regflag)
{
}


AosSendTaskDataTrans::AosSendTaskDataTrans(
		const int svr_id,
		const u32 to_proc_id,
		const u64 taskDocId,
		const OmnString dataId,
		const AosBuffPtr &confBuff,
		const AosBuffPtr &dataBuff)
:
AosTaskTrans(AosTransType::eSendTaskData, svr_id, to_proc_id, false, true)
{
	mTaskDocId = taskDocId;
	mDataId = dataId;
	mConfBuff = confBuff;
	mDataBuff = dataBuff;
}


AosSendTaskDataTrans::~AosSendTaskDataTrans()
{
}


bool
AosSendTaskDataTrans::serializeFrom(const AosBuffPtr &buff)
{
	OmnTagFuncInfo << endl;

	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTaskDocId = buff->getU64(0);
	mDataId = buff->getOmnStr("");

	//how to serialize AosBuff???
	//
	u32 buff_len = buff->getU32(0);
	mConfBuff = buff->getBuff(buff_len, true AosMemoryCheckerArgs); 
	aos_assert_r(mConfBuff, false);  

	buff_len = buff->getU32(0);
	mDataBuff = buff->getBuff(buff_len, true AosMemoryCheckerArgs); 
	aos_assert_r(mDataBuff, false);  
	return true;
}


bool
AosSendTaskDataTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mTaskDocId);
	buff->setOmnStr(mDataId);
	buff->setU32(mConfBuff->dataLen());
	buff->setBuff(mConfBuff);
	buff->setU32(mDataBuff->dataLen());
	buff->setBuff(mDataBuff);
	return true;
}


AosTransPtr
AosSendTaskDataTrans::clone()
{
	return OmnNew AosSendTaskDataTrans(false);
}


bool
AosSendTaskDataTrans::proc()
{
	//get the thread taskdocid<--->taskThrdPtr map
	OmnTagFuncInfo << "start to find task: " << mTaskDocId << endl;
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	AosTaskThrd* t = NULL;
	int tryTimes = 0;

	while (!t)
	{
		//sometimes, the task thread is not fully setup yet when
		//the data from upsteam is available. Need to wait for
		//a while for the task thread to complete initialization
		t = AosTaskThrd::findTaskThrd(mTaskDocId);
		tryTimes++;
		if (tryTimes > 10)
			break;
	}

	if (!t)
	{
		resp_buff->setU8(0);
		sendResp(resp_buff);
	}
	aos_assert_r(t, false);

	OmnTagFuncInfo << "Give data to the thread and signal thread to run task: " << mTaskDocId << endl;
	bool rslt = t->dataArrived(mDataId, mConfBuff, mDataBuff);
	if (rslt)
	{
		resp_buff->setU8(1);
		sendResp(resp_buff);
		return true;
	}

	resp_buff->setU8(0);
	sendResp(resp_buff);
	return false;
}


#endif
