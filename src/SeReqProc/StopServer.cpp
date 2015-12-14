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
#include "SeReqProc/StopServer.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"


AosStopServer::AosStopServer(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_STOP_SERVER, AosSeReqid::eStopServer, rflag)
{
}


bool 
AosStopServer::proc(const AosRundataPtr &rdata)
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

	// 'args' stop the username and password:
	// 		"uname:xxx,passwd:xxx"
	//
	// IMPORTANT: username and password should not contain ',' and ':'
	OmnString args = root->getChildTextByAttr("name", "args");

	AosStrSplit split;
	OmnString parts[2];
	bool finished;
	int nn = split.splitStr(args.data(), ",", parts, 2, finished);
	if (nn != 2)
	{
		rdata->setError() << "args are incorrect(001): " << args;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString username, password;
	for (int i=0; i<nn; i++)
	{
		OmnString pair[2];
		int mm = split.splitStr(parts[i].data(), ":", pair, 2);
		if (mm != 2)
		{
			rdata->setError() << "args are incorrect(002): " << args;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		if (pair[0] == "uname") username = pair[1];
		else if (pair[0] == "passwd") password = pair[1];
		else
		{
			rdata->setError() << "args are incorrect(003): " << args;
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	// Security Check
	OmnNotImplementedYet;
	rdata->setError() << "Not implemented yet!";
	AOSLOG_LEAVE(rdata);
	return false;
}

