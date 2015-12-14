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
// This is a Term Template. When creating a new term, one should copy
// this file and modify it. 
//
// Modification History:
// 2013/12/04 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Query/TermJoin2.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterClt/CounterClt.h"
#include "CounterUtil/CounterQuery.h"
#include "CounterUtil/ResolveCounterName.h"
#include "CounterUtil/CounterUtil.h"
#include "SEUtil/IILName.h"
#include "Query/TermIILType.h"
#include "Query/TermOr.h"
#include "Query/TermAnd.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/BuffArray.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/Ptrs.h" 
#include "XmlUtil/SeXmlParser.h"


AosTermJoin2::AosTermJoin2(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_JOIN2, AosQueryType::eJoin2, regflag),
mDataLoaded(false),
mPsize(eDftPsize),
mStartIdx(0),
mCrtIdx(0),
mCrtLIdx(0),
mCrtRIdx(0)
{
}


AosTermJoin2::AosTermJoin2(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_JOIN2, AosQueryType::eJoin2, false),
mDataLoaded(false),
mPsize(eDftPsize),
mStartIdx(0),
mCrtIdx(0),
mCrtLIdx(0),
mCrtRIdx(0)
{
	mIsGood = parse(def, rdata);
}


AosTermJoin2::~AosTermJoin2()
{
}


bool 	
AosTermJoin2::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	docid = 0;
	finished = false;
	if (!mDataLoaded) loadData(rdata);

	if (mCrtIdx >= mDocsNum)
	{
		finished = true;
		return true;
	}
	docid = ++mCrtIdx;
	return true;
}


bool
AosTermJoin2::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermJoin2::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	if (query_rslt) 
	{
		query_rslt->appendDocid(0);
		query_rslt->reset();
	}
	if(query_context)
	{
		query_context->setFinished(true);
	}
	return true;
}


AosXmlTagPtr
AosTermJoin2::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
{
	aos_assert_r(docid > 0, 0);

	i64 idx = docid - 1;
	aos_assert_r(idx < (i64)mDocs.size(), 0);

	OmnString str = mDocs[idx];
	AosXmlTagPtr doc = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	return doc;
}


bool 	
AosTermJoin2::moveTo(const i64 &pos, const AosRundataPtr &rdata)
{
	aos_assert_r(pos >= mStartIdx && pos < mStartIdx + mPsize, false);
	mCrtIdx = pos - mStartIdx;
	return true;
}


i64		
AosTermJoin2::getTotal(const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	return mDocsNum;
}


void	
AosTermJoin2::reset(const AosRundataPtr &rdata)
{
}

void
AosTermJoin2::toString(OmnString &str)
{
}


AosXmlTagPtr
AosTermJoin2::getNextDoc(
		const AosQueryTermObjPtr &term,
		bool &finished,
		const AosRundataPtr &rdata)
{
	aos_assert_r(term, 0);

	bool rslt = true;
	OmnString value;
	u64 docid = 0;
	if (term->withValue())
	{
		rslt = term->nextDocid(0, docid, value, finished, rdata);
	}
	else
	{
		rslt = term->nextDocid(0, docid, finished, rdata);
	}
	if (!rslt) return 0;
	if (docid == AOS_INVDID && finished) return 0;

	AosXmlTagPtr doc;
	if (term->withValue())
	{
		doc = term->getDoc(docid, value, rdata);
	}
	else
	{
		doc = term->getDoc(docid, rdata);
	}

	return doc;
}


bool
AosTermJoin2::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;
	
	bool rslt = mLTerm->loadData(rdata);
	aos_assert_r(rslt, false);

	rslt = mRTerm->loadData(rdata);
	aos_assert_r(rslt, false);

	if (!mIsSameOrder)
	{
		return loadData2(rdata);
	}

	i64 l_total = mLTerm->getTotal(rdata);
	i64 r_total = mRTerm->getTotal(rdata);

	bool swap = false;
	if (l_total > r_total) 
	{
		swap = true;
		i64 total_tmp = l_total;
		l_total = r_total;
		r_total = total_tmp;

		AosQueryTermObjPtr temp = mLTerm;
		mLTerm = mRTerm;
		mRTerm = temp;

		bool tmp_bool = mLIsStat;
		mLIsStat = mRIsStat;
		mRIsStat = tmp_bool;

		i64 tmp_idx = mLStatIdx;
		mLStatIdx = mRStatIdx;
		mRStatIdx = tmp_idx;

		OmnString tmp_key = mLStatKey;
		mLStatKey = mRStatKey;
		mRStatKey = tmp_key;

		OmnString tmp_name = mLStatValue;
		mLStatValue = mRStatValue;
		mRStatValue = tmp_name;
	}

	AosQueryRsltObjPtr l_data = mLTerm->getQueryData();
	AosQueryRsltObjPtr r_data = mRTerm->getQueryData();
	aos_assert_r(l_data && r_data, false);
	l_data->reset();
	r_data->reset();

	mDocsNum = 0;
	i64 ignore_num = mStartIdx;
	bool l_finished = false, r_finished = false;
	OmnString l_key, r_key, k;
	vector<OmnString> strs;
	u64 l_value, r_value;

	while (!l_finished)
	{
		mCrtLIdx++;
		rslt = l_data->nextDocidValue(l_value, l_key, l_finished, rdata);
		if (l_finished && mCrtLIdx < l_total)
		{
			mLTerm->setStartIdx(mCrtLIdx - 1);
			mLTerm->runQuery(rdata);
			l_data = mLTerm->getQueryData();
			aos_assert_r(l_data, false);
			l_finished = false;
			rslt = l_data->nextDocidValue(l_value, l_key, l_finished, rdata);
		}
		if (l_finished)
		{
			break;
		}
		if (!rslt || l_key == "")
		{
			OmnAlarm << "Failed to retrieve the l_key: " << enderr;
			continue;
		}
		if (mLIsStat)
		{
			strs.clear();
			AosCounterUtil::splitTerm2(AosCounterUtil::getAllTerm2(l_key), strs);
			if ((i64)strs.size() < mLStatIdx)
			{
				OmnAlarm << "error" << enderr;
				continue;
			}
			l_key = strs[mLStatIdx];
			if (l_key == "")
			{
				OmnAlarm << "Failed to retrieve the l_key: " << enderr;
				continue;
			}
		}

		if (l_key > r_key)
		{
			while (!r_finished)
			{
				mCrtRIdx++;
				rslt = r_data->nextDocidValue(r_value, r_key, r_finished, rdata);
				if (r_finished && mCrtRIdx < r_total)
				{
					mRTerm->setStartIdx(mCrtRIdx - 1);
					mRTerm->runQuery(rdata);
					r_data = mRTerm->getQueryData();
					aos_assert_r(r_data, false);
					r_finished = false;
					rslt = r_data->nextDocidValue(r_value, r_key, r_finished, rdata);
				}
				if (r_finished)
				{
					break;
				}
				if (!rslt || r_key == "")
				{
					OmnAlarm << "Failed to retrieve the r_key: " << enderr;
					continue;
				}
				if (mRIsStat)
				{
					strs.clear();
					AosCounterUtil::splitTerm2(AosCounterUtil::getAllTerm2(r_key), strs);
					if ((i64)strs.size() < mRStatIdx)
					{
						OmnAlarm << "error" << enderr;
						continue;
					}
					r_key = strs[mRStatIdx];
					if (r_key == "")
					{
						OmnAlarm << "Failed to retrieve the r_key: " << enderr;
						continue;
					}
				}

				if (l_key <= r_key)
				{
					break;
				}
			}
		}
		
		if (r_finished)
		{
			break;
		}
		
		if (l_key < r_key)
		{
			continue;	
		}
		
		if (l_key != r_key)
		{
			OmnAlarm << "error" << enderr;
			continue;	
		}

		mDocsNum++;
		if (ignore_num > 0)
		{
			ignore_num--;
			continue;
		}

		if ((i64)mDocs.size() < mPsize)
		{
			OmnString str = "<record";
			if (swap)
			{
				str << " key=\"" << l_key << "\""
					<< " l_value=\"" << r_value << "\""
					<< " r_value=\"" << l_value << "\"/>";
			}
			else
			{
				str << " key=\"" << l_key << "\""
					<< " l_value=\"" << l_value << "\""
					<< " r_value=\"" << r_value << "\"/>";
			}
			mDocs.push_back(str);
		}
		else
		{
			if (!mNeedSpecificTotal)
			{
				break;
			}
		}
	}

	if (!mNeedSpecificTotal)
	{
		if (mCrtLIdx <= 0)
		{
			mDocsNum = 0;
		}
		else
		{
			mDocsNum = (l_total + 1) * (mDocs.size() + mStartIdx) / mCrtLIdx;
		}
	}
	mDataLoaded = true;
	return true;
}


bool
AosTermJoin2::loadData2(const AosRundataPtr &rdata)
{
	i64 l_total = mLTerm->getTotal(rdata);
	i64 r_total = mRTerm->getTotal(rdata);

	AosQueryRsltObjPtr l_data;
	AosQueryRsltObjPtr r_data;
	if (!mLIsStat)
	{
		l_data = mLTerm->getQueryData();
		aos_assert_r(l_data, false);
		l_data->reset();
	}
	if (!mRIsStat)
	{
		r_data = mRTerm->getQueryData();
		aos_assert_r(r_data, false);
		r_data->reset();
	}

	mDocsNum = 0;
	i64 ignore_num = mStartIdx;
	bool rslt = true;
	bool l_finished = false, r_finished = false;
	OmnString l_key, r_key, k;
	vector<OmnString> strs;
	u64 l_value, r_value;
	AosXmlTagPtr xml;

	map<OmnString, u64> r_map;
	map<OmnString, u64>::iterator r_map_itr;

	while (!r_finished)
	{
		mCrtRIdx++;
		if (mRIsStat)
		{
			rslt = mRTerm->nextDocid(0, r_value, r_key, r_finished, rdata);
			if (rslt && !r_finished)
			{
				xml = mRTerm->getDoc(r_value, r_key, rdata);
				if (!xml)
				{
					OmnAlarm << "Failed to retrieve the xml: " << enderr;
					continue;
				}
				
				r_key = xml->getAttrStr(mRStatKey, "");
				if (r_key == "")
				{
					OmnAlarm << "Failed to retrieve the r_key: " << enderr;
					continue;
				}
				r_value = xml->getAttrU64(mRStatValue, 0);
			}
		}
		else
		{
			rslt = r_data->nextDocidValue(r_value, r_key, r_finished, rdata);
			if (r_finished && mCrtRIdx < r_total)
			{
				mRTerm->setStartIdx(mCrtRIdx - 1);
				mRTerm->runQuery(rdata);
				r_data = mRTerm->getQueryData();
				aos_assert_r(r_data, false);
				r_finished = false;
				rslt = r_data->nextDocidValue(r_value, r_key, r_finished, rdata);
			}
		}
		if (r_finished)
		{
			break;
		}
		if (!rslt || r_key == "")
		{
			OmnAlarm << "Failed to retrieve the r_key: " << enderr;
			continue;
		}
		r_map[r_key] = r_value;
	}

	while (!l_finished)
	{
		mCrtLIdx++;
		if (mLIsStat)
		{
			rslt = mLTerm->nextDocid(0, l_value, l_key, l_finished, rdata);
			if (rslt && !l_finished)
			{
				xml = mLTerm->getDoc(l_value, l_key, rdata);
				if (!xml)
				{
					OmnAlarm << "Failed to retrieve the xml: " << enderr;
					continue;
				}
				
				l_key = xml->getAttrStr(mLStatKey, "");
				if (l_key == "")
				{
					OmnAlarm << "Failed to retrieve the l_key: " << enderr;
					continue;
				}
				l_value = xml->getAttrU64(mLStatValue, 0);
			}
		}
		else
		{
			rslt = l_data->nextDocidValue(l_value, l_key, l_finished, rdata);
			if (l_finished && mCrtLIdx < l_total)
			{
				mLTerm->setStartIdx(mCrtLIdx - 1);
				mLTerm->runQuery(rdata);
				l_data = mLTerm->getQueryData();
				aos_assert_r(l_data, false);
				l_finished = false;
				rslt = l_data->nextDocidValue(l_value, l_key, l_finished, rdata);
			}
		}
		if (l_finished)
		{
			break;
		}
		if (!rslt || l_key == "")
		{
			OmnAlarm << "Failed to retrieve the l_key: " << enderr;
			continue;
		}

		r_map_itr = r_map.find(l_key);
		if (r_map_itr == r_map.end())
		{
			continue;	
		}

		r_value = r_map_itr->second;

		mDocsNum++;
		if (ignore_num > 0)
		{
			ignore_num--;
			continue;
		}

		if ((i64)mDocs.size() < mPsize)
		{
			OmnString str = "<record";
			str << " key=\"" << l_key << "\""
				<< " l_value=\"" << l_value << "\""
				<< " r_value=\"" << r_value << "\"/>";
			mDocs.push_back(str);
		}
		else
		{
			if (!mNeedSpecificTotal)
			{
				break;
			}
		}
	}

	if (!mNeedSpecificTotal)
	{
		if (mCrtLIdx <= 0)
		{
			mDocsNum = 0;
		}
		else
		{
			mDocsNum = (l_total + 1) * (mDocs.size() + mStartIdx) / mCrtLIdx;
		}
	}
	mDataLoaded = true;
	return true;
}


bool
AosTermJoin2::parse(const AosXmlTagPtr &conds, const AosRundataPtr &rdata)
{
	mIsGood = false;
	aos_assert_r(rdata, false);
	aos_assert_r(conds, false);

	mNeedSpecificTotal = conds->getAttrBool("need_specific_total", false);
	mIsSameOrder = conds->getAttrBool("is_same_order", true);

	AosXmlTagPtr l_conds = conds->getFirstChild("l_conds");
	AosXmlTagPtr r_conds = conds->getFirstChild("r_conds");
	aos_assert_r(l_conds && r_conds, false);

	mLIsStat = l_conds->getAttrBool("is_stat");
	mRIsStat = r_conds->getAttrBool("is_stat");
	if (mLIsStat)
	{
		mLStatIdx = l_conds->getAttrInt64("stat_idx", -1);
		aos_assert_r(mLStatIdx >= 0, false);
		if (!mIsSameOrder)
		{
			mLStatKey = l_conds->getAttrStr("stat_key", "");
			aos_assert_r(mLStatKey != "", false);
			mLStatValue = l_conds->getAttrStr("stat_value", "");
			aos_assert_r(mLStatValue != "", false);
		}
	}
	if (mRIsStat)
	{
		mRStatIdx = r_conds->getAttrInt64("stat_idx", -1);
		aos_assert_r(mRStatIdx >= 0, false);
		if (!mIsSameOrder)
		{
			mRStatKey = l_conds->getAttrStr("stat_key", "");
			aos_assert_r(mRStatKey != "", false);
			mRStatValue = l_conds->getAttrStr("stat_value", "");
			aos_assert_r(mRStatValue != "", false);
		}
	}

	i64 guard = 20;
	AosXmlTagPtr p = conds;
	while ((p = p->getParentTag()) && guard-- > 0)
	{
		if (p->getAttrStr("start_idx") != "")
		{
			mStartIdx = p->getAttrInt64("start_idx", 0);
			mPsize = p->getAttrInt64("psize", eDftPsize);
			break;
		}
	}

	mLTerm = createQueryTerm(l_conds, rdata);
	mRTerm = createQueryTerm(r_conds, rdata);
	aos_assert_r(mLTerm && mRTerm, false);
	aos_assert_r(mLTerm->withValue() && mRTerm->withValue(), false);

	if (!mDataLoaded) loadData(rdata);
	rdata->setOk();
	mIsGood = true;
	return true;
}


AosQueryTermObjPtr
AosTermJoin2::createQueryTerm(
		const AosXmlTagPtr &conds, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(conds, 0);

	AosQueryTermObjPtr or_term = OmnNew AosTermOr();
	AosQueryTermObjPtr cond = OmnNew AosTermAnd();

	i64 guard = 20;
	AosXmlTagPtr andterm = conds->getFirstChild();
	while (guard-- && andterm) 
	{
		OmnString type = andterm->getAttrStr("type");
		if (andterm->getTagname() == AOSTAG_QUERY_STAT)
        {
			AosXmlTagPtr statTerm = andterm->getFirstChild();
			aos_assert_r(statTerm, 0);

			AosQueryType::E type = AosQueryType::toEnum(
				statTerm->getAttrStr("type"));
			aos_assert_r(AosQueryType::isValidCounterType(type), 0);

			if (type == AosQueryType::eCounterMulti)
			{
				bool rslt = or_term->addStatTerm(statTerm, rdata);
				aos_assert_r(rslt, 0);
			}
			else
			{
				AosQueryTermObjPtr term = cond->addTerm(statTerm, rdata);
				aos_assert_r(term, 0);

				cond->setNeedOrder(false);
				or_term->addTerm(cond, rdata);
				or_term->setDocRetriever(term);
				or_term->loadData(rdata);
        	}
			return or_term;
		}

		if (type != "AND")
		{
			rdata->setError() << "Expecting the AND term but failed!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		AosXmlTagPtr term = andterm->getFirstChild();
		while (term)
		{
			cond->addTerm(term, rdata);
			term = andterm->getNextChild();
		}

		or_term->addTerm(cond, rdata);
		or_term->loadData(rdata);
		or_term->reset(rdata);

		andterm = conds->getNextChild();
	}
	
	return or_term;
}


AosQueryTermObjPtr
AosTermJoin2::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermJoin2(def, rdata);
}

