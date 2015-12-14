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
#include "CounterServer/RetrieveCounterSet.h"

#include "CounterServer/VirCtnrSvr.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/Ptrs.h"
#include "CounterTime/CounterTimeInfo.h"
#include "TransUtil/Ptrs.h"
#include "TransServer/TransServer.h"
#include "Thread/Mutex.h"


AosRetrieveCounterSet::AosRetrieveCounterSet(const ReqId id, const bool regflag)
:
AosCounterProc(id, regflag)
{
}


bool
AosRetrieveCounterSet::proc(
		const AosXmlTagPtr &request,
		const AosRundataPtr &rdata)
{
	// This function retrieves a set of counter.  
	//
	// The request should be in the form:
	// 	<request transid="xxx" 
	// 		counter_type="MMYYYY"
	// 		counter_startime="2011-01-01"
	// 		counter_endtime="2011-12-01"
	// 		counter_docid="xxx"
	// 		counter_name="Industry"
	// 		operation="retrieveCounterSet">
	// 		<records>
	// 			<record name="Industry"></record>
	// 			<record name="Agriculture"></record>
	// 		</records>
	// 	</request>
	//
	// 1. Determine virtual counter server
	// 2. Call its member function: updateCounter(...) 
	aos_assert_r(request, false);
	if (!request)
	{
		OmnAlarm << "Failed to retrieve the request" << enderr;
		OmnString errmsg = "Failed to retrieve the request";
		return false;
	}

	//OmnString ctype = request->getAttrStr(AOSTAG_COUNTERTYPE, "");
	//OmnString startTime = request->getAttrStr(AOSTAG_COUNTERSTARTTIME, "");
	//OmnString endTime = request->getAttrStr(AOSTAG_COUNTERENDTIME, "");
	u64 docid = request->getAttrU64(AOSTAG_COUNTERCTNRDOCID, 0);
	//OmnString cname = request->getAttrStr(AOSTAG_COUNTERNAME, "");
	//int64_t value = request->getAttrInt64(AOSTAG_COUNTERVALUE,0);
	
	aos_assert_r(docid > 0, false);

	// Determine the virtual server. This is done by:
	// 		docid % N
	u64 vserver_id = docid % AosPhyCtnrSvr::getVirtualServerNum();
	AosVirCtnrSvrPtr vserver = AosPhyCtnrSvr::getVirtualServer(vserver_id);
	aos_assert_r(vserver, false);

	// We need to update the counter. 
	// 1. Find the index record. Note that the index record is to be
	//    determined by docid/N
	docid = docid / AosPhyCtnrSvr::getVirtualServerNum();

	return vserver->retrieveCounterSet(request, rdata);
}


AosCounterProcPtr
AosRetrieveCounterSet::clone()
{
	return OmnNew AosRetrieveCounterSet(eRetrieveCounterSet, false);
}
#endif
