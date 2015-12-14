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
// 2013/03/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryMsgs/MsgRetrvQueryBlockResp.h"


AosMsgRetrvQueryBlockResp::AosMsgRetrvQueryBlockResp(
		const AosRundataPtr &rdata, 
		const int send_phyid, 
		const int recv_phyid, 
		const u64 &query_id, 
		const OmnString &iilname, 
		const AosBitmapObjPtr &bitmap,
		const AosBuffPtr &nodelist)
:
AosBuffMsg(rdata, send_phyid, recv_phyid, OmnMsgId::eRetrvQueryBlockResp)
{
	AosBuffPtr buff;
	bitmap->saveToBuff(buff);
	setFieldU64(0, query_id);
	setFieldStr(1, iilname);
	setFieldBuff(2, buff);
	setFieldBuff(3, nodelist);
}


AosMsgRetrvQueryBlockResp::~AosMsgRetrvQueryBlockResp()
{
}

