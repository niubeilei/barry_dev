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
// 2013/02/15 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILCache/MsgProcRetrvQueryBlock.h"


AosMsgProcRetrvQueryBlock::AosMsgProcRetrvQueryBlock()
{
}


AosMsgProcRetrvQueryBlock::~AosMsgProcRetrvQueryBlock()
{
}


bool 
AosMsgProcRetrvQueryBlock::procMsg(
		const AosRundataPtr &rundata,
		const AosBuffMsgPtr &themsg)
{
	OmnMsgId::E msgid = themsg->getMsgId();
	aos_assert_rr(msgid == OmnMsgId::eRetrvQueryBlock, rundata, false);
	AosRundataPtr rdata = themsg->getRundata();
	aos_assert_rr(rdata, rundata, false);

	AosIILQueryReqObjPtr query = AosGetIILCacheEngine()->retrieveQuery(rdata, themsg);
	if (!query)
	{
		query = AosGetIILCacheEngine()->createQuery(rdata, themsg);
		aos_assert_fr(query, rdata, false);
	}

	AosMsgRetrvQueryBlockPtr msg = (AosMsgRetrvQueryBlock*)themsg.getPtr();
	return query->retrieveQueryBlock(rdata, msg);
}
#endif
