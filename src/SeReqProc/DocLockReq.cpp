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
// 10/17/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/DocLockReq.h"

#include "EventMgr/EventMgr.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocLock/DocLock.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/OnlineMgr.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "SeReqProc/Ptrs.h"


AosDocLockReq::AosDocLockReq(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_DOCLOCKREQ, 
		AosSeReqid::eDocLockReq, rflag)
{
}


bool 
AosDocLockReq::proc(const AosRundataPtr &rdata)
{
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		return false;
	}
	
	AosXmlTagPtr reqobj = root->getFirstChild("objdef");
	if (!reqobj|| !(reqobj = reqobj->getFirstChild()))
	{
		rdata->setError() << "Missing the object to be created";
		return false;
	}

	OmnString objid = reqobj->getAttrStr(AOSTAG_OBJID, "");
	if (objid == "")
	{
		rdata->setError() << "Missing objid!";
		return false;
	}
		

	u64 docid = AosDocClientObj::getDocClient()->getDocidByObjid(objid, rdata);
	if (!docid)
	{
		rdata->setError() << "Missing the objid,  objid:" << objid;
		return false;
	}
	aos_assert_r(reqobj->getAttrStr(AOSTAG_LOCK_TYPE, "") != "", false);
	bool rslt = AosDocClientObj::getDocClient()->procDocLock(rdata, reqobj, docid);
	if (!rslt)
	{
		return false;
	}
	rdata->setOk();
	return true;
}
