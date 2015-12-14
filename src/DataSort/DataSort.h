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
#ifndef AOS_DataSort_DataSort_h
#define AOS_DataSort_DataSort_h

#include "API/AosApi.h"
#include "DataTypes/DataColOpr.h"
#include "DataSort/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/DataSortObj.h"
#include "SEInterfaces/Ptrs.h"
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

class AosDataSort : public AosDataSortObj
{
	OmnDefineRCObject;

	enum
	{
		eMaxMergeFile = 10,
		eMaxMergeFileSize = 500000000
	};

private:
	OmnMutexPtr				mLock;
	OmnMutex *				mLockRaw;
	OmnString 				mTableid;
	AosTaskObjPtr			mTask;
	AosTaskObj *			mTaskRaw;
	i64						mFinishedEntries;	
	AosCompareFunPtr		mComp;
	AosCompareFun *			mCompRaw;
	i64						mAddTotal;
	i64						mAppendTotal;
	i64						mMergedTotal;
	i64						mProcTotal;

	AosDataColOpr::E		mOperator;
	i64						mMaxCacheSize;
	i64						mMaxFileSize;
	int						mMinSize;
	int						mMaxSize;
	int						mBuffSize;
	u64 					mJobDocid;
	OmnString				mTaskId;
	bool					mIsFinished;
	AosDataSortCacherPtr	mDataSortCacher;
	AosDataSortCacher *		mDataSortCacherRaw;

	vector<u64>				mFileIds;
	vector<u64>				mNewFileIds;
	OmnString				mIILName;

public:
	AosDataSort(
		const OmnString &datacolid,
		const AosTaskObjPtr &task,
		const AosCompareFunPtr &comp,
		const AosDataColOpr::E opr,
		const i64 maxcachesize, 
		const i64 minsize,
		const i64 maxsize,
		const i64 buffsize);

	~AosDataSort();
	
	bool	addDataSync(const AosBuffArrayPtr &buff_array, AosRundata * rdata);

	AosCompareFunPtr getComp(){return mComp;}
	OmnString createFileName()
	{
		OmnString fname = "mg_iil_";
		fname <<  mJobDocid << "_" << mTaskId << "_" << mTableid << "_";
		return fname;
	}
	
	OmnString createTmpFileName()
	{
		OmnString fname = "tmp_";
		fname <<  mJobDocid << "_" << mTaskId << "_" << mTableid << "_";
		return fname;
	}

	void setIILName(OmnString iilname)
	{
		mIILName = iilname;
	}

	u32 getVirtualId()
	{
		vector<u32> total_vids;
		AosGetLocalVirtualIds(total_vids);
		aos_assert_r(total_vids.size(), 0);

		u32 idx = rand() % (total_vids.size());
		return total_vids[idx];
	}

	OmnString	getTableid() {return mTableid;}
	i64			getTotalEntries() {return mAddTotal;} 
	i64			getAppendTotal() {return mAppendTotal;} 
	u64			getMaxCacheSize() {return mMaxCacheSize;}
	
	bool		reset(const AosRundataPtr &rdata);
	
	void		updateMergedTotal(const i64 total) 
	{
		mMergedTotal += total;
	}

	void		updateProcTotal(const i64 total) 
	{
		mLockRaw->lock();
		mProcTotal += total;
		mLockRaw->unlock();
	}

	void		updateAddTotal(const i64 total)
	{	
		mAddTotal += total;
	}
	
	void		setIsFinished(const bool isFinish)
	{	
		mLockRaw->lock();
		mIsFinished = isFinish;
		mLockRaw->unlock();
	}
	
	bool		finish(
					const u64 totalentries, 
					const AosRundataPtr &rdata);
	bool 		clear();
	
	bool   		saveToFile(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	bool		createSaveFileTask(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata);
	void   		deleteFiles(
					const vector<u64> &fileids,
					const AosRundataPtr &rdata);
	bool		finishCreateFile(const AosRundataPtr &rdata);

	bool		sanitycheck(char *begin, int length);
private:

	void		mergeFileIds();
	bool		createMergeFileTask(const AosRundataPtr &rdata);
	void		getNetFiles(
					vector<AosNetFileObjPtr> &virfiles,
					const vector<u64> &fileids);
	
};
#endif

