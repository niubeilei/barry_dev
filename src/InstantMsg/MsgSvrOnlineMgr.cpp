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
// 08/03/2011 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "InstantMsg/MsgSvrOnlineMgr.h"

#include "InstantMsg/IMSessionMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/MsgClientObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEModules/ObjMgr.h"



OmnSingletonImpl(AosMsgSvrOnlineMgrSingleton, 
				 AosMsgSvrOnlineMgr, 
				 AosMsgSvrOnlineMgrSelf, 
				 "AosMsgSvrOnlineMgr");

AosMsgSvrOnlineMgr::AosMsgSvrOnlineMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosMsgSvrOnlineMgr::~AosMsgSvrOnlineMgr()
{
}


bool 
AosMsgSvrOnlineMgr::start()
{
	return true;
}


bool 
AosMsgSvrOnlineMgr::stop()
{
	return true;
}


bool
AosMsgSvrOnlineMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


bool 
AosMsgSvrOnlineMgr::start(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosMsgSvrOnlineMgr::getUserOnlineStatus(
		int &status,
		const OmnString &friend_cid,
		const AosRundataPtr &rdata)
{
	OmnString home_ctnr;
	home_ctnr << AOSCTNR_USERHOME << "." << friend_cid;
	AosXmlTagPtr home_doc = AosDocClientObj::getDocClient()->getDocByObjid(home_ctnr, rdata);
	aos_assert_rr(home_doc, rdata, false);
	status = home_doc->getAttrInt("zky_status", 0);
	return true;
}

bool
AosMsgSvrOnlineMgr::setUserOnlineStatus(
		const OmnString &status,
		const AosRundataPtr &rdata)
{
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	OmnString home_ctnr;
	home_ctnr << AOSCTNR_USERHOME << "." << sender_cid;
	AosXmlTagPtr home_doc = AosDocClientObj::getDocClient()->getDocByObjid(home_ctnr, rdata);
	aos_assert_rr(home_doc, rdata, false);
	u64 home_docid = home_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(home_docid != 0, rdata, false);
	
	bool modify = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
		home_docid, home_ctnr, "zky_status", status, "",
		false, false, true);
	aos_assert_rr(modify, rdata, false);

	if (status == "0")
	{
		OmnString o_request;
		o_request << "<request operation=\"disconnect\" cid=\"" << sender_cid << "\"/>";
		bool notify = AosMsgClientObj::procMsgSvrRequestStatic(o_request, rdata);
		aos_assert_rr(notify, rdata, false);
	}

	return true;
}
