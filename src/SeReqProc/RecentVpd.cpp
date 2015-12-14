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
// 10/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RecentVpd.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"


AosRecentVpd::AosRecentVpd(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RECENT_VPD, AosSeReqid::eRecentVpd, rflag)
{
}


bool 
AosRecentVpd::proc(const AosRundataPtr &rdata)
{
	// A user requests logging out. Logout means canceling the session.
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();

	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// 	<request>
	// 		<item name='zky_siteid'><![CDATA[xxx]]></item>
	// 		<item name='operation'><![CDATA[serverreq]]></item>
	// 		<item name='reqid'><![CDATA[recentvpd]]></item>
	// 		<item name='username'><![CDATA[xxx]]></item>
	// 		<item name='args'><![CDATA[vpdname|$|vpd_796picEditor|$|cookie]]></item>
	// 		<item name='trans_id'><![CDATA[8]]></item>
	// 	</request>
	OmnString args = root->getChildTextByAttr("name", "args");
	
	// 'args' is in the form:
	// 		"vpdname|$|the-vpd-name|$|the-cookie"
	AosStrSplit split;
	OmnString parts[10];
	bool finished;
	int nn = split.splitStr(args.data(), "|$|", parts, 10, finished);
	if (nn != 3)
	{
		OmnString errmsg = "args are incorrect: ";
		OmnAlarm << errmsg << enderr;
		AOSLOG_LEAVE(rdata);
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString vname = parts[1];
	OmnString cookie = parts[2];

	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

