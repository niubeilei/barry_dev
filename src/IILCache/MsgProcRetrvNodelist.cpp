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
// 2013/02/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILCache/MsgProcRetrvNodelist.h"


AosMsgProcRetrvNodelist::AosMsgProcRetrvNodelist()
{
}


AosMsgProcRetrvNodelist::~AosMsgProcRetrvNodelist()
{
}


bool 
AosMsgProcRetrvNodelist::procMsg(
		const AosRundataPtr &rundata,
		const AosBuffMsgPtr &themsg)
{
	OmnMsgId::E msgid = themsg->getMsgId();
	aos_assert_rr(msgid == OmnMsgId::eRetrvNodelist, rundata, false);
	AosRundataPtr rdata = themsg->getRundata();
	aos_assert_rr(rdata, rundata, false);

	AosIILQueryReqObjPtr query = AosGetIILCacheEngine()->retrieveQuery(rdata, themsg);
	if (!query)
	{
		queryFailed(rdata, buff, AosFileAndLine);
		return false;
	}

	AosMsgRetrvNodelistPtr msg = (AosMsgRetrvNodelist*)themsg.getPtr();
	return query->retrieveNodelist(rdata, msg);
}
#endif
