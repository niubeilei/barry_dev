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
#include "JobTrans/GetRunTimeValueTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "TaskUtil/Ptrs.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskDriverRequest.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/TaskMgrObj.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("GetRunTimeValueThrd", __FILE__, __LINE__);

AosGetRunTimeValueTrans::AosGetRunTimeValueTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetRunTimeValue, regflag)
{
}


AosGetRunTimeValueTrans::AosGetRunTimeValueTrans(
		const OmnString &key,
		const u64 &job_docid,
		const u64 &task_docid,
		const int svr_id)
:
AosTaskTrans(AosTransType::eGetRunTimeValue, svr_id, false, true),
mKey(key),
mJobDocid(job_docid),
mTaskDocid(task_docid)
{
}


AosGetRunTimeValueTrans::~AosGetRunTimeValueTrans()
{
}


bool
AosGetRunTimeValueTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mKey = buff->getOmnStr("");
	mJobDocid = buff->getU64(0);
	mTaskDocid= buff->getU64(0);
	return true;
}


bool
AosGetRunTimeValueTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mKey);
	buff->setU64(mJobDocid);
	buff->setU64(mTaskDocid);
	return true;
}


AosTransPtr
AosGetRunTimeValueTrans::clone()
{
	return OmnNew AosGetRunTimeValueTrans(false);
}


bool
AosGetRunTimeValueTrans::proc()
{
	AosTransPtr thisptr(this, false);
	OmnThrdShellProcPtr runner = OmnNew GetRunTimeValueThrd(mKey, mJobDocid, mTaskDocid, thisptr, mRdata);
	sgThreadPool->proc(runner);
	return true;
}

bool
AosGetRunTimeValueTrans::GetRunTimeValueThrd::run()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff = AosTaskMgrObj::getTaskMgr()->getRunTimeValue(mJobDocid, mTaskDocid, mKey, mRdata);
	aos_assert_r(resp_buff, false);
	return mTrans->sendResp(resp_buff);
}
