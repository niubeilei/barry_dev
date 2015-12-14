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
// Paging
// ======
// mRemovedNum is the 'current position', or the position at which it
// holds the data. 'mStartIdx' is the start of the requested data. 
//
// Modification History:
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermAnd.h"

#include "Alarm/Alarm.h"
#include "Query/TermArith.h"
#include "Query/TermKeywordsAnd.h"
#include "Query/TermTagAnd.h"
#include "Query/TermTypes.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "Util/OmnNew.h"
#include "Util/Opr.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"



AosTermAnd::AosTermAnd()
:
AosQueryTerm(AOSTERMTYPE_AND, AosQueryType::eAnd, false),
mNumTerms(0),
mDataLoaded(false),
mPsize(eDftPsize),
mStartIdx(0),
mExpectSize(0),
mBlockSize(0),
mNeedOrder(true),
mRemovedNum(0),
mHasOrderTerm(false)
{
	mReverse = false;
}


AosTermAnd::AosTermAnd(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_AND, AosQueryType::eAnd, regflag),
mNumTerms(0),
mDataLoaded(false),
mPsize(eDftPsize),
mStartIdx(0),
mExpectSize(0),
mBlockSize(0),
mNeedOrder(true),
mRemovedNum(0),
mHasOrderTerm(false)
{
	mReverse = false;
}


void
AosTermAnd::toString(OmnString &str)
{
	str << "<andcond type=\"AND\">";
	for (i64 i=0; i<mNumTerms; i++)
	{
		mTerms[i]->toString(str);
	}
	str << "</andcond>";
}


/*
bool
AosTermAnd::preproc(const AosRundataPtr &rdata)
{
	// It sorts all its terms based on the following:
	// All ordered terms are put in front
	// Terms that have fewer docs are in the front.
	//
	// IMPORTANT: In the current implementation, we only support
	// one ordered condition.
	i64 sortstartidx = 0;
	for (i64 i=1; i<mNumTerms; i++)
	{
		if (mTerms[i]->isOrdered())	
		{
			AosQueryTermObjPtr tmp = mTerms[0];
			mTerms[0] = mTerms[i];
			mTerms[i] = tmp;
			sortstartidx = 1;
			break;
		}
	}

	// Next, sort all the conditions based on the size, smaller
	// ones first.
	for (i64 i=sortstartidx; i<mNumTerms-1; i++)
	{
		i64 total1 = mTerms[i]->getTotal(rdata);
		i64 total2 = total1;
		i64 idx = -1;
		for (i64 j=i+1; j<mNumTerms; j++)
		{
			i64 tt = mTerms[j]->getTotal(rdata);
			if (tt < total2) 
			{
				total2 = tt;
				idx = j;
			}
		}

		if (total2 < total1)
		{
			AosQueryTermObjPtr tmp = mTerms[i];
			mTerms[i] = mTerms[idx];
			mTerms[idx] = tmp;
		}
	}
	return true;
}
*/


i64		
AosTermAnd::getTotal(const AosRundataPtr &rdata)
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

	if (!mDataLoaded) loadData(rdata);
	return mTotal;
}


void
AosTermAnd::reset(const AosRundataPtr &rdata)
{
	mNoMoreDocs = false;
	for (i64 i=0; i<mNumTerms; i++) 
	{
		mTerms[i]->reset(rdata);
	}
	if (mQueryData) mQueryData->reset();
}


bool 	
AosTermAnd::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		OmnString &key, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the next docid. If the term has no
	// more docs to search, it returns immediately. Otherwise, it 
	// checks whether the term contains only two term, one by 
	// docid and the other by siteid. If this is the case, 
	// we can use just the docid term to search. 
	//
	// Otherwise, it searches from the first term, and verifies 
	// by all other terms.
	
	if (mNumTerms == 1 && mTerms[0]->needHandleNextDocid())
	{
		AosQueryTermObjPtr thisptr(this, false);
		return mTerms[0]->nextDocid(thisptr, docid, key, finished, rdata);
	}

	docid = AOS_INVDID;
	if (mNoMoreDocs || mNumTerms == 0) 
	{
		finished = true;
		mNoMoreDocs = true;
		return true;
	}

	if (!mDataLoaded) loadData(rdata);

	finished = false;

	if (!mQueryData)
	{
		finished = true;
		docid = 0;
		key = "";
		return false;
	}

	bool rslt = mQueryData->nextDocidValue(docid, key, mNoMoreDocs, rdata);
	finished = mNoMoreDocs;
	return rslt;
}


bool 	
AosTermAnd::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the next docid. If the term has no
	// more docs to search, it returns immediately. Otherwise, it 
	// checks whether the term contains only two term, one by 
	// docid and the other by siteid. If this is the case, 
	// we can use just the docid term to search. 
	//
	// Otherwise, it searches from the first term, and verifies 
	// by all other terms.
	
	if (mNumTerms == 1 && mTerms[0]->needHandleNextDocid())
	{
		AosQueryTermObjPtr thisptr(this, false);
		return mTerms[0]->nextDocid(thisptr, docid, finished, rdata);
	}

	docid = AOS_INVDID;
	if (mNoMoreDocs || mNumTerms == 0) 
	{
		finished = true;
		mNoMoreDocs = true;
		return true;
	}

	// Need to check whether it is to retrieve docs by docid:
	// 	1. There shall be two terms: one is TermArith and the
	// 	   other TermTag.
	// 	2. The TermArith is retrieve by docid
	// 	3. The TermTag is siteid
	if (!mDataLoaded) loadData(rdata);


	finished = false;
//	if (mNumTerms == 2)
//	{
//		i64 idx = -1;
//		if (mTerms[0]->isByDocid() && mTerms[1]->isSiteidTag())
//		{
//			idx = 0;	
//		}
//		else if (mTerms[1]->isByDocid() && mTerms[0]->isSiteidTag())
//		{
//			idx = 1;
//		}
//
//		if (idx != -1)
//		{
//			// This means that 'idx' is the docid term and the other 
//			// is siteid. 
//			mTerms[idx]->nextDocid(docid, errmsg, finished);
//			if (docid == AOS_INVDID)
//			{
//				mNoMoreDocs = true;
//			}
//			finished = true;
//			return true;
//		}
//	}

	if (!mQueryData)
	{
		finished = true;
		docid = 0;
		return false;
	}

	// Chen Ding, 07/14/2012
	// return mQueryData->nextDocid(docid, mNoMoreDocs, rdata);
	bool rslt = mQueryData->nextDocid(docid, mNoMoreDocs, rdata);
	finished = mNoMoreDocs;
	return rslt;
}


bool
AosTermAnd::getTotal(const i64 &idx, const AosRundataPtr &rdata)
{
	// This is a private function used by AosTermAnd::getTotal()
	u64 did;
	bool finished = false;
	mTotal = 0;
	i64 safeguard = 0;
	AosQueryTermObjPtr thisptr(this, false);
	while (safeguard++ < eMaxCheckedTotal)
	{
		aos_assert_r(mTerms[idx]->nextDocid(thisptr, did, finished, rdata), false);
		if (finished)
		{
			// No more docs
			return true;
		}

		// Check whether the docid is also selected by all other terms.
		i64 i;
		for (i=1; i<mNumTerms; i++)
		{
			if (i == idx) continue;
			if (!mTerms[i]->checkDocid(did, rdata)) break;
		}
		
		if (i >= mNumTerms) mTotal++;
	}

	// This means it runs out of the tries. Simply return
	OmnAlarm << "Run out of tries!" << enderr;
	return true;	
}


bool 	
AosTermAnd::moveToSingle(const i64 &startidx, const AosRundataPtr &rdata)
{
	aos_assert_r(startidx == mStartIdx,false);

	aos_assert_r(mRemovedNum == 0,false);
	if(!mQueryData  || mPsize > mQueryData->getNumDocs()) loadData(rdata);
	if(!mQueryData) return true;


	mQueryData->reset();
	
	return true;
}


bool 	
AosTermAnd::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	// It moves the pointer to the specified location. 
	// This is done by starting from the beginning, 
	// selecting docids until it selects the 'startidx'-th
	// docids.
	//
	// This does not seem right. If mQueryData is null, it should call
	// 'query(...)' to load the data. After that, it calls mQueryData::moveTo(...)
	// Chen Ding, 08/02/2011
	bool single_term = (mNumTerms == 1);
	if(single_term)
	{
		return moveToSingle(startidx,rdata);
	}
	bool rslt = false;

	aos_assert_r(startidx == mStartIdx,false);
	countExpectBlockSize();	
	aos_assert_r(mStartIdx >= mRemovedNum,false);

	if(!mQueryData  || mStartIdx + mPsize > mRemovedNum + mQueryData->getNumDocs()) loadData(rdata);
	if(!mQueryData) return true;
	if(mQueryContext && !mQueryContext->finished())
	{
		aos_assert_r(mStartIdx + mPsize <= mRemovedNum + mQueryData->getNumDocs() ,false);
	}
	// currently we remove all the old data before the start idx, next step we may need to remain some pages as possible
	rslt = mQueryData->removeOldData(mStartIdx - mRemovedNum);
	mRemovedNum = mStartIdx;

	mQueryData->reset();
	bool finished = false;
	rslt = mQueryData->moveTo(mStartIdx - mRemovedNum, finished, rdata);

	if (!rslt || finished) 
	{
		mNoMoreDocs = true;
		return true;
	}
	
	return true;
}


bool
AosTermAnd::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	for (i64 i=0; i<mNumTerms; i++)
	{
		if (!mTerms[i]->checkDocid(docid, rdata)) return false;
	}
	return true;
}


bool
AosTermAnd::addTerm(
		const AosQueryTermObjPtr &term,
		const AosRundataPtr &rdata)
{
	if (mNumTerms >= eMaxTerms) 
	{
		AosSetError(rdata, AosErrmsgId::eTooManyTerms);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mTerms[mNumTerms++] = term;
	return true;
}


AosQueryTermObjPtr
AosTermAnd::addTerm(
		const AosXmlTagPtr &term,
		const AosRundataPtr &rdata)
{
	if (mNumTerms >= eMaxTerms) 
	{
		OmnAlarm << "Too many terms: " << mNumTerms << enderr;
		return 0;
	}

	AosQueryTermObjPtr newterm;
	AosQueryTermObjPtr thisptr(this, false);
	AosQueryType::E type = AosQueryType::toEnum(term->getAttrStr(AOSTAG_TYPE));
	switch (type)
	{
	case AosQueryType::eTagAnd:
		 {
		 return AosTermTagAnd::addTerms(thisptr, term, rdata);
		 }

	case AosQueryType::eKeywordsAnd:
		 {
		 return AosTermKeywordsAnd::addTerms(thisptr, term, rdata);
		 }

	case AosQueryType::eJimo:
		 newterm = getJimoTerm(rdata, term);
		 if (newterm)
		 {
			 mTerms[mNumTerms++] = newterm;
			 return newterm;
		 }
		 return 0;

	default:
		 break;
	}

	newterm = AosQueryTerm::getTerm(type, term, rdata);
	if (newterm)
	{
		if (!newterm->isGood())
		{
			// Term is not good. Ignore it.
			return 0;
		}

		mTerms[mNumTerms++] = newterm;
		return newterm;
	}

	rdata->setError() << "Unrecognized term: " << type;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return 0;
}


/*
bool
AosTermAnd::parseEpochCond(
	const AosXmlTagPtr &term,
	const AosRundataPtr &rdata)
{
	// 'term' is an epoch time based condition:
	// 	<cond type="ep" 
	// 		reverse="true|false"
	// 		order="true|false"
	// 		opr="xx"
	// 		aname="attrname"
	// 		year="yyyy"
	// 		month="mm"		optional
	// 		day="dd"		optional
	// 		hour="hh"		optional
	// 		min="mm"		optional
	// 		sec="ss"		optional
	// 	>
	// This function retrieves the time and convert the time into
	// the Epoch time. It then construct the query for the identified
	// attribute.
	i64 year = term->getAttrInt64("year", -1);
	if (year == -1)
	{
		rdata->setError() << "Missing the year in the query!";
		return false;
	}

	// Retrieve the operator
	AosOpr opr = AosOpr_toEnum(term->getAttrStr("opr"));

	// Retrieve the attribute name
	OmnString aname = term->getAttrStr("aname");
	if (aname == "")
	{
		rdata->setError() << "Missing the attribute name!";
		return false;
	}

	if (aname == "ctime")
	{
		aname = AOSTAG_CT_EPOCH;
	}
	else if (aname == "mtime")
	{
		aname = AOSTAG_MT_EPOCH;
	}

	i64 month = term->getAttrInt64("month", 0);
	i64 day = term->getAttrInt64("day", 1);
	i64 hour = term->getAttrInt64("hour", 0);
	i64 min = term->getAttrInt64("min", 0);
	i64 sec = term->getAttrInt64("sec", 0);
	i64 epoch = AosGetEpochTime(year, month, day, hour, min, sec);

	OmnString value;
	value << epoch;
	AosQueryTermObjPtr newterm = OmnNew AosTermArith( 
			aname, value, opr, false, rdata); 
	aos_assert_r(newterm, false);
	mIsGood = newterm->isGood();
	if (!mIsGood)
	{
		rdata->setError() << "Failed to parse the condition!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mTerms[mNumTerms++] = newterm;
	rdata->setOk();
	return true;
}
*/


/*
bool
AosTermAnd::procWordsForm1(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	// This function processes words, which are a string that 
	// contains words separated by ','.
	
	OmnString contents = term->getNodeText();
	AosStrSplit split;
	OmnString words[eMaxWords];
	bool finished = false;
	i64 nn = split.splitStr(contents.data(), ",", words, eMaxWords, finished);
	AosQueryTermObjPtr newterm;
	for (i64 i=0; i<nn; i++)
	{
		newterm = OmnNew AosTermKeywords("", words[i], false, false, rdata);
		aos_assert_r(newterm, false);
		mTerms[mNumTerms++] = newterm;
	}
	return true;
}
*/


bool
AosTermAnd::setOrder(
		const OmnString &container,
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	// This function assumes the term whose name is 'name' should
	// be used to control the order. Whether it is normal order
	// or reverse order is indicated by 'order'. This function
	// checks all its member terms. If it finds the term whose
	// name is 'name', it moves the term to the top, serving
	// as the controlling term.
	if (!mNeedOrder) return true;
	for (i64 i=0; i<mNumTerms; i++)
	{
		if (mTerms[i]->setOrder(container, name, reverse, rdata))
		{
			// It is the term that should control the order. Move 
			// it to the top.
			AosQueryTermObjPtr tt = mTerms[i];
			mTerms[i] = mTerms[0];
			mTerms[0] = tt;
			return true;
		}
	}

	// This means that the ordered term is not in the conditions
	// yet. Need to add it.
	aos_assert_r(mNumTerms < eMaxTerms-1, false);
	AosQueryTermObjPtr newterm = OmnNew AosTermArith(name, "", true, reverse, 
			eAosOpr_an, container, rdata);
	aos_assert_r(newterm, false);
	for (i64 i=mNumTerms-1; i>=0; i--)
	{
		mTerms[i+1] = mTerms[i];
	}
	mTerms[0] = newterm;
	mNumTerms++;
	return true;
}


void
AosTermAnd::parseFinished()
{
	if (mNumTerms <= 0) return;
	if (mTerms[0]->getType() == AosQueryType::eNotExist)
	{
		AosQueryTermObjPtr tmp = mTerms[0];
		mTerms[0] = mTerms[mNumTerms-1];
		mTerms[mNumTerms-1] = tmp;
	}
}


bool
AosTermAnd::loadData(const AosRundataPtr &rdata)
{
	// This function loads data from iils.

	// 1. Adjust term order
return loadData3(rdata);

	if (mDataLoaded) return true;

	for(i64 i=0; i<mNumTerms; i++)
	{
		if(!mTerms[i]->hasDoc())
		{
			mQueryData = AosQueryRsltObj::getQueryRsltStatic(); 
			mQueryData->setWithValues(mWithValues);
			mTotal = 0;
			mDataLoaded = true;
			return true;
		}
	}

	if(mNumTerms == 1)
	{
		mTerms[0]->setOrdered(true);
	}

	if(mNumTerms > 1)
	{
		bool hasOrderTerm = false;
		AosQueryTermObjPtr first_term = mTerms[0];
		for(i64 i=0; i<mNumTerms; i++)
		{
			if(mTerms[i]->isOrdered())
			{
				if (hasOrderTerm)
				{
					mTerms[i]->setOrdered(false);
				}
				hasOrderTerm = true;
			}

			// Chen Ding, 08/02/2011
			// IMPORTANT: We will no longer use this condition. Need to re-think
			// about how to handle siteid. For the time being, the following is
			// turned off.
			// if (mTerms[i]->isSiteidTag())
			// {
			// 	// exchange the siteidtag to the last one
			// 	AosQueryTermObjPtr tmp_term = mTerms[i];
			// 	mTerms[i] = mTerms[mNumTerms-1];
			// 	mTerms[mNumTerms-1] = mTerms[i];
			// }
		}

		if(!hasOrderTerm)
		{
			first_term->setOrdered(true);
		}
	}


	// 2. Get the bitmap if needed
	// The following will loop over all the non-order terms. The result should
	// be a bitmap that is the AND of all the terms (except the order).
	bool useBitmap = (mNumTerms > 1);
	AosBitmapObjPtr bitmap;
	for(i64 i=0; i<mNumTerms; i++)
	{		
		//1. if it is sort term, query_rslt shouldn't be null
		if (mTerms[i]->isOrdered())
		{
			continue;
		}
		
		//2. set crt_bitmap
		if(useBitmap)
		{
			if(bitmap.isNull())
			{
				bitmap = AosBitmapMgrObj::getBitmapStatic();
			}
		}
		
		// get data
		bool rslt = mTerms[i]->getDocidsFromIIL(0, bitmap, mQueryContext, rdata);
		if(!rslt)
		{
			OmnAlarm << "can not get rslt from IIL" << enderr;
			releaseResource(bitmap, 0);
			bitmap = 0;
			return false;
		}

		if(bitmap->isEmpty())
		{
			// no result found
			break;
		}
	}

	if(bitmap && bitmap->isEmpty())
	{
		mQueryData = AosQueryRsltObj::getQueryRsltStatic();
		mQueryData->setWithValues(mWithValues);
		mTotal = 0; 
		mDataLoaded = true;
		return true;
	}
	
	if(!mQueryContext)
	{
		mQueryContext = AosQueryContextObj::createQueryContextStatic(); 
	}
	// It may or may not have a bitmap (stored in bitmap1). The following 
	// continue the processing
	AosQueryRsltObjPtr crt_rslt;
	crt_rslt = 0;
	for(i64 i=0; i<mNumTerms; i++)
	{
		//1. if it is sort term, query_rslt shouldn't be null
		if(mTerms[i]->isOrdered())
		{
			crt_rslt = AosQueryRsltObj::getQueryRsltStatic();
			mQueryContext->setReverse(mTerms[i]->isReverse());
			crt_rslt->setWithValues(mWithValues);
		
			// get data
			bool rslt = mTerms[i]->getDocidsFromIIL(crt_rslt, bitmap, mQueryContext, rdata);
			if(!rslt)
			{
				OmnAlarm << "can not get rslt from IIL" << enderr;
				releaseResource(bitmap,crt_rslt);			
				bitmap = 0;
				return false;
			}
		}
	}
	

	if (!crt_rslt)
	{
		// should not happen
		//OmnAlarm << "no sorted term" << enderr;
		releaseResource(bitmap, crt_rslt);
		bitmap = 0;
		mNoMoreDocs = true;
		return true;
	}
	
	releaseResource(bitmap,0);
	bitmap = 0;
	mQueryData = crt_rslt;
	mTotal = mQueryData->getNumDocs();

	mDataLoaded = true;
	return true;
}


bool
AosTermAnd::query(
		const AosQueryRsltObjPtr &query_rslt,
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// 1. Construct the ANDed bitmap based on all the conditions. 
	// 2. Check whether there is any term that is used to sort the results. 
	//    If yes, move the term to the beginning. If no such term, do nothing.
	// 3. Save the bitmap to 'mQueryRslt'. 
	// 4. If there is only one term, it does nothing. 
	//
	// This function assumes all data have been loaded into mQueryData[].
	// This function retrieves the next docid
	if (mNoMoreDocs || mNumTerms == 0)
	{
		finished = true;
		return true;
	}

	bool rslt = true;
	if (!mDataLoaded) rslt = loadData(rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool 	
AosTermAnd::getRsltSizeFromIIL(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 	
AosTermAnd::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


void 
AosTermAnd::releaseResource(
		const AosBitmapObjPtr &bitmap,
		const AosQueryRsltObjPtr &rslt)
{
	if (bitmap.notNull())
	{
		AosBitmapMgrObj::returnBitmapStatic(bitmap);
	}
}


bool
AosTermAnd::runQuery(const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	return true;
}


bool
AosTermAnd::setPagesize(const i64 &psize)
{
    mPsize = psize;
	if (mPsize <= 0) 
	{
		mPsize = eDftPsize;
	}
	mExpectSize = 0;
	mBlockSize = 0;

	mNoMoreDocs = false;

    return true;
}

bool	
AosTermAnd::setStartIdx(const i64 &start)
{
	mStartIdx = start;
	mExpectSize = 0;
	mBlockSize = 0;
	mNoMoreDocs = false;
	return true;
}


AosQueryTermObjPtr
AosTermAnd::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


bool
AosTermAnd::collectInfo(const AosRundataPtr &rdata)
{
	for (i64 i=0; i<mNumTerms; i++)
	{
		mTerms[i]->collectInfo(rdata);
	}
	return false;
}


AosXmlTagPtr 
AosTermAnd::getDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	if (mNumTerms >= 1) return mTerms[0]->getDoc(docid, rdata);
	return AosQueryTerm::getDoc(docid, rdata);
}


bool	
AosTermAnd::withValue()const
{
	if (mNumTerms >= 1) return mTerms[0]->withValue();
	return AosQueryTerm::withValue();
}


AosXmlTagPtr 
AosTermAnd::getDoc(const u64 &docid, const OmnString &value, const AosRundataPtr &rdata)
{
	if (mNumTerms >= 1) return mTerms[0]->getDoc(docid, value, rdata);
	return AosQueryTerm::getDoc(docid, value, rdata);
}


i64
AosTermAnd::adjustTermOrder(const AosRundataPtr &rdata)
{
	i64 orderTermSeq = 0;
	if(mNumTerms == 1)
	{
		mTerms[0]->setOrdered(true);
		mHasOrderTerm = false;
		return 0;
	}

	if(mNumTerms > 1)
	{
		bool hasOrderTerm = false;
		AosQueryTermObjPtr first_term = mTerms[0];
		for(i64 i=0; i<mNumTerms; i++)
		{
			if(mTerms[i]->isOrdered())
			{
				if (hasOrderTerm)
				{
					mTerms[i]->setOrdered(false);
				}
				else
				{
					orderTermSeq = i;
				}
				hasOrderTerm = true;
			}
		}

		if(!hasOrderTerm)
		{
			first_term->setOrdered(true);
			orderTermSeq = 0;
		}
		mHasOrderTerm = hasOrderTerm;		// Chen Ding, 2013/08/02
		return orderTermSeq;
	}

	mHasOrderTerm = false;					// Chen Ding, 2013/08/02
	OmnShouldNeverComeHere;
	return 0;
}


i64
AosTermAnd::adjustTermOrder3(const AosRundataPtr &rdata)
{
	i64 orderTermSeq = 0;
	if(mNumTerms == 1)
	{
		mTerms[0]->setOrdered(true);
		return 0;
	}

	aos_assert_r(mNumTerms > 1,0);
	
	// 1. Find the order term
	// 2. re-range the order of other terms


	// 1. Find the order term
	bool hasOrderTerm = false;

	AosQueryTermObjPtr first_term = mTerms[0];
	for(i64 i=0; i<mNumTerms; i++)
	{
		if(mTerms[i]->isOrdered())
		{
			if (hasOrderTerm)
			{
				mTerms[i]->setOrdered(false);
			}
			else
			{
				orderTermSeq = i;
			}
			hasOrderTerm = true;
		}
	}

	if(!hasOrderTerm)
	{
		first_term->setOrdered(true);
		orderTermSeq = 0;
	}
	
	// 2. re-range the order of other terms
	// 2.1 move the order term to the first one
	if(orderTermSeq != 0)
	{
		switchTerm(0,orderTermSeq);
	}
	
	// 2.2 handle 2 terms case 
	if(mNumTerms == 2)
	{
		return 0;
	}
	
	// 2.2 more than 2 terms(at least 3), get rslt size
	for(i64 i=1; i<mNumTerms; i++)
	{		
		//bool rslt = mTerms[i]->getRsltSizeFromIIL(mQueryContext, rdata);
		mTerms[i]->getRsltSizeFromIIL(rdata);
		//mTerms[i]->setTotalInRslt(mQueryContext->getTotalDocInRslt());
	}	

	for(i64 i=mNumTerms-1; i>1; i--)
	{	
		for(i64 j = i;j < mNumTerms;j++)
		{
			// compare j & j-1
			if(mTerms[j-1]->getTotalInRslt() > mTerms[j]->getTotalInRslt())
			{
				switchTerm(j-1,j);
			}
		}
	}	
	

	return 0;
}


void
AosTermAnd::switchTerm(const i64 &a,const i64 &b)
{
	AosQueryTermObjPtr tmp = mTerms[a];
	mTerms[a] = mTerms[b];
	mTerms[b] = tmp;
}

void	
AosTermAnd::countExpectBlockSizeSingle()
{
	if(mQueryData)
	{
		mRemovedNum += mQueryData->removeOldData();
	}

	if(	mExpectSize != 0 && mBlockSize != 0) return;

	if((u64)mStartIdx < mRemovedNum)
	{
		// reset all the infomation
		mRemovedNum = 0;
		mQueryContext = 0;
		mQueryData = 0;
		mDataLoaded = false;
	}
	
	mExpectSize = mPsize;
	
	mBlockSize = mExpectSize;

	if(mBlockSize < eDftBsizeSingle)
	{
		mBlockSize = eDftBsizeSingle;
	}

	if(mBlockSize > eMaxBsize)
	{
		mBlockSize = eMaxBsize;
	}	
}

void	
AosTermAnd::countExpectBlockSize()
{
	//bool single_term = (mNumTerms == 1);
	// comment by shawn
//	if(single_term)
//	{
//		countExpectBlockSizeSingle();
//		return;
//	}

	if(	mExpectSize != 0 && mBlockSize != 0) return;
	
	if(mQueryData)
	{
		mRemovedNum += mQueryData->removeOldData();
	}

	if(mStartIdx < mRemovedNum)
	{
		// reset all the infomation
		mRemovedNum = 0;
		mQueryContext = 0;
		mQueryData = 0;
		mDataLoaded = false;
	}
	
	mExpectSize = mStartIdx + mPsize;
	mBlockSize = mExpectSize - mRemovedNum;

	if(mBlockSize < eDftBsize)
	{
		mBlockSize = eDftBsize;
	}

	if(mBlockSize > eMaxBsize)
	{
		mBlockSize = eMaxBsize;
	}	
}


bool
AosTermAnd::loadData2(const AosRundataPtr &rdata)
{
	// This function loads data from iils.
	countExpectBlockSize();

	// 1. Adjust term order
	if(mNumTerms <= 0)
	{
		return true;
	}
	
	if(mQueryContext && mQueryContext->finished())
	{
		return true;
	}
	
	for(i64 i=0; i<mNumTerms; i++)
	{
		if(!mTerms[i]->hasDoc())
		{
			if (!mQueryData) mQueryData = AosQueryRsltObj::getQueryRsltStatic();
			if (!mQueryContext) mQueryContext = AosQueryContextObj::createQueryContextStatic();
			mQueryContext->setFinished(true);
			mTotal = 0;
			mDataLoaded = true;
			return true;
		}
	}

	i64 orderTermSeq = adjustTermOrder(rdata);
	aos_assert_r(orderTermSeq>=0,false);

	// 1. get a piece of order term rslt
	
	AosQueryRsltObjPtr cur_rslt = AosQueryRsltObj::getQueryRsltStatic();
	if(!mQueryContext) mQueryContext = AosQueryContextObj::createQueryContextStatic();
	mQueryContext->setPageSize(mPsize);
	
	mQueryContext->setBlockSize(mBlockSize);
	
	mQueryContext->setFinished(false);
	while(!mQueryData || 
		  (!mQueryContext->finished() && mQueryData->getNumDocs() + mRemovedNum < mExpectSize))
	{
		// 1. get a piece of data from ordered term
		mQueryContext->setCheckRslt(false);
		bool rslt = mTerms[orderTermSeq]->getDocidsFromIIL(cur_rslt, 0, mQueryContext, rdata);
		if(!rslt)
		{
			OmnAlarm << "can not get rslt from IIL" << enderr;
			return false;
		}
		mQueryContext->incNumDocChecked(cur_rslt->getNumDocs());	
		mQueryContext->setCheckRslt(true);		
		for(i64 i=0; i<mNumTerms; i++)
		{		
			if(i == orderTermSeq)continue;
			bool rslt = mTerms[i]->getDocidsFromIIL(cur_rslt, 0, mQueryContext, rdata);
			if(!rslt)
			{
				OmnAlarm << "can not get rslt from IIL" << enderr;
				return false;
			}
			if(cur_rslt->isEmpty())
			{
				break;
			}
		}
		
		if(mQueryData)
		{
			if(cur_rslt->isEmpty())
			{
				continue;
			}
			// get some doc in cur_rslt
			mQueryData->mergeFrom(cur_rslt);
		}else
		{
			mQueryData = cur_rslt;
			cur_rslt = AosQueryRsltObj::getQueryRsltStatic();
			//mQueryContext->setFinished(false);
		}
	}

	if(mQueryContext->finished())
	{
		mTotal = mQueryData->getNumDocs();
	}
	else
	{
		//Estimate mTotal
// set Estimate flag
		i64 total_in_rslt = mQueryContext->getTotalDocInRslt();
		if(total_in_rslt == 0 )total_in_rslt = mQueryContext->getTotalDocInIIL();
		i64 checked_in_rslt = mQueryContext->getNumDocChecked();
		if(total_in_rslt <= 0 || checked_in_rslt <=0)
		{
			mTotal = mQueryData->getNumDocs(); 
OmnScreen << "mTotal = mNumDocs:" << mTotal << endl;
		}
		else
		{
OmnScreen << "mTotal = mNumDocs:" << mQueryData->getNumDocs()
          << " * total_in_rslt:" << total_in_rslt
          << " / checked_in_rslt:" << checked_in_rslt  << endl;
			mTotal = mQueryData->getNumDocs() * total_in_rslt / checked_in_rslt;
		}
	}
	
	mDataLoaded = true;
	return true;
}

bool
AosTermAnd::loadData3(const AosRundataPtr &rdata)
{
	// This function loads data from iils.
	countExpectBlockSize();

	// 1. Adjust term order
	if(mNumTerms <= 0)
	{
		mNoMoreDocs = true;
		return true;
	}
	
	if(mQueryContext && mQueryContext->finished())
	{
		return true;
	}
	
	if(!mQueryContext)
	{
		mQueryContext = AosQueryContextObj::createQueryContextStatic();
	}

	if(!mDataLoaded)
	{
		for(i64 i=0; i<mNumTerms; i++)
		{
			if(!mTerms[i]->hasDoc())
			{
				if (!mQueryData) mQueryData = AosQueryRsltObj::getQueryRsltStatic();
			//	if (!mQueryContext) mQueryContext = AosQueryContextObj::createQueryContextStatic();
				mQueryContext->setFinished(true);
				mNoMoreDocs = true;
				mTotal = 0;
				mDataLoaded = true;
				return true;
			}
		}

		i64 orderTermSeq = adjustTermOrder3(rdata);
		aos_assert_r(orderTermSeq == 0, false);
	}

	// 1. get a piece of order term rslt
	AosQueryRsltObjPtr cur_rslt = AosQueryRsltObj::getQueryRsltStatic();
	mQueryContext->setPageSize(mPsize);
	
	mQueryContext->setBlockSize(mBlockSize);		
	mQueryContext->setFinished(false);

//	if(mNumTerms == 1)
//	{
//		// For one term case, set page start, recount the blocksize
//		mQueryContext->setPageStart(mStartIdx);
//	}
	
	bool useBitmap = (mNumTerms > 1);
	if (!useBitmap) mBitmap = 0;
		
	if (!mDataLoaded)
	{
		if(useBitmap)
		{
//			if(mBitmap.isNull())
//			{
				mBitmap = AosBitmapMgrObj::getBitmapStatic();
//			}
		
			// get bitmap
			for(i64 i = 1;i < mNumTerms;i++)
			{
				//bool rslt = mTerms[i]->getDocidsFromIIL(0, mBitmap, mQueryContext, rdata);
				// Ken Lee, 2013/06/09
				AosQueryContextObjPtr context = AosQueryContextObj::createQueryContextStatic();
				//context->setBlockSize(mBlockSize);		// We need to work on the details
				context->setBlockSize(0);

				bool rslt = mTerms[i]->getDocidsFromIIL(0, mBitmap, context, rdata);
				if(!rslt)
				{
					OmnAlarm << "can not get rslt from IIL" << enderr;
					return false;
				}
				if(mBitmap->isEmpty())
				{
					// no result found
					mQueryContext->setFinished(true);
					break;
				}
			}
		}
	}
	
	// get data from sorting iil, no matter it is the first time or not
	while(!mQueryData || (!mQueryContext->finished() && mQueryData->getNumDocs() + mRemovedNum < mExpectSize))
	{
		// 1. get a piece of data from ordered term
		mQueryContext->setCheckRslt(false);
		bool rslt = mTerms[0]->getDocidsFromIIL(cur_rslt, mBitmap, mQueryContext, rdata);
		if(!rslt)
		{
			OmnAlarm << "can not get rslt from IIL" << enderr;
			return false;
		}
		mQueryContext->incNumDocChecked(cur_rslt->getNumDocs());	

		if(mQueryData)
		{
			if(cur_rslt->isEmpty())
			{
				continue;
			}
			// get some doc in cur_rslt
			mQueryData->mergeFrom(cur_rslt);
		}
		else
		{
			mQueryData = cur_rslt;
			cur_rslt = AosQueryRsltObj::getQueryRsltStatic();
			//mQueryContext->setFinished(false);
		}
	}

	if(mQueryContext->finished())
	{
		mTotal = mQueryData->getNumDocs();
	}
	else
	{
		//Estimate mTotal
		// set Estimate flag
		i64 total_in_rslt = mQueryContext->getTotalDocInRslt();
		if (total_in_rslt == 0) total_in_rslt = mQueryContext->getTotalDocInIIL();
		i64 checked_in_rslt = mQueryContext->getNumDocChecked();
		if (total_in_rslt <= 0 || checked_in_rslt <=0)
		{
			mTotal = mQueryData->getNumDocs(); 
		}
		else
		{
			mTotal = mQueryData->getNumDocs() * total_in_rslt / checked_in_rslt;
		}
	}

	mTerms[0]->setQueryData(mQueryData);
	
	mDataLoaded = true;
	return true;
}


bool	
AosTermAnd::queryFinished()
{
	if(!mQueryContext)
	{
		return false;
	}
	return mNoMoreDocs && mQueryContext->finished();
}

bool
AosTermAnd::isCompoundQuery() const
{
	aos_assert_r(mNumTerms > 0, false);
	return mNumTerms > 1;
}


// Chen Ding, 2014/01/29
bool 
AosTermAnd::getTimeCond(
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


// Ketty 2014/02/13
bool
AosTermAnd::getCondTerms(vector<AosQueryTermObjPtr> & conds)
{
	for (int i=0; i<mNumTerms; i++)
	{
		conds.push_back(mTerms[i]);
	}
	return true;	
}

bool
AosTermAnd::addInternalStatIdTerm(
		const AosRundataPtr &rdata,
		const OmnString &iil_name,
		const u32 stat_internal_id)
{
	if (mNumTerms >= eMaxTerms) 
	{
		AosSetError(rdata, AosErrmsgId::eTooManyTerms);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString value;
	value << stat_internal_id;
	AosQueryTermObjPtr term = OmnNew AosTermArith(iil_name, eAosOpr_eq,
			value, false, false, rdata);
	mTerms[mNumTerms++] = term;
	return true;
}

bool
AosTermAnd::removeStatValueTerms(
		const AosRundataPtr &rdata,
		vector<AosQueryTermObjPtr> &value_terms)
{
	bool rslt;
	for(u32 i=0; i<value_terms.size(); i++)
	{
		rslt = removeEachStatValue(rdata, value_terms[i]);	
		aos_assert_r(rslt, false);
	}

	// Temp.
	//for(u32 i=0; i<mNumTerms; i++)
	//{
	//	AosTermArith * term = (AosTermArith *)(mTerms[i].getPtr());
	//	OmnScreen << "!!!!terms; iil_name:" << term->getIILName()
	//		<< endl;
	//}
	return true;
}


bool
AosTermAnd::removeEachStatValue(
		const AosRundataPtr &rdata,
		const AosQueryTermObjPtr &term)
{
	for (int i=0; i<mNumTerms; i++)
	{
		if(mTerms[i] != term)	continue;
		
		if(i < mNumTerms -1)
		{
			mTerms[i] = mTerms[mNumTerms-1];
			mTerms[mNumTerms-1] = 0;
		}
	
		mNumTerms--;
		return true;
	}
	
	OmnShouldNeverComeHere;
	return false;
}


AosQueryTermObjPtr 
AosTermAnd::getJimoTerm(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), def, 1);
	if (!jimo)
	{
		jimo = AosCreateJimo(rdata.getPtr(), def);
		AosSetError(rdata, "termand_failed_create_jimo") << def << enderr;
		return 0;
	}

	//if (!jimo->config(rdata, def, 0)) return 0;

	if (jimo->getJimoType() != AosJimoType::eQueryTerm)
	{
		AosSetError(rdata, "termand_internal_error") << def << enderr;
		return 0;
	}

	AosQueryTermObjPtr term = dynamic_cast<AosQueryTermObj*>(jimo.getPtr());
	aos_assert_rr(term, rdata, 0);
	return term;
}

