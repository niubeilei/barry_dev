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
// The torturer is in SengTorturer/TesterResolveUrlNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ResolveUrl.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SeReqProc/ResumeReq.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEModules/UrlMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEUtil/Docid.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosResolveUrl::AosResolveUrl(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RESOLVEURL, 
		AosSeReqid::eResolveUrl, rflag)
{
}


bool 
AosResolveUrl::proc(const AosRundataPtr &rdata)
{
	// This function processes the "resolve_uri" request. It assumes
	// the request must be in the following format:
	// 	<request>
	// 		<parm name="args">url</parm>
	// 		...
	// 	</request>
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString url = root->getChildTextByAttr("name", "args");
	OmnString full_url = root->getChildTextByAttr("name", AOSTAG_FULL_URL);
	OmnString query_str = root->getChildTextByAttr("name", AOSTAG_URL_QUERY_STR);
	if (url == "" && full_url == "") 
	{	
		AOSLOG_LEAVE(rdata);
		return true;
	}
	
	OmnString vpdname, objname, svpd;
	u32 siteid = rdata->getSiteid();
	AosXmlTagPtr urldoc;
	bool rslt = AosUrlMgr::getSelf()->resolveUrl(url, full_url, query_str,
			vpdname, objname, svpd, urldoc, rdata);
	if (!rslt || vpdname == "" || !urldoc)
	{
		rdata->setError() << "Docid not not have a URI"; 
		AOSLOG_LEAVE(rdata);
		return true;
	}

	AosXmlTagPtr vpd = AosDocClientObj::getDocClient()->getDocByObjid(vpdname, rdata);
	if (!vpd)
	{
		rdata->setError() << "Failed to retrieve VPD";
		AOSLOG_LEAVE(rdata);
		return true;
	}

	AosXmlTagPtr obj;
	if (objname != "")
	{
		//AosXmlTagPtr obj = AosIILClient::getSelf()->getDoc(mSiteid, objname);
		obj = AosDocClientObj::getDocClient()->getDocByObjid(objname, rdata);
	}

	//AosXmlRc errcode;
	OmnString errmsg;
	AosWebRequestPtr req = rdata->getReq();
	rslt = AosSeReqProc::retrieveObjPublic(req, root, vpd, false, obj, urldoc, rdata);
	if (!rslt)
	{
		//--Ketty
		AOSLOG_LEAVE(rdata);
		return false;
	}


	rdata->setOk();
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_RESOLVEURL, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}

