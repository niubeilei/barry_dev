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
#include "JobTrans/SetRunTimeValueByJobTrans.h"

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

AosSetRunTimeValueByJobTrans::AosSetRunTimeValueByJobTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSetRunTimeValueByJob, regflag)
{
}


AosSetRunTimeValueByJobTrans::AosSetRunTimeValueByJobTrans(
		const OmnString &key,
		const AosValueRslt &value,
		const u64 &job_docid,
		const int svr_id)
:
AosTaskTrans(AosTransType::eSetRunTimeValueByJob, svr_id, false, true),
mKey(key),
mValue(value),
mJobDocid(job_docid)
{
}


AosSetRunTimeValueByJobTrans::~AosSetRunTimeValueByJobTrans()
{
}


bool
AosSetRunTimeValueByJobTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mKey = buff->getOmnStr("");
	//mValue.serializeFrom(buff);
	mJobDocid = buff->getU64(0);
	return true;
}


bool
AosSetRunTimeValueByJobTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mKey);
	//mValue.serializeTo(buff);
	buff->setU64(mJobDocid);
	return true;
}


AosTransPtr
AosSetRunTimeValueByJobTrans::clone()
{
	return OmnNew AosSetRunTimeValueByJobTrans(false);
}


bool
AosSetRunTimeValueByJobTrans::proc()
{
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, mRdata);
	aos_assert_r(job, false);
	bool rslt = job->setRunTimeValue(mKey, mValue, mRdata);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	return sendResp(resp_buff);
}
