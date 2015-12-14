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
// 2015/01/19	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/GetRunTimeValueByJobTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "TaskUtil/Ptrs.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskDriverRequest.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/JobObj.h"

AosGetRunTimeValueByJobTrans::AosGetRunTimeValueByJobTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetRunTimeValueByJob, regflag)
{
}


AosGetRunTimeValueByJobTrans::AosGetRunTimeValueByJobTrans(
		const OmnString &key,
		const u64 &job_docid,
		const int svr_id)
:
AosTaskTrans(AosTransType::eGetRunTimeValueByJob, svr_id, false, true),
mKey(key),
mJobDocid(job_docid)
{
}


AosGetRunTimeValueByJobTrans::~AosGetRunTimeValueByJobTrans()
{
}


bool
AosGetRunTimeValueByJobTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mKey = buff->getOmnStr("");
	mJobDocid = buff->getU64(0);
	return true;
}


bool
AosGetRunTimeValueByJobTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mKey);
	buff->setU64(mJobDocid);
	return true;
}


AosTransPtr
AosGetRunTimeValueByJobTrans::clone()
{
	return OmnNew AosGetRunTimeValueByJobTrans(false);
}


bool
AosGetRunTimeValueByJobTrans::proc()
{
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, mRdata);
	aos_assert_r(job, false);
	AosValueRslt value = job->getRunTimeValue(mKey, mRdata);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	//value.serializeFrom(resp_buff);
	return sendResp(resp_buff);
}
