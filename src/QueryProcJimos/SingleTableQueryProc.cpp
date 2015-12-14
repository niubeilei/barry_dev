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
// 2015/01/19 Copied from AosQueryReqNew by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/SingleTableQueryProc.h"



AosSingleTableQueryProc::AosSingleTableQueryProc(
		AosRundata *rdata, 
		const AosJqlSelectObjPtr &orig_statement, 
		const AosJqlSelectObjPtr &crt_statement)
:
mOrigStatement(orig_statement),
mCrtStatement(crt_statement)
{
}


AosSingleTableQueryProc::~AosSingleTableQueryProc()
{
}


bool
AosSingleTableQueryProc::parseConds(
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
AosSingleTableQueryProc::config()
{
	mIsGood = false;

	mGetTotal = mCrtStatement->needTotal();
	mStartIdx = mCrtStatement->getStartIdx();
	mPageSize = mCrtStatement->getPageSize();

	AosXmlTagPtr conds = def->getFirstChild("conds");
	bool rslt = parseConds(rdata, conds);
	aos_assert_r(rslt, false);

	if (!mOrTerm) return false;
	mIsGood = true;
	return true;
}


bool
AosSingleTableQueryProc::runQuery(const AosRundataPtr &rdata)
{
	aos_assert_rr(mOrTerm, rdata, false);
	mOrTerm->setPagesize(mPsize);
	mOrTerm->setStartIdx(mStartIdx);

	mNumValues = 0;
	u64 start_time = OmnGetSecond();
	bool rslt = analyzeQuery(rdata);
	aos_assert_rr(rslt, rdata, false);
	u64 end_time = OmnGetSecond();

	if (!mIsSmartQuery)
	{
		mTotal = mOrTerm->getTotal(rdata);
		OmnTagFuncInfo << ". Total is " << mTotal << endl;
	}

	OmnString str;
	mOrTerm->toString(str);
	OmnTagFuncInfo << "Query: " << str << endl;
	return true;
}


bool
AosSingleTableQueryProc::analyzeQuery(const AosRundataPtr &rdata)
{

	OmnScreen  << "======Run Query 002=========" << endl;;
	if (!AosQueryConfig::usingSmartQuery())
	{
		return normalQuery(rdata);
	}

	i64 num = 0;
	AosQueryTermObjPtr *terms = mOrTerm->getTerms(num);
	aos_assert_r(num >= 1, false);
	aos_assert_r(terms, false);

// CHENDING TEMP
//	if (num > 1)
//	{
		OmnScreen  << "========================Run Query 003====================================" << endl;
		return smartQuery(rdata);
//	}
}


bool
AosSingleTableQueryProc::willRunSmartQuery(const AosRundataPtr &rdata)
{
	aos_assert_r(mOrTerm, false);
	if (!AosQueryConfig::usingSmartQuery())
	{
		return false;
	}
	return mOrTerm->isCompoundQuery();
}


bool
AosSingleTableQueryProc::normalQuery(const AosRundataPtr &rdata) 
{
	aos_assert_r(mOrTerm, false);
	return mOrTerm->loadData(rdata);
}


bool
AosSingleTableQueryProc::smartQuery(const AosRundataPtr &rdata)
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
AosSingleTableQueryProc::generateRslt(
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
AosSingleTableQueryProc::getRsltInfor(
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
AosSingleTableQueryProc::toString() const
{
	return "SingleTableQueryProc: not implemented yet!";
}


bool 
AosSingleTableQueryProc::getTimeCond(
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
AosSingleTableQueryProc::queryFinished(
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
AosSingleTableQueryProc::queryFailed(
		const AosRundataPtr &rdata,
		const OmnString &errmsg)
{
	mLock1->lock();
	mOrigQueryRslt = 0;
	mCondVar->signal();
	mLock1->unlock();
	return true;
}

