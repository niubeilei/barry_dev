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
// The torturer is in SengTorturer/TesterRebuildVersionNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RebuildVersion.h"

#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
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


AosRebuildVersion::AosRebuildVersion(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_REBUILDVERSION, 
		AosSeReqid::eRebuildVersion, rflag)
{
}


bool 
AosRebuildVersion::proc(const AosRundataPtr &rdata)
{
	// This function processes the "resolve_uri" request. It assumes
	// the request must be in the following format:
	// 	<request>
	// 		<parm name="args">docid:version:seqno:offset</parm>      //old version
	// 		<parm name="args">docid:version</parm>
	// 		<objdef><versiondoc/><objdef>
	// 		...
	// 	</request>
	//
	// 	'args' is in the form:
	// 		docid:seqno:offset:version
	//
	//args << docid << ":" << version << ":" << seqno << ":" << offset;
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	AosXmlTagPtr doc = root->getFirstChild("objdef");
	if (!doc)
	{
		AosSetErrorUser(rdata, "internal_error") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	doc = doc->getFirstChild();

	if (!doc)
	{
		AosSetErrorUser(rdata, "internal_error") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosStrSplit split;
	OmnString parts[2];
	bool finished;
	int nn = split.splitStr(args.data(), ":", parts, 2, finished);
	if (nn != 2)
	{
		OmnString errmsg = "args are incorrect: ";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	u64 docid = aos_atoull(parts[0].data());
	OmnString version = parts[1];

	AosVersionServer::getSelf()->addVersionDoc(docid, doc->toString(), version, rdata);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

