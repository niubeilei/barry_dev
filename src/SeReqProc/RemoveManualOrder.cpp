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
#include "SeReqProc/RemoveManualOrder.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/IILClientObj.h"


AosRemoveManualOrder::AosRemoveManualOrder(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_REMOVE_MANUAL_ORDER, 
		AosSeReqid::eRemoveManualOrder, rflag)
{
}


bool 
AosRemoveManualOrder::proc(const AosRundataPtr &rdata)
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
	
	OmnString container_docid, v1, d1, ordername;
	AosParseArgs(args, "container_docid", container_docid, "v1", v1, "d1", d1, "ordername", ordername);

	if (container_docid == "")
	{
		errmsg = "Missing container_docid";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (v1 == "" || d1 == "")
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
	//bool rs=  AosIILClient::getSelf()->removeManualOrder(tree_iil, 
	bool rs=  AosIILClientObj::getIILClient()->removeManualOrder(tree_iil, 
			atoll(v1.data()), atoll(d1.data()), rdata);
	if(!rs)
	{
		errmsg = "Fail To removeManualOrder";
		OmnAlarm << errmsg << enderr;
		rdata->setError();
		AOSLOG_LEAVE(rdata);
		return false;
	}

	contents = "<Contents></Contents>";
	rdata->setResults(contents);

	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}
