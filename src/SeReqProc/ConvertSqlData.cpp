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
// 06/15/2011	Created by Tracy 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ConvertSqlData.h"

#include "AppMgr/App.h"
#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "QueryUtil/QrUtil.h"
#include "QueryClient/QueryClient.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "SQLClient/SQLClient.h"
#include "SQLClient/Ptrs.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosConvertSqlData::AosConvertSqlData(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CONVERTSQLDATA, 
		AosSeReqid::eConvertSqlData, rflag)
{
}


bool 
AosConvertSqlData::proc(const AosRundataPtr &rdata)
{
	// It retrieves the domain. The domain name is in "args".
	// If 'args' is empty, it is an error. If 'args' is
	// 'dftdomain', it is the system default domain.

	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}


	OmnString errmsg;
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		errmsg = "Missing args!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString sdocid;
	AosParseArgs(args, "sdocid", sdocid);
	
	if (sdocid == "")
	{
		errmsg = "Missing sdocid";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		errmsg = "Missing siteid";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDocByObjid(sdocid, rdata);

	//Get Query Data
	AosXmlTagPtr query = sdoc->getFirstChild("query");
	AosXmlTagPtr cmd = query->getFirstChild("cmd");
	AosQueryReqObjPtr mQuery = AosQueryClient::getSelf()->createQuery(cmd, rdata);
	if(!mQuery)
	{
		errmsg = "Missing Query";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString contents;
	bool rslt = mQuery->procPublic(cmd, contents, rdata);
	aos_assert_r(rslt, false);
	rdata->setResults(contents);
	//End Query Data
	AosXmlParser parser;
	AosXmlTagPtr qur = parser.parse(contents, "" AosMemoryCheckerArgs);
	sdoc->addNode(qur);	
	//AosSqlClientPtr client = OmnNew AosSqlClient();
	//client->start(OmnApp::getAppConfig());
	//client->convertData(sdoc, rdata);
	AosSQLClientPtr client = OmnNew AosSQLClient();
	client->start(OmnApp::getAppConfig());
	client->convertData(sdoc, rdata);

	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}	
