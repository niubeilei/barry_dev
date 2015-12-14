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
#include "JobTrans/AbortTaskTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/TaskMgr.h"

AosAbortTaskTrans::AosAbortTaskTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eAbortTask, regflag)
{
}


AosAbortTaskTrans::AosAbortTaskTrans(
		const int server_id,
		const u64 &job_docid,
		const u64 &task_docid)
:
AosTaskTrans(AosTransType::eAbortTask, server_id, true, false),
mJobDocid(job_docid),
mTaskDocid(task_docid)
{
}


AosAbortTaskTrans::~AosAbortTaskTrans()
{
}


bool
AosAbortTaskTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mJobDocid = buff->getU64(0);
	mTaskDocid = buff->getU64(0);

	return true;
}


bool
AosAbortTaskTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mJobDocid);
	buff->setU64(mTaskDocid);

	return true;
}


AosTransPtr
AosAbortTaskTrans::clone()
{
	return OmnNew AosAbortTaskTrans(false);
}


bool
AosAbortTaskTrans::proc()
{
	/*
	bool rslt = AosTaskMgr::getSelf()->abortTaskLocal(mJobDocid, mTaskDocid, mRdata);
	
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	AosTransPtr thisptr(this, false);
	AosSendResp(thisptr, resp_buff);
	
	//OmnString str;
	//str << "<rsp rslt=\"" << rslt << "\"/>";
	//resp_buff->setBuff(str.data(), str.length());
	*/
	return false;
}


