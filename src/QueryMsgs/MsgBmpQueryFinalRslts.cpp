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
// 2013/03/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryMsgs/MsgBmpQueryFinalRslts.h"


AosMsgBmpQueryFinalRslts::AosMsgBmpQueryFinalRslts(
		const AosRundataPtr &rdata, 
		const int send_phyid, 
		const int recv_phyid, 
		const u64 &query_id) 
:
AosQueryMsg(rdata, OmnMsgId::eBmpQueryFinalRslts, send_phyid, recv_phyid, true, query_id)
{
}


AosMsgBmpQueryFinalRslts::~AosMsgBmpQueryFinalRslts()
{
}

