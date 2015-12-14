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
// 1. This class maintaines the fixed numbers of buckets to cache iiltrans 
// that is a sorted  array of AosIILEntry. 
// 2. Every bucket has boundary of low and hight, the entries will append
// to the matched bucket by the range. 
// 3. The bucket can automaticly ajust range.  The mechanism of mergering 
// and spliting bucket by the aging and total entries  of the bucket. 
// If the bucket is too old. it will be merged to it's left bucket or the
// right bucket, and the bucket will free. If the bucket has full, 
// the entries will be proccesed, and the bucket will free. when the entrie 
// has not in all the buckets range .it will get the  free buckets 
// to split the range.
// 
// Modification History:
// 04/09/2012 Created by Felicia
//
////////////////////////////////////////////////////////////////////////////
#include "DataSort/DataSort.h"

#include "DataSort/DataSortCacher.h"
#include "DataSort/DataSortMgr.h"
#include "DataSort/MergeFileReq.h"
#include "DataSort/SaveFileReq.h"
#include "DataCollector/DataCollectorMgr.h"
#include "Job/JobMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ActionObj.h"
#include "Sorter/MergeSorter.h"
#include "Sorter/MergeSorterMgr.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Thread/ThreadShellMgr.h"
#include "Util/File.h"
#include "NetFile/NetFile.h"
#include "Porting/Sleep.h"
#include "Job/Job.h"

AosDataSort::AosDataSort(
		const OmnString &datacolid,
		const AosTaskObjPtr &task,
		const AosCompareFunPtr &comp,
		const AosDataColOpr::E opr,
		const i64 maxcachesize,
		const i64 minsize, 
		const i64 maxsize,
		const i64 buffsize)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mTableid(datacolid),
mTask(task),
mTaskRaw(task.getPtr()),
mFinishedEntries(0),
mComp(comp),
mCompRaw(comp.getPtr()),
mAddTotal(0),
mAppendTotal(0),
mMergedTotal(0),
mProcTotal(0),
mOperator(opr),
mMaxCacheSize(maxcachesize),
mMaxFileSize(maxcachesize/3),
mMinSize(minsize),
mMaxSize(maxsize),
mBuffSize(buffsize),
mJobDocid(mTaskRaw->getJobDocid()),
mTaskId(mTaskRaw->getTaskId()),
mIsFinished(false),
mDataSortCacherRaw(0)
{
}


AosDataSort::~AosDataSort()
{
	//OmnScreen << "~~~ AosDataSort Destructor " << endl;
}


bool 
AosDataSort::addDataSync(const AosBuffArrayPtr &buff_array, AosRundata *rdata)
{
	// When a sender sent some data, receiver will receive a request
	// and it will call this function. 
	// 1. It adds 'buff_arry' to its cache and updates its size. 
	// 2. If it is not the first block, check whether there is an outstanding
	//    level 0 block. If yes, it kicks off a thread to merge them.
	// 3. If the cache is not full, return.
	// 4. If the cache is full, create a Task and add the task to the queue.
	
	aos_assert_r(buff_array, false);

	i64 num = buff_array->getNumEntries();
	aos_assert_r(num > 0, false);

	mLockRaw->lock();
	if (!mDataSortCacher)
	{
		mDataSortCacher = OmnNew AosDataSortCacher(
			this, mTableid, mCompRaw, mOperator, mMaxCacheSize, mMaxFileSize, rdata);
		mDataSortCacherRaw = mDataSortCacher.getPtr();
	}
	aos_assert_rl(mDataSortCacherRaw, mLockRaw, false);

	while (mDataSortCacherRaw->isCacheFull())
	{
		mLockRaw->unlock();
		OmnMsSleep(10);
		mLockRaw->lock();
	}

	bool rslt = mDataSortCacherRaw->addDataSync(buff_array, rdata);
	aos_assert_rl(rslt, mLockRaw, false);
	updateAddTotal(num);
	mLockRaw->unlock();
	return true;
}


bool 
AosDataSort::sanitycheck(char *begin, int length)
{
//return true;
	aos_assert_r(length%mCompRaw->size == 0, false);
	int size = length/mCompRaw->size;
	for(int i=0; i<size; i++)
	{
//		aos_assert_r(begin[0] != 0, false);
		if (i>0)
		{
			aos_assert_r(mCompRaw->cmp(begin, begin-mCompRaw->size) >= 0, false);
		}
		begin += mCompRaw->size;
	}
	return true;
}


bool
AosDataSort::finish(
		const u64 totalentries,
		const AosRundataPtr &rdata)
{
	mLockRaw->lock();
	if (!mDataSortCacherRaw)
	{
		aos_assert_rl(totalentries == 0, mLockRaw, false);
		aos_assert_rl(mAddTotal == 0, mLockRaw, false);
		mLockRaw->unlock();
		return true;
	}
	mFinishedEntries = totalentries;	
	aos_assert_rl(mDataSortCacherRaw, mLockRaw, false);
	mLockRaw->unlock();
	bool rslt = mDataSortCacherRaw->finish(totalentries);
	aos_assert_r(rslt, false);

	while(1)
	{
		mLockRaw->lock();
		if (mIsFinished)
		{
			mLockRaw->unlock();
			break;
		}
		mLockRaw->unlock();
		rslt = mDataSortCacherRaw->procReq();
		aos_assert_r(rslt, false);
		//OmnSleep(1);
		OmnMsSleep(10);
	}

	return true;
}


bool 
AosDataSort::clear()
{
	OmnTagFuncInfo << endl;

	mLockRaw->lock();
	mAddTotal = 0;
	mAppendTotal = 0;
	mLockRaw->unlock();
	return true;
}

bool
AosDataSort::createSaveFileTask(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	// It is the time to save the cached. 'buff' is a newly created
	// buff. It needs to check whether it is the root (by 'leve'). 
	// If it is not the root, it needs to merge all the way to the
	// root.

	AosDataSortObjPtr thisptr(this, false);
	AosDataSortReqPtr req = OmnNew AosSaveFileReq(thisptr, buff, rdata);
	AosDataSortMgr::getSelf()->addSaveFileReq(req);
	return true;
}


bool
AosDataSort::finishCreateFile(const AosRundataPtr &rdata)
{
	mLockRaw->lock();
	int sleep_sec = 0;
	while(1)
	{
		if (mAppendTotal + mProcTotal+ mMergedTotal == mAddTotal*mCompRaw->size) break;
		if (sleep_sec >= 300000)
		{
			OmnScreen << "************ mergeFile error  " << mTableid << "  mAppendTOtal : " << mAppendTotal << " mProcTotal : " << mProcTotal << endl;
			sleep_sec = 0;
		}
		
		sleep_sec += 1;
		mLockRaw->unlock();
		//OmnSleep(1);
		OmnMsSleep(10);
		mLockRaw->lock();
	}
	
	bool rslt = createMergeFileTask(rdata);
	aos_assert_rl(rslt, mLockRaw, false);

	aos_assert_rl(mFileIds.size() == 0, mLockRaw, false);
	aos_assert_rl(mAppendTotal == 0, mLockRaw, false);
	mergeFileIds();
	
	OmnScreen << mTableid << " Sort  success !!! " << endl;
	
	mIsFinished = true;
	mLockRaw->unlock();
	return true;
}


bool
AosDataSort::createMergeFileTask(const AosRundataPtr &rdata)
{
	if (mFileIds.size() == 0) return true;

	u64 file_id;
	OmnLocalFilePtr newfile;

	OmnTagFuncInfo << "Datasort merge file taskdata for task: " 
		<< mTask->getTaskDocid() << endl;
	AosTaskDataObjPtr task_data =  mTaskRaw->getOutPut(
		AosTaskDataType::eOutPutIIL, mTableid, rdata);

	if (!task_data)
	{
		OmnString fname = createFileName();
		aos_assert_r(fname != "", false);

		newfile = AosNetFileCltObj::createRaidFileStatic(
			rdata.getPtr(), file_id, fname, 0, false);
		aos_assert_r(newfile && newfile->isGood(), false);
			
		task_data = AosTaskDataObj::createTaskDataOutPutIIL(
			AosGetSelfServerId(), mTableid, file_id, 0);
		aos_assert_r(task_data, false);

		bool rslt = mTaskRaw->updateTaskOutPut(task_data, rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
OmnScreen << "DataSort ========================== get output from task doc ============" << endl;
		AosXmlTagPtr output;
		bool rslt = task_data->serializeTo(output, rdata);
		aos_assert_r(rslt && output, false);

		file_id = output->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
		newfile = AosNetFileCltObj::openLocalFileStatic(file_id, rdata.getPtr());
		aos_assert_r(newfile && newfile->isGood(), false);

		newfile->resetFile();
	}

	vector<u64> fileids = mFileIds;
	mFileIds.clear();
	mNewFileIds.push_back(file_id);
	
	mAppendTotal = 0;
	vector<AosNetFileObjPtr> virfiles;
	getNetFiles(virfiles, fileids);
	aos_assert_r(virfiles.size() > 0, false);

	AosMultiFileSorterPtr sorter = OmnNew AosMultiFileSorter(
		mMinSize, mMaxSize, mBuffSize, mOperator, mCompRaw, virfiles, rdata);

	AosDataSortReqPtr req = OmnNew AosMergeFileReq(
		this, sorter, newfile, fileids, virfiles, true, rdata);
	AosDataSortMgr::getSelf()->addMergeFileReq(req);

	return true;
}

void
AosDataSort::getNetFiles(
		vector<AosNetFileObjPtr> &virfiles,
		const vector<u64> &fileids)
{
	int serverid = AosGetSelfServerId();
	for(u32 i=0; i<fileids.size(); i++)
	{
		AosNetFileObjPtr file = OmnNew AosIdNetFile(serverid, fileids[i]);
		virfiles.push_back(file);
	}
}

void
AosDataSort::mergeFileIds()
{
	int sleep_sec = 0;
	while(1)
	{
		if (mProcTotal+ mMergedTotal == mAddTotal*mCompRaw->size) break;
		if (sleep_sec >= 300)
		{
			OmnScreen << "************ mergeFile error  " << mTableid << "  mAppendTOtal : " << mAppendTotal << " mProcTotal : " << mProcTotal << endl;
			sleep_sec = 0;
		}
		
		sleep_sec += 1;
		mLockRaw->unlock();
		//OmnSleep(1);
		OmnMsSleep(10);
		mLockRaw->lock();
	}
}


bool
AosDataSort::saveToFile(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	aos_assert_rr(buff && buff->dataLen() > 0, rdata, false);
	
	OmnString fname = createTmpFileName();
	aos_assert_rr(fname != "", rdata, false);
	
	u64 file_id = 0;
	OmnFilePtr file = AosNetFileCltObj::createTempFileStatic(
		rdata.getPtr(), file_id, fname, 0, false);
	aos_assert_r(file && file->isGood(), false);
	
	bool rslt = file->append(buff->data(), buff->dataLen(), true);
	aos_assert_r(rslt, false);

	mLockRaw->lock();
	rslt = sanitycheck(buff->data(), buff->dataLen());
	aos_assert_r(rslt, false);
	mFileIds.push_back(file_id);

	mAppendTotal += buff->dataLen();

	if (mAppendTotal >= eMaxMergeFileSize)
	{
		bool rslt = createMergeFileTask(rdata);
		aos_assert_rl(rslt, mLockRaw, false);
		mLockRaw->unlock();
		mDataSortCacherRaw->reduceCacheSize(buff->dataLen(), false);
		mDataSortCacherRaw->updateAppendTotal(buff->dataLen());
		return rslt;
	}
	mLockRaw->unlock();
	mDataSortCacherRaw->reduceCacheSize(buff->dataLen(), false);
	mDataSortCacherRaw->updateAppendTotal(buff->dataLen());
	return true;
}

void
AosDataSort::deleteFiles(
		const vector<u64> &fileids,
		const AosRundataPtr &rdata)
{
	bool svr_death;
	int self_svrid = AosGetSelfServerId(); 
	for (u64 i=0; i<fileids.size(); i++)
	{
		AosNetFileCltObj::deleteFileStatic(
			fileids[i], self_svrid, svr_death, rdata.getPtr());
	}
}

bool		
AosDataSort::reset(const AosRundataPtr &rdata)
{
	clear();
	mDataSortCacher = 0;
	return true;
}

