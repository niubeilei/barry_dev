////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/09/2012 Created by Felicia
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataSort_DataSortCacher_h
#define AOS_DataSort_DataSortCacher_h

#include "API/AosApi.h"
#include "DataTypes/DataColOpr.h"
#include "DataSort/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Sorter/Ptrs.h"
#include "Sorter/SortCaller.h"
#include "Sorter/MultiFileSorter.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArray.h"
#include <list>
#include <vector>

using namespace std;

struct Req : public OmnRCObject
{
	OmnDefineRCObject;
	AosBuffArrayPtr 		mBuff;
	int     				mLevel;
	bool 					mSaveFlag;
	u64 					mSecond;

	public:
	Req(
			const AosBuffArrayPtr &buff, 
			const int level, 
			const bool saveflag)
	:
	mBuff(buff),
	mLevel(level),
	mSaveFlag(saveflag),
	mSecond(OmnGetSecond())
	{
	}

	~Req(){}
};

struct ReqCmp
{
	bool operator () (const ReqPtr lhs, const ReqPtr &rhs) const
	{
		if(lhs->mLevel < rhs->mLevel)
		{
			return true;
		}
		else if (lhs->mLevel == rhs->mLevel)
		{
			return lhs->mSecond > rhs->mSecond;
		}
		else
		{
			return false;
		}
	}
};
	

//class AosDataSortCacher : virtual public OmnThreadedObj, public AosSortCaller
class AosDataSortCacher : public AosSortCaller
{
	OmnDefineRCObject;

private:
	OmnMutexPtr					mLock;
	OmnMutex *					mLockRaw;
	OmnString 					mTableid;
	i64							mFinishedEntries;	
	//AosDataSortObjPtr			mCaller;
	AosDataSortObj *			mCallerRaw;
	//AosCompareFunPtr			mComp;
	AosCompareFun *				mCompRaw;
	i64							mAddTotal;
	i64							mAppendTotal;
	i64							mMergedTotal;

	// Chen Ding, 10/25/2012
	AosDataColOpr::E			mOperator;
	i64							mMaxCacheSize;
	i64							mMaxFileSize;
	i64							mCachedSize;
	bool 						mIsFinish;
	bool 						mTotalFinish;
	multiset<ReqPtr, ReqCmp>	mQueue;		
	bool						mIsError;
	AosRundataPtr 				mRundata;

public:
	AosDataSortCacher(
		AosDataSortObj * datasort,
		const OmnString &datacolid,
		AosCompareFun * comp,
		const AosDataColOpr::E opr,
		const i64 &maxcachesize, 
		const i64 &maxfilesize, 
		AosRundata *rdata);
	~AosDataSortCacher();
	
	// ThreadedObj Interface
//	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
//	virtual bool    signal(const int threadLogicId);
//	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool    	procReq();
	bool 		addDataSync(const AosBuffArrayPtr &buff, AosRundata *rdata);

	//AosCompareFunPtr getComp(){return mComp;}

	bool 		mergeAllData();
	//void		setTableid(const OmnString &tableid) {mTableid = tableid;}
	//OmnString	getTableid() { return mTableid;}
	void		updateAppendTotal(const i64 &total) 
	{
		mLockRaw->lock();
		mAppendTotal += total;
		mLockRaw->unlock();
	}
	
	void		clearAppendTotal() 
	{
		mLockRaw->lock();
		mAppendTotal = 0;
		mLockRaw->unlock();
	}
	
	void		updateMergedTotal(const i64 &total) 
	{
		mMergedTotal += total;
	}

	void		updateAddTotal(const i64 &total)
	{	
		mAddTotal += total;
	}
	
	i64			getTotalEntries() const {return mAddTotal;} 
	i64			getAppendTotal() const {return mAppendTotal;} 
	
	bool		finish(const u64 totalentries);

	bool		mergeBlocks(
					const AosBuffArrayPtr &block1, 
					const AosBuffArrayPtr &block2, 
					const int level, 
					const bool saveflag,
					const AosRundataPtr &rdata);
	
	bool		reduceCacheSize(const i64 delta, const bool merged);
	bool		createSaveTask(
					const AosBuffPtr &buff,
					const int level,
					const AosRundataPtr &rdata);
	u64         getMaxCacheSize(){return mMaxCacheSize;}
	bool		mergeSortFinish(
					const AosBuffPtr &buff,
					const int level,
					void *data,
					const AosRundataPtr &rdata);

	bool		isCacheFull();

private:
	bool		sanitycheck(char *begin, int length);

	bool		saveToFile(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	void		buffSort(const AosBuffArrayPtr &buffarray);
};
#endif

