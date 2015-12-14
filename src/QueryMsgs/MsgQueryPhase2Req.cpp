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
// 2013/03/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryMsgs/MsgQueryPhase2Req.h"


AosMsgQueryPhase2Req::AosMsgQueryPhase2Req(
		const AosRundataPtr &rdata, 
		const int send_phyid, 
		const int recv_phyid, 
		const OmnString &iilname, 
		const u64 &query_id, 
		const OmnString &context_str, 
		const u64 &block_size)
:
AosQueryMsg(rdata, OmnMsgId::eQueryPhase2Req, send_phyid, recv_phyid, true, query_id)
{
	setFieldStr(0, iilname);
	setFieldU64(1, query_id);
	setFieldStr(2, context_str);
	setFieldU64(3, block_size);
}


AosMsgQueryPhase2Req::~AosMsgQueryPhase2Req()
{
}

