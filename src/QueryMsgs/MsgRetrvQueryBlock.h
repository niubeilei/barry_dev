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
#ifndef Aos_QueryMsgs_MsgRetrvQueryBlock_h
#define Aos_QueryMsgs_MsgRetrvQueryBlock_h

#include "Message/QueryMsg.h"


class AosMsgRetrvQueryBlock : public AosQueryMsg
{
public:
	AosMsgRetrvQueryBlock(
		const AosRundataPtr &rdata, 
		const int send_phyid, 
		const int recv_phyid, 
		const OmnString &iilname, 
		const u64 query_id, 
		const OmnString &context_str, 
		const u64 &block_size);
	~AosMsgRetrvQueryBlock(); 

	OmnString getIILName() 		{return getFieldStr(0, "");}
	u64 getQueryid()  			{return getFieldU64(1, 0);}
	OmnString getContextStr()  	{return getFieldStr(2, "");}
	u64 getBlockSize()  		{return getFieldU64(3, 0);}
};
#endif

