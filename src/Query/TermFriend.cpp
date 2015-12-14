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
// 11/09/2010 Created by James Kong
////////////////////////////////////////////////////////////////////////////
#include "Query/TermFriend.h"

#include "Alarm/Alarm.h"
#include "SEModules/OnlineMgr.h"
#include "XmlUtil/XmlTag.h"



#if 0
AosTermFriend::AosTermFriend(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosQueryTerm(AosQueryType::eFriend)
{
	// The def should be:
	// 	<term type=AOSQUERYTYPE_FRIEND  cid="" buddyType="xxx" siteid="xxx" reverse="true" />
	mBuddyType = def->getAttrStr("buddyType");
	mReverse = (def->getAttrStr("reverse", "") == "true");
	mOrder = (def->getAttrStr("order", "") == "true");
	OmnString cid = def->getAttrStr("cid");
	AosXmlTagPtr sdoc = AosDocMgr::getSelf()->getDocByCloudId(mSiteid, cid, rdata);
	if (sdoc)
	{
		mUid = sdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	}
}


AosTermFriend::AosTermFriend(
		const OmnString &siteid,
		const OmnString &buddyType, 
		const bool order,
		const OmnString &cid, 
		const AosRundataPtr &rdata)
:
AosQueryTerm(AosQueryType::eFriend)
{
	mSiteid = siteid;
	//get userid.
	//AosXmlTagPtr sdoc = AosDocServerSelf->getDocByCloudid(mSiteid, cid);
	AosXmlTagPtr sdoc = AosDocMgr::getSelf()->getDocByCloudId(mSiteid, cid, rdata);
	if (sdoc)
	{
		mUid = sdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	}
	mBuddyType = buddyType;
	mReverse = order;
	mIsGood = retrieveIIL(rdata);
	aos_assert(mIsGood);
}


AosTermFriend::~AosTermFriend()
{
}


void
AosTermFriend::toString(OmnString &str)
{
	OmnString od = (mReverse)?"true":"false";
	str << "<tag type=\"xxx\" buddyType=\"" << mBuddyType
	<< "\" reverse=\"" << od
	<< "\" order=\"" << mOrder
	<< "\"/>";
}


bool 	
AosTermFriend::nextDocid(u64 &docid, bool &finished, const AosRundataPtr &rdata)
{
	bool isunique;
	finished = false;

	if (mNoMoreDocs)
	{
		finished = true;
		docid = AOS_INVDID;
		return true;
	}
	if (!mUid)
	{
		finished = true;
		docid = AOS_INVDID;
		return true;
	}
	//retrieve friend_id by userid.
	//the value is not unique.
	
	AosIILClient::getSelf()->nextDocidSafe(mIILName,
										   mCrtIdx, 
										   mCrtIILIdx, 
										   false, 
										   eAosOpr_eq, 
										   mUid, 
										   docid, 
										   isunique,rdata);

	if (docid == AOS_INVDID || mCrtIdx < 0)
	{
		finished = true;
		mNoMoreDocs = true;
		return true;
	}
	finished = false;
	return true;
}


bool	
AosTermFriend::addTerm(const AosQueryTermObjPtr &t)
{
	OmnShouldNeverComeHere;
	return false;
}


i64		
AosTermFriend::getTotal(const AosRundataPtr &rdata)
{
	if (mNoMoreDocs) return 0;
	i64 total = AosIILClient::getSelf()->getTotalNumDocsSafe(mIILName,rdata);
	return total;
}


bool	
AosTermFriend::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	if (mNoMoreDocs) return false;
	return AosIILClient::getSelf()->docExistSafe(mIILName,docid,rdata);
}


void	
AosTermFriend::reset()
{
	mCrtIdx = -10;
	mCrtIILIdx = -10;
	mNoMoreDocs = false;
}


bool 	
AosTermFriend::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	bool finished = false;
	u64 docid;
	mCrtIdx = -10;
	mCrtIILIdx = -10;
	for (i64 i=0; i<startidx; i++)
	{
		aos_assert_r(nextDocid(docid, finished, rdata), false);
		if (finished) return false;
	}
	return true;
}


bool	
AosTermFriend::setOrder(const OmnString &name, const bool order, const AosRundataPtr &)
{
	return false;
}


bool
AosTermFriend::query(
		const AosQueryRsltObjPtr &query_rslt,
		bool &finished, 
		const AosRundataPtr &rdata)
{
	finished = true;
	OmnNotImplementedYet;
	return false;

}


AosXmlTagPtr
AosTermFriend::getDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	//get friend doc, and friend status.
	//AosXmlTagPtr doc = AosDocServer::getSelf()->getDoc(docid);
	AosXmlTagPtr doc = AosDocMgr::getSelf()->getDoc(docid, "", rdata);
	aos_assert_r(doc, 0);
	//search friend in iil.
	OmnString tm;
	u32 status;
	//AosOnlineMgr::getSelf()->CheckOnlineStatus(docid, tm, status, rdata);
	doc->setAttr(AOSTAG_ONLINETIME, tm);
	switch(status)
	{
		case 1:
			doc->setAttr(AOSTAG_ONLINESTATUS, "在线");
			break;
		case 0:
			doc->setAttr(AOSTAG_ONLINESTATUS, "离线");
			break;
		default:
			doc->setAttr(AOSTAG_ONLINESTATUS, "离线");
			break;
	}
	return doc;
}


bool
AosTermFriend::retrieveIIL(const AosRundataPtr &rdata)
{
	if (!mBuddyType) return true;
	OmnString errmsg;
	//1. get iil1name.
	OmnString buddylist;
	buddylist << mBuddyType << "." << mSiteid;

	return true;
}

bool 
AosTermFriend::getTime(time_t tt, OmnString &stm)
{
	struct tm *p;
	p = gmtime(&tt);
	if (!p)	return false;
	i64 hh = (p->tm_hour + 8)%24;
	stm << 1900+p->tm_year << "-" << 1+p->tm_mon << "-" << p->tm_mday 
		<< "  " << hh << ":" << p->tm_min << ":" << p->tm_sec;
	return true;
}
#endif


bool
AosTermFriend::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	/*
	//      "buddylist_type|$|AOSQUERYTYPE_FRIEND|$|*[|$|reverse]
	AOSMONITORLOG_LINE(rdata);
	bool rev = (nn >= 3 && fields[2] == "true")?true:false;
	mSpecialTerm = OmnNew AosTermFriend(mSiteid, fields[0], rev, cid, rdata);
	return true;
	*/
	OmnAlarm << "Friend needs to rework: " << cond_str << enderr;
	return false;
}


bool
AosTermFriend::convertQuery(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

