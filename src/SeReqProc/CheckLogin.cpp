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
#include "SeReqProc/CheckLogin.h"

#include "SEServer/SeReqProc.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SearchEngine/DocServer.h"
#include "Security/SessionMgr.h"


AosCheckLogin::AosCheckLogin(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CHECKLOGIN, 
		AosSeReqid::eCheckLogin, rflag)
{
}


bool 
AosCheckLogin::proc(const AosRundataPtr &rdata)
{
	// MsgServer . This function checklogin
	//<request reqid = "checklogin" >
	//	<objdef>
	//		<Contents>
	//			<ssid>xxxx</ssid>
	//			<cid>xxxx</cid>
	//		</Contents>
	//	</objdef>
	//</request>
	
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError() << "Missing objdef";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr contents = objdef->getFirstChild("Contents");
	if (!contents)
	{
		rdata->setError() << "Missing the contents";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString ssid = contents->getNodeText("ssid");
	if (ssid == "")
	{
		rdata->setError() << "Missing the session id";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosSessionPtr session = AosSessionMgr::getSelf()->getSession1(ssid, rdata);
	if (!session)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString cid = contents->getNodeText("cid");
	if (cid == "")
	{
		rdata->setError() << "Missing the user cid id";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString user_cid = session->getCid();
	if (cid != user_cid)
	{
		rdata->setError() << "Missing match user session";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString cnts;
	cnts << "<Contents/>";
	rdata->setResults(cnts);
	rdata->setOk();
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_CHECKLOGIN, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}

