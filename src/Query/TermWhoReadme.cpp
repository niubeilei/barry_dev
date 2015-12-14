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
// "Who Read Me" is a container log feature. We may configure a container
// to keep track of who read a doc in a container. The log is identified
// by a log-type. 
//
// Modification History:
// 11/09/2010 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "Query/TermWhoReadme.h"

#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEModules/LogMgr.h"

#include "XmlUtil/XmlTag.h"

#if 0
AosTermWhoReadme::AosTermWhoReadme(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosQueryTerm(AosQueryType::eWhoReadme)
{
	// The def should be:
	// 	<term type=AOSQUERYTYPE_WHOREADME siteid="xxx" reverse="true">
	// 		<Records>
	// 			<record objid|docie=""/>
	// 			...
	// 		</Records>
	// 	</term>
	//
	mIILName = "";
	mdocObjid = def->getAttrStr("docid");
	mReverse = (def->getAttrStr("reverse", "") == "true");
	mOrder = (def->getAttrStr("order", "") == "true");
	aos_assert(retrieveIIL(rdata));
	mIsGood = true;
}


AosTermWhoReadme::AosTermWhoReadme(
		const OmnString &siteid,
		const OmnString &doc_objid, 
		const bool reverse, 
		const bool filter, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
:
AosQueryTerm(AosQueryType::eWhoReadme)
{
	mSiteid = siteid;
	mdocObjid = doc_objid;
	mReverse = reverse;
	mIsGood = retrieveIIL(rdata);
	mFilter = filter;
	mCid = cid;
	aos_assert(mIsGood);
}


AosTermWhoReadme::~AosTermWhoReadme()
{
}


void
AosTermWhoReadme::toString(OmnString &str)
{
	OmnString od = (mReverse)?"true":"false";
	str << "<tag type=\"xxx\" sdoc_objid=\"" << mdocObjid
	<< "\" reverse=\"" << od
	<< "\" order=\"" << mOrder
	<< "\"/>";
}


bool 	
AosTermWhoReadme::nextDocid(u64 &docid, bool &finished, const AosRundataPtr &rdata)
{
	bool isunique;
	finished = false;
	if (mNoMoreDocs)
	{
		finished = true;
		docid = AOS_INVDID;
		return true;
	}

	//get log docid.
	//sqno + offset.
	
	u64 logTime;	//Actually, logTime is value of iil_entry, and docid is logSeqno-the docid of iil_entry
	AosIILClient::getSelf()->nextDocidSafe(mIILName, 
						mCrtIdx, 
						mCrtIILIdx, 
						mReverse, 
						eAosOpr_an, 
						logTime, 
						docid, 
						isunique,
						rdata);

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
AosTermWhoReadme::addTerm(const AosQueryTermObjPtr &t)
{
	OmnShouldNeverComeHere;
	return false;
}


i64		
AosTermWhoReadme::getTotal(const AosRundataPtr &rdata)
{
	if (mNoMoreDocs) return 0;
	i64 total = AosIILClient::getSelf()->getTotalNumDocsSafe(mIILName,rdata);
	return total;
}


bool	
AosTermWhoReadme::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	if (mNoMoreDocs) return false;
	return AosIILClient::getSelf()->docExistSafe(mIILName,docid,rdata);
}


void	
AosTermWhoReadme::reset()
{
	mCrtIdx = -10;
	mCrtIILIdx = -10;
	mNoMoreDocs = false;
}


bool 	
AosTermWhoReadme::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
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
AosTermWhoReadme::setOrder(
		const OmnString &container,
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &)
{
	return false;
}



bool
AosTermWhoReadme::query(
		const AosQueryRsltObjPtr &query_rslt,
		bool &finished, 
		const AosRundataPtr &rdata)
{
	finished = true;
	OmnNotImplementedYet;
	return false;

}


AosXmlTagPtr
AosTermWhoReadme::getDoc(const u64 &docid)
{
	//retrieve log entry by docid, this docid is logSeqno.
	AosXmlTagPtr doc = AosLogMgr::getSelf()->getLogEntry(docid);
	if (mFilter && doc && mCid != "" && doc->getAttrStr(AOSTAG_CLOUDID) == mCid)
	{
		return 0;
	}
	return doc;
}


bool
AosTermWhoReadme::retrieveIIL(const AosRundataPtr &rdata)
{
	if (mdocObjid == "") return true;
	
	OmnString errmsg;
	
	//1. Retrieve the docid by the mdocObjid 
	u64 docid;
	bool isunique;
	bool rslt = AosIILClient::getSelf()->getDocidByObjid(
					mSiteid, 
					mdocObjid, 
					docid, 
					isunique,
					rdata);

	aos_assert_r(rslt && docid != AOS_INVDID, false);

	//2.Create the IIL Name
	OmnString iilname = AOSZTG_WHOVISITED;
	iilname << docid;

	return true;
}


bool 
AosTermWhoReadme::getTime(time_t tt, OmnString &stm)
{
	struct tm *p;
	p = gmtime(&tt);
	if (!p)	return false;
	i64 hh = (p->tm_hour + 8)%24;
	stm << 1900+p->tm_year << "-" << 1+p->tm_mon << "-" << p->tm_mday 
		<< "  " << hh << ":" << p->tm_min << ":" << p->tm_sec;
	return true;
}

bool
AosTermWhoReadme::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	/*
	//      "AOSQUERYTYPE_WHOREADME|$|=|$|objid|$|reverse|$|filter
	AOSMONITORLOG_LINE(rdata);
	bool rev = (nn >= 4 && fields[3] == "true")?true:false;
	bool filter = !(nn >= 5 && fields[4] == "false");
	mSpecialTerm = OmnNew AosTermWhoReadme(mSiteid, fields[2], rev, filter, cid, rdata);
	return true;
	*/
	OmnAlarm << "Who read me needs to re-work: " << cond_str << enderr;
	return false;
}


bool
AosTermWhoReadme::convertQuery(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

#endif
