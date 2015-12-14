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
// The torturer is in SengTorturer/TesterRebuildLogNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RebuildLog.h"

#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SeReqProc/ResumeReq.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/Docid.h"
#include "SEUtil/UrlMgr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosRebuildLog::AosRebuildLog(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_REBUILDLOG, 
		AosSeReqid::eRebuildLog, rflag)
{
}


bool 
AosRebuildLog::proc(const AosRundataPtr &rdata)
{
	// This function processes the "rebuildlog" request. It assumes
	// the request must be in the following format:
	// 	<request>
	// 		<parm name="args">seqno:offset</parm>
	//		<objdef><logdoc/><objdef>
	// 		...
	// 	</request>
	//
	// 	'args' is in the form:
	// 		seqno:offset
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rootchild = root->getFirstChild();
	AosXmlTagPtr logdoc = rootchild->getFirstChild("objdef");
	if (!logdoc)
	{
		OmnString errmsg = "Missing the object to be created";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	logdoc = logdoc->getFirstChild();
	if (!logdoc)
	{
		OmnString errmsg = "Missing the object to be created";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Young. 2014/10/31
	//AosSeLogClientObj::getLogClient()->rebuildLogEntry(logdoc, rdata);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

