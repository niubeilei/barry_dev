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
// 12/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CreateOprArd.h"

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


AosCreateOprArd::AosCreateOprArd(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATE_OPRARD, AosSeReqid::eCreateOprArd, rflag)
{
}


bool 
AosCreateOprArd::proc(const AosRundataPtr &rdata)
{
	// It creates an Operation Access Record. There shall be one and only
	// one Operation Access Record per User Domain. This operation must be
	// done by administrators only.
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();

	if (!root)
	{
		AosSetError(rdata, AOSLT_MISSING_REQUEST_DOC);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	if (args =="")
	{
		AosSetError(rdata, AOSLT_MISSING_ARGS);
		rdata->setError() << "Missing args!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString user_domain, sdoc_objid;
	AosParseArgs(args, "userdomain", user_domain, "sdoc", sdoc_objid); 

	if (user_domain == "")
	{
		AosSetError(rdata, AOSLT_MISSING_USER_DOMAIN);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (sdoc_objid == "")
	{
		AosSetError(rdata, AOSLT_MISSING_SDOC_OBJID);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	if (!objdef)
	{
		AosSetError(rdata, AOSLT_MISSING_DOC);
		OmnAlarm << rdata->getErrmsg() << ". Data: " << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	rdata->setReceivedDoc(root, true);
	rdata->setWorkingDoc(objdef, false);
	bool rslt = AosSmartDoc::runSmartdoc(sdoc_objid, rdata);
	if (!rslt) 
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

