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
// All requests are stored in mRequests. When there are too many, it will 
// stop adding requests to mRequests. When mRequests are empty, it checks
// whether there are additional requests from the log file. If yes, it
// reads in all the requests to mRequests. 
//
// Modification History:
// 06/24/2011: Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgSvr/AllShortMsgProc.h"

#include "ShortMsgSvr/ProcSendShm.h"

//AosStr2U32_t			sgReqidMap;		// Ketty 2013/02/25 temp.
AosShmReqProcPtr		sgShmReqProcs[AosShmReqid::eMax];
AosAllShortMsgProc 		sgAllShortMsgProc;
AosAllShortMsgProc::AosAllShortMsgProc()
{
static AosProcSendShm           sgAosProcSendShm(true);
}
