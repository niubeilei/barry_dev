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
// 08/08/2011 Assigned to Linda Lin	
////////////////////////////////////////////////////////////////////////////
#include "Query/TermComment.h"

#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "QueryCond/QueryCond.h"
#include "XmlUtil/XmlTag.h"


AosTermComment::AosTermComment(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_COMMENT, AosQueryType::eComment, regflag)
{
}


AosTermComment::AosTermComment(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_COMMENT, AosQueryType::eComment, false)
{
	// The def should be:
	// 	<term type=AOSQUERYTYPE_COMMENT 
	// 		zky_siteid="xxx" 
	// 		reverse="true|false" 
	// 		order="true|false"
	// 		cmtd_objid="xxx" 
	// 		cmtd_docid="xxx"/>
	// 		<cond .../>
	// 	</term>
	mCmtDocid = def->getAttrU64("cmtd_docid", 0);
	mCmtobjid = def->getAttrStr("cmtd_objid");
	if (!mCmtDocid)
	{
		AosXmlTagPtr cmtdoc = AosDocClientObj::getDocClient()->getDocByObjid(mCmtobjid, rdata);
		aos_assert(cmtdoc);	
		mCmtDocid = cmtdoc->getAttrU64(AOSTAG_DOCID, 0);
	}
	mReverse = (def->getAttrStr("reverse", "") == "true");
	mOrder = (def->getAttrStr("order", "") == "true");
	mIsGood = AosIILSelector::resolveIIL(def, mIILName, rdata);
	aos_assert(mIsGood);
	mCond = AosQueryCond::parseCondStatic(def, rdata);
	mIsGood = true;
}


AosTermComment::AosTermComment(
		const OmnString &cmt_docid,
		const OmnString &cmt_objid,
		const bool order, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_COMMENT, AosQueryType::eComment, false)
{
	/*
	if (cmt_docid != "")
	{
		u32 len = cmt_docid.length();
		cmt_docid.parseU64(0, len, mCmtDocid);
	}
	if (cmt_docid == "" && cmt_objid != "")
    {
    	AosXmlTagPtr doc = AosDocClient::getSelf()->getDoc(rdata->getSiteid(), cmt_objid, rdata);
    	if (doc)
    		mCmtDocid = doc->getAttrU64(AOSTAG_DOCID, 0);
    }
	mReverse = order;
	mIsGood = true;
	mIILName = AosIILName::composeCmtIILName();
	*/
	OmnNotImplementedYet;
}


void
AosTermComment::toString(OmnString &str)
{
	OmnString od = (mReverse)?"true":"false";
	str << "<tag type=\"xxx\" cmtd_docid=\"" << mCmtDocid
	<< "\" cmtd_objid=\"" << mCmtobjid
	<< "\" reverse=\"" << od
	<< "\" order=\"" << mOrder
	<< "\"/>";
}


bool
AosTermComment::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	/*
	 * "AOSQUERYTYPE_COMMENT|$|cmtd_docid|$|cmtd_objid|$|*|$|reverse"
	 * AOSMONITORLOG_LINE(rdata);
	 * bool rev = (nn >= 3 && fields[4] == "true")?true:false;
	 * mSpecialTerm = OmnNew AosTermComment(mSiteid, fields[1], fields[2], rev, rdata);
	 * return true;
	 * 
	*/
	OmnAlarm << "Comment needs to rework: " << cond_str << enderr;
	return false;
}


AosQueryTermObjPtr
AosTermComment::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermComment(def, rdata);
}


