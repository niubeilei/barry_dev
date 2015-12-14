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
#include "Query/TermAllowance.h"

#if 0
#include "Alarm/Alarm.h"
#include "SearchEngine/DocMgr.h"
#include "XmlUtil/XmlTag.h"




AosTermAllowance::AosTermAllowance(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosQueryTerm(AosQueryType::eAllowance)
{
	// The def should be:
	// 	<term type=AOSQUERYTYPE_ALLOWANCE siteid="xxx" reverse="true" sdoc_objid="xxx"/>
	mIILName = "";
	mSdocObjid = def->getAttrStr("sdoc_objid");
	mReverse = (def->getAttrStr("reverse", "") == "true");
	mOrder = (def->getAttrStr("order", "") == "true");
	aos_assert(retrieveIIL(rdata));
	mIsGood = true;
}


AosTermAllowance::AosTermAllowance(
		const OmnString &siteid,
		const OmnString &sdoc_objid, 
		const bool order, 
		const AosRundataPtr &rdata)
:
AosQueryTerm(AosQueryType::eAllowance)
{
	mSiteid = siteid;
	mSdocObjid = sdoc_objid;
	mReverse = order;
	mIsGood = retrieveIIL(rdata);
	aos_assert(mIsGood);
}



void
AosTermAllowance::toString(OmnString &str)
{
	OmnString od = (mReverse)?"true":"false";
	str << "<tag type=\"xxx\" sdoc_objid=\"" << mSdocObjid
	<< "\" reverse=\"" << od
	<< "\" order=\"" << mOrder
	<< "\"/>";
}


bool 	
AosTermAllowance::nextDocid(u64 &docid, bool &finished, const AosRundataPtr &rdata)
{
	bool isunique;
	finished = false;
	if (mNoMoreDocs)
	{
		finished = true;
		docid = AOS_INVDID;
		return true;
	}

	//get iilu32 nextdoc.
	u64 vv;
	AosIILClient::getSelf()->nextDocidSafe(mIILName, 
										   mCrtIdx, 
										   mCrtIILIdx, 
										   false, 
										   eAosOpr_an, 
										   docid, 
										   vv, 
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
AosTermAllowance::addTerm(const AosQueryTermObjPtr &t)
{
	OmnShouldNeverComeHere;
	return false;
}


i64		
AosTermAllowance::getTotal(const AosRundataPtr &rdata)
{
	if (mNoMoreDocs) return 0;
	i64 total = AosIILClient::getSelf()->getTotalNumDocsSafe(mIILName, rdata);
	return total;
}


bool	
AosTermAllowance::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	if (mNoMoreDocs) return false;
	return AosIILClient::getSelf()->docExistSafe(mIILName, docid, rdata);
}


void	
AosTermAllowance::reset(const AosRundataPtr &rdata)
{
	mCrtIdx = -10;
	mCrtIILIdx = -10;
	mNoMoreDocs = false;
}


bool 	
AosTermAllowance::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
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
AosTermAllowance::setOrder(const OmnString &name, const bool order, const AosRundataPtr &)
{
	return false;
}


AosXmlTagPtr
AosTermAllowance::getDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	//AosXmlTagPtr doc = AosDocServer::getSelf()->getDoc(docid);
	AosXmlTagPtr doc = AosDocMgr::getSelf()->getDoc(docid, "", rdata);
	aos_assert_r(doc, 0);

	i64 idx = -10;
	i64 iilidx = -10;
	bool isunique;
	u64 vv;
	u64 newVV = docid;
	bool rslt = AosIILClient::getSelf()->nextDocidSafe(mIILName,
												   idx, 
												   iilidx, 
												   false, 
												   eAosOpr_eq, 
												   newVV, 
												   vv, 
												   isunique,rdata);
	aos_assert_r(rslt, 0);
	aos_assert_r(isunique, 0);
	aos_assert_r(vv > 0, 0);
	u32 tt = (vv >> 32);
	OmnString tm;
	getTime((time_t)tt, tm);
	u32 value = (u32)vv;

	doc->setAttr(AOSTAG_ALLOWANCETIME, tm);
	doc->setAttr(AOSTAG_ALLOWANCEVALUE, value);
	return doc;
}


bool
AosTermAllowance::retrieveIIL(const AosRundataPtr &rdata)
{
	if (mSdocObjid == "") return true;

	//AosXmlTagPtr sdoc = AosDocServer::getSelf()->getDoc(mSiteid, mSdocObjid);
	AosXmlTagPtr sdoc = AosDocMgr::getSelf()->getDoc(mSiteid, mSdocObjid, rdata);
	if (!sdoc)
	{
		OmnAlarm << "Failed retrieving smartdoc: " << mSdocObjid << enderr;
		return false;
	}

	OmnString str = sdoc->getAttrStr(AOSTAG_IILNAME, "");
	if (str == "")
	{
		OmnAlarm << "no iilname!" << enderr;
		return false;
	}

	OmnString iilname;
	iilname << AOSZTG_ALLOWANCE << str;			
//	if (!hasIIL()) mNoMoreDocs = true;
	return true;
}


bool 
AosTermAllowance::getTime(time_t tt, OmnString &stm)
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
AosTermAllowance::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	/* Allowance needs to be re-worked
	// It is to retrieve allowances. The query should be in the form:
	// 		"<smartdoc_objid>|$|AOSQUERYTYPE_ALLOWANCE|$|*[|$|reverse]
	AOSMONITORLOG_LINE(rdata);
	bool rev = (nn >= 3 && fields[2] == "true")?true:false;
	mSpecialTerm = OmnNew AosTermAllowance(mSiteid, fields[0], rev, rdata);
	return true;
	*/
	OmnAlarm << "Allowance needs to re-work: " << cond_str << enderr;
	return false;
}


bool
AosTermAllowance::convertQuery(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}
