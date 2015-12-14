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
#include "JobTrans/StopTaskProcessTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "TaskUtil/Ptrs.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskDriverRequest.h"
#include "SEInterfaces/TaskMgrObj.h"


AosStopTaskProcessTrans::AosStopTaskProcessTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eStopTaskProcess, regflag)
{
}


AosStopTaskProcessTrans::AosStopTaskProcessTrans(
		const int server_id,
		const u64 &task_docid)
:
AosTaskTrans(AosTransType::eStopTaskProcess, server_id, false, false),
mTaskDocid(task_docid)
{
}


AosStopTaskProcessTrans::~AosStopTaskProcessTrans()
{
}


bool
AosStopTaskProcessTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTaskDocid = buff->getU64(0);
	return true;
}


bool
AosStopTaskProcessTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mTaskDocid);
	return true;
}


AosTransPtr
AosStopTaskProcessTrans::clone()
{
	return OmnNew AosStopTaskProcessTrans(false);
}


bool
AosStopTaskProcessTrans::proc()
{
	bool rslt = AosTaskMgrObj::getTaskMgr()->stopTask(mTaskDocid, mRdata);
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}
