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
#include "CounterServer/RetrieveCountersByQry.h"

#include "CounterServer/VirCtnrSvr.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/Ptrs.h"
#include "CounterUtil/CounterQueryType.h"
#include "CounterTime/CounterTimeInfo.h"
#include "SeLogClient/SeLogClient.h"
#include "SEUtil/SysLogName.h"
#include "TransUtil/Ptrs.h"
#include "TransServer/TransServer.h"
#include "Thread/Mutex.h"


AosRetrieveCountersByQry::AosRetrieveCountersByQry(const ReqId id, const bool regflag)
:
AosCounterProc(id, regflag)
{
}


bool
AosRetrieveCountersByQry::proc(
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
	// 		operation="xxxx">
	// 	</request>
	//
	// 1. Determine virtual counter server
	// 2. Call its member function: queryCounters(...) 
	aos_assert_r(request, false);
	if (!request)
	{
		OmnAlarm << "Failed to retrieve the request" << enderr;
		OmnString errmsg = "Failed to retrieve the request";
		return false;
	}

	u64 docid = request->getAttrU64(AOSTAG_COUNTERCTNRDOCID, 0);
	aos_assert_rr(docid > 0, rdata, false);

	// Determine the virtual server. This is done by:
	// 		docid % N
	u64 vserver_id = docid % AosPhyCtnrSvr::getVirtualServerNum();
	AosVirCtnrSvrPtr vserver = AosPhyCtnrSvr::getVirtualServer(vserver_id);
	aos_assert_r(vserver, false);

	// We need to update the counter. 
	// 1. Find the index record. Note that the index record is to be
	//    determined by docid/N
	docid = docid / AosPhyCtnrSvr::getVirtualServerNum();
	OmnString queryType = request->getAttrStr(AOSTAG_QUERYTYPE, "");
    AosCounterQueryType qType = AosCounterQueryType_strToCode(queryType);
    switch(qType)
    {
    case eAosCounter_Query_Single:
       	 {
			 bool rslt = getSingleCountersByQuery(vserver, request, rdata);
			 AOSSYSLOG_CREATE(rslt, AOSSYSLOGNAME_QUERYCOUNTER, rdata);
			 return rslt;
		 }
		 break;

    case eAosCounter_Query_Multi:
       	 {
			 bool rslt = getMultiCountersByQuery(vserver, request, rdata);
			 AOSSYSLOG_CREATE(rslt, AOSSYSLOGNAME_QUERYCOUNTER, rdata);
			 return rslt;
		 }
		 break;

	case eAosCounter_Query_Sub:
       	 {
			 bool rslt = getSubCountersByQuery(vserver, request, rdata);
			 AOSSYSLOG_CREATE(rslt, AOSSYSLOGNAME_QUERYCOUNTER, rdata);
			 return rslt;
		 }
		 break;

    default:
        OmnAlarm << "Unrecognized Query Type" << queryType << enderr;
        return false;
    }
	return false;
}


bool 
AosRetrieveCountersByQry::getSingleCountersByQuery(
						const AosVirCtnrSvrPtr &vserver,
						const AosXmlTagPtr &request, 
						const AosRundataPtr &rdata)
{
	return vserver->queryCounters(request, rdata);		
}


bool 
AosRetrieveCountersByQry::getMultiCountersByQuery(
						const AosVirCtnrSvrPtr &vserver,
						const AosXmlTagPtr &request, 
						const AosRundataPtr &rdata)
{
	aos_assert_r(rdata, false);
	aos_assert_rr(vserver, rdata, false);
	aos_assert_rr(request, rdata, false);
	AosXmlTagPtr req = request->clone(AosMemoryCheckerArgsBegin);
	req->removeNode("Contents", false, true);

	AosXmlTagPtr contents = request->getFirstChild();
	aos_assert_rr(contents, rdata, false);

	bool needSubTotal = request->getAttrBool("needSubTotal", true); 
	if (request->isRootTag()) contents = contents->getFirstChild();
	aos_assert_rr(contents, rdata, false);

	AosXmlTagPtr record = contents->getFirstChild();
	AosXmlParser parser;
	AosXmlTagPtr root;
	int crt = 0;
	OmnString respcontents = "<contents total=\"";
	OmnString resp;
	// The "resp" should be in the form:
	// <contents>
	// 		<counter_1 cname="aaa" time="xxx" value="" />
	// 		<counter_2 cname="bbb" time="xxx" value="" />
	// 		....
	// </contents>
	//
	//or nosum
	//<contents>
	//	<records>
	//		<counter cname="aaa" time="xxx" value=""/>
	//		<counter cname="aaa" time="xxx" value=""/>
	//		...
	//	</records>
	//	...
	//</contents>
	while(record)
	{
		OmnString cname = record->getAttrStr(AOSTAG_COUNTERNAME, "");
		if (cname != "")
		{
			crt++;
			req->setAttr(AOSTAG_COUNTERNAME, cname);
			bool rslt = vserver->queryCounters(req, rdata);		
			aos_assert_r(rslt, false);
			root = parser.parse(rdata->getContents(), "" AosMemoryCheckerArgs);
			aos_assert_rr(root, rdata, false);
			AosXmlTagPtr rcds = root->getFirstChild();
			//resp << "<counter " << AOSTAG_ALLCNAME << "=\"" << cname << "\" "; 
			OmnString tempName;
			if (rcds)
			{
				AosXmlTagPtr cntrecord = rcds->getFirstChild();
				if (!needSubTotal)
				{
					resp << "<records>";
					while(cntrecord)
					{
						resp << cntrecord->toString();
						cntrecord = rcds->getNextChild();
					}
					resp << "</records>";
				}
				else
				{
					resp << "<counter " << AOSTAG_ALLCNAME << "=\"" << cname << "\" "; 
					int64_t cValue = 0;
					OmnString timestr;
					while(cntrecord)
					{
						if (timestr !="") timestr << " ";
						timestr << cntrecord->getAttrStr(AOSTAG_TIME, "");
						tempName = cntrecord->getAttrStr(AOSTAG_CNAME, "");
						//int64_t value = cntrecord->getAttrInt64(AOSTAG_VALUE, 0);
						int64_t value = cntrecord->getAttrInt64("zky_sum", 0);
						cValue += value;	
						cntrecord = rcds->getNextChild();
					}
					//resp << "zky_value=\"" << cValue << "\" zky_cname=\"" << tempName << "\"/>";
					resp << "zky_sum=\"" << cValue << "\" zky_cname=\"" << tempName << "\" zky_time=\"" << timestr << "\"/>";
				}
			}
			else
			{
				resp << "zky_value=\"0\" zky_cname=\"" << tempName << "\"/>";  
			}
		}
		record = contents->getNextChild();
	}
	respcontents << crt << "\">" << resp;
	respcontents << "</contents>";
	rdata->setContents(respcontents);
	rdata->setOk();
	return true;
}


bool
AosRetrieveCountersByQry::getSubCountersByQuery(
		const AosVirCtnrSvrPtr &vserver,
        const AosXmlTagPtr &request,
	    const AosRundataPtr &rdata)
{
	aos_assert_r(rdata, false);
	aos_assert_rr(vserver, rdata, false);
	aos_assert_rr(request, rdata, false);
	return vserver->queryCountersByRanges(request, rdata);
}

AosCounterProcPtr
AosRetrieveCountersByQry::clone()
{
	return OmnNew AosRetrieveCountersByQry(eRetrieveCountersByQry, false);
}
#endif
