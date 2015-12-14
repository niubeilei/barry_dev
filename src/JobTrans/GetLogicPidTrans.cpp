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
// 08/08/2014	Created by Phil
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/GetLogicPidTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/TaskMgr.h"
#include "Debug/Debug.h"


AosGetLogicPidTrans::AosGetLogicPidTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetLogicPid, regflag)
{
}


AosGetLogicPidTrans::AosGetLogicPidTrans(
		const u64 taskDocId,
		const int svr_id)
:
AosTaskTrans(AosTransType::eGetLogicPid, svr_id, false, true),
mTaskDocId(taskDocId)
{
}


AosGetLogicPidTrans::~AosGetLogicPidTrans()
{
}


bool
AosGetLogicPidTrans::serializeFrom(const AosBuffPtr &buff)
{
	OmnTagFuncInfo << endl;

	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mTaskDocId = buff->getU64(0);
	return true;
}


bool
AosGetLogicPidTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mTaskDocId);
	return true;
}


AosTransPtr
AosGetLogicPidTrans::clone()
{
	return OmnNew AosGetLogicPidTrans(false);
}


//
//get logic pid from TaskMgr
//
bool
AosGetLogicPidTrans::proc()
{
	OmnTagFuncInfo << "start to proc getLogicPid trans" << endl;
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	u32 logicPid = AosTaskMgr::getSelf()->getLogicPid(mTaskDocId);

	resp_buff->setU32(logicPid);
	sendResp(resp_buff);

	OmnTagFuncInfo << "logicPid is: " << logicPid
		<< " taskDocId is: " << mTaskDocId << endl;
	//if (logicPid == 0)
	//	return false;

	return true;
}

