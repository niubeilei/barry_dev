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
// 05/26/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CounterServer/RetrieveCounter.h"

#include "CounterServer/VirCtnrSvr.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/Ptrs.h"
#include "CounterTime/CounterTimeInfo.h"
#include "TransUtil/Ptrs.h"
#include "TransServer/TransServer.h"
#include "Thread/Mutex.h"


AosRetrieveCounter::AosRetrieveCounter(const bool regflag)
:
AosCounterProc(AosCounterOperation::eRetrieveCounter, regflag)
{
}


bool
AosRetrieveCounter::proc(
		const AosXmlTagPtr &request,
		const AosRundataPtr &rdata)
{
	// This function retrieves a set of counter.  
	//
	// The request should be in the form:
	// 	<request transid="xxx" 
	// 		counter_docid="xxx"
	// 		counter_name="Industry"
	// 		connter_type="xxx(normal, time)"
	// 		connter_stattype="sum|average|max|min.."
	//      connter_time="2011-01-01"
	// 		operation="retrieveCounter">
	// 	</request>
	//
	// 1. Determine virtual counter server
	// 2. Call its member function: updateCounter(...) 
	aos_assert_r(request, false);
	if (!request)
	{
		AosSetErrorU(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString cname = request->getAttrStr(AOSTAG_COUNTERNAME, "");
	if (cname == "")
	{
		AosSetErrorU(rdata, "missing_counter_name");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosVirCtnrSvrPtr vserver = AosPhyCtnrSvr::getVirtualServer(cname);
	if (!vserver)
	{
		AosSetErrorU(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return vserver->queryCounters(request, rdata);
}


AosCounterProcPtr
AosRetrieveCounter::clone()
{
	return OmnNew AosRetrieveCounter(false);
}
#endif
