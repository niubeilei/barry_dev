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
// 2013/04/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProcTemplate/SeReqProcTemplate.h"

#include "Rundata/Rundata.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "Thread/Mutex.h"


static OmnMutex sgLock;
static bool 	sgRegistered = false;

AosSeReqProcTemplate::AosSeReqProcTemplate()
:
AosSeRequestProc(AOSREQIDNAME_TEMPLATE)
{
}


bool
AosSeReqProcTemplate::init()
{
	if (!sgRegistered)
	{
		sgLock.lock();
		if (sgRegistered)
		{
			sgLock.unlock();
			return true;
		}

		OmnString errmsg;
		bool rslt = registerSeProc(OmnNew AosSeReqProcTemplate(), errmsg);
		sgRegistered = true;
		sgLock.unlock();
		if (!rslt)
		{
			OmnAlarm << "Failed registering proc: " << AOSREQIDNAME_TEMPLATE << enderr;
			return false;
		}
	}

	return true;
}


bool 
AosSeReqProcTemplate::proc(const AosRundataPtr &rdata)
{
	// The message format is:
	// 	<request ...>
	// 		...
	// 		<parms>
	// 			<parmname>xxx</parmname>
	// 			<parmname>xxx</parmname>
	// 			...
	// 		</parms>
	// 	</request>
	rdata->getLog() << "the-operation";

	AosXmlTagPtr message = rdata->getRequestRoot();
	if (!message)
	{
		AosSetErrorU(rdata, "internal_error");
		return false;
	}

	/*
	OmnString parm1 = AosGetReqParmStr(rdata, message, "parm1", "");
	u64 parm2 = AosGetReqParmU64(rdata, message, "parm2", 0);
	int64_t parm3 = AosGetReqParmU64(rdata, message, "parm3", -1);

	if (parm1 != "")
	{
		AosSetErrorU(rdata, "something-is-wrong");
		return false;
	}
	*/

	rdata->setOk();
	return true;
}

