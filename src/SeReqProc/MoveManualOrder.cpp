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
#include "SeReqProc/MoveManualOrder.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"


AosMoveManualOrder::AosMoveManualOrder(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_MOVE_MANUAL_ORDER, 
		AosSeReqid::eMoveManualOrder, rflag)
{
}


bool 
AosMoveManualOrder::proc(const AosRundataPtr &rdata)
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
	
	OmnString container_docid, v1, d1, v2, d2, ordername, flag;
	AosParseArgs(args, "container_docid", container_docid, "v1", v1, "d1", d1, "v2", v2, "d2", d2, "ordername", ordername, "flag", flag);

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
		errmsg = "Missing ordername";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	if (flag == "")
	{
		errmsg = "Missing flag";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString tree_iil;
	tree_iil << AOSZTG_GROUPORDER << rdata->getSiteid() << ":" << container_docid << ":" << ordername;
	u64 v11 = atoll(v1.data());
	u64 v22 = atoll(v2.data());

	//bool rs=  AosIILClient::getSelf()->moveManualOrder(tree_iil, v11, 
	bool rs=  AosIILClientObj::getIILClient()->moveManualOrder(tree_iil, v11, 
			atoll(d1.data()), v22,atoll(d2.data()), flag, rdata);
	if(!rs)
	{
		errmsg = "Fail To moveManualOrder";
		OmnAlarm << errmsg << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	contents ="<Contents>";
		contents << "<record did=\"" << d1 << "\" value=\"" << v11 << "\"></record>";
		contents << "<record did=\"" << d2 << "\" value=\"" << v22 << "\"></record>";
		contents << "</Contents>";
	rdata->setResults(contents);
	
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}
