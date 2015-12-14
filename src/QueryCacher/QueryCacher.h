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
// 2013/12/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryCacher_QueryCacher_h
#define Aos_QueryCacher_QueryCacher_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryCacherObj.h"
#include "SEInterfaces/QueryProcCallback.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/Ptrs.h"
#include "Util/RCObjImp.h"
#include <vector>
using namespace std;

class AosQueryCacher : public AosQueryCacherObj, 
						   public AosQueryProcCallback
{
	OmnDefineRCObject;

	struct Cacher
	{
		i64 	mBlockStart;
		i64		mBlockSize;
		i64		mRawEntryCount;
		i64		mEntryStart;
		i32 	mNumEntries;
		i32		mCursor;
		u32		mHits;
		bool	mIsEOF;
		AosQueryRsltObjPtr	mQueryRslts;

		i32 	getHit() const {return mHits;}
		void 	hit() {mHits++;}
		i64 	getBlockEndPos() const {return mBlockStart + mBlockSize;}
		bool 	isFarFromRequested(const float density, const i64 start_pos);
		bool 	isInCloseRange(const i64 pos, const int delta);
		bool 	mergeCacher(const Cacher &cacher);
		AosQueryRsltObjPtr getNextPage(i64 &entry_start, int &page_size);
		AosQueryRsltObjPtr getOnePage(const i64 start_pos, const int page_size);
		bool tryOnePage( 	i64 &queried_start, 
							int &page_size, 
							AosQueryRsltObjPtr &results);
		bool appendResults(	const AosQueryRsltObjPtr &results, 
							const i64 block_start, 
							const i64 block_size, 
							const bool is_end);
		bool getResults(	AosQueryRsltObjPtr &rslts, 
							const i64 queried_start, 
							const int page_size);
		bool containPartialPage(const i64 start_pos, const int page_size)
		{
			OmnNotImplementedYet;
			return false;
		}
		bool hasEnoughContents(const int page_size) const
		{
			if (!mQueryRslts) return false;
			return mQueryRslts->getNumDocs() - mCursor >= page_size;
		}
		i64 getNextBlockStart() const {return mBlockStart+mRawEntryCount;}
		i64 getNextBlockSize() const {return mBlockSize;}
		i64 getNextBlockSize(const float density, const int page_size)
		{
			aos_assert_r(density > 0, mBlockSize);
			if (mBlockSize * density >= page_size) return mBlockSize;
			return page_size / density;
		}
		bool containPage(const i64 start_pos, const int page_size)
		{
			// It checks whether the cacher contains the page.
			return start_pos >= mEntryStart && start_pos + page_size <= mEntryStart + mNumEntries;
		}
	};

	enum Status
	{
		eInvalid,

		eFetchContents,
		eMergeCachers,
		eCreateDensity
	};

private:
	vector<Cacher>		mCachers;
	AosQueryReqObjPtr	mQueryReq;
	Status			mStatus;
	i64				mRequestedStart;
	i64				mBlockStart;
	i64				mBlockEnd;
	i64				mBlockSize;
	i64				mQueriedStart;
	int				mPageSize;
	float			mDensity;
	u32				mFullHit;
	u32				mFetchGapBlockHit;
	u32				mPartialHit;
	u32				mExtensionHit;
	int				mCrtCacherId;
	AosQueryProcObjPtr	mQueryProc;
	i64				mTotalNumDocsQueried;
	i64				mTotalNumRawEntriesTried;
	i64				mTotal;
	i64				mSortingIILSize;
	AosQueryProcCallbackPtr	mCallback;

	static i64		smInitBlockSize;
	static int		smDefaultPageSize;
	static int		smMaxPageSize;
	static int		smMinCacherGap;
	static int		smCloseEnoughMax;
	static i64		smCreateDensityBlockSize;

public:
	AosQueryCacher();
	~AosQueryCacher();

	virtual bool runQuery(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &query_req, 
					const AosQueryProcCallbackPtr &callback);

	virtual bool queryFinished(
					const AosRundataPtr &rdata, 
					const AosQueryRsltObjPtr &results,
					const AosQueryProcObjPtr &proc);

private:
	bool retrieveNextPage(const AosRundataPtr &rdata);
	bool moveToPage(const AosRundataPtr &rdata);
	bool createDensity(const AosRundataPtr &rdata);
	bool fetchData(const AosRundataPtr &rdata);
	bool createNewCacher(
					const i64 block_start, 
					const i64 block_size, 
					const AosQueryRsltObjPtr &results, 
					const bool is_end);
	bool createNewCacher(const AosRundataPtr &rdata, const i64 block_start);
	bool queryFinishedPriv(const AosRundataPtr &rdata, const AosQueryRsltObjPtr &results);
	bool isDensityGoodEnough();
	int  findClosestCacher(const i64 start_pos);
	bool isCacherCloseEnoughToNext(const int cacher_id, const i64 block_start, const i64 block_size);
	bool insertCacher(const Cacher &cacher);
	bool removeOneCacher();
	bool tooManyCachers();
	AosXmlTagPtr createDefaultQuery(const AosRundataPtr &rdata);
};
#endif

