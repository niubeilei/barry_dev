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
#ifndef AOS_DataSort_DataSortVar_h
#define AOS_DataSort_DataSortVar_h

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

class AosDataSortVar : public AosDataSortObj
{
	OmnDefineRCObject;

	enum
	{
		eMaxMergeFile = 10,
		eMaxBodyBuffSize = 500000000,	// 500M
		eMaxMergeFileSize = 100000000	// 100M
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

	// Chen Ding, 10/25/2012
	AosDataColOpr::E		mOperator;
	i64						mMaxCacheSize;
	i64						mMaxFileSize;
	int						mMinSize;
	int						mMaxSize;
	int						mBuffSize;
	u64 					mJobDocid;
	OmnString				mTaskId;
	int						mIsFinished;
	AosDataSortCacherPtr	mDataSortCacher;
	AosDataSortCacher *		mDataSortCacherRaw;

	vector<u64>				mFileIds;
	vector<u64>				mNewFileIds;
	OmnString				mIILName;

	map<i64, AosBuffArrayVarPtr>	mArray;
	OmnCondVarPtr			mCondVar;
	OmnCondVarPtr			mCondVar1;

public:
	AosDataSortVar(
		const OmnString &datacolid,
		const AosTaskObjPtr &task,
		const AosCompareFunPtr &comp,
		const AosDataColOpr::E opr,
		const i64 maxcachesize, 
		const i64 minsize,
		const i64 maxsize,
		const i64 buffsize);

	~AosDataSortVar();
	
	bool	addDataSync(const AosBuffArrayVarPtr &buff_array, AosRundata * rdata);

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

	//void		setTableid(const OmnString &tableid) {mTableid = tableid;}
	//void		setJobDocId(const u64 job_docid) {mJobDocid = job_docid;}
	OmnString	getTableid() {return mTableid;}
	i64			getTotalEntries() {return mAddTotal;} 
	i64			getAppendTotal() {return mAppendTotal;} 
	u64			getMaxCacheSize() {return mMaxCacheSize;}
	
	bool		reset(const AosRundataPtr &rdata);
	
	void		updateMergedTotal(const i64 total) 
	{
		mLockRaw->lock();
		mMergedTotal += total;
		if (mIsFinished == 1 &&
				mAppendTotal && 
				mAppendTotal + mProcTotal + mMergedTotal == mAddTotal*mCompRaw->size) 
		{
			mCondVar1->signal();
		}
		mLockRaw->unlock();
	}

	void		updateProcTotal(const i64 total) 
	{
		mLockRaw->lock();
		mProcTotal += total;
		if (mIsFinished == 1 &&
				mAppendTotal && 
				mAppendTotal + mProcTotal + mMergedTotal == mAddTotal*mCompRaw->size) 
		{
			mCondVar1->signal();
		}
		mLockRaw->unlock();
	}

	void		updateAddTotal(const i64 total)
	{	
		mAddTotal += total;
	}
	
	void		setIsFinished(const bool isFinish)
	{	
		mLockRaw->lock();
		if (mIsFinished == 1 && mProcTotal + mMergedTotal == mAddTotal*mCompRaw->size) 
		{
			mIsFinished = 2;
			mCondVar->signal();
		}
		mLockRaw->unlock();
	}
	
	bool		finish(
					const u64 totalentries, 
					const AosRundataPtr &rdata);
	bool 		clear();
	
//	bool		reduceCacheSize(const i64 delta, const bool merged);

//	void 		setMemory(const u64 maxmem);
//	bool   		saveToFile(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	bool   		saveToFileInMem(const AosBuffPtr &buff, const AosRundataPtr &rdata);
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
	bool		procHeaderBuff(
					const AosBuffPtr &headBuff,
					AosBuffPtr &bodyBuff);
	
};
#endif

