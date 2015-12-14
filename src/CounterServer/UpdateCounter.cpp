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
#include "CounterServer/UpdateCounter.h"

#include "CounterServer/VirCtnrSvr.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/Ptrs.h"
#include "CounterTime/CounterTimeInfo.h"
#include "SeLogClient/SeLogClient.h"
#include "SEUtil/SysLogName.h"
#include "TransUtil/Ptrs.h"
#include "TransServer/TransServer.h"
#include "Thread/Mutex.h"


AosUpdateCounter::AosUpdateCounter(const bool regflag)
:
AosCounterProc(AosCounterOperation::eUpdateCounter, regflag)
{
}


bool
AosUpdateCounter::proc(
		const AosXmlTagPtr &request, 
		const AosRundataPtr &rdata)
{
	// This function updates a counter. A counter is identified by 
	// a Counter Name. Counter names are mapped to Counter ID (ctid).
	// Updating a counter means to add a value (specified in the
	// input data) to the counter. If the counter is not there
	// yet, it will create it (setting the initial value to 0).
	// This means that we need the following parameters:
	// 	1. Counter Name
	// 	2. Value
	// Updating counters is controlled by a smart doc, which is
	// stored in 'rdata'. 
	//
	// The request is in 'rdata'.
	//
	// The request should be in the form:
	// 	<request transid="xxx" 
	// 		connter_type="xxx(normal, time)"
	// 		connter_stattype="max, min, average..."
	// 		connter_time="2011-01-01"
	// 		counter_name="xxx" 
	// 		counter_value="xxx"
	// 		counter_ctnrdocid="xxx"
	// 		counter_userid="xxx"
	// 		counter_lastsend="xxx"
	// 		operation="xxxx"
	// 		.../>
	// If 'value' is not there, it defaults to 1. 
	//
	// 1. Determine virtual counter server
	// 2. Call its member function: updateCounter(...) 
	if (!request)
	{
		rdata->setError() << "Failed to retrieve the request" ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 ctnrdocid = request->getAttrU64(AOSTAG_COUNTERCTNRDOCID, 0);
	OmnString cname = request->getAttrStr(AOSTAG_COUNTERNAME, "");
	OmnString statType = request->getAttrStr(AOSTAG_COUNTERSTATTYPE, "");
	OmnString timeStr = request->getAttrStr(AOSTAG_COUNTERTIME, "");
	OmnString time_gran = request->getAttrStr(AOSTAG_TIME_GRAN);
	OmnString time_format = request->getAttrStr(AOSTAG_TIME_FORMAT);
	int64_t value = request->getAttrInt64(AOSTAG_COUNTERVALUE,0);
	
	// Determine the virtual server. This is done by:
	// 		ctnrdocid % N
	u64 vserver_id = ctnrdocid % AosPhyCtnrSvr::getVirtualServerNum();
	AosVirCtnrSvrPtr vserver = AosPhyCtnrSvr::getVirtualServer(vserver_id);
	aos_assert_r(vserver, false);

	// We need to update the counter. 
	// 1. Find the index record. Note that the index record is to be
	//    determined by ctnrdocid/N
	ctnrdocid = ctnrdocid / AosPhyCtnrSvr::getVirtualServerNum();

	bool rslt = vserver->updateCounter(cname, timeStr, time_gran, statType, 
			time_format, value, rdata);
	AOSSYSLOG_CREATE(rslt, AOSSYSLOGNAME_UPDATACOUNTER, rdata);
	return rslt;
}


AosCounterProcPtr
AosUpdateCounter::clone()
{
	return OmnNew AosUpdateCounter(eUpdateCounter, false);
}
#endif
