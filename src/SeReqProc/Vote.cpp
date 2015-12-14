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
// 10/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/Vote.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosVote::AosVote(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_VOTE, AosSeReqid::eVote, rflag)
{
}


bool 
AosVote::proc(const AosRundataPtr &rdata)
{
	// A user requests logging out. Logout means canceling the session.
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();

	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	if (args =="")
	{
		rdata->setError() << "Missing args!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString objid, type, sobjid, collection;
	AosParseArgs(args, "objid", objid, "type", type, 
			"sdocobjid", sobjid, "collection", collection);

	if (objid == "")
	{
		rdata->setError() << "Missing objid!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (type == "")
	{
		rdata->setError() << "Missing type!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString tt = AOSZTG_VOTE;	
	AosXmlTagPtr doc;
	if (objid == "" || !(doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata)))
	{
		AosSetError(rdata, "object_not_found") << ": " << objid;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if(sobjid != "")
	{
		rdata->setRetrievedDoc(root, true);
		rdata->setArg1(AOSARG_OBJID, objid);
		rdata->setArg1(AOSARG_COLLECTION, collection);
		AosSmartDocObj::procSmartdocsStatic(sobjid, rdata);
		if (!rdata->isOk())
		{
			OmnString errmsg = rdata->getErrmsg();
			rdata->setError() << errmsg;
			AOSLOG_LEAVE(rdata);
			return true;
		}

		AosDocClientObj::getDocClient()->incrementValue(
				doc->getAttrU64(AOSTAG_DOCID, 0), 
				doc->getAttrStr(AOSTAG_OBJID),
				type, "0", true, true, rdata);

		AosDocClientObj::getDocClient()->incrementValue(
				doc->getAttrU64(AOSTAG_DOCID, 0), 
				doc->getAttrStr(AOSTAG_OBJID), 
				AOSTAG_VOTETOTAL, "0", true, true, rdata);
	}
	else
	{
		rdata->setError() << "Missing SmartDoc!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
		
	OmnString sdocs = doc->getAttrStr(AOSTAG_SMARTDOCS_VOTE);
	if (sdocs != "")
	{
		// Chen Ding, 2011/02/09
		AosSmartDocObj::procSmartdocsStatic(sdocs, rdata);
	}
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

