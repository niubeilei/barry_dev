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
#include "JobTrans/SendCmdTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/CommandRunnerObj.h"

AosSendCmdTrans::AosSendCmdTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSendCmd, regflag)
{
}


AosSendCmdTrans::AosSendCmdTrans(
		const OmnString &args,
		const int svr_id)
:
AosTaskTrans(AosTransType::eSendCmd, svr_id, false, false),
mArgs(args)
{
}


AosSendCmdTrans::~AosSendCmdTrans()
{
}


bool
AosSendCmdTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mArgs = buff->getOmnStr("");
	return true;
}


bool
AosSendCmdTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mArgs);
	return true;
}


AosTransPtr
AosSendCmdTrans::clone()
{
	return OmnNew AosSendCmdTrans(false);
}


bool
AosSendCmdTrans::proc()
{
	bool rslt = AosCommandRunnerObj::runCommandStatic(AosModuleId::eJobMgr, mArgs, 0, mRdata);
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}


