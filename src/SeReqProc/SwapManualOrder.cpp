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
#include "SeReqProc/SwapManualOrder.h"

#include "EventMgr/EventMgr.h"
#include "SEInterfaces/IILClientObj.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"


AosSwapManualOrder::AosSwapManualOrder(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SWAP_MANUAL_ORDER, 
		AosSeReqid::eSwapManualOrder, rflag)
{
}


bool 
AosSwapManualOrder::proc(const AosRundataPtr &rdata)
{
	// This function is created by Tank 2010/12/22
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
		errmsg = "Missing args!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString container_docid, v1, d1, v2, d2, ordername;
	AosParseArgs(args, "container_docid", container_docid, "v1", 
			v1, "d1", d1, "v2", v2, "d2", d2, "ordername", ordername);

	if (container_docid == "")
	{
		errmsg = "Missing container_docid";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (v1 == "" || d1 == "" || v2 == "" || d2 == "")
	{
		errmsg = "Missing docid or value";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (ordername == "")
	{
		AosSetErrorU(rdata, "missing_ordername") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString tree_iil;
	tree_iil << AOSZTG_GROUPORDER << rdata->getSiteid() << ":" << container_docid << ":" << ordername;
	//bool rs=  AosIILClient::getSelf()->swapManualOrder(tree_iil, 
	bool rs=  AosIILClientObj::getIILClient()->swapManualOrder(tree_iil, 
			atoll(v1.data()), atoll(d1.data()), atoll(v2.data()), atoll(d2.data()), rdata);
	if(!rs)
	{
		errmsg = "Fail To swapManualOrder!";     
		OmnAlarm << errmsg << enderr;

		rdata->setError();
		AOSLOG_LEAVE(rdata);
		return false;
	}

	contents ="<Contents>";
	contents << "<record did=\"" << d1 << "\" value=\"" << v2 << "\"></record>"
			 << "<record did=\"" << d2 << "\" value=\"" << v1 << "\"></record>"
		 << "</Contents>";

	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}
