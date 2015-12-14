////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/MsgClientObj.h"

AosMsgClientObjPtr AosMsgClientObj::smMsgClient;

bool
AosMsgClientObj::startProcCbStatic(const u32 logic_pid)
{
	aos_assert_r(smMsgClient, false);
	return smMsgClient->startProcCb(logic_pid);
}

bool
AosMsgClientObj::sotpPRocCbStatic(const u32 logic_pid)
{
	aos_assert_r(smMsgClient, false);
	return smMsgClient->stopProcCb(logic_pid);
}

bool
AosMsgClientObj::procMsgSvrRequestStatic(
			const OmnString &request,
			const AosRundataPtr &rdata)
{
	aos_assert_r(smMsgClient, false);
	return smMsgClient->procMsgSvrRequest(request, rdata);
}

OmnString
AosMsgClientObj::getMsgSvrUrlStatic()
{
	aos_assert_r(smMsgClient, "");
	return smMsgClient->getMsgSvrUrl();
}


