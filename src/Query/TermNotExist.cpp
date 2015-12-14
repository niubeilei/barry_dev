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
// This term is used to assert docs not exist. As an example, if we want to
// pick docs that do not have the attribute 'lastname', we may use this 
// term. Note that "Not Exist" is different from "Not Equal". "Not Equal"
// is a super set of "Not Exist". 
//
// The processing is different from the normal terms. If a container is 
// specified, it is an array of all the docs in that container and they do
// not have the named attribute. This is done by the container member listing, 
// and then removing all the docs that are in the corresponding attribute 
// IIL.
//
// If no container is specified, it retrieves all the docs and then removes
// all the docs that are in the corresponding attribute IIL.
//
// Modification History:
// 09/16/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermNotExist.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"

const i64 AosTermNotExist::mMaxDocs = 0xffffffff;

AosTermNotExist::AosTermNotExist(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_NOTEXIST, AosQueryType::eNotExist, regflag)
{
}


AosTermNotExist::AosTermNotExist(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_NOTEXIST, AosQueryType::eNotExist, false)
{
	/*
	// 'def' is in the form:
	// 	<term type="nx">
	// 		<lhs><![CDATA[xxx]]></lhs>
	//	</term>

	mOpr = AosOpr_toEnum(def->getAttrStr("type"));
	bool exist;
	mFieldname = def->xpathQuery("lhs/_#text", exist, "");
	mIsGood = parse(mFieldname, rdata);
	aos_assert(mIsGood);
	*/
	OmnShouldNeverComeHere;
}


AosTermNotExist::AosTermNotExist(
		const OmnString &lhs, 
		const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_NOTEXIST, AosQueryType::eNotExist, false)
{
	// mIsGood = parse(lhs, rdata);
	// aos_assert(mIsGood);
	OmnShouldNeverComeHere;
}


void
AosTermNotExist::toString(OmnString &str)
{
	// 	<arith iilname="xxx" reverse="xxx" opr="xxx" value="xxx"/>
	// OmnString od = (mReverse)?"true":"false";
	// str << "<arith iilname=\"" << mIILName
	// 	<< "\" reverse=\"" << od
	// 	<< "\" order=\"" << mOrder
	// 	<< "\" opr=\"" << AosOpr_toStr(mOpr)
	// 	<< "\"/>";
	OmnShouldNeverComeHere;
}


bool
AosTermNotExist::parse(
		const OmnString &lhs,
		const AosRundataPtr &rdata)
{
	/*
	if (lhs == "")
	{
		rdata->setError() << "Attribute name is empty!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (lhs == AOSTAG_DOCID)
	{
		// It is to retrieve a doc by docid. This is processed specially.
		mIsByDocid = true;
	}
	else
	{
		mIILName = AosIILName::composeAttrIILName(lhs);
	}
	return true;
	*/
	OmnShouldNeverComeHere;
	return false;
}


bool
AosTermNotExist::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// It checks whether the doci 'docid' can be selected
	// by this term.
	// return AosIILClient::getSelf()->docExistSafe(mIILName,docid,rdata);
	OmnShouldNeverComeHere;
	return true;
}


bool 	
AosTermNotExist::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return true;
}


void	
AosTermNotExist::reset(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
}


i64		
AosTermNotExist::getTotal(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	// return mMaxDocs;
	return -1;
}


bool
AosTermNotExist::setOrder(
		const OmnString &container, 
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &)
{
	OmnShouldNeverComeHere;
	return true;
}


bool 
AosTermNotExist::convertQuery(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermNotExist::nextDocid(
		const AosQueryTermObjPtr &and_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosQueryTermObjPtr
AosTermNotExist::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermNotExist(def, rdata);
}


bool
AosTermNotExist::collectInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 	
AosTermNotExist::getRsltSizeFromIIL(const AosRundataPtr &rdata)
{
	setTotalInRslt(1);
	return true;
}

