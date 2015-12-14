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
#include "Query/QueryReqNew.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "DataRecord/RecordXml.h"
#include "Porting/TimeOfDay.h"
#include "Query/Ptrs.h"
#include "QueryClient/QueryClient.h"
#include "Query/TermOr.h"
#include "Query/TermAnd.h"
#include "Query/TermLog.h"
#include "Query/TermKeywords.h"
#include "Query/TermAllowance.h"
#include "Query/TermContainer.h"
#include "Query/TermWhoReadme.h"
#include "Query/TermUserCustom.h"
#include "Query/TermFriend.h"
#include "Query/TermTag.h"
#include "Query/TermArith.h"
#include "Query/TermCounterSingle.h"
#include "Query/TermComment.h"
#include "Query/TermMonitorLog.h"
#include "QueryUtil/QrUtil.h"
#include "QueryUtil/QueryConfig.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryEngineObj.h"
#include "SEInterfaces/QueryCacherObj.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Thread/Mutex.h"
#include "Util/Opr.h"
#include "Util/StrSplit.h"
#include "Util/StrParser.h"
#include "Util/OmnNew.h"
#include "Query/Update.h"
#include "WordParser/WordNorm.h"
#include "VersionServer/VersionServer.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/XmlRc.h"
#include "Debug/Debug.h"


OmnMutexPtr	  	AosQueryReqNew::smIDLock = OmnNew OmnMutex();
i64			  	AosQueryReqNew::smIDCur = 0;


AosQueryReqNew::AosQueryReqNew()
:
mLock(OmnNew OmnMutex()),
mLock1(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mIsGood(false),
mQueryId(0),
mGetTotal(true),
mTotal(0),
mNumValues(0),
mStartIdx(0),
mPsize(eDftPsize),
mReverseOrder(false),
mCrtIdx(-1),
mIsSmartQuery(false)
{
	OmnScreen << "create query req:[" << this << "]" << endl; 
}


AosQueryReqNew::~AosQueryReqNew()
{
	OmnScreen << "delete query req:[" << this << "]" << endl; 
}


AosQueryReqObjPtr
AosQueryReqNew::createQuery(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	try
	{
		AosQueryReqNew * query = OmnNew AosQueryReqNew();
		bool rslt = query->config(def, rdata);
		aos_assert_r(rslt, 0);
		return query;
	}
	catch(...)
	{
		OmnAlarm << "failed to create query" << enderr;
		return 0;
	}
	return 0;
}


i64 
AosQueryReqNew::getNewQueryID()
{
	smIDLock->lock();
	i64 new_id = ++smIDCur;
	smIDLock->unlock();
	return new_id;
}


bool
AosQueryReqNew::config(
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
AosQueryReqNew::parseConds(
		const AosRundataPtr rdata,
		const AosXmlTagPtr &conds)
{
	aos_assert_r(conds, false);

	i64 guard = 20;
	if (!mOrTerm) mOrTerm = OmnNew AosTermOr();

	AosQueryTermObjPtr and_term = OmnNew AosTermAnd(false);
	AosXmlTagPtr and_term_xml = conds->getFirstChild("cond");
	while (guard-- && and_term_xml)
	{
		OmnString type = and_term_xml->getAttrStr("type");
		if (type != "AND")
		{
			rdata->setError() << "Expecting the AND term but failed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		AosXmlTagPtr term = and_term_xml->getFirstChild("term");
		while (term)
		{
			and_term->addTerm(term, rdata);
			term = and_term_xml->getNextChild("term");
		}

		and_term_xml = conds->getNextChild("cond");
		mOrTerm->addTerm(and_term, rdata);
	}

	aos_assert_r(and_term->getNumTerms() > 0, false);

	if (mOrderFname != "") 
	{
		mOrTerm->setOrder(mOrderContainer, mOrderFname, mReverseOrder, rdata);
	}

	return true;
}


bool
AosQueryReqNew::runQuery(const AosRundataPtr &rdata)
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
AosQueryReqNew::analyzeQuery(const AosRundataPtr &rdata)
{

	OmnScreen  << "========================Run Query 002====================================" << endl;;
	//if (!AosQueryConfig::usingSmartQuery() || !mIsNeedSmartQuery)
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

	AosQueryTermObjPtr andTerm = terms[0];
	aos_assert_r(andTerm, false);

	AosQueryTermObjPtr *tt = andTerm->getTerms(num);
	aos_assert_r(num >= 1, false);
	aos_assert_r(tt, false);

	if (num == 1) 
	{
		return normalQuery(rdata);
	}

	for (i64 i=0; i<num; i++)
	{
		switch (tt[i]->getType())
		{
		case AosQueryType::eByDocid:
			 OmnAlarm << "Please do not use this" << enderr;
			 return normalQuery(rdata);

		default:
			 break;
		}

		if (!tt[i]->canUseBitmapQuery())
		{
			return normalQuery(rdata);
		}
	}

	return smartQuery(rdata);
}


bool
AosQueryReqNew::willRunSmartQuery(const AosRundataPtr &rdata)
{
	aos_assert_r(mOrTerm, false);
	if (!AosQueryConfig::usingSmartQuery())
	{
		return false;
	}
	return mOrTerm->isCompoundQuery();
}


bool
AosQueryReqNew::normalQuery(const AosRundataPtr &rdata) 
{
	aos_assert_r(mOrTerm, false);
	return mOrTerm->loadData(rdata);
}


bool
AosQueryReqNew::smartQuery(const AosRundataPtr &rdata)
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
AosQueryReqNew::generateRslt(
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
AosQueryReqNew::getRsltInfor(
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
AosQueryReqNew::toString() const
{
	return "QueryReqNew: not implemented yet!";
}


bool 
AosQueryReqNew::getTimeCond(
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
AosQueryReqNew::queryFinished(
		const AosRundataPtr &rdata,
		const AosQueryRsltObjPtr &results,
		const AosBitmapObjPtr &bitmap,
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
AosQueryReqNew::queryFailed(
		const AosRundataPtr &rdata,
		const OmnString &errmsg)
{
	mLock1->lock();
	mOrigQueryRslt = 0;
	mCondVar->signal();
	mLock1->unlock();
	return true;
}

