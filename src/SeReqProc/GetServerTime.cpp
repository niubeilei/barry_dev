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
// The torturer is in SengTorturer/TesterGetServerTimeNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/GetServerTime.h"

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


AosGetServerTime::AosGetServerTime(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GETSERVERTIME, 
		AosSeReqid::eGetServerTime, rflag)
{
}


bool 
AosGetServerTime::proc(const AosRundataPtr &rdata)
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

	OmnString contents = "<Contents ";
	contents << AOSTAG_TIME << "=\"" << OmnGetMDY() << "\" "
			<< AOSTAG_FULLTIME << "=\"" << OmnGetTime(AosLocale::getDftLocale()) << "\" "
			<< AOSTAG_EPOTIME << "=\"" << (u64)OmnGetSecond() << "\" />";
	rdata->setOk();
	rdata->setResults(contents);
	AOSLOG_LEAVE(rdata);
	return true;
}

