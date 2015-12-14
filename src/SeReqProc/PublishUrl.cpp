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
// 06/15/2011	Created by Peng Hui
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/PublishUrl.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEModules/UrlMgr.h"
#include "SEBase/SeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"


AosPublishUrl::AosPublishUrl(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_PUBLISH, AosSeReqid::ePublishUrl, rflag)
{
}


bool 
AosPublishUrl::proc(const AosRundataPtr &rdata)
{

	// This function processes the "Publish" request. It assumes
	// the request must be in the following format:
	// 	<request>
	// 		<objdef>
	// 		<Contents override="xxx">
	// 			<url>xxx</url>
	// 			<vpdname>xxx</vpdname>
	// 			<description>xxx</description>
	// 			<objname>xxx</objname>
	// 			<zky_wrapper zky_vpdname="xxx" zky_appid="xxx" zky_instid="xxx"/>
	// 		</Conents>
	// 		</objdef>
	// 	</request>
	//
	// 1. Check whether the URL already exists. If yes, it
	//    sends a response, prompting the user for overriding.
	//    If 'override' is present and is 'true', it means 
	//    to override the url.
	// 2. If it is to override the url, it checks whether
	//    the user has the right to override. If not, 
	//    it informs the user. The request is rejected.
	// 3. If the url is not used by others, it checks
	//    whether the user has the right. If not, 
	//    it informs the user and the operation is aborted.
	// 4. Otherwise, it creates an object for the url.
	
	// This function is created by john 2010/12/16
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// 1. Retrieve the Object
	AosXmlTagPtr rootchild = root->getFirstChild();
	if (!rootchild)
	{
		AosSetError(rdata, "eDataNull");
		OmnAlarm << rdata->getErrmsg() << ". Data: " 
			<< root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	if (!objdef || !(objdef = objdef->getFirstChild()))
	{
		AosSetError(rdata, "eMissingObject");
		OmnAlarm << rdata->getErrmsg() << ". Data: "
			<< root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString url = objdef->getNodeText(AOSTAG_URL);
	if (url == "")
	{
		AosSetError(rdata, AOSLT_MISSING_URL);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool url_used = false;
	bool rslt = AosUrlMgr::getSelf()->createUrl(rdata, objdef, url_used);
	if (!rslt) 
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	// This means that the user has the right to perform the operation.
	if (url_used)
	{
		// The url has been used by someone else
		if (objdef->getAttrStr(AOSTAG_OVERRIDE) == "true")
		{
			rslt = AosUrlMgr::getSelf()->overrideUrl(rdata, objdef);
			if (!rslt)
			{
				AOSLOG_LEAVE(rdata);
				return false;
			}
			AOSSYSLOG_CREATE(true, AOSREQIDNAME_PUBLISH, rdata);
			AOSLOG_LEAVE(rdata);
			return true;
		}

		// The url has been used by someone else. Need to prompt the user
		// for overriding.
		AOSSYSLOG_CREATE(true, AOSREQIDNAME_PUBLISH, rdata);
		rdata->setError() << "URL is used, need to overriding!";
		AOSLOG_LEAVE(rdata);
		return true;
	}

	OmnString contents = "<Contents><zky_url><![CDATA[";
	contents << url << "]]></zky_url></Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_PUBLISH, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}


