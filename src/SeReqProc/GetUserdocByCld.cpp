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
// 08/15/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/GetUserdocByCld.h"

#include "SEInterfaces/DocClientObj.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "XmlUtil/XmlTag.h"


AosGetUserdocByCld::AosGetUserdocByCld(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GETUSERDOCBYCLD, AosSeReqid::eGetUserdocByCld, rflag)
{
}


bool 
AosGetUserdocByCld::proc(const AosRundataPtr &rdata)
{
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr xmldoc = root->getFirstChild("objdef");
	if (!xmldoc)
	{
		rdata->setError() << "Missing object";
		OmnAlarm <<  rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr doc = xmldoc->getFirstChild();
	aos_assert_r(doc, false);

	OmnString cid =	root->getChildTextByAttr("name", "cid");
	if (cid == "")
	{
		rdata->setError() << "Missing Cloudid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr userdoc  = AosDocClientObj::getDocClient()->getDocByCloudid(
									cid, rdata);
	if (!userdoc)
	{
		rdata->setError() << "Failed to retrieve doc by cloud id";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString contents = "<Contents>";
	contents << userdoc->toString() << "</Contents>";
	rdata->setOk();
	rdata->setResults(contents);
	AOSLOG_LEAVE(rdata);
	return true;                
}

