////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/25/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "InstantMsg/IMSessionMgr.h"

#include "InstantMsg/IMManager.h"
#include "InstantMsg/MsgSvrOnlineMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Thread/Mutex.h"
#include "SEUtil/IILName.h"


OmnSingletonImpl(AosIMSessionMgrSingleton,
				 AosIMSessionMgr,
				 AosIMSessionMgrSelf,
				 "AosIMSessionMgr");


AosIMSessionMgr::AosIMSessionMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosIMSessionMgr::~AosIMSessionMgr()
{
}

bool
AosIMSessionMgr::start()
{
	return true;
}


bool
AosIMSessionMgr::stop()
{
	return true;
}


bool
AosIMSessionMgr::config(const AosXmlTagPtr &def)
{
	return true;
}

bool
AosIMSessionMgr::start(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosIMSessionMgr::getIMSession(
		AosIMSessionPtr &session,
		const OmnString &recver_cid,
		const AosRundataPtr &rdata)
{
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);
	OmnString id;
	id << sender_cid << "_" << recver_cid;
	OmnIMSessionMapItr it = mIMSessions.find(id);
	if (it == mIMSessions.end())
	{
		id = "";
		id << recver_cid << "_" << sender_cid;
		it = mIMSessions.find(id);
		if (it == mIMSessions.end())
		{
			//create session
			session = OmnNew AosIMSession(sender_cid, recver_cid);
			mIMSessions.insert(make_pair(id, session));
		}
		else
		{
			session = it->second;
		}
	}
	else
	{
		session = it->second;
	}
	return true;
}

bool
AosIMSessionMgr::addMsg(
		const OmnString &recver_cid,
		const OmnString &msg,
		const AosRundataPtr &rdata)
{
	AosIMSessionPtr session;
	getIMSession(session, recver_cid, rdata);
	aos_assert_rr(session, rdata, false);
	session->addMsg(msg, rdata);
	return true;
}

bool 
AosIMSessionMgr::finishSession(
		const AosXmlTagPtr &notes,
		const AosRundataPtr &rdata)
{
	//check all friends
	//if friend offline to finish session
	//else return
	
	OmnString sender_cid = rdata->getCid();
	aos_assert_rr(sender_cid != "", rdata, false);

	OmnString friend_ctnr;
	friend_ctnr << AOSTAG_CTNR_FRIEND << "." << sender_cid;
	OmnString g_iilname = AosIILName::composeCtnrMemberObjidIILName(friend_ctnr);
	//OmnString g_iilname = AosIILName::composeCtnrMemberListing(friend_ctnr, "");
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

	// Chen Ding, 03/04/2012
	// ICE_WARN, how to set page size, need review
	query_context->setOpr(eAosOpr_an);
	query_context->setStrValue("");
	bool rslt = AosQueryColumn(g_iilname, queryRslt, 0, query_context, rdata);

	aos_assert_rr(rslt, rdata, false);
	u64 docid = 0;
	bool finished = false;
	while(queryRslt->nextDocid(docid, finished, rdata))
	{
		if (finished)
		{
			break;
		}
		aos_assert_rr(docid != 0, rdata, false);
		AosXmlTagPtr group_doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (!group_doc)
		{
			rdata->setError() << "group doc is not exist: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			continue;
		}

		OmnString group_id = group_doc->getAttrStr("zky_objid", "");
		aos_assert_rr(group_id != "", rdata, false);
		OmnString f_iilname = AosIILName::composeCtnrMemberObjidIILName(group_id);
		//OmnString f_iilname = AosIILName::composeCtnrMemberListing(group_id, "");
		AosQueryRsltObjPtr f_queryRslt = AosQueryRsltObj::getQueryRsltStatic();
		AosQueryContextObjPtr f_query_context = AosQueryContextObj::createQueryContextStatic();

		// Chen Ding, 03/04/2012
		// ICE_WARN, how to set page size, need review
		f_query_context->setOpr(eAosOpr_an);
		bool f_rslt = AosQueryColumn(f_iilname, f_queryRslt, 0, f_query_context, rdata);

		aos_assert_rr(f_rslt, rdata, false);
		u64 f_docid = 0;
		bool f_finished = false;
		while(f_queryRslt->nextDocid(f_docid, f_finished, rdata))
		{
			if (f_finished)
			{
				break;
			}
			aos_assert_rr(f_docid != 0, rdata, false);
			AosXmlTagPtr self_friend_doc = AosDocClientObj::getDocClient()->getDocByDocid(f_docid, rdata);
			if (!self_friend_doc)
			{
				rdata->setError() << "friend doc is not exist: " << f_docid;
				OmnAlarm << rdata->getErrmsg() << enderr;
				continue;
			}
			OmnString friend_cid = self_friend_doc->getAttrStr("zky_friend_cid", "");
			aos_assert_rr(friend_cid !="", rdata, false);
			int status = 0;
			AosMsgSvrOnlineMgr::getSelf()->getUserOnlineStatus(status, friend_cid, rdata);
			if (status != 0)
			{
				continue;
			}
			AosIMSessionPtr session;
			getIMSession(session, friend_cid, rdata);
			aos_assert_rr(session, rdata, false);
			bool rslt = session->finishSession(friend_cid, notes, rdata);
			aos_assert_rr(rslt, rdata, false);
		}
	}
	return true;
}
