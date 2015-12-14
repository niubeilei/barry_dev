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
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Message/QueryMsg.h"


AosQueryMsg::AosQueryMsg(
		const AosRundataPtr &rdata, 
		const OmnMsgId::E msgid,
		const int send_phyid, 
		const int recv_phyid, 
		const bool is_request,
		const u64 &query_id,
		const int version)
:
AosBuffMsg(rdata, msgid, send_phyid, recv_phyid, is_request, version),
mQueryId(0)
{
}


AosQueryMsg::AosQueryMsg(
		const AosRundataPtr &rdata, 
		const OmnMsgId::E msgid,
		const u32 send_cubeid, 
		const u32 recv_cubeid, 
		const bool is_request,
		const u64 &query_id,
		const int version)
:
AosBuffMsg(rdata, msgid, send_cubeid, recv_cubeid, is_request, version),
mQueryId(0)
{
}


AosQueryMsg::~AosQueryMsg()
{
}

