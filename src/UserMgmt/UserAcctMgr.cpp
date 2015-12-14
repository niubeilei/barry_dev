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
// 08/27/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UserMgmt/UserAcctMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "UserMgmt/UserAcct.h"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(AosUserAcctMgrSingleton,
				 AosUserAcctMgr,
				 AosUserAcctMgrSelf,
				 "AosUserAcctMgr");



AosUserAcctMgr::AosUserAcctMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosUserAcctMgr::~AosUserAcctMgr()
{
}


bool
AosUserAcctMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosUserAcctMgr::start()
{
	return true;
}

bool
AosUserAcctMgr::stop()
{
	return true;
}


AosUserAcctObjPtr
AosUserAcctMgr::getUserAcct(const AosXmlTagPtr &userdoc, const AosRundataPtr &rdata)
{
	try
	{
		if (!userdoc)
		{
			AosSetError(rdata, AosErrmsgId::eMissingUserDoc);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		
		OmnString cid = userdoc->getAttrStr(AOSTAG_CLOUDID);
		if (cid == "")
		{
			AosSetError(rdata, AosErrmsgId::eMissingCloudid) 
				<< userdoc->getAttrStr(AOSTAG_DOCID);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		
		if (userdoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_USERACCT)
		{
			AosSetError(rdata, AosErrmsgId::eNotUserAccount) 
				<< userdoc->getAttrStr(AOSTAG_DOCID);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		return OmnNew AosUserAcct(userdoc, rdata);
	}

	catch (...)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateUserAcct);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
}


AosUserAcctObjPtr 
AosUserAcctMgr::getUserAcctByCloudid(
		const OmnString &cid,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr userdoc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
	return getUserAcct(userdoc, rdata);
}

 
AosUserAcctObjPtr 
AosUserAcctMgr::getUserAcctByDocid(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr userdoc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	return getUserAcct(userdoc, rdata);
}


AosUserAcctObjPtr 
AosUserAcctMgr::getUserAcctByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr userdoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	return getUserAcct(userdoc, rdata);
}


// Chen Ding, 12/30/2012
bool 
AosUserAcctMgr::checkManagership(
		const AosUserAcctObjPtr &requester,
		const AosUserAcctObjPtr &user,
		const OmnString &manager_attrname,
		const bool direct_mgr,
		const AosRundataPtr &rdata)
{
	// This function checks whether 'requester' is a manager
	// of 'user'. If A is B's manager, A should be in the
	// attribute 'manager_attrname' of B's account doc (direct manager)
	// or A is the manager of one of B's manager (superviser).
	aos_assert_rr(requester, rdata, false);	
	aos_assert_rr(user, rdata, false);	
	aos_assert_rr(manager_attrname != "", rdata, false);

	AosXmlTagPtr req_doc = requester->getDoc();
	AosXmlTagPtr user_doc = user->getDoc();
	aos_assert_rr(req_doc, rdata, false);
	aos_assert_rr(user_doc, rdata, false);

	OmnString req_username = req_doc->getAttrStr(AOSTAG_USERNAME);
	if (req_username == "") return false;

	OmnString direct_mgrs = user_doc->getAttrStr(manager_attrname);
	if (direct_mgrs == "") return false;

	if (direct_mgrs.hasCommonWords(req_username, ",")) return true;

	// Will allocate someone to do it. Chen Ding, 12/30/2012
	OmnNotImplementedYet;
	return false;
}

