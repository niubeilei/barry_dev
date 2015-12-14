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
#include "StreamEngine/SendStreamDataTrans.h"

#include "XmlUtil/XmlTag.h"
#include "TaskMgr/Task.h"
#include "StreamEngine/StreamDataProc.h"
#include "StreamEngine/RDD.h"

static AosSendStreamDataTrans sgSendStreamDataTrans(true);
static bool	sgRegistered = false;

AosSendStreamDataTrans::AosSendStreamDataTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSendStreamData, regflag)
{
}


AosSendStreamDataTrans::AosSendStreamDataTrans(
		const AosRDDPtr &rdd,
		const int svr_id,
		const u32 to_proc_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eSendStreamData, svr_id, to_proc_id, need_save, true),
mRDD(rdd)
{
}


AosSendStreamDataTrans::~AosSendStreamDataTrans()
{
}


bool
AosSendStreamDataTrans::serializeFrom(const AosBuffPtr &buff)
{
	OmnTagFuncInfo << endl;

	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mRDD = OmnNew AosRDD();
	mRDD->serializeFrom(mRdata, buff);
	return true;
}


bool
AosSendStreamDataTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	mRDD->serializeTo(mRdata, buff);
	return true;
}


AosTransPtr
AosSendStreamDataTrans::clone()
{
	return OmnNew AosSendStreamDataTrans(false);
}


bool
AosSendStreamDataTrans::proc()
{
	OmnString fullDataId = mRDD->getSendDataProcName();
	OmnString dataId = mRDD->getDataId();
//OmnScreen << "*******************debug*************rdd dataId: " << dataId << endl;
	if(fullDataId == "")
	{
		fullDataId << dataId;
	}
	else
	{
		fullDataId << "." << dataId;
	}
	bool rslt = AosService::dataArrived(mRdata, fullDataId, mRDD);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	mRDD = 0;
	return true;
}

bool
AosSendStreamDataTrans::registerSelf()
{
	if (sgRegistered)
		return true;

	sgRegistered = true;
	return AosConnMsg::registerDynMsg(
			AosTransType::eSendStreamData, &sgSendStreamDataTrans);
}
