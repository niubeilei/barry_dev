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
#include "JobTrans/ProcMsgTrans.h"

#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/MsgServerObj.h"


AosProcMsgTrans::AosProcMsgTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eProcMsg, regflag)
{
}


AosProcMsgTrans::AosProcMsgTrans(
		const OmnString &contents,
		const int svr_id,
		const u32 to_proc_id,
		const bool need_save)
:
AosTaskTrans(AosTransType::eProcMsg, svr_id, to_proc_id, need_save, false),
mContents(contents)
{
}


AosProcMsgTrans::~AosProcMsgTrans()
{
}


bool
AosProcMsgTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mContents = buff->getOmnStr("");
	return true;
}


bool
AosProcMsgTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mContents);
	return true;
}


AosTransPtr
AosProcMsgTrans::clone()
{
	return OmnNew AosProcMsgTrans(false);
}


bool
AosProcMsgTrans::proc()
{
	aos_assert_r(mContents != "", false);
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(mContents, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	bool rslt = AosMsgServerObj::procRequestStatic(root, mRdata);
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}


