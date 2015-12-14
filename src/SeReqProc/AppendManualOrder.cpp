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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/AppendManualOrder.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "SEServer/SeReqProc.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"


AosAppendManualOrder::AosAppendManualOrder(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_APPEND_MANUALORDER, 
		AosSeReqid::eAppendManualOrder, rflag)
{
}


bool 
AosAppendManualOrder::proc(const AosRundataPtr &rdata)
{
	// This function is created by Tank 2010/12/31
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString errmsg;
	OmnString contents;

	//1.get parameters
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		AosSetErrorU(rdata, "missing_args") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString container_docid, docids, ordername;
	AosParseArgs(args, "container_docid", container_docid, "docids", docids, "ordername", ordername);

	if (container_docid == "")
	{
		AosSetErrorU(rdata, "missing_container_docid") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (docids == "")
	{
		AosSetErrorU(rdata, "missing_docid") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	if (ordername == "")
	{
		AosSetErrorU(rdata, "missing_ordername") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	u64 seqn;
	OmnString order_iil;
	order_iil << AOSZTG_GROUPORDER << rdata->getSiteid() << ":" << container_docid << ":" << ordername;
	OmnStrParser1 parser(docids, "$");
	OmnString docid;
	
	contents ="<Contents>";
	while ((docid = parser.nextWord()) != "")
	{
		//bool rs=  AosIILClient::getSelf()->appendManualOrder(
		bool rs=  AosIILClientObj::getIILClient()->appendManualOrder(
				order_iil, atoll(docid.data()), seqn, rdata);
		if(!rs)
		{
			errmsg = "Fail To appendManualOrder!";
			OmnAlarm << errmsg << enderr;
			rdata->setError();
			AOSLOG_LEAVE(rdata);
			return false;
		}
		contents << "<record did=\"" << docid << "\" value=\"" << seqn << "\"></record>";
	}
	contents << "</Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}	
