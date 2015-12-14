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
#include "JobTrans/UpdateTaskProgressTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/TaskMgrObj.h"


AosUpdateTaskProgressTrans::AosUpdateTaskProgressTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eUpdateTaskProgress, regflag)
{
}


AosUpdateTaskProgressTrans::AosUpdateTaskProgressTrans(
		const u64 &task_docid,
		const int percent,
		const int server_id)
:
AosTaskTrans(AosTransType::eUpdateTaskProgress, server_id, false, false),
mTaskDocid(task_docid),
mPercent(percent)
{
}


AosUpdateTaskProgressTrans::~AosUpdateTaskProgressTrans()
{
}


bool
AosUpdateTaskProgressTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTaskDocid = buff->getU64(0);
	mPercent = buff->getInt(0);

	return true;
}


bool
AosUpdateTaskProgressTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mTaskDocid);
	buff->setInt(mPercent);

	return true;
}


AosTransPtr
AosUpdateTaskProgressTrans::clone()
{
	return OmnNew AosUpdateTaskProgressTrans(false);
}


bool
AosUpdateTaskProgressTrans::proc()
{
	bool rslt = AosTaskMgrObj::updateTaskProgressStatic(
		mTaskDocid, mPercent, mRdata);
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}


