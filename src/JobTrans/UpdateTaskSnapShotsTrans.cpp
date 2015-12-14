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
#include "JobTrans/UpdateTaskSnapShotsTrans.h"

#include "SEInterfaces/TaskMgrObj.h"
#include "TaskUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosUpdateTaskSnapShotsTrans::AosUpdateTaskSnapShotsTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eUpdateTaskSnapShots, regflag)
{
}


AosUpdateTaskSnapShotsTrans::AosUpdateTaskSnapShotsTrans(
		const u64 &task_docid,
		const AosXmlTagPtr &snapshots_tag,
		const int server_id)
:
AosTaskTrans(AosTransType::eUpdateTaskSnapShots, server_id, true, true),
mTaskDocid(task_docid),
mSnapShotsTag(snapshots_tag)
{
}


AosUpdateTaskSnapShotsTrans::~AosUpdateTaskSnapShotsTrans()
{
}


bool
AosUpdateTaskSnapShotsTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid != 0, false);

	OmnString str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	mSnapShotsTag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(mSnapShotsTag, false);


	return true;
}


bool
AosUpdateTaskSnapShotsTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mTaskDocid);
	aos_assert_r(mSnapShotsTag, false);
	buff->setOmnStr(mSnapShotsTag->toString());
	return true;
}


AosTransPtr
AosUpdateTaskSnapShotsTrans::clone()
{
	return OmnNew AosUpdateTaskSnapShotsTrans(false);
}


bool
AosUpdateTaskSnapShotsTrans::proc()
{
	bool rslt = AosTaskMgrObj::updateTaskSnapShotsStatic(mTaskDocid, mSnapShotsTag, mRdata);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	return true;
}
