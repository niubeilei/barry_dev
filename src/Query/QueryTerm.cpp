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
#include "Query/QueryTerm.h"

#include "Query/TermArith.h"
#include "Query/TermContainer.h"
#include "Query/TermDeletedDocs.h"
#include "Query/TermKeywords.h"
#include "Query/TermVersionDocs.h"
#include "QueryCond/QueryCond.h"
#include "SEInterfaces/BitmapObj.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/DocClientObj.h"


static OmnMutex 			sgLock;
extern OmnSPtr<AosQueryTerm> sgTerms[AosQueryType::eMax];

AosQueryTerm::AosQueryTerm(
		const OmnString &name, 
		const AosQueryType::E type, 
		const bool regflag)
:
mTermType(type),
mIsGood(false),
mReverse(false),
mOrder(false),
mNoMoreDocs(false),
mWithValues(false),
mTotal(0),
mTotalInRslt(0),
mIILId(0),
mCrtIdx(0)
{
	if (regflag)
	{
		OmnString errmsg;
		OmnSPtr<AosQueryTerm> thisptr(this, false);
		if (!registerTerm(thisptr, name, errmsg))
		{
			OmnExcept e(__FILE__, __LINE__, errmsg);
			throw e;
		}
	}
}


AosQueryTerm::~AosQueryTerm()
{
}


AosXmlTagPtr
AosQueryTerm::getDoc(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
}

AosXmlTagPtr
AosQueryTerm::getDocStatic(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
}

AosXmlTagPtr
AosQueryTerm::getDoc(
		const u64 &docid,
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	return AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
}


bool
AosQueryTerm::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosOpr opr, 
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	/*
	aos_assert_rr(mIILName != "", rdata, false);
	return AosIILClient::getSelf()->querySafe(
				 mIILName, query_rslt, query_bitmap, mOpr, mValue, rdata);
	*/
	OmnNotImplementedYet;
	return false;
}


AosQueryTermObjPtr
AosQueryTerm::getTerm(
		const AosQueryType::E type,
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(term, NULL);
	if (!AosQueryType::isValid(type))
	{
		rdata->setError() << "Invalid term: " << type;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	// Chen Ding, 08/23/2011
	// Backward compatibility
	// If it is arith type, its lhs is AOSTAG_DOCID and
	// opr == 'eq', it is a TermDocid.
	if (!term->getFirstChild(AOSTAG_SELECTOR))
	{
		OmnString lhs = term->xpathQuery("lhs/_#text");
		OmnString opr = term->getAttrStr("type");
		if (lhs == AOSTAG_DOCID && opr == "eq")
		{
			return sgTerms[AosQueryType::eByDocid]->clone(term, rdata);
		}
	}

	return sgTerms[type]->clone(term, rdata);	
}


bool
AosQueryTerm::registerTerm(
		const OmnSPtr<AosQueryTerm> &term, 
		const OmnString &name, 
		OmnString &errmsg)
{
	sgLock.lock();
	if (!AosQueryType::isValid(term->getType()))
	{
		sgLock.unlock();
		errmsg = "Incorrect reqid: ";
		errmsg << term->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgTerms[term->getType()])
	{
		sgLock.unlock();
		errmsg = "SeProc already registered: ";
		errmsg << term->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgTerms[term->getType()] = term;
	bool rslt = AosQueryType::addName(name, term->getType(), errmsg);
	sgLock.unlock();
	return rslt;
}


// Chen Ding, 09/24/2011
bool 
AosQueryTerm::retrieveContainersStatic(
		const AosXmlTagPtr &query_def,
		AosHtmlCode &code,
		const AosRundataPtr &rdata)
{
	// This function retrieves the containers used by all the conditions. 
	// The query is passed in through 'query_def', which is in the form:
	// 	<query ...>
	// 		<conds>
	// 			<cond type="AND|OR" ...>
	// 				<term .../>
	// 				<term .../>
	// 				...
	// 			</cond>
	// 		</conds>
	// 	</query>
	aos_assert_rr(query_def, rdata, false);

	AosXmlTagPtr conds_tag = query_def->getFirstChild();
	if (!conds_tag)
	{
		// This is normally an error.
		AosSetError(rdata, "queryterm_missing_conds_tag") << query_def->toString() << enderr;
		return true;
	}

	AosXmlTagPtr cond_tag = conds_tag->getFirstChild();
	i64 guard = eMaxConds;
	while (cond_tag && guard--)
	{
		AosQueryType::E type = AosQueryType::toEnum(cond_tag->getAttrStr(AOSTAG_TYPE));
		if (!AosQueryType::isValid(type))
		{
			AosSetError(rdata, AosErrmsgId::eUnrecognizedTermType);
			OmnAlarm << rdata->getErrmsg() << ". Term: " 
				<< cond_tag->toString() << enderr;
		}
		else
		{
			AosQueryTermObjPtr term = sgTerms[type];
			term->retrieveContainers(cond_tag, code, rdata);
		}
		cond_tag = conds_tag->getNextChild();
	}
	return true;
}

AosQueryRsltObjPtr 
AosQueryTerm::nextQueryRslt(i64 &size, bool &islast, const AosRundataPtr &rdata){ 
	islast = false;
	AosQueryRsltObjPtr queryrslt = getQueryData();
	aos_assert_r(queryrslt, NULL);
	if((queryrslt->getTotal(rdata)-mCrtIdx) <= size)
	{
		size = queryrslt->getTotal(rdata)-mCrtIdx;
		islast = true;	
	}
	u64 * docids = queryrslt->getDocidsPtr();
	AosQueryRsltObjPtr obj = AosQueryRsltObj::getQueryRsltStatic();
	obj->appendBlock(docids+mCrtIdx,size);
	mCrtIdx += size;
	return obj;
}


bool	
AosQueryTerm::queryFinished()
{
	return true;
}


bool 	
AosQueryTerm::nextDocid(
			const AosQueryTermObjPtr &and_term,
			u64 &docid, 
			OmnString &value, 
			bool &finished, 
			const AosRundataPtr &rdata)
{
	return nextDocid(and_term, docid, finished, rdata);

}


bool
AosQueryTerm::isCompoundQuery() const
{
	return false;
}


OmnString 
AosQueryTerm::getTermName() const
{
	return AosQueryType::toStr(getType());
}


