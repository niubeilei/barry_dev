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
// The torturer is in SengTorturer/TesterResolveUriNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ResolveUri.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEUtil/Docid.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosResolveUri::AosResolveUri(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RESOLVEURI, 
		AosSeReqid::eResolveUri, rflag)
{
}


bool 
AosResolveUri::proc(const AosRundataPtr &rdata)
{
	// This function processes the "resolve_uri" request. It assumes
	// the request must be in the following format:
	// 	<request>
	// 		<parm name="args">docid</parm>
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

	OmnString docid = root->getChildTextByAttr("name", "args");
	
	u64 did = AosDocid::convertToU64(docid);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(did, rdata);
	if (!doc)
	{
		rdata->setError() << "Docid not exist"; 
		AOSLOG_LEAVE(rdata);
		return true;
	}

	// We assume 'doc' has an attribute AOSTAG_URI, which contains
	// the doc's URI.
	OmnString uri = doc->getAttrStr(AOSTAG_URI);
	if (uri == "")
	{
		rdata->setError() << "Docid not not have a URI"; 
		AOSLOG_LEAVE(rdata);
		return true;
	}

	// The response should be in the form:
	// 	<Contents>uri</Contents>
	OmnString contents;
	contents << "<Contents>" << uri << "</Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}
