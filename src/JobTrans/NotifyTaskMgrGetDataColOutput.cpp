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
#include "JobTrans/NotifyTaskMgrGetDataColOutput.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "TaskUtil/Ptrs.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskDriverRequest.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/TaskMgrObj.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("transgetfilelist", __FILE__, __LINE__);


AosNotifyTaskMgrGetDataColOutput::AosNotifyTaskMgrGetDataColOutput(const bool regflag)
:
AosTaskTrans(AosTransType::eNotifyTaskMgrGetDataColOutput, regflag)
{
}


AosNotifyTaskMgrGetDataColOutput::AosNotifyTaskMgrGetDataColOutput(
		const OmnString &datacol_id,
		const u64 &job_id,
		const u64 &task_docid,
		const int svr_id)
:
AosTaskTrans(AosTransType::eNotifyTaskMgrGetDataColOutput, svr_id, false, true),
mDataColId(datacol_id),
mJobDocid(job_id),
mTaskDocid(task_docid)
{
}


AosNotifyTaskMgrGetDataColOutput::~AosNotifyTaskMgrGetDataColOutput()
{
}


bool
AosNotifyTaskMgrGetDataColOutput::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDataColId = buff->getOmnStr("");
	mJobDocid = buff->getU64(0);
	mTaskDocid= buff->getU64(0);
	return true;
}


bool
AosNotifyTaskMgrGetDataColOutput::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mDataColId);
	buff->setU64(mJobDocid);
	buff->setU64(mTaskDocid);
	return true;
}


AosTransPtr
AosNotifyTaskMgrGetDataColOutput::clone()
{
	return OmnNew AosNotifyTaskMgrGetDataColOutput(false);
}


bool
AosNotifyTaskMgrGetDataColOutput::proc()
{
	AosTransPtr thisptr(this, false);
	OmnThrdShellProcPtr runner = OmnNew GetFileListThrd(mDataColId, mJobDocid, mTaskDocid, thisptr, mRdata);
	sgThreadPool->proc(runner);
	return true;
}

bool
AosNotifyTaskMgrGetDataColOutput::GetFileListThrd::run()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	bool rslt = AosTaskMgrObj::getTaskMgr()->getDataColOutput(mDataColId, mJobDocid, mTaskDocid,
			resp_buff, mRdata);
	aos_assert_r(rslt, false);
	return mTrans->sendResp(resp_buff);
}
