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
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "DataSort/MergeFileReq.h"
#include "Thread/ThreadPool.h"
#include "Debug/Debug.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("MergeFileReq", __FILE__, __LINE__);

AosMergeFileReq::AosMergeFileReq(
		AosDataSortObj * caller,
		const AosMultiFileSorterPtr &sorter,
		const OmnLocalFilePtr &file,
		const vector<u64> &fileids,
		const vector<AosNetFileObjPtr> &virfiles,
		const bool  merge_file,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mCallerRaw(caller),
mMultiSorter(sorter),
mMultiSorterRaw(sorter.getPtr()),
mFile(file),
mFileRaw(file.getPtr()),
mMergeFile(merge_file),
mMergeFinished(false),
mFileIds(fileids),
mFiles(virfiles),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mRundataRaw(mRundata.getPtr())
{
}

	
AosMergeFileReq::~AosMergeFileReq()
{
}

	
bool 
AosMergeFileReq::procReq()
{
	AosDataSortReqPtr thisptr(this, false);
	OmnThrdShellProcPtr runner = OmnNew MergeFileThrd(thisptr);
	bool rslt = sgThreadPool->proc(runner); 
	aos_assert_r(rslt, false);
	rslt = mMultiSorterRaw->sort(mRundataRaw);
	aos_assert_r(rslt, false);
	mLockRaw->lock();
	mMergeFinished = true;
	mLockRaw->unlock();
	return true;			
}


bool
AosMergeFileReq::saveToFile()
{
	OmnTagFuncInfo << endl;
	AosBuffPtr buff;
	AosBuffPtr allbuff;
	while (1)
	{
		mLockRaw->lock();
		allbuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
		buff = mMultiSorterRaw->nextBuff();
		while (buff && allbuff->dataLen() <= eMaxBuffSize) // 1G
		{
			allbuff->appendBuff(buff);	
			buff = mMultiSorterRaw->nextBuff();
		}

		if (buff)
		{
			allbuff->appendBuff(buff);	
		}

		if (allbuff->dataLen() > 0)
		{
			if (mMergeFile)
			{
				aos_assert_rl(mFileRaw, mLockRaw, false);
				bool rslt = mCallerRaw->sanitycheck(allbuff->data(), allbuff->dataLen()); 
				aos_assert_rl(rslt, mLockRaw, false);

				mFileRaw->append(allbuff->data(), allbuff->dataLen(), true);
				mCallerRaw->updateProcTotal(allbuff->dataLen());
			}
			else
			{
				//	mCallerRaw->mergeData(allbuff, mRundata);
				//	procTotal += allbuff->dataLen();
				//	OmnScreen << " @@@@@@@@ bufflen : " << allbuff->dataLen() << "  procTotal : " << procTotal << endl;
			}
		}

		if (mMergeFinished)
		{
			//state = OmnThrdStatus::eExit;
			if (mMergeFile)
			{
				mCallerRaw->deleteFiles(mFileIds, mRundata);
			}
			int64_t mergedtotal = mMultiSorterRaw->getMergedTotal();
			aos_assert_rl(mergedtotal >= 0, mLockRaw, false);

			//mCallerRaw->updateProcTotal(mergedtotal);
			mCallerRaw->updateMergedTotal(mergedtotal);

			aos_assert_rl(mMultiSorterRaw, mLockRaw, false);
			mMultiSorterRaw->deleteScanner();
			mMultiSorterRaw->clear();
			mMultiSorterRaw = 0;

			mLockRaw->unlock();
			break;
		}
		mLockRaw->unlock();
	}
	return true;
}


bool
AosMergeFileReq::MergeFileThrd::run()
{
	OmnTagFuncInfo << endl;
	AosMergeFileReq * caller = dynamic_cast<AosMergeFileReq *>(mCaller.getPtr());
	bool rslt = caller->saveToFile();
	aos_assert_r(rslt, false);
	return rslt;
}


bool
AosMergeFileReq::MergeFileThrd::procFinished()
{
	return true;
}

