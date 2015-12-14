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
// This query assumes the user sends a doc of the following format:
// 	<docs idtype="xxx" ...>
// 		<doc .../>
// 		<doc .../>
// 		...
// 	</docs>
//
// The query will generate the results:
// 	<Contents>
// 		<record .../>
// 		<record .../>
// 		...
// 		<record .../>
// 	</Contents>
//
// where each <record> is a doc retrieved based on a docid that is determined
// by the IDType. If 'idtype' is 'objid', the docid's objid is stored in the
// attribute AOSTAG_OBJID in <doc>; if 'idtype' is 'docid', the docid is stored
// in the attribute AOSTAG_DOCID in <doc>; if 'idtype' is 'cloudid', the
// docid is stored in the attribute AOSTAG_CLOUDID. 
//
// Modification History:
// 11/09/2010 Created by James Kong
////////////////////////////////////////////////////////////////////////////
#include "Query/TermUserCustom.h"

#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "QueryCond/QueryCond.h"
#include "Rundata/Rundata.h"
#include "SEModules/LogMgr.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"


AosTermUserCustom::AosTermUserCustom(const bool flag)
:
AosQueryTerm(AOSTERMTYPE_USERCUSTOM, AosQueryType::eUserCustom, flag),
mDataLoaded(false)
{
}


AosTermUserCustom::AosTermUserCustom(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_USERCUSTOM, AosQueryType::eUserCustom, false),
mDataLoaded(false)
{
	// The query format is:
	// 	<term type="xxx" reverse="true|false" order="true|false" idtype="xxx">
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 	</term>
	mIsGood = false;
	aos_assert(def);
	mQuery = def;
	mReverse = def->getAttrBool("reverse", false);
	mOrder = def->getAttrBool("order", false);
	mIdType = mQuery->getAttrStr("idtype", "");
	mRecord = mQuery->getFirstChild();
	mIsGood = true;
}


void
AosTermUserCustom::reset(const AosRundataPtr &rdata)
{
    mNoMoreDocs = false;
}

bool
AosTermUserCustom::collectInfo(const AosRundataPtr &rdata)
{
    OmnNotImplementedYet;
    return false;
}

AosQueryTermObjPtr
AosTermUserCustom::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
    return OmnNew AosTermUserCustom(def, rdata);
}


AosTermUserCustom::AosTermUserCustom(
		const AosXmlTagPtr &query,
		const bool reverse, 
		const OmnString &cid,
		const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_USERCUSTOM, AosQueryType::eUserCustom, false),
mDataLoaded(false)
{
	// 'query' should be an XML of the following format:
	// 	<docs>
	// 		<doc .../>
	// 		<doc .../>
	// 		...
	// 	</docs>
	//
	mQuery = query;	
	if (!mQuery)
	{
		AosSetError(rdata, AosErrmsgId::eQueryIncorrect);
		OmnAlarm << rdata->getErrmsg() << enderr;
		mIsGood = false;
		return;
	}

	mIsGood = true;
	mReverse = reverse;
	mIdType = mQuery->getAttrStr("idtype", "");
	mRecord = mQuery->getFirstChild();
}


AosTermUserCustom::~AosTermUserCustom()
{
}


void
AosTermUserCustom::toString(OmnString &str)
{
	aos_assert(mQuery);
	OmnString reverse = (mReverse)?"true":"false";
	OmnString order = (mOrder)?"true":"false";
	str << "<term type=\"usercustom\""
	<< "\" reverse=\"" << reverse
	<< "\" order=\"" << order 
	<< "\">"
	<< mQuery
	<< "</term>";
}


bool 	
AosTermUserCustom::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid,
		bool &finished,
		const AosRundataPtr &rdata)
{
	// This function retrieves the next docid based on 'mIdType'.
	// 1. If mIdType == "objid", the next docid is the doc's docid. 
	if (mNoMoreDocs) 
	{
		docid = 0;
		finished = true;
		return true;
	}

	if (!mRecord)
	{
		docid = 0;
		finished = true;
		mNoMoreDocs = true;
		return true;
	}

	if (mIdType == "objid")
	{
		OmnString objid = mRecord->getAttrStr(AOSTAG_OBJID, "");
		if (objid == "")
		{
			docid = 0;
			finished = true;
			mNoMoreDocs = true;
			AosSetError(rdata, AosErrmsgId::eMissingObjid);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		
		bool rslt;
		bool isunique;
		//rslt = AosIILClient::getSelf()->getDocidByObjid(rdata->getSiteid(), 
		rslt = AosIILClientObj::getIILClient()->getDocidByObjid(rdata->getSiteid(), 
				objid, docid, isunique, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AosErrmsgId::eFailedRetrieveDocid);
			OmnAlarm << rdata->getErrmsg() << ". Objid: " << objid 
				<< ". Siteid: " << rdata->getSiteid() << enderr;
			finished = true;
			docid = 0;
			mNoMoreDocs = true;
			return false;
		}
	}

	if (mIdType == "docid")
	{
		docid = mRecord->getAttrU64(AOSTAG_DOCID, 0);
		if (docid == 0)
		{
			finished = true;
			mNoMoreDocs = true;
			AosSetError(rdata, AosErrmsgId::eMissingDocid);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	if (mIdType == "cloudid")
	{
		OmnString cloudid = mRecord->getAttrStr(AOSTAG_CLOUDID, "");
		if (cloudid == "")
		{
			docid = 0;
			mNoMoreDocs = true;
			finished = true;
			AosSetError(rdata, AosErrmsgId::eMissingCloudid);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		
		AosXmlTagPtr  doc = AosDocClientObj::getDocClient()->getDocByCloudid(cloudid, rdata);
		if (!doc)
		{
			docid = 0;
			mNoMoreDocs = true;
			finished = true;
			AosSetError(rdata, AosErrmsgId::eFailedRetrieveDocByCloudid);
			OmnAlarm << rdata->getErrmsg() << ". Cloudid: " << cloudid << enderr;
			return false;
		}
		docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	}

	mRecord = mQuery->getNextChild();
	return true;
}


i64		
AosTermUserCustom::getTotal(const AosRundataPtr &rdata)
{
	aos_assert_rr(mQuery, rdata, 0);
	return mQuery->getNumSubtags();
}


bool	
AosTermUserCustom::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	aos_assert_rr(mQueryRslt, rdata, false);
	return mQueryRslt->checkDocid(docid, rdata);
}


bool 	
AosTermUserCustom::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	// This query assumes an XML doc of the following format:
	// 	<docs>
	// 		<doc .../>
	// 		<doc .../>
	// 		...
	// 		<doc .../>
	// 	</docs>
	if (!mQuery)
	{
		AosSetError(rdata, AosErrmsgId::eMissingQuery);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!mDataLoaded) loadData(rdata);
	aos_assert_rr(mQueryRslt, rdata, false);
	mQueryRslt->reset();
	bool finished;
	return mQueryRslt->moveTo(startidx, finished, rdata);
}


bool
AosTermUserCustom::setOrder(
		const OmnString &container, 
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &)
{
	return false;
}


bool
AosTermUserCustom::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;
	// Chen Ding, 11/28/2012
	if (!mQueryRslt) mQueryRslt = AosQueryRsltObj::getQueryRsltStatic();
	if (!mQueryContext) mQueryContext = AosQueryContextObj::createQueryContextStatic();
	mQueryContext->setReverse(mReverse);
	return getDocidsFromIIL(mQueryRslt, 0, mQueryContext, rdata);
}

bool 	
AosTermUserCustom::getRsltSizeFromIIL(const AosRundataPtr &rdata)
{
	setTotalInRslt(1);
	return true;
}

bool
AosTermUserCustom::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	bool finished = false;
	aos_assert_rr(mQuery, rdata, false);

	mRecord = mQuery->getFirstChild();
	mNoMoreDocs = false;
	u64 docid;
	while (mRecord)
	{
		if (!nextDocid(0, docid, finished, rdata))
		{
			return false;
		}

		if (query_rslt)
		{
			if (!query_bitmap)
			{
				query_rslt->appendDocid(docid);
			}
		}
		else
		{
			if (!query_bitmap)
			{
				AosSetError(rdata, AosErrmsgId::eMissingQueryRslt);
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		}
	}

	return true;
}


bool
AosTermUserCustom::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	// AOSQUERY_USERCUSTOM|$|
	// bool rev = (nn >= 4 && fields[3] == "true")?true:false;
	// bool filter = !(nn >= 5 && fields[4] == "false");
	// mSpecialTerm = OmnNew AosTermUserCustom(mSiteid, mCmd, rev, filter, cid);
	// return true;
	OmnAlarm << "UserCustom Log needs to rework: " << cond_str << enderr;
	return false;
}

