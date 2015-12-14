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
// 2015/01/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/QueryProcSmartQuery.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"

OmnMutexPtr	  	AosQueryProcSmartQuery::smIDLock = OmnNew OmnMutex();
i64			  	AosQueryProcSmartQuery::smIDCur = 0;


AosQueryProcSmartQuery::AosQueryProcSmartQuery()
:
{
}


AosQueryProcSmartQuery::~AosQueryProcSmartQuery()
{
}


bool
AosQueryProcSmartQuery::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	mIsGood = false;
	aos_assert_r(def, false);

	mOrigReq = def->clone(AosMemoryCheckerArgsBegin);
	mQueryId = def->getAttrInt64(AOSTAG_QUERYID, 0);
	if (!mQueryId) mQueryId = getNewQueryID();

	mGetTotal = def->getAttrBool("get_total");
	setStartIdx(def->getAttrInt64("start_idx", 0));
	setPsize(def->getAttrInt64("psize", eDftPsize));

	mOrderContainer = def->getAttrStr("orderctnr");
	mOrderFname = def->getAttrStr("order");
	mReverseOrder = def->getAttrBool("reverse", false);

	//mIsNeedSmartQuery  = def->getAttrBool("isNeedSmartQuery", false);

	AosXmlTagPtr conds = def->getFirstChild("conds");
	bool rslt = parseConds(rdata, conds);
	aos_assert_r(rslt, false);

	if (!mOrTerm)
	{
		AosSetErrorU(rdata, "Missing conditions! :") << def->toString() << enderr;
		return false;
	}

	mIsGood = true;
	return true;
}


bool
AosQueryProcSmartQuery::runQuery(
		AosRundata *rdata, 
		const AosJqlStatement &orig_statement, 
		const AosJqlStatement &crt_statement, 
		const AosQueryCallerPtr &caller)
{
	// This class assume 'crt_statement':
	// 1. Has no joins
	// 2. Has no sub-queries
	// 3. All conditioned fields are indexed
	aos_assert_rr(!crt_statement->hasJoins(), rdata, false);
	aos_assert_rr(!crt_statement->hasSubqueries(), rdata, false);
	aos_assert_rr(crt_statement->getConditionedFieldsIndexed(), rdata, false);

	mOrigStatement = orig_statement;
	mCrtStatement = crt_statement;
	mCaller = caller;

	bool rslt = parseConditions(rdata);
	if (!rslt) return false;
}


bool
AosQueryProcSmartQuery::parseConds(AosRundata &rdata)
{
}


bool
AosQueryProcSmartQuery::parseConds(
		const AosRundataPtr rdata,
		const AosXmlTagPtr &conds)
{
	aos_assert_r(conds, false);

	i64 guard = 20;
	AosQueryTermObjPtr cond = OmnNew AosTermAnd(false);
	AosXmlTagPtr andterm = conds->getFirstChild("cond");
	while (guard-- && andterm)
	{
		OmnString type = andterm->getAttrStr("type");
		if (type != "AND")
		{
			rdata->setError() << "Expecting the AND term but failed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		AosXmlTagPtr term = andterm->getFirstChild("term");
		while (term)
		{
			cond->addTerm(term, rdata);
			term = andterm->getNextChild("term");
		}

		andterm = conds->getNextChild("cond");
	}

	aos_assert_r(cond->getNumTerms() > 0, false);

	if (!mOrTerm) mOrTerm = OmnNew AosTermOr();
	mOrTerm->addTerm(cond, rdata);

	if (mOrderFname != "") 
	{
		mOrTerm->setOrder(mOrderContainer, mOrderFname, mReverseOrder, rdata);
	}

	return true;
}


bool
AosQueryProcSmartQuery::runQuery(const AosRundataPtr &rdata)
{
	rdata->setOk();
	mLock->lock();

	aos_assert_rl(mOrTerm, mLock, false);
	mOrTerm->setPagesize(mPsize);
	mOrTerm->setStartIdx(mStartIdx);

	mNumValues = 0;
	bool rslt = analyzeQuery(rdata);
	aos_assert_rl(rslt, mLock, false);

	if (!mIsSmartQuery)
	{
		mTotal = mOrTerm->getTotal(rdata);
		OmnTagFuncInfo << ". Total is " << mTotal << endl;
	}

	OmnString str;
	mOrTerm->toString(str);
	OmnTagFuncInfo << "Query: " << str << endl;

	mLock->unlock();
	return true;
}



bool
AosQueryProcSmartQuery::smartQuery(const AosRundataPtr &rdata)
{
	bool rslt = false;
	mLock1->lock();
	mIsSmartQuery = true;
	AosQueryReqObjPtr thisptr(this, false);
	AosQueryProcCallbackPtr thisptr2(this, false);
	rslt = AosQueryEngineObj::getQueryEngine()->runQueryStatic(rdata, thisptr, thisptr2);
	aos_assert_rl(rslt, mLock1, false);
	mCondVar->wait(mLock1);
	mLock1->unlock();
	return rslt;
}


bool
AosQueryProcSmartQuery::generateRslt(
		AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata) 
{
	query_rslt = 0;

	if (mIsSmartQuery)
	{
		query_rslt = mOrigQueryRslt;
		if (query_rslt)
		{
			mNumValues = query_rslt->getNumDocs();
			mCrtIdx += mNumValues;
		}
		return true;
	}
	
	if (mStartIdx != mCrtIdx) 
	{
	 	if (mStartIdx < 0)
		{
			mStartIdx = (mOrderFname != "" && mReverseOrder) ? mTotal - 1 : 0;
		}
		if (mStartIdx < 0)
		{
			mStartIdx = 0;
		}

	 	mCrtIdx = mStartIdx;
	}
	else
	{
		if (mStartIdx < 0)
		{
			mStartIdx = 0;
			mCrtIdx = 0;
		}
	}

	if (mStartIdx < 0) mStartIdx = 0;

	mOrTerm->moveTo(mStartIdx, rdata);

	AosQueryRsltObjPtr query_data = mOrTerm->getQueryData();
	aos_assert_r(query_data, false);

	query_rslt = AosQueryRsltObj::getQueryRsltStatic();

	u64 docid;
	bool rslt;
	bool finish = false;
	int num = 0;
	while (!finish && num < mPsize)
	{
		rslt = query_data->nextDocid(docid, finish, rdata);
		aos_assert_r(rslt, false);

		if (finish) break;

		rslt = query_rslt->appendDocid(docid);
		aos_assert_r(rslt, false);

		num++;
	}
		
	mNumValues = query_rslt->getNumDocs();
	mCrtIdx += mNumValues;

	return true;
}


bool
AosQueryProcSmartQuery::getRsltInfor(
		AosXmlTagPtr &infor,
		const AosRundataPtr &rdata)
{
	if (mCrtIdx < 0 && mNumValues > 0) mCrtIdx = 0;

	OmnString str = "<Contents";
	str << " total=\"" << mTotal << "\""
		<< " start_idx=\"" << mStartIdx << "\""
		<< " crt_idx=\"" << mCrtIdx << "\""
		<< " queryid=\"" << mQueryId << "\""
		<< " num=\"" << mNumValues << "\""
		<< "/>";

	infor = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(infor, false);

	return true;
}


OmnString 
AosQueryProcSmartQuery::toString() const
{
	return "QueryProcSmartQuery: not implemented yet!";
}


bool 
AosQueryProcSmartQuery::getTimeCond(
		const AosRundataPtr &rdata, 
		const OmnString &time_fieldname, 
		int &num_matched, 
		int *start_days, 
		int *end_days)
{
	aos_assert_rr(mOrTerm, rdata, false);
	return mOrTerm->getTimeCond(rdata, time_fieldname, num_matched, start_days, end_days);
}


bool 
AosQueryProcSmartQuery::queryFinished(
		const AosRundataPtr &rdata,
		const AosQueryRsltObjPtr &results,
		const AosQueryProcObjPtr &proc)
{
	mLock1->lock();
	mOrigQueryRslt = results;
	mTotal = proc->getTotalInRslt();
	mCondVar->signal();
	mLock1->unlock();
	return true;
}


bool 
AosQueryProcSmartQuery::queryFailed(
		const AosRundataPtr &rdata,
		const OmnString &errmsg)
{
	mLock1->lock();
	mOrigQueryRslt = 0;
	mCondVar->signal();
	mLock1->unlock();
	return true;
}

