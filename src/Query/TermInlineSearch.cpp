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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermInlineSearch.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DbQuery/Query.h"
#include "SEInterfaces/DocClientObj.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "QueryCond/CondNames.h"
#include "QueryUtil/QrUtil.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosTermInlineSearch::AosTermInlineSearch(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_INLINESEARCH, AosQueryType::eInlineSearch, regflag)
{
}


AosTermInlineSearch::AosTermInlineSearch(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_INLINESEARCH, AosQueryType::eInlineSearch, false)
{
	AosXmlTagPtr selector_tag = def->getFirstChild(AOSTAG_SELECTOR);
	mReverse = def->getAttrBool(AOSTAG_REVERSE);

	if (!selector_tag)
	{
		mIsGood = false;
		return;
	}

	mCtnrObjid = selector_tag->getAttrStr(AOSTAG_CONTAINER_NOZK);
	OmnString aname = selector_tag->getAttrStr(AOSTAG_ANAME);
	mSeqId = selector_tag->getAttrStr("seqid");
	mIILName = AosIILName::composeInlineSearchContainerAttrIILName(mCtnrObjid, aname);
	
	mOpr = eAosOpr_eq;
	bool exist;
	mValue = def->xpathQuery("cond/_$text", exist, "");
	mIsGood = true;
}


void
AosTermInlineSearch::toString(OmnString &str)
{
	// 	<arith iilname="xxx" reverse="xxx" opr="xxx" value="xxx"/>
	str << "<arith iilname=\"" << mIILName
		<< "\" reverse=\"" << mReverse
		<< "\" order=\"" << mOrder
		<< "\" odrattr=\"" << mCtnrObjid
		<< "\" opr=\"" << AosOpr_toStr(mOpr)
		<< "\" value=\"" << mValue << "\"/>";
}


AosQueryTermObjPtr
AosTermInlineSearch::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermInlineSearch(def, rdata);
}


AosXmlTagPtr 
AosTermInlineSearch::getDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	bool is_unique = true;
	OmnString seqid;
	seqid << (docid & 0xffffffff); 
	u64 id = AosQuery::getSelf()->getMember(mCtnrObjid, mSeqId, seqid, is_unique, rdata);
	if(!id) return 0;
	return AosDocClientObj::getDocClient()->getDocByDocid(id, rdata);
}

