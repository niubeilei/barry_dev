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
#include "SeReqProc/GetUserOprArd.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/UserAcctObj.h"
#include "SEInterfaces/UserDomainObj.h"
#include "EventMgr/EventMgr.h"
#include "QueryUtil/QrUtil.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "UserMgmt/UserAcct.h"

AosGetUserOprArd::AosGetUserOprArd(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GET_USER_OPRARD, AosSeReqid::eGetUserOprArd, rflag)
{
}


bool 
AosGetUserOprArd::proc(const AosRundataPtr &rdata)
{
	// User Operation Access Record is a doc whose objid is composed through
	// 'AosObjid::composeUserOprArdObjid(userid)'. This function retrieves
	// the doc. If the doc does not exist, it will create it. 
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

	OmnString username;
	u64 userid = 0;
	AosParseArgs(args, "username", username); 

	if (username == "")
	{
		// This is to retrieve the requester's own user operation ard.
		userid = rdata->getUserid();
		if (!userid)
		{
			AosSetError(rdata, AOSLT_ACCESS_DENIED);
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}
	else
	{
		// Need to make sure the requester is an admin. 
		if (!AosSecurityMgr::isRequesterAdmin(rdata))
		{
			AosSetError(rdata, AOSLT_ACCESS_DENIED);
			AOSLOG_LEAVE(rdata);
			return true;
		}
		userid = AosLoginMgr::getUserid(username, rdata);
		if (!userid)
		{
			AosSetError(rdata, AOSLT_USER_NOT_FOUND);
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	// The userid should have been retrieved.
	if (!userid)
	{
		AosSetError(rdata, AOSLT_INTERNAL_ERROR);
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (username == "")
	{
		username = AosUserAcctObj::getUsername(userid, rdata);
		if (username == "")
		{
			AosSetError(rdata, AOSLT_USER_NOT_VALID);
			OmnAlarm << rdata->getErrmsg() << ": " << userid << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	OmnString uid;
	uid << userid;
	OmnString objid = AosObjid::composeUserOprArdObjid(uid);
	if (objid == "")
	{
		AosSetError(rdata, AOSLT_INTERNAL_ERROR);
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (!doc)
	{
		// The doc is not there yet. Need to create it.
		doc = AosUserDomainObj::createUserOprArd(userid, username, rdata);
		if (!doc) 
		{
			AOSLOG_LEAVE(rdata);
			return true;
		}
	}

	OmnString contents = "<Contents>";
	vector<AosQrUtil::FieldDef> fielddef;
	AosQrUtil::createRecord(rdata, contents, doc, fielddef);
	contents << "</Contents>";
	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

