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
#include "DataSort/DataSortCacher.h"

//#include "DataCollector/DataCollectorMgr.h"
//#include "Job/JobMgr.h"
#include "DataSort/ProcQueueReq.h"
#include "DataSort/DataSortMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ActionObj.h"
#include "Sorter/MergeSorter.h"
#include "Sorter/MergeSorterMgr.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Thread/ThreadShellMgr.h"
#include "Util/File.h"
#include "Porting/Sleep.h"

AosDataSortCacher::AosDataSortCacher(
		AosDataSortObj * datasort,
		const OmnString	&datacolid,
		AosCompareFun * comp_raw,
		const AosDataColOpr::E opr,
		const i64 &maxcachesize,
		const i64 &maxfilesize, 
		AosRundata *rdata)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mTableid(datacolid),
mFinishedEntries(0),
mCallerRaw(datasort),
mCompRaw(comp_raw),
mAddTotal(0),
mAppendTotal(0),
mMergedTotal(0),
mOperator(opr),
mMaxCacheSize(maxcachesize),
mMaxFileSize(maxcachesize/3),
mCachedSize(0),
mIsFinish(false),
mTotalFinish(false),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
}


AosDataSortCacher::~AosDataSortCacher()
{
	//OmnScreen << "~~~ AosDataSortCacher Destructor " << endl;
}


bool 
AosDataSortCacher::addDataSync(
		const AosBuffArrayPtr &buff_array,
		AosRundata *rdata)
{
	// When a sender sent some data, receiver will receive a request
	// and it will call this function. 
	// 1. It adds 'buff_array' to its cache and updates its size. 
	// 2. If it is not the first block, check whether there is an outstanding
	//    level 0 block. If yes, it kicks off a thread to merge them.
	// 3. If the cache is not full, return.
	// 4. If the cache is full, create a Task and add the task to the queue.
	aos_assert_r(buff_array, false);

	i64 total = buff_array->size();
	aos_assert_rr(total > 0, rdata, false);
	aos_assert_r(total % mCompRaw->size == 0, false);

	mLockRaw->lock();
	while (isCacheFull())
	{
		mLockRaw->unlock();
		OmnMsSleep(10);
		mLockRaw->lock();
	}
	mCachedSize += total;

	aos_assert_rl(mCachedSize % mCompRaw->size == 0, mLockRaw, false);
	updateAddTotal(total / mCompRaw->size);
//OmnScreen << "+++++ " << buff_array->size() << "  addtotal:" << mAddTotal << "  cachedSize : " << mCachedSize << endl;
	bool saveflag = false;
	if (isCacheFull() || mIsFinish)
	{
		saveflag = true;
	}

	ReqPtr req = OmnNew Req(buff_array, 0, saveflag);
	mQueue.insert(req);
	AosDataSortCacherPtr thisptr(this, false);
	AosDataSortReqPtr procReq = OmnNew AosProcQueueReq(thisptr);
	AosDataSortMgr::getSelf()->addProcQueueReq(procReq);
	mLockRaw->unlock();
	return true;
}


bool 
AosDataSortCacher::sanitycheck(char *begin, int length)
{
//return true;
	for(int i=0; i<length; i++)
	{
	//	aos_assert_r(begin[0] != 0, false);
		if (i>0)
		{
			aos_assert_r(mCompRaw->cmp(begin, begin-mCompRaw->size) >= 0, false);
		}
		begin += mCompRaw->size;
	}
	return true;
} 


bool
AosDataSortCacher::finish(const u64 totalentries)
{
	aos_assert_r(totalentries > 0, false);
	mLockRaw->lock();
	aos_assert_rl(mAddTotal > 0, mLockRaw, false);
	mFinishedEntries = totalentries;	
	mLockRaw->unlock();
	u32 s = OmnGetSecond();
	OmnScreen << "mmmmmmmm : mergerData, s:" << s << ", collectorid : " << mTableid << endl;
	bool rslt = mergeAllData();
	aos_assert_r(rslt, false);
	return rslt;
}


bool
AosDataSortCacher::isCacheFull()
{
	return mCachedSize > mMaxCacheSize;
}


bool
AosDataSortCacher::mergeSortFinish(
		const AosBuffPtr &buff,
		int level,
		void *saveflag,
		const AosRundataPtr &rdata)
{
	// This function is called when two buffs were merged. The new 
	// buff is 'buff'. The merged level is 'level'. This function
	// checks whether there is a pending block for 'level' + 1. 
	// If yes, it creates a new merge request and add the request
	// to the manager.
	i64 bufflen = buff->dataLen();
	aos_assert_r(bufflen%mCompRaw->size == 0, false);

	level++;
	aos_assert_r(level >= 0, false);

	AosBuffArrayPtr buff_array = OmnNew AosBuffArray(buff, mCompRaw, false);
	buff_array->setSorted(true);
	bool rslt = sanitycheck(buff_array->first(), buff_array->getNumEntries());
	aos_assert_r(rslt, false);

	mLockRaw->lock();
	bool flag = saveflag;
	ReqPtr req = OmnNew Req(buff_array, level, flag); 
	mQueue.insert(req);
	AosDataSortCacherPtr thisptr(this, false);
	AosDataSortReqPtr procReq = OmnNew AosProcQueueReq(thisptr);
	AosDataSortMgr::getSelf()->addProcQueueReq(procReq);
	mLockRaw->unlock();
	return true;
}


bool
AosDataSortCacher::mergeBlocks(
		const AosBuffArrayPtr &block1, 
		const AosBuffArrayPtr &block2, 
		const int level, 
		const bool saveflag,
		const AosRundataPtr &rdata)
{
	// This function merges the two blocks into a new one. 
	// This is done through a thread. When finished, it checks
	// whether it needs to be merged with its higher level 
	// outstanding block. If yes, it keeps on merging.
	try
	{
		aos_assert_r(block1->size() % mCompRaw->size == 0, false);
		aos_assert_r(block2->size() % mCompRaw->size == 0, false);
		AosDataSortCacherPtr thisptr(this, false);
		OmnSPtr<AosMergeSorter<AosBuffArrayPtr> > runner = 
			OmnNew AosMergeSorter<AosBuffArrayPtr>(block1, block2, mOperator, mCompRaw, thisptr, level, (void*)saveflag, rdata);
		AosMergeSorterMgr<OmnSPtr<AosMergeSorter<AosBuffArrayPtr> > >::getSelf()->addReq(runner, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating the runner: " << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


bool
AosDataSortCacher::reduceCacheSize(const i64 delta, const bool merged)
{
	aos_assert_r(delta >= 0, false);
	mLockRaw->lock();
	aos_assert_rl(delta % mCompRaw->size == 0, mLockRaw, false);
	aos_assert_rl(mCachedSize % mCompRaw->size == 0, mLockRaw, false);
	aos_assert_rl(mCachedSize >= delta, mLockRaw, false);
	mCachedSize -= delta;
	aos_assert_rl(mCachedSize % mCompRaw->size == 0, mLockRaw, false);
	aos_assert_rl(mCachedSize >= 0, mLockRaw, false);
//OmnScreen << "----- " << delta << "  addtotal : " << mAddTotal << "  cachedSize : " << mCachedSize << endl;
	if (merged)
	{
		updateMergedTotal(delta);
		mCallerRaw->updateMergedTotal(delta);
		mLockRaw->unlock();
		//mCallerRaw->updateProcTotal(delta);
		return true;
	}
	mLockRaw->unlock();
	return true;
}


bool
AosDataSortCacher::mergeAllData()
{
	// All data were received. It is the time to process the data. 
	// 1. If the cache is still being sorted, wait. 
	// 2. Save the cache
	// 3. If no need to process the data, return.
	// 4. Now, there are N files. It reads in one block, sort it, 
	//    and then give it to the proc. 
	// 5. It repeats 4 until all data are processed.
	//
	// Each file is sorted. If the block size is BlockSize and there are
	// N files, it reads BlockSize / N / 4 number of blocks from the first
	// file. Now it has the max value. It then reads in all the data that
	// are no less than the max value from all other files. During reading
	// the remaining files, if the block becomes too big, it will reduce
	// the max value from the file whose current values are the most, and
	// then repeat this process. 
	//
	// If there are multiple values that match the max value, it should not
	// read more than the per file size. 
	//
	// This function uses a number of File Reader Threads. Whenever it needs
	// to read more data from the files, it will give the file to one of the
	// threads. After the thread finish reading the data, it will call back.
	//
	// Once a block is read, it adds the block to a queue. There are a number
	// of Block Process Threads. 
	
	mLockRaw->lock();
	aos_assert_rl(mFinishedEntries > 0, mLockRaw, false);
	aos_assert_rl(mFinishedEntries >= mAddTotal, mLockRaw, false);
	while (mFinishedEntries != mAddTotal)
	{
		mLockRaw->unlock();
		OmnMsSleep(10);
		mLockRaw->lock();
	}

	mIsFinish = true;
	AosDataSortCacherPtr thisptr(this, false);
	AosDataSortReqPtr procReq = OmnNew AosProcQueueReq(thisptr);
	AosDataSortMgr::getSelf()->addProcQueueReq(procReq);
OmnScreen << " mergeAllData mFinishedEntries = " << mFinishedEntries << ", mAddTotal = " << mAddTotal << ", EntrySize = " << mCompRaw->size << endl;
	mLockRaw->unlock();
	
	return true;
}


bool
AosDataSortCacher::saveToFile(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	aos_assert_r(mCallerRaw, false);
	bool rslt = mCallerRaw->saveToFile(buff, rdata);
	aos_assert_r(rslt, false);
//	rslt = sanitycheck(buff->data(), buff->dataLen()/mCompRaw->size);
//	aos_assert_r(rslt, false);
	return rslt;
}


bool    
AosDataSortCacher::procReq()
{
	mLockRaw->lock();
	if (mTotalFinish)
	{
		mLockRaw->unlock();
		return true;
	}
	aos_assert_rl(mRundata, mLockRaw, false);

	if (mQueue.empty())
	{
		if (mIsFinish && mFinishedEntries == mAddTotal && mFinishedEntries == (mAppendTotal+mMergedTotal)/mCompRaw->size)
		{
			aos_assert_rl(mCachedSize == 0, mLockRaw, false);
			aos_assert_rl(mQueue.size() == 0, mLockRaw, false);
			mTotalFinish = true;
			mLockRaw->unlock();
			//to ice
			bool rslt = mCallerRaw->finishCreateFile(mRundata);
			aos_assert_r(rslt, false);
			return true;
		}
		mLockRaw->unlock();
		return true;
	}

	set<ReqPtr>::iterator itr = mQueue.begin();
	int ctnNum = 0;
	for (; itr != mQueue.end();)
	{
		ctnNum++;
		set<ReqPtr>::iterator tmp = itr;
		++itr;
		if ((*tmp)->mSaveFlag == true || mIsFinish)
		{
			// It is the time to save the contents into the buff. 
			if (itr != mQueue.end())
			{
				// It is not the root.
				mergeBlocks((*tmp)->mBuff,(*itr)->mBuff, -1, true, mRundata);
				mQueue.erase(tmp);
				tmp = itr;
				++itr;
				mQueue.erase(tmp);
				continue;
			}
			
			// It is the root. 
			if (((*tmp)->mBuff)->size() > mMaxFileSize && !mIsFinish)
			{
				aos_assert_rl(((*tmp)->mBuff)->size()%mCompRaw->size == 0, mLockRaw, false);
				
				buffSort((*tmp)->mBuff);
				
				if (((*tmp)->mBuff)->size() > mMaxFileSize)
				{
					mCallerRaw->createSaveFileTask(((*tmp)->mBuff)->getBuff(), mRundata);
					mQueue.erase(tmp);
				}
				else
				{
					(*tmp)->mSaveFlag = false;
				}
				continue;
			}
					
			// It is not big enough to save to file. Need to check whether
			// it finished. If it is not finished, it means the cache is
			// no longer full. Do nothing.
			if (mIsFinish && (*tmp)->mBuff->size() == mCachedSize)
			{
				// The stream finished. 
				// The stream finished and there are no other buffs. 
				// (or in other word, the queue should contain only
				// this root buff).
				aos_assert_rl(mQueue.size() == 1, mLockRaw, false);
				aos_assert_rl(((*tmp)->mBuff)->size()%mCompRaw->size == 0, mLockRaw, false);
				
				buffSort((*tmp)->mBuff);
				
				AosBuffPtr tmpbuff = ((*tmp)->mBuff)->getBuff();

				mQueue.erase(tmp);
				mLockRaw->unlock();

				saveToFile(tmpbuff, mRundata);

				mLockRaw->lock();
				aos_assert_rl(mCachedSize == 0, mLockRaw, false);
				aos_assert_rl(mQueue.size() == 0, mLockRaw, false);
				aos_assert_rl(mAppendTotal+mMergedTotal == mAddTotal*mCompRaw->size, mLockRaw, false);
				mTotalFinish = true;
				mLockRaw->unlock();

				//to ice
				bool rslt = mCallerRaw->finishCreateFile(mRundata);
				aos_assert_r(rslt, false);

				return true;
			}

			continue;
		}
			
		// The save flag is not set. Check whether the current buff
		// is the root. If it is the root, check whether it is bigger
		// than the file max size. If yes, it should save. 
		if (itr == mQueue.end() && ((*tmp)->mBuff)->size() > mMaxFileSize)
		{
			// It is the root. 
			// It is the time to save to file. Saving to file is handled
			// by a thread. When that task finishes, it will reduce the 
			// cache size.

			aos_assert_rl(((*tmp)->mBuff)->size()%mCompRaw->size == 0, mLockRaw, false);
			
			buffSort((*tmp)->mBuff);

			if (((*tmp)->mBuff)->size() > mMaxFileSize)
			{
				mCallerRaw->createSaveFileTask(((*tmp)->mBuff)->getBuff(), mRundata);
				mQueue.erase(tmp);
			}
		}
		
		if (itr != mQueue.end() && (*itr)->mLevel == (*tmp)->mLevel)
		{
			// These are two buffs with the same level. It needs to merge these 
			// two buffs.
			mergeBlocks((*tmp)->mBuff, (*itr)->mBuff, (*tmp)->mLevel, (*tmp)->mSaveFlag | (*itr)->mSaveFlag, mRundata);
			mQueue.erase(tmp);
			tmp = itr;
			++itr;
			mQueue.erase(tmp);
		}
	}
	mLockRaw->unlock();
	return true;
}

void
AosDataSortCacher::buffSort(const AosBuffArrayPtr &buffarray)
{
	if (!buffarray->isSorted())
	{
		i64 bsize = buffarray->size();
		buffarray->sort(mOperator);
		i64 asize = buffarray->size();
		i64 mergedSize = bsize-asize;
		if (mergedSize>0)
		{
			mCachedSize -= mergedSize;
			updateMergedTotal(mergedSize);
			mCallerRaw->updateMergedTotal(mergedSize);
		}
	}
	bool rslt = sanitycheck(buffarray->first(), buffarray->getNumEntries());
	aos_assert(rslt);
}

/*
bool    
AosDataSortCacher::signal(const int threadLogicId)
{
	return true;
}


bool    
AosDataSortCacher::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}
*/
