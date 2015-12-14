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
// 2013/03/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryMsgs_MsgRetrvNodeSectionsResp_h
#define Aos_QueryMsgs_MsgRetrvNodeSectionsResp_h

#include "Message/QueryMsg.h"


class AosMsgRetrvNodeSectionsResp : public AosQueryMsg
{
public:
	AosMsgRetrvNodeSectionsResp(
		const AosRundataPtr &rdata, 
		const int send_phyid, 
		const int recv_phyid, 
		const u64 query_id) 
	~AosMsgRetrvNodeSectionsResp(); 
};
#endif

