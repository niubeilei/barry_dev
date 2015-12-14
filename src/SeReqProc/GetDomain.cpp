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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/GetDomain.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosGetDomain::AosGetDomain(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GET_DOMAIN, 
		AosSeReqid::eGetDomain, rflag)
{
}


bool 
AosGetDomain::proc(const AosRundataPtr &rdata)
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
	OmnString domain = root->getChildTextByAttr("name", "args");

	if (domain == "")
	{
		errmsg = "Missing the domain name!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString rslt;
	if (domain == "dftdomain")
	{
		rslt = "http://";
		rslt << AosSeReqProc::getDomainAddr()<< "/images";
	}
	else
	{
		errmsg = "Domain name not recognized: ";
		errmsg << domain;
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString contents = "<Contents>";
	contents << rslt << "</Contents>";

	rdata->setResults(contents);
	rdata->setOk();

	AOSLOG_LEAVE(rdata);
	return true;
}	
