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
#include "JobTrans/SetRunTimeValueTrans.h"


#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "TaskUtil/Ptrs.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskDriverRequest.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/TaskMgrObj.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("SetRunTimeValueThrd", __FILE__, __LINE__);

AosSetRunTimeValueTrans::AosSetRunTimeValueTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSetRunTimeValue, regflag)
{
}


AosSetRunTimeValueTrans::AosSetRunTimeValueTrans(
		const OmnString &key,
		const AosValueRslt &value,
		const u64 &job_docid,
		const u64 &task_docid,
		const int svr_id)
:
AosTaskTrans(AosTransType::eGetRunTimeValue, svr_id, false, true),
mKey(key),
mValue(value),
mJobDocid(job_docid),
mTaskDocid(task_docid)
{
}


AosSetRunTimeValueTrans::~AosSetRunTimeValueTrans()
{
}


bool
AosSetRunTimeValueTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mKey = buff->getOmnStr("");
	//mValue.serializeFrom(buff);
	mJobDocid = buff->getU64(0);
	mTaskDocid= buff->getU64(0);
	return true;
}


bool
AosSetRunTimeValueTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mKey);
	//mValue.serializeTo(buff);
	buff->setU64(mJobDocid);
	buff->setU64(mTaskDocid);
	return true;
}


AosTransPtr
AosSetRunTimeValueTrans::clone()
{
	return OmnNew AosSetRunTimeValueTrans(false);
}


bool
AosSetRunTimeValueTrans::proc()
{
	AosTransPtr thisptr(this, false);
	OmnThrdShellProcPtr runner = OmnNew SetRunTimeValueThrd(mKey, mValue, mJobDocid, mTaskDocid, thisptr, mRdata);
	sgThreadPool->proc(runner);
	return true;
}

bool
AosSetRunTimeValueTrans::SetRunTimeValueThrd::run()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff = AosTaskMgrObj::getTaskMgr()->setRunTimeValue(mJobDocid, mTaskDocid, mKey, mValue, mRdata);
	aos_assert_r(resp_buff, false);
	return mTrans->sendResp(resp_buff);
}
