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
#include "Query/TermOr.h"

#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "Query/TermAnd.h"
#include "Query/TermCounterMulti.h"
#include "Query/TermCounterSub.h"
#include "Query/TermCounterSub2.h"
#include "Query/TermCounterMultiSub.h"
#include "Query/TermCounterMultiSub2.h"
#include "Query/TermCounterSingle.h"
#include "Query/TermCounterAll.h"
#include "Query/TermGroup.h"
#include "Query/TermJoin.h"
#include "Query/TermJoin2.h"
#include "Query/TermFilter.h"
#include "Query/TermCounterOfUWDivOrder.h"
#include "Util/StrSplit.h"
#include "Util/OmnNew.h"
#include "Util/Opr.h"
#include "XmlUtil/XmlTag.h"



AosTermOr::AosTermOr()
:
AosQueryTerm(AOSTERMTYPE_OR, AosQueryType::eOr, false),
mNumTerms(0),
mHelpTerm(0),
mIgnore(false)
{
	mReverse = false;
}


AosTermOr::~AosTermOr()
{
}


bool	
AosTermOr::addTerm(
		const AosQueryTermObjPtr &term,
		const AosRundataPtr &rdata)
{
	mTerms[mNumTerms++] = term;
	return true;
}
	

// Do not use this function
// Chen Ding, 08/02/2011
// i64
// AosTermOr::getTotalDocids()
// {
// 	// It returns the largest
// 	if (mNumTerms == 0) return 0;
// 	i64 largest = 0;
// 	for (i64 i=0; i<mNumTerms; i++)
// 	{
// 		i64 td = mTerms[i]->getTotalDocids();
// 		if (td > largest) largest = td;
// 	}
// 	return largest;
// }


i64		
AosTermOr::getTotal(const AosRundataPtr &rdata)
{
	// It first finds the smallest term and then checks whether
	// it has small enough number of docids. If yes, it will 
	// calculate the total by actually running the query. 
	// Otherwise, it will estimate a total.
	if (mNumTerms == 0) 
	{
		mTotal = 0;
		return 0;
	}

	// Linda, 2012/08/31
	if (mDocRetriever)                           
	{
		mTotal = mDocRetriever->getTotal(rdata);
		return mTotal;
	}

	if (mNumTerms == 1)
	{
		mTotal = mTerms[0]->getTotal(rdata);
		return mTotal;
	}

	OmnNotImplementedYet;
	return -1;
	/*
	// Find the largest term
	u32 largest = mTerms[0]->getTotalDocids();
	i64 idx = 0;
	for (i64 i=1; i<mNumTerms; i++)
	{
		u32 td = mTerms[i]->getTotalDocids();
		if (td > largest ) 
		{
			largest = td;
			idx = i;
		}
	}

	// Too many. We will estimate it.
	if (largest > eTotalThreshold)
	{
		mTotal = largest / mNumTerms;
		return mTotal;
	}

	// Need to actually calculate it. 
	aos_assert_r(getTotal(idx), -1);
	return mTotal;
	*/
}


void
AosTermOr::reset(const AosRundataPtr &rdata)
{
	for (i64 i=0; i<mNumTerms; i++) mTerms[i]->reset(rdata);
	mNoMoreDocs = false;
}


void
AosTermOr::toString(OmnString &str)
{
	for (i64 i=0; i<mNumTerms; i++)
	{
		mTerms[i]->toString(str);
	}
}


/*
bool
AosTermOr::preproc(const AosRundataPtr &rdata)
{
	for (i64 i=0; i<mNumTerms; i++)
	{
		mTerms[i]->preproc(rdata);
	}
	return true;
}
*/


bool 	
AosTermOr::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the next docid
	docid = AOS_INVDID;
	finished = false;
	if (mNoMoreDocs || mNumTerms == 0) 
	{
		finished = true;
		return true;
	}

	AosQueryTermObjPtr thisptr(this, false);
	if (mHelpTerm) return mHelpTerm->nextDocid(thisptr, docid, finished, rdata);
	//Linda, AAAAA
	if (mDocRetriever) return mDocRetriever->nextDocid(thisptr, docid, finished, rdata);
	if (mNumTerms == 1) return mTerms[0]->nextDocid(thisptr, docid, finished, rdata);

	// Currently we do not support OR
	OmnNotImplementedYet;	
	return false;	
}

bool	
AosTermOr::withValue() const
{
	aos_assert_r(mNumTerms > 0, false);
	if (mDocRetriever)
	{
   		return  mDocRetriever->withValue();
	}

	if (mNumTerms == 1) return mTerms[0]->withValue();

	OmnNotImplementedYet;
	return false;
}

bool 	
AosTermOr::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		OmnString &value, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the next docid
	docid = AOS_INVDID;
	finished = false;
	if (mNoMoreDocs || mNumTerms == 0) 
	{
		finished = true;
		return true;
	}

	AosQueryTermObjPtr thisptr(this, false);
	if (mHelpTerm) return mHelpTerm->nextDocid(thisptr, docid, value, finished, rdata);
	//Linda, AAAAA
	if (mDocRetriever) return mDocRetriever->nextDocid(thisptr, docid, value, finished, rdata);
	if (mNumTerms == 1) return mTerms[0]->nextDocid(thisptr, docid, value, finished, rdata);

	// Currently we do not support OR
	OmnNotImplementedYet;	
	return false;	
}


bool
AosTermOr::runQuery(const AosRundataPtr &rdata)
{
	// This function retrieves the next docid
	if (mNoMoreDocs || mNumTerms == 0)
	{
		return true;
	}

	if (mNumTerms == 1) return mTerms[0]->runQuery(rdata);

	// Currently we do not support OR
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermOr::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	aos_assert_r(mNumTerms > 0, false);
	// Linda, 2012/11/28
	if (mDocRetriever)
	{
   		return  mDocRetriever->moveTo(startidx, rdata);
	}

	if (mNumTerms == 1) return mTerms[0]->moveTo(startidx, rdata);

	OmnNotImplementedYet;
	return false;
}


bool
AosTermOr::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	for (i64 i=0; i<mNumTerms; i++)
	{
		if (mTerms[i]->checkDocid(docid, rdata)) return true;
	}
	return false;
}


bool
AosTermOr::setOrder(
		const OmnString &container,
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	for (i64 i=0; i<mNumTerms; i++)
	{
		if (mTerms[i]->setOrder(container, name, reverse, rdata)) return true;
	}

	return false;
}


// Do not use this function. Chen Ding, 08/02/2011
// void
// AosTermOr::parseFinished()
// {
// 	for (i64 i=0; i<mNumTerms; i++)
// 		mTerms[i]->parseFinished();
// }


bool
AosTermOr::loadData(const AosRundataPtr &rdata)
{
	for (i64 i=0; i<mNumTerms; i++)
	{
		mTerms[i]->loadData(rdata);
	}
	return true;
}

bool 	
AosTermOr::getRsltSizeFromIIL(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 	
AosTermOr::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosTermOr::addStatTerm(
		const AosXmlTagPtr &statTerm, 
		const AosRundataPtr &rdata)
{
	// Stat term is in the form:
	// 	<stat>
	// 		<statterm type="xxx" .../>
	// 		...
	// 	</stat>
	//
	AosQueryTermObjPtr thisptr(this, false);
	AosQueryType::E type = AosQueryType::toEnum(statTerm->getAttrStr("type", ""));
	if (!AosQueryType::isValidCounterType(type))
	{
		rdata->setError() << "Unrecognized statistics type: " 
			<< statTerm->getAttrStr("type", "");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	switch (type)
	{
	case AosQueryType::eCounterSingle:
		 mHelpTerm = OmnNew AosTermCounterSingle(statTerm, rdata);
		 break;

	case AosQueryType::eCounterMulti:
		 mHelpTerm = OmnNew AosTermCounterMulti(statTerm, thisptr, rdata);
		 break;

	case AosQueryType::eCounterSub:
		 mHelpTerm = OmnNew AosTermCounterSub(statTerm, rdata); 
		 break;
	
	case AosQueryType::eCounterSub2:
		 mHelpTerm = OmnNew AosTermCounterSub2(statTerm, rdata); 
		 break;
	
	case AosQueryType::eCounterMultiSub:
		 mHelpTerm = OmnNew AosTermCounterMultiSub(statTerm, rdata);
		 break;

	case AosQueryType::eGroup:
		 mHelpTerm = OmnNew AosTermGroup(statTerm, rdata);
		 break;

	case AosQueryType::eJoin:
		 mHelpTerm = OmnNew AosTermJoin(statTerm, rdata);
		 break;
	
	case AosQueryType::eJoin2:
		 mHelpTerm = OmnNew AosTermJoin2(statTerm, rdata);
		 break;
	
	case AosQueryType::eFilter:
		 mHelpTerm = OmnNew AosTermFilter(statTerm, rdata);
		 break;

	case AosQueryType::eCounterOfUWDivOrder:
		 mHelpTerm = OmnNew AosTermCounterOfUWDivOrder(statTerm, rdata);
		 break;

	case AosQueryType::eCounterMultiSub2:
		 mHelpTerm = OmnNew AosTermCounterMultiSub2(statTerm, rdata);
		 break;

	case AosQueryType::eCounterAll:
		 mHelpTerm = OmnNew AosTermCounterAll(statTerm, rdata);
		 break;
	default:
		 rdata->setError() << "Unrecognized statistics type: " 
			<< statTerm->getAttrStr("type", "");
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 break;
	}

	mIgnore = true;
	aos_assert_r(mHelpTerm, false);
	return true;
}


AosQueryRsltObjPtr
AosTermOr::getQueryData() const
{
	if (mDocRetriever && mDocRetriever->getType() == AosQueryType::eCounterSub2)
	{
		return mDocRetriever->getQueryData();
	}
	if (mNumTerms == 1) 
	{
		if (mTerms[0]->getType() == AosQueryType::eAnd)
		{
			return mTerms[0]->getQueryData();
		}
	}

	OmnNotImplementedYet;
	return 0;
}


/*void
AosTermOr::setQueryDataEmpty()
{
	if (mNumTerms == 1) 
	{
		if (mTerms[0]->getType() == AosQueryType::eAnd)
		{
			//return ((AosTermAnd*)mTerms[0].getPtr())->setQueryDataEmpty();
			
		}
	}

	OmnNotImplementedYet;
}
*/

AosXmlTagPtr 
AosTermOr::getDoc(const u64 &docid, const AosRundataPtr &rdata)
{
    if (mHelpTerm) return mHelpTerm->getDoc(docid, rdata);
	if (mDocRetriever) return mDocRetriever->getDoc(docid, rdata);
	if (mNumTerms == 1) return mTerms[0]->getDoc(docid, rdata);
    return AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
}


AosXmlTagPtr 
AosTermOr::getDoc(const u64 &docid, const OmnString &value, const AosRundataPtr &rdata)
{
    if (mHelpTerm) return mHelpTerm->getDoc(docid, value, rdata);
	if (mDocRetriever) return mDocRetriever->getDoc(docid, value, rdata);
	if (mNumTerms == 1) return mTerms[0]->getDoc(docid, value, rdata);
    return AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
}


bool
AosTermOr::setPagesize(const i64 &psize)
{
    if (mNumTerms == 1) return mTerms[0]->setPagesize(psize);
    OmnAlarm << "Not implemented Yet: " << mNumTerms << enderr;
    return false;
}

bool	
AosTermOr::setStartIdx(const i64 &num)
{
    if (mNumTerms == 1) return mTerms[0]->setStartIdx(num);
    OmnAlarm << "Not implemented Yet: " << mNumTerms << enderr;
    return false;
}

AosQueryTermObjPtr
AosTermOr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


bool
AosTermOr::collectInfo(const AosRundataPtr &rdata)
{
	mCondInfo.reset();
	for (i64 i=0; i<mNumTerms; i++)
	{
		mTerms[i]->collectInfo(rdata);
		mCondInfo.mergeOr(mTerms[i]->getCondInfo());
	}
	return true;
}

bool	
AosTermOr::queryFinished()
{
	if (mDocRetriever) return mDocRetriever->queryFinished();
	if(!mTerms[0])return true;
	return mTerms[0]->queryFinished();
}

bool 
AosTermOr::isCompoundQuery() const
{
	aos_assert_r(mNumTerms > 0, false);
	if (mNumTerms > 1) return true;
	return mTerms[0]->isCompoundQuery();
}


// Chen Ding, 2014/01/29
bool 
AosTermOr::getTimeCond(
		const AosRundataPtr &rdata, 
		const OmnString &time_fname, 
		int &num_matched,
		int *start_days, 
		int *end_days)
{
	for (int i=0; i<mNumTerms; i++)
	{
		if (!mTerms[i]->getTimeCond(rdata, time_fname, num_matched, start_days, end_days)) return false;
		if (num_matched > 0) return true;
	}

	num_matched = 0;
	return true;
}
	
/*
bool
AosTermOr::findTermByStatKeyField(
		const OmnString &key_field, 
		bool &find,
		AosQueryTermObjPtr &term)
{
	if (mNumTerms == 1) return mTerms[0]->findTermByStatKeyField(key_field, find, term);
	return false;
}

*/

bool
AosTermOr::getCondTerms(vector<AosQueryTermObjPtr> & conds)
{
	if (mNumTerms == 1) return mTerms[0]->getCondTerms(conds);
	return false;
}
	

bool
AosTermOr::addInternalStatIdTerm(
		const AosRundataPtr &rdata,
		const OmnString &iil_name,
		const u32 stat_internal_id)
{
	if(mNumTerms == 1)	return mTerms[0]->addInternalStatIdTerm(rdata, iil_name, stat_internal_id);	
	return false;
}

bool
AosTermOr::removeStatValueTerms(
		const AosRundataPtr &rdata,
		vector<AosQueryTermObjPtr> &value_terms)
{
	if(mNumTerms == 1)	return mTerms[0]->removeStatValueTerms(rdata, value_terms);	
	return false;
}

