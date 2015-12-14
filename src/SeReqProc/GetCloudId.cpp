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
#include "SeReqProc/GetCloudId.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "SEServer/SeReqProc.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosGetCloudId::AosGetCloudId(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GET_CLOUDID, 
		AosSeReqid::eGetCloudId, rflag)
{
}


bool 
AosGetCloudId::proc(const AosRundataPtr &rdata)
{
	//  <request ...>
	//        <item name="args">docid:xxx</item>
	//    </request>
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString errmsg;

	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		errmsg = "Missing args!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString userid;
	AosParseArgs(args, "userid", userid);
	
	if (userid == "")
	{
		errmsg = "Missing userid";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	u64 uid = atoll(userid.data());
	if (!uid)
	{
		errmsg = "Not a userid: ";
		errmsg << userid;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return true;
	}

	OmnString cid = AosDocClientObj::getDocClient()->getCloudid(uid, rdata);
	if (cid == "")
	{
		errmsg = "No cloudid found!";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return true;
	}
	
	OmnString contents = "<Contents ";
	contents << AOSTAG_CLOUDID << "=\"" << cid << "\"/>";
	rdata->setOk();
	rdata->setResults(contents);
	AOSLOG_LEAVE(rdata);
	return true;
}	
