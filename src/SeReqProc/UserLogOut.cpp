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
#include "SeReqProc/UserLogOut.h"

#include "EventMgr/EventMgr.h"
#include "SEServer/SeReqProc.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "Security/ScrtUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/SeLogClientObj.h"


AosUserLogOut::AosUserLogOut(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_USER_LOGOUT, 
		AosSeReqid::eUserLogOut, rflag)
{
}


bool 
AosUserLogOut::proc(const AosRundataPtr &rdata)
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

	rdata->setArg1("zky_login_status", "logout");
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_LOGIN, rdata);
	AosSessionObjPtr session = rdata->getSession();
	if(session)
	{
		session->logout();
		OmnString contents = "<zky_lgnobj zky_ssid=\"logout\"/>";

		OmnString cookie;
		cookie << AosComposeSsidCookieName(rdata->getUrldocDocid()) << "=logout"
			        << ";path=/;Max-Age=0;";
		rdata->setSendCookie(cookie);
		rdata->setResults(contents);
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;
	}

	rdata->setError() << "Failed retrieving the session!";
	AOSLOG_LEAVE(rdata);
	return false;
}
