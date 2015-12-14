//////////////////////////////////////////////////////////////////////////
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
// 05/05/2009: Created by Sharon Shen 
// 12/31/2012: turned off by Chen Ding
//////////////////////////////////////////////////////////////////////////
#if 0
#include "LogServer/Processor.h"

#include "Debug/Debug.h"
#include "LogServer/Log.h"
#include "LogServer/LogMgr.h"
#include "LogServer/LogReq.h"
#include "LogServer/Ptrs.h"
#include "Proggie/ProggieUtil/NetRequest.h"
#include "Util/OmnNew.h"
#include "Util/String.h"
#include "util_c/strutil.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/Ptrs.h"


static AosLogMgrPtr sgLogMgr;

AosLogProcessor::AosLogProcessor()
{
	if(!sgLogMgr)
	{
		sgLogMgr = OmnNew AosLogMgr();
	}
}


AosLogProcessor::~AosLogProcessor()
{
}


bool
AosLogProcessor::procRequest(const OmnConnBuffPtr &req)
{
	const AosNetRequestPtr request = OmnNew AosLogReq(req->getData());
    AosLogPtr log;
    if(request->getOperator() == "create")
    {
        log                    = sgLogMgr->createLog(request);
        OmnTcpClientPtr client = req->getConn();
        OmnString logId        = log->getLogId();
        OmnTrace << "Send Log id is : " << logId << endl;
        client->smartSend(logId.data(), logId.length());
    }
    else
    {
        log = sgLogMgr->getLog(request);
        log->procReq(request);
    }
    return true;
}

#endif
