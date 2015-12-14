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
// 2013/03/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryMsgs_MsgQueryPhase2Req_h
#define Aos_QueryMsgs_MsgQueryPhase2Req_h

#include "Message/QueryMsg.h"


class AosMsgQueryPhase2Req : public AosQueryMsg
{
public:
	AosMsgQueryPhase2Req(
		const AosRundataPtr &rdata, 
		const int send_phyid, 
		const int recv_phyid, 
		const OmnString &iilname, 
		const u64 &query_id, 
		const OmnString &context_str, 
		const u64 &block_size);
	~AosMsgQueryPhase2Req(); 

	OmnString getIILName()  	{return getFieldStr(0, "");}
	u64 getQueryid()  			{return getFieldU64(1, 0);}
	OmnString getContextStr() 	{return getFieldStr(2, "");}
	u64 getBlockSize()  		{return getFieldU64(3, 0);}
};
#endif

