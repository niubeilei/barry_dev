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
// The torturer is in SengTorturer/TesterGetServerInfoNew.cpp
//   
//
// Modification History:
// 05/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/GetServerInfo.h"

#include "API/AosApi.h"
#include "SeReqProc/ReqidNames.h"
#include "Rundata/Rundata.h"
#include "SEServer/SeReqProc.h"

AosGetServerInfo1::AosGetServerInfo1(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GETSERVERINFO, AosSeReqid::eGetServerInfo, rflag)
{
}


bool 
AosGetServerInfo1::proc(const AosRundataPtr &rdata)
{
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		AosSetErrorUser(rdata, "missing_request") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	// 1. Retrieve the Object
	OmnString errmsg;
	AosXmlTagPtr rootchild = root->getFirstChild();
	AosXmlTagPtr doc;
	if (!rootchild || !(doc = rootchild->getFirstChild()))
	{
		AosSetErrorUser(rdata, "missing_root_child") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString rslts = "<Contents>";
	rslts << AosRetrieveServerInfo(doc, rdata.getPtrNoLock()) << "</Contents>";
	rdata->setResults(rslts);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}


