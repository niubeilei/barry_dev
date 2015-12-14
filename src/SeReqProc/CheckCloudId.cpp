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
#include "SeReqProc/CheckCloudId.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosCheckCloudId::AosCheckCloudId(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CHECK_CLOUDID, 
		AosSeReqid::eCheckCloudId, rflag)
{
}


bool 
AosCheckCloudId::proc(const AosRundataPtr &rdata)
{
	//  <request ...>
	//        <item name="args">cloudid:xxx</item>
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

	OmnString cid;
	AosParseArgs(args, "cloudid", cid);

	if (cid == "")
	{
		errmsg = "Missing cloudid";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	u64 userid = 0;
	bool rslt = AosDocClientObj::getDocClient()->isCloudidBound(cid, userid, rdata);
	OmnString rr = (rslt)?"true":"false";

	OmnString contents = "<Contents rslt=\"";
	contents << rr << "\" userid=\"" << userid << "\"/>";
	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

