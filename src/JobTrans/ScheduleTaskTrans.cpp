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
#include "JobTrans/ScheduleTaskTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskMgrObj.h"

AosScheduleTaskTrans::AosScheduleTaskTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eScheduleTask, regflag)
{
}


AosScheduleTaskTrans::AosScheduleTaskTrans(
		const u64 &job_docid,
		const OmnString &logic_id,
		const OmnString &task_type,
		const int crt_jobsvrid,
		const u64 &task_docid,
		const int svr_id,
		const u64 &timestamp)
:
AosTaskTrans(AosTransType::eScheduleTask, svr_id, false, false),
mJobDocid(job_docid),
mLogicId(logic_id),
mTaskType(task_type),
mCrtJobSvrId(crt_jobsvrid),
mTaskDocid(task_docid),
mStartStamp(timestamp)
{
}

AosScheduleTaskTrans::~AosScheduleTaskTrans()
{
}

bool
AosScheduleTaskTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mJobDocid = buff->getU64(0);
	mLogicId = buff->getOmnStr("");
	mTaskType = buff->getOmnStr("");
	mCrtJobSvrId = buff->getInt(-1);
	aos_assert_r(mCrtJobSvrId >= 0, false);
	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid, false);
	mStartStamp = buff->getU64(0);
	aos_assert_r(mStartStamp, false);
	return true;
}


bool
AosScheduleTaskTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mJobDocid);
	buff->setOmnStr(mLogicId);
	buff->setOmnStr(mTaskType);
	buff->setInt(mCrtJobSvrId);
	buff->setU64(mTaskDocid);
	buff->setU64(mStartStamp);
	return true;
}


AosTransPtr
AosScheduleTaskTrans::clone()
{
	return OmnNew AosScheduleTaskTrans(false);
}


bool
AosScheduleTaskTrans::proc()
{
	aos_assert_r(mCrtJobSvrId >= 0, false);
	aos_assert_r(mTaskDocid, false);
	bool rslt = AosTaskMgrObj::addTaskLocalStatic(mJobDocid, mLogicId, mTaskType, mCrtJobSvrId, mTaskDocid, mStartStamp, mRdata);
	aos_assert_r(rslt, false);
	return true;
}


OmnString
AosScheduleTaskTrans::toString()
{
	OmnString str = "Start Task By Docid:";
	str << mTaskDocid;
	return str;
}

