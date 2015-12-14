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
// 2014/09/24 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataSort/MergeVarFileReq.h"

#include "Debug/Debug.h"
#include "NetFile/NetFile.h"
#include "Thread/ThreadPool.h"
#include "Util/BuffArrayVar.h"


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("MergeFileReq", __FILE__, __LINE__);

AosMergeVarFileReq::AosMergeVarFileReq(
		AosDataSortObj * caller,
		const AosCompareFunPtr &comp,
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
mComp(comp),
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

	
AosMergeVarFileReq::~AosMergeVarFileReq()
{
}

	
bool 
AosMergeVarFileReq::procReq()
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
AosMergeVarFileReq::saveToFile()
{
	OmnTagFuncInfo << endl;
	AosBuffPtr buff;
	AosBuffPtr headBuff, bodyBuff;
	while (1)
	{
		mLockRaw->lock();
		headBuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
		buff = mMultiSorterRaw->nextBuff();
		while (buff && headBuff->dataLen() <= eMaxBuffSize) // 100M
		{
			headBuff->appendBuff(buff);	
			buff = mMultiSorterRaw->nextBuff();
		}

		if (buff)
		{
			headBuff->appendBuff(buff);	
		}

		if (headBuff->dataLen() > 0)
		{
			if (mMergeFile)
			{
				aos_assert_rl(mFileRaw, mLockRaw, false);
				bool rslt = mCallerRaw->sanitycheck(headBuff->data(), headBuff->dataLen()); 
				aos_assert_rl(rslt, mLockRaw, false);

				bodyBuff = 0;
				rslt = procHeaderBuff(headBuff, bodyBuff);
				aos_assert_rl(rslt, mLockRaw, false);
				aos_assert_rl(bodyBuff, mLockRaw, false);

				mFileRaw->append(bodyBuff->data(), bodyBuff->dataLen(), true);
				mCallerRaw->updateProcTotal(headBuff->dataLen());
			}
			else
			{
				//	mCallerRaw->mergeData(headBuff, mRundata);
				//	procTotal += headBuff->dataLen();
				//	OmnScreen << " @@@@@@@@ bufflen : " << headBuff->dataLen() << "  procTotal : " << procTotal << endl;
			}
		}

		if (mMergeFinished)
		{
			if (mMergeFile)
			{
				mCallerRaw->deleteFiles(mFileIds, mRundata);
			}
			int64_t mergedtotal = mMultiSorterRaw->getMergedTotal();
			aos_assert_rl(mergedtotal >= 0, mLockRaw, false);

			//mCallerRaw->updateProcTotal(mergedtotal);
			mCallerRaw->updateMergedTotal(mergedtotal);
			mCallerRaw->setIsFinished(true);

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
AosMergeVarFileReq::procHeaderBuff(
		const AosBuffPtr &headBuff,
		AosBuffPtr &bodyBuff)
{
	set<i64> bodyAddrSet;
	bool rslt = AosBuffArrayVar::procHeaderBuff(mComp, headBuff, bodyBuff,bodyAddrSet); 
	aos_assert_r(rslt, false);
	AosIdNetFileVar *file;
	for (u32 i=0; i<mFiles.size(); i++)
	{
		file = dynamic_cast<AosIdNetFileVar*>(mFiles[i].getPtr());
		aos_assert_r(file, false);

		rslt = file->checkBodyAddrs(bodyAddrSet);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosMergeVarFileReq::MergeFileThrd::run()
{
	OmnTagFuncInfo << endl;
	AosMergeVarFileReq * caller = dynamic_cast<AosMergeVarFileReq *>(mCaller.getPtr());
	bool rslt = caller->saveToFile();
	aos_assert_r(rslt, false);
	return rslt;
}


bool
AosMergeVarFileReq::MergeFileThrd::procFinished()
{
	return true;
}

