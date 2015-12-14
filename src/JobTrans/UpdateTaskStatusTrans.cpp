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
#include "JobTrans/UpdateTaskStatusTrans.h"

#include "XmlUtil/XmlTag.h"
#include "TaskUtil/Ptrs.h"
#include "SEInterfaces/TaskMgrObj.h"
#include "XmlUtil/SeXmlParser.h"

AosUpdateTaskStatusTrans::AosUpdateTaskStatusTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eUpdateTaskStatus, regflag)
{
}


AosUpdateTaskStatusTrans::AosUpdateTaskStatusTrans(
		const AosTaskStatus::E &status,
		const AosTaskErrorType::E &error_type,
		const OmnString &error_msg,
		const u64 &task_docid,
		const int server_id)
:
AosTaskTrans(AosTransType::eUpdateTaskStatus, server_id, true, true),
mStatus(status),
mErrorType(error_type),
mErrorMsg(error_msg),
mTaskDocid(task_docid)
{
}


AosUpdateTaskStatusTrans::~AosUpdateTaskStatusTrans()
{
}


bool
AosUpdateTaskStatusTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid != 0, false);
	mStatus = AosTaskStatus::toEnum(buff->getOmnStr(""));
	mErrorType = AosTaskErrorType::toEnum(buff->getOmnStr(""));
	mErrorMsg = buff->getOmnStr("");
	return true;
}


bool
AosUpdateTaskStatusTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mTaskDocid);
	buff->setOmnStr(AosTaskStatus::toStr(mStatus));
	buff->setOmnStr(AosTaskErrorType::toStr(mErrorType));
	buff->setOmnStr(mErrorMsg);
	return true;
}


AosTransPtr
AosUpdateTaskStatusTrans::clone()
{
	return OmnNew AosUpdateTaskStatusTrans(false);
}


bool
AosUpdateTaskStatusTrans::proc()
{
	bool rslt = AosTaskMgrObj::updateTaskStatusStatic(mStatus, mErrorType, mErrorMsg, getFromProcId(), mTaskDocid, mRdata);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	return true;
}


OmnString
AosUpdateTaskStatusTrans::toString()
{
	return "";
}

