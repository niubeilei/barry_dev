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
// 08/15/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RetrieveLog.h"

#include "SeReqProc/ReqidNames.h"
#include "SmartDoc/SMDMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeLogUtil/LogNames.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "XmlUtil/XmlTag.h"


AosRetrieveLog::AosRetrieveLog(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RETRIEVELOG, AosSeReqid::eRetrieveLog, rflag)
{
}


bool 
AosRetrieveLog::proc(const AosRundataPtr &rdata)
{
	// Chen Ding, 2013/06/17
	OmnNotImplementedYet;
	return false;
	/*
	AOSLOG_ENTER_R(rdata, false);		
	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr xmldoc = root->getFirstChild("objdef");
	if (!xmldoc)
	{
		rdata->setError() << "Missing object";
		OmnAlarm <<  rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr loginfo = xmldoc->getFirstChild();
	aos_assert_r(loginfo, false);

	// root should be this form:
	// 		<loginfo><[CDATA[logid]]></loginfo>
	OmnString logidstr = loginfo->getNodeText();
	if (logidstr == "")
	{
		//AosSetError(rdata, AosErrmsgId::eMissingLogid);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Logid: " << logidstr << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	u64 logid = (u64) atoll(logidstr.data());
<<<<<<< Updated upstream
	AosXmlTagPtr resplog = AosSeLogClientObj::getSelf()->retrieveLog(logid, rdata);
=======
	AosXmlTagPtr resplog = AosSeLogClientObj::getLogClient()->retrieveLog(logid, rdata);
>>>>>>> Stashed changes
	if (!resplog)
	{
		AosSetError(rdata, AosErrmsgId::eFailedRetrieveLog);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Logid: " << logid << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString resp = "<Contents>";
	resp << resplog->toString() << "</Contents>";
	rdata->setResults(resp);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
	*/
}

