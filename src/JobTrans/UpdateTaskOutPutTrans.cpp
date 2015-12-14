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
#include "JobTrans/UpdateTaskOutPutTrans.h"

#include "SEInterfaces/TaskMgrObj.h"
#include "TaskUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosUpdateTaskOutPutTrans::AosUpdateTaskOutPutTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eUpdateTaskOutPut, regflag)
{
}


AosUpdateTaskOutPutTrans::AosUpdateTaskOutPutTrans(
		const u64 &task_docid,
		const AosXmlTagPtr &xml,
		const int server_id)
:
AosTaskTrans(AosTransType::eUpdateTaskOutPut, server_id, true, true),
mTaskDocid(task_docid),
mXml(xml)
{
}


AosUpdateTaskOutPutTrans::~AosUpdateTaskOutPutTrans()
{
}


bool
AosUpdateTaskOutPutTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid != 0, false);

	OmnString str = buff->getOmnStr("");
	aos_assert_r(str != "", false);
	mXml = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(mXml, false);


	return true;
}


bool
AosUpdateTaskOutPutTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mTaskDocid);
	aos_assert_r(mXml, false);
	buff->setOmnStr(mXml->toString());
	return true;
}


AosTransPtr
AosUpdateTaskOutPutTrans::clone()
{
	return OmnNew AosUpdateTaskOutPutTrans(false);
}


bool
AosUpdateTaskOutPutTrans::proc()
{
	bool rslt = AosTaskMgrObj::updateTaskOutPutStatic(mTaskDocid, mXml, mRdata);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	return true;
}
