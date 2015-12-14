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
// 05/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogSvr/ProcCreateLog.h"

#include "LogSvr/LogSvr.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"


AosProcCreateLog::AosProcCreateLog(
		const OmnString &name, 
		const AosLogProcId::E id, 
		const bool regflag)
:
AosLogProc(name, id, regflag)
{
}


bool
AosProcCreateLog::procReq(const AosRundataPtr &rdata)
{
	// This function creates a log. The log doc is stored
	// in rdata
	AosXmlTagPtr logdoc = rdata->getReceivedDoc();
	if (!logdoc)
	{
		rdata->setError() << "Missing the log doc";
		return false;
	}

	return AosLogSvr::getSelf()->createLog(logdoc, rdata);
}

