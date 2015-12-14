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
#include "Query/TermByDocid.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DeviceId.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosTermByDocid::AosTermByDocid(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_BYDOCID, AosQueryType::eByDocid, regflag),
mDocidRetrieved(false),
mDocid(0)
{
}


AosTermByDocid::AosTermByDocid(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_BYDOCID, AosQueryType::eByDocid, false),
mDocidRetrieved(false),
mDocid(0)
{
	// 'def' should be:
	// 	<term type=AOSTERMTYPE_BYDOCID docid="xxx"/>
	// Chen Ding, 08/23/2011
	// Backward Compatibility
	if (!def->getFirstChild(AOSTAG_SELECTOR))
	{
		OmnString dd = def->xpathQuery("rhs/_#text");
		mDocid = atoll(dd.data());
	}
	else
	{
		mDocid = def->getAttrU64("docid", 0);
	}
	mIsGood = true;
}


AosTermByDocid::AosTermByDocid(const u64 &docid, const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_BYDOCID, AosQueryType::eByDocid, false),
mDocidRetrieved(false),
mDocid(docid)
{
}


void
AosTermByDocid::toString(OmnString &str)
{
	// 	<arith iilname="xxx" reverse="xxx" opr="xxx" value="xxx"/>
	OmnString od = (mReverse)?"true":"false";
	str << "<term type=\"bydocid\" docid=\"" << mDocid << "\"/>"; 
}


bool 	
AosTermByDocid::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	if (mDocidRetrieved)
	{
		finished = true;
		mNoMoreDocs = true;
		return true;
	}
	mDocidRetrieved = true;

	docid = mDocid;
	return true;
}


bool
AosTermByDocid::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	return (docid == mDocid);
}


bool 	
AosTermByDocid::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	if (startidx == 0) 
	{
		mDocidRetrieved = false;
		return true;
	}
	mDocidRetrieved = true;
	return true;
}


void	
AosTermByDocid::reset(const AosRundataPtr &rdata)
{
	mDocidRetrieved = false;
	return;
}


i64		
AosTermByDocid::getTotal(const AosRundataPtr &rdata)
{
	if (mDocid) return 1;
	return 0;
}


bool
AosTermByDocid::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_rslt || query_bitmap ,false);
	if(query_rslt)
	{
		aos_assert_r(query_rslt->isEmpty(),false);
	}
	
	//1. if query_rslt
	//   if bitmap exist, use bitmap to check mDocid, then add mDocid to rslt.
	if(query_rslt)
	{
		if(query_bitmap && query_bitmap->checkDoc(mDocid))
		{
			return true;
		}
		query_rslt->appendDocid(mDocid);
		return true;
	}
	
	// query_rslt is null
	// 1. query_bitmap is not empty
	if(query_bitmap->isEmpty())
	{
		query_bitmap->appendDocid(mDocid);
		return true;
	}
	// 2. query_bitmap is not empty
	if(query_bitmap->checkDoc(mDocid))
	{
		// 2. query_bitmap is not empty
		// mDocid is in the bitmap 
		return true;
	}
	else
	{
		query_bitmap->clear();
		return true;
	}

	OmnShouldNeverComeHere;
	return false;	
}

bool 	
AosTermByDocid::getRsltSizeFromIIL(const AosRundataPtr &rdata)
{
	setTotalInRslt(1);
	return true;
}


AosQueryTermObjPtr
AosTermByDocid::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermByDocid(def, rdata);
}


bool	
AosTermByDocid::collectInfo(const AosRundataPtr &rdata)
{
	mCondInfo.mMin = mDocid;
	mCondInfo.mMax = mDocid;
	mCondInfo.mDeviceId = AOSDEVICEID_LOCAL;
	return true;
}



