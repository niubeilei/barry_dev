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
// 2013/03/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILCache/MsgProcRetrvIILBlock.h"


AosMsgProcRetrvIILBlock::AosMsgProcRetrvIILBlock()
{
}


AosMsgProcRetrvIILBlock::~AosMsgProcRetrvIILBlock()
{
}


bool 
AosMsgProcRetrvIILBlock::procMsg(
		const AosRundataPtr &rundata,
		const AosBuffMsgPtr &themsg)
{
	OmnMsgId::E msgid = themsg->getMsgId();
	aos_assert_rr(msgid == OmnMsgId::eRetrvIILBlock, rundata, false);
	AosRundataPtr rdata = themsg->getRundata();
	aos_assert_rr(rdata, rundata, false);

	AosMsgRetrvIILBlockPtr msg = (AosMsgRetrvIILBlock*)themsg.getPtr();
	AosBuffPtr context_buff = msg->getContextBuff();
	aos_assert_fr(context_buff, rdata, false);

	AosQueryContextObjPtr context = AosCreateQueryContext(context_buff);
	aos_assert_fr(context, rdata, false);

	OmnString iilname = msg->getIILName();
	aos_assert_fr(iilname != "", rdata, false);
	aos_assert_fr(AosIsIILLocal(iilname), rdata, false);

	u64 start_time = OmnGetTimestamp();
	
	// Ready to retrieve the query block.
	AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
	aos_assert_fr(iilmgr, rdata, false);
	AosBuffPtr bitmap_buff, nodelist_buff;
	u64 total_docs = 0;
	u64 picked_docs = 0;
	bool finished;
	bool rslt = iilmgr->retrieveQueryBlock(rdata, mIILName, 
			bitmap_buff, nodelist_buff, total_docs, picked_docs, finished);
	aos_assert_fr(rslt, rdata, false);
	
	u64 end_time = OmnGetTimestamp();

	AosCubeMsgPtr resp = OmnNew AosMsgRetrvIILBlockResp(rdata, 
			AosGetSelfPhysicalId(), msg->getSendPhyId(), msg->getQueryId(), 
			iilname, bitmap_buff, nodelist_buff, context_buff,
			start_time, end_time, total_docs, picked_docs, finished);

	bool rslt = AosSendCubeMsg(rdata, resp);
	aos_assert_fr(rslt, rdata, false);
	return true;
}
#endif
