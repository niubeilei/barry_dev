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
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClient/DocidMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "IILUtil/IILId.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEInterfaces/IILClientObj.h"

OmnSingletonImpl(AosDocidMgrSingleton,
                 AosDocidMgr,
                 AosDocidMgrSelf,
                "AosDocidMgr");

AosDocidMgr::AosDocidMgr()
:
mInitDocid(0),
mNormalDocid(0)
{
}


AosDocidMgr::~AosDocidMgr()
{
}


bool
AosDocidMgr::config(const AosXmlTagPtr &config)
{
	// Need to retrieve the number of virtuals, xxx;
	aos_assert_r(config, false);
	AosXmlTagPtr docconfig = config->getFirstChild("docclient");
	if (!docconfig)
	{
		OmnAlarm << "Missing Doc Client Config tag" << enderr;
		exit(-1);
	}
	
	AosXmlTagPtr defs = docconfig->getFirstChild("defs");
	if (!defs)
	{
		OmnAlarm << "Missing Doc Client Config tag" << enderr;
		exit(-1);
	}
	AosXmlTagPtr def = defs->getFirstChild();
	if (!def)
	{
		OmnAlarm << "Missing Doc Client Config tag" << enderr;
		exit(-1);
	}

	mInitDocid = def->getAttrU64("crtid", 0);
	aos_assert_r(mInitDocid, false);

	mNormalDocid = OmnNew OmnDocid(mInitDocid, 1, AOSDOCIDKEY_NORMAL);
	mLogGroupDocid = OmnNew OmnDocid(mInitDocid, 1, AOSDOCIDKEY_LOG_GROUP);

	return true;
}


bool
AosDocidMgr::stop()
{
    return true;
}


bool
AosDocidMgr::start()
{
	return true;
}


u64
AosDocidMgr::nextDocid(OmnString &objid, const AosRundataPtr &rdata)
{
	return nextDocid(-1, objid, rdata);
}

u64
AosDocidMgr::nextDocid(
		const int &vid,
		OmnString &objid, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mInitDocid, 0);
	aos_assert_r(mNormalDocid, 0);
	u64 docid = 0;

	while (1)
	{
		docid = mNormalDocid->nextDocid(vid, rdata); 
		bool rslt = verifyDocid(docid, rdata);
		if (rslt) 
		{
			// Chen Ding, 03/14/2012, OBJID_CHANGE
			if (objid == "") objid = AosObjid::getDftObjid(docid, rdata);
			return docid;
		}
	}
	OmnShouldNeverComeHere;
	return 0;
}


u64
AosDocidMgr::nextLogGroupDocid(const AosRundataPtr &rdata)
{
	aos_assert_r(mInitDocid, 0);
	aos_assert_r(mLogGroupDocid, 0);
	u64 docid = 0;

	// Modify by Young, 2014/10/11
	docid = mLogGroupDocid->nextDocid(-1, rdata); 
	aos_assert_r(docid > 0, 0);

	AosDocType::setDocidType(AosDocType::eLogDoc, docid);

	return docid;

	//while (1)
	//{
	//	docid = mLogGroupDocid->nextDocid(vid, rdata); 
	//	bool rslt = verifyDocid(docid, rdata);
	//	if (rslt) 
	//	{
	//		if (objid == "") objid = AosObjid::getDftObjid(docid, rdata);
	//		return docid;
	//	}
	//}
	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDocidMgr::verifyDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata);
	if (doc)
	{
		OmnScreen << "Docid is not unique:" << docid << endl;
		return false;
	}
	return true;
}


void
AosDocidMgr::returnDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
}


bool
AosDocidMgr::bindObjid(
		OmnString &objid,
		u64 &docid,          
		bool &objid_changed,
		const bool resolve,
		const bool keepDocid,
		const AosRundataPtr &rdata)
{
	// This function binds 'objid' with 'docid'. If the objid is already used by someone
	// else, 'resolve' tells whether to resolve the objid. If 'resolve' is true, it
	// will resolve the objid by appending "(nnn)", where nnn is a small integer that makes 
	// the objid unique.
	//
	// Chen Ding, 03/14/2012, OBJID_CHANGE
	// This function is moved from DocClient.
	
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);

	objid_changed = false;
	if (objid != "")
	{
		if (!AosObjid::isValidObjid(objid, rdata))
		{
			OmnAlarm << rdata->getErrmsg() << enderr;
			if (!resolve) return false;
			objid = "";
			objid_changed = true;
		}
	}

	if (objid == "")
	{
		if (!resolve)
		{
			AosSetErrorU(rdata, "objid_is_empty_001");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	if (!keepDocid) 
	{
		if (docid)
		{
			OmnAlarm << "To bind an objid but docid is not null: " << docid << enderr;
		}
		if (objid == "") objid_changed = true;
		docid = nextDocid(objid, rdata);
	}
	else
	{
		if (!docid)
		{
			OmnAlarm << "To bind an objid but docid is null: " << objid << enderr;
			if (objid == "") objid_changed = true;
			docid = nextDocid(objid, rdata);
		}
	}

	aos_assert_r(docid, false);
	aos_assert_r(objid != "", false);

	bool rslt = AosIILClientObj::getIILClient()->bindObjid(objid, docid, rdata);
	if (!rslt) 
	{
		AosSetError(rdata, "failed_bind_objid_001")
			<< ". objid:" << objid << ", docid:" << docid;
		//	OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	// Ketty RlbTest tmp.
	//AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByObjid(objid, rdata);
	//if (doc)
	//{                                                                       
	//	AosSetError(rdata, "failed_bind_objid_001") << ". objid: " << objid 
	//		<< ", docid: " << docid;
	//	return false;
	//}

	//if (AosDocClient::showLog())
	//{
	//	OmnScreen << "Objid bound: " << objid << ":" << docid 
	//		<< ":" << rdata->getTransid() << endl;
	//}

	rdata->setOk();
	return true;
}


// Chen Ding, 2013/11/01
u64
AosDocidMgr::nextDocid(
		const AosRundataPtr &rdata,
		const int vid)
{
	aos_assert_r(mInitDocid, 0);
	aos_assert_r(mNormalDocid, 0);

	while (1)
	{
		u64 docid = mNormalDocid->nextDocid(vid, rdata); 
		bool rslt = verifyDocid(docid, rdata);
		if (rslt) return docid;
	}
	OmnShouldNeverComeHere;
	return 0;
}


