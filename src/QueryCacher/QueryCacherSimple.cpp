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
// mQueriedStart:	This is the entry start position to fetch
// mPageSize:		This is the size of the page to fetch
// mBlockStart:		This is the block start position
// mBlockSize:		This is the block size
// mEntryCursor:	This is the next entry position. If no start position
// 					is specified, this serves as the next page start position.
// mDensity:		This is the current density. -1 means no density yet.
//
// Modification History:
// 2013/12/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryCacher/QueryCacherSimple.h"

#include "API/AosApi.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryProcObj.h"
#include "SEInterfaces/QueryReqObj.h"


extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosQueryCacherObj_0(const AosRundataPtr &rdata, const u32 &version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosQueryCacherSimple(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


// AosGetConfig(rdata, AOSCONFIG_QUERY_DEFAULT_PAGE_SIZE, 20);
static AosXmlTagPtr sgDefaultQuery;

AosQueryCacherSimple::AosQueryCacherSimple(const u32 &version)
:
AosQueryCacherObj(),
AosJimo(AosJimoType::eQueryCacher,version)
{
}


AosQueryCacherSimple::~AosQueryCacherSimple()
{
}


bool 
AosQueryCacherSimple::runQuery(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &query_req, 
		const AosQueryProcCallbackPtr &callback)
{
	// This is the start point to query data. 
	aos_assert_rr(query_req, rdata, false);
	aos_assert_rr(callback, rdata, false);
	mCallback = callback;
	mQueryReq = query_req;

	mQueryProc = query_req->getQueryProc();
	if (!mQueryProc)
	{
		AosJimoPtr jimo;

		AosXmlTagPtr orig_req = query_req->getOrigReq();
		if (orig_req)
		{
			jimo = AosCreateJimo(rdata, orig_req);
		}

		if (!jimo)
		{
			if (!sgDefaultQuery)
			{
				createDefaultQuery(rdata);
			}
			aos_assert_rr(sgDefaultQuery, rdata, false);
			jimo = AosCreateJimo(rdata, 0, sgDefaultQuery, 0);
		}

		if (!jimo)
		{
			// jimo = AosQueryProcObj::getObject();
			// aos_assert_rr(jimo, rdata, false);
			// jimo = jimo->cloneJimo();
			AosSetErrorUser(rdata, "failed_creating_jimo") << query_req->toString() << enderr;
			return false;
		}

		if (jimo->getJimoType() != AosJimoType::eQueryCacher)
		{
			AosSetErrorUser(rdata, "incorrect_jimo") << query_req->toString() << enderr;
			return false;
		}

		AosQueryProcObjPtr thisPtr(this,false);
		query_req->setQueryProc(thisPtr);
	}
	
	mQueriedStart = query_req->getStartIdx();
	mPageSize = query_req->getPageSize();
	if (mPageSize < 0) mPageSize = smDefaultPageSize;
	if (mPageSize > smMaxPageSize) mPageSize = smMaxPageSize;

	// Query starts in one of the three scenarios:
	// 1. No start position specified: retrieve the next page
	// 2. Some data being cached: move to a page
	// 3. No cached data and wants to go to a specific location: create the density first
	if (mQueriedStart < 0) return retrieveNextPage(rdata);
	if (mCacher.mIsValid) return moveToPage(rdata);
	return createDensity(rdata);
}


bool
AosQueryCacherSimple::retrieveNextPage(const AosRundataPtr &rdata)
{
	if (!mCacher.mIsValid)
	{
		// This is to retrieve the first page
		mQueriedStart = 0;
		mBlockStart = 0;
		mBlockSize = smInitBlockSize;
		mBlockEnd = -1;
		mStatus = eFetchContents;
		return fetchData(rdata);
	}

	// Some contents were cached. 
	mCacher.hit();
	AosQueryRsltObjPtr rslts;
	if (mCacher.tryOnePage(mQueriedStart, mPageSize, rslts))
	{
		mFullHit++;
		return queryFinishedPriv(rdata, rslts);
	}
	
	// The cacher does not have enough contents (or have any content at all).
	// Need to fetch.
	mBlockStart = mCacher.getNextBlockStart();
	mBlockSize = mCacher.getNextBlockSize();
	mPartialHit++;
	mBlockEnd = -1;
	mStatus = eFetchContents;
	return fetchData(rdata);
}


bool
AosQueryCacherSimple::moveToPage(const AosRundataPtr &rdata)
{
	// The density is good enough. The following information is available:
	// mControllingIILSize:		This is the number of raw entries in the
	// 							controlling IIL.
	// mDensity:				The density of the controlling IIL.
	// mCachers:				A number of cachers (possibly zero).
	// mPageSize:
	// 
	// 1. Only one cacher
	//    The cacher must start with Raw Entry 0. It checks whether the cacher
	//    covers any requested contents. 
	AosQueryRsltObjPtr rslts;
	bool rslt = mCacher.tryOnePage(mQueriedStart, mPageSize, rslts);
	aos_assert_r(rslt, false);

	// The requested is available in the cahcer.
	if (rslts)
	{
		mFullHit++;
		mCacher.hit();
		return queryFinishedPriv(rdata, rslts);
	}

	if (mCacher.containPartialPage(mQueriedStart, mPageSize))
	{
		// It needs to retrieve more contents
		mBlockStart = mCacher.getNextBlockStart();
		mBlockSize = mCacher.getNextBlockSize(mDensity, mPageSize);
		mBlockEnd = -1;
		mStatus = eFetchContents;
		mPartialHit++;
		mCacher.hit();
		return fetchData(rdata);
	}

	// The requested is not in the cache at all. Check whether it is
	// far enough from the current cacher.
	if (mCacher.isFarFromRequested(mDensity, mQueriedStart))
	{
		// It is far from the current cacher. Need to create a new cacher.
		return createNewCacher(rdata, mQueriedStart/mDensity);
	}

	// It is not far from the current cacher. Need to retrieve more:
	mBlockStart = mCacher.getNextBlockStart();
	mBlockSize = mCacher.getNextBlockSize(mDensity, mPageSize);
	mBlockEnd = -1;
	mStatus = eFetchContents;
	mExtensionHit++;
	mCacher.hit();
	return fetchData(rdata);
}


bool
AosQueryCacherSimple::createDensity(const AosRundataPtr &rdata)
{
	// This is similar to fetch the first page. 
	mBlockStart = 0;
	mBlockSize = smCreateDensityBlockSize;
	mBlockEnd = -1;
	mStatus = eCreateDensity;
	createNewCacher(rdata, 0);
	return fetchData(rdata);
}


bool
AosQueryCacherSimple::fetchData(const AosRundataPtr &rdata)
{
	AosQueryProcCallbackPtr thisptr(this, false);

 
//	mod by shawn : 1. QueryProc will call getStartIdx & getPageSize, 2. setBlockEnd is no use now
//	mQueryProc->setBlockStart(mBlockStart);
//	mQueryProc->setBlockSize(mBlockSize);
//	mQueryProc->setBlockEnd(mBlockEnd);

	// here we need to send this ptr to proc
	return mQueryProc->runQuery(rdata, mQueryReq, thisptr);
}


bool
AosQueryCacherSimple::createNewCacher(const AosRundataPtr &rdata, const i64 block_start)
{
	// It wants to move to position 'mQueriedStart'. The density
	// is available. 
	mBlockStart = block_start; 
	mBlockSize = smInitBlockSize;
	mBlockEnd = -1;
	mStatus = eFetchContents;
	mCacher.reset();
	return fetchData(rdata);
}


bool
AosQueryCacherSimple::queryFinishedPriv(
		const AosRundataPtr &rdata, 
		const AosQueryRsltObjPtr &results)
{
	
/*	OmnString records;
	bool rslt = mQueryReq->generateResults(rdata, results, records);
	if (!rslt)
	{
		return mCallback->queryFailed(rdata, rdata->getErrmsg());
	}
	return mCallback->queryFinished(rdata, records);
*/
	// mod by shawn : do we need to return thisptr here?
	//return mCallback->queryFinished(rdata, results, thisPtr);
	return mCallback->queryFinished(rdata, results, 0);
}


bool 
AosQueryCacherSimple::queryFinished(
		const AosRundataPtr &rdata, 
		const AosQueryRsltObjPtr &results,
		const AosQueryProcObjPtr &proc)
{
	i64 matched_size = proc->getMatchedSize();
	i64 block_start = proc->getBlockStart();
	i64 block_size = proc->getQueryCursor();
	bool is_end = proc->finished();

	// Some results are obtained. 
	i64 num_docs = results->getNumDocs();
	aos_assert_r(num_docs >= 0, false);
	if (num_docs == 0)
	{
		// No contents were found. 
		return queryFinishedPriv(rdata, 0);
	}

	mTotalNumDocsQueried += num_docs;
	mTotalNumRawEntriesTried += block_size;
	mDensity = mTotalNumDocsQueried * 1.0 / mTotalNumRawEntriesTried;
	if (mSortingIILSize < 0) mSortingIILSize = matched_size;
	mTotal = mSortingIILSize * mDensity;

	AosQueryRsltObjPtr rslts;
	switch (mStatus)
	{
	case eFetchContents:
		 mCacher.appendResults(results, block_start, block_size, is_end);
		 if (!mCacher.getResults(rslts, mQueriedStart, mPageSize)) 
		 {
			 return queryFailed(rdata, "internal_error");
		 }
		 return queryFinishedPriv(rdata, rslts);

	case eCreateDensity:
		 mCacher.appendResults(results, block_start, block_size, is_end);
		 return moveToPage(rdata.getPtr());

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


AosXmlTagPtr 
AosQueryCacherSimple::createDefaultQuery(const AosRundataPtr &rdata)
{
	OmnString jimostr = "<jimo ";
	jimostr << "current_version=\"version_0\" "
		<< "zky_classname=\"AosBitmapQueryProc\" "
		<< "zky_objid=\"bitmap_query_jimo\">"
		<< "<versions>"
		<< 		"<version_0>libQueryProc.so</version_0>"
		<< "</versions>"
		<< "</jimo>";

	sgDefaultQuery = AosStr2Xml(rdata, jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgDefaultQuery, 0);
	return sgDefaultQuery;
}


bool
AosQueryCacherSimple::Cacher::tryOnePage(
		i64 &queried_start, 
		int &page_size, 
		AosQueryRsltObjPtr &results)
{
	// 'entry_start' is the start position for the results to retrieve. 
	if (queried_start < 0) queried_start = mCursor;
	aos_assert_r(queried_start >= mEntryStart, false);
	if (queried_start - mEntryStart + page_size + 1 > mNumEntries) 
	{
		if (mIsEOF)
		{
			mCursor = mNumEntries;
			if (queried_start >= mNumEntries)
			{
				results = 0;
				return true;
			}
			results = mQueryRslts->getResults(queried_start - mEntryStart, page_size);
			return true;
		}
		results = 0;
		return true;
	}

	mCursor += page_size;
	results = mQueryRslts->getResults(queried_start - mEntryStart, page_size);
	aos_assert_r(results->getNumDocs() == page_size, false);
	return true;
}


bool
AosQueryCacherSimple::queryFailed(
		const AosRundataPtr &rdata, 
		const OmnString &errmsg)
{
	mCallback->queryFailed(rdata, errmsg);
	return true;
}


AosJimoPtr
AosQueryCacherSimple::cloneJimo() const
{
	return OmnNew AosQueryCacherSimple(*this);
}

u64 
AosQueryCacherSimple::getQueryId() const
{
	aos_assert_r(mQueryProc,0);
	return mQueryProc->getQueryId();
}

u64 
AosQueryCacherSimple::getMatchedSize()const
{
	aos_assert_r(mQueryProc,0);
	return mQueryProc->getMatchedSize();
}

i64 
AosQueryCacherSimple::getBlockStart()const
{
	aos_assert_r(mQueryProc,0);
	return mQueryProc->getBlockStart();
}

i64 
AosQueryCacherSimple::getQueryCursor()const
{
	aos_assert_r(mQueryProc,0);
	return mQueryProc->getQueryCursor();
}

u64 
AosQueryCacherSimple::getTotalInRslt()const
{
	aos_assert_r(mQueryProc,0);
	return mQueryProc->getTotalInRslt();
}

bool 
AosQueryCacherSimple::finished()const
{
	aos_assert_r(mQueryProc,true);
	return mQueryProc->finished();
}

bool 
AosQueryCacherSimple::queryFinished(
		const AosRundataPtr &rdata, 
		const AosQueryRsltObjPtr &result, 
		const AosBitmapQueryTermObjPtr &term)
{
	OmnShouldNeverComeHere;
	return false;
}

int 
AosQueryCacherSimple::getPageSize() const
{
	OmnShouldNeverComeHere;
	return 0;
}

u64 
AosQueryCacherSimple::getStartIdx() const
{
	OmnShouldNeverComeHere;
	return 0;
}

int 
AosQueryCacherSimple::getInitBlockSize()
{
	OmnShouldNeverComeHere;
	return 0;
}

int 
AosQueryCacherSimple::nextBlockSize(int a)
{
	OmnShouldNeverComeHere;
	return 0;
}
