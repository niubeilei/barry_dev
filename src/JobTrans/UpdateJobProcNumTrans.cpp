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
#include "JobTrans/UpdateJobProcNumTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/TaskMgrObj.h"
#include "XmlUtil/SeXmlParser.h"


AosUpdateJobProcNumTrans::AosUpdateJobProcNumTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eUpdateJobProcNum, regflag)
{
}


AosUpdateJobProcNumTrans::AosUpdateJobProcNumTrans(
		const u64 task_docid,
		const AosXmlTagPtr &xml,
		const int svr_id)
:
AosTaskTrans(AosTransType::eUpdateJobProcNum, svr_id,  true, true),
mTaskDocid(task_docid),
mXml(xml)
{
}


AosUpdateJobProcNumTrans::~AosUpdateJobProcNumTrans()
{
}


bool
AosUpdateJobProcNumTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTaskDocid = buff->getU64(0);
	OmnString str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	mXml = xml;
	return true;
}


bool
AosUpdateJobProcNumTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mTaskDocid);
	buff->setOmnStr(mXml->toString());

	return true;
}


AosTransPtr
AosUpdateJobProcNumTrans::clone()
{
	return OmnNew AosUpdateJobProcNumTrans(false);
}


bool
AosUpdateJobProcNumTrans::proc()
{
	bool rslt = AosTaskMgrObj::updateTaskProcNumStatic(mTaskDocid, mXml, mRdata);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	return true;
}


