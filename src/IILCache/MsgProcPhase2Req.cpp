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
// Modification History:
// 2013/03/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILCache/MsgProcPhase2Req.h"


AosMsgProcPhase2Req::AosMsgProcPhase2Req()
{
}


AosMsgProcPhase2Req::~AosMsgProcPhase2Req()
{
}


bool 
AosMsgProcPhase2Req::procMsg(
		const AosRundataPtr &rundata,
		const AosBuffMsgPtr &themsg)
{
	OmnMsgId::E msgid = themsg->getMsgId();
	aos_assert_rr(msgid == OmnMsgId::ePhase2Req, rundata, false);
	AosRundataPtr rdata = themsg->getRundata();
	aos_assert_rr(rdata, rundata, false);

	AosIILQueryReqObjPtr query = AosGetIILCacheEngine()->retrieveQuery(rdata, themsg);
	if (!query)
	{
		queryFailed(rdata, buff, AosFileAndLine);
		return false;
	}

	AosMsgPhase2ReqPtr msg = (AosMsgPhase2Req*)themsg.getPtr();
	return query->procPhase2Req(rdata, msg);
}
#endif
