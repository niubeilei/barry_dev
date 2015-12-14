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
#ifndef AOS_DataSort_MergeFileReq_h
#define AOS_DataSort_MergeFileReq_h

#include "API/AosApi.h"
#include "DataSort/DataSortReq.h"
#include "Rundata/Ptrs.h"
#include "Sorter/MultiFileSorter.h"
#include "Sorter/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/File.h"
#include "Util/BuffArray.h"

using namespace std;

class AosMergeFileReq : public AosDataSortReq 
{
	OmnDefineRCObject;

	enum
	{
		eMaxBuffSize = 500000000   //1G
	};

	OmnMutexPtr					mLock;
	OmnMutex *					mLockRaw;
	//AosDataSortObjPtr			mCaller;
	AosDataSortObj *	        mCallerRaw;
	AosMultiFileSorterPtr		mMultiSorter;
	AosMultiFileSorter *		mMultiSorterRaw;
	OmnLocalFilePtr				mFile;
	OmnLocalFile *				mFileRaw;
	bool						mMergeFile;
	bool						mMergeFinished;
	vector<u64>					mFileIds;
	vector<AosNetFileObjPtr>	mFiles;
	AosRundataPtr				mRundata;
	AosRundata *				mRundataRaw;
//	OmnThreadPtr 				mThread;

public:
	AosMergeFileReq(
		AosDataSortObj * caller,
		const AosMultiFileSorterPtr &sorter,
		const OmnLocalFilePtr &file,
		const vector<u64> &fileids,
		const vector<AosNetFileObjPtr> &virfiles,
		const bool merge_file,
		const AosRundataPtr &rdata);
	
	~AosMergeFileReq();
	
	bool 	procReq();
	bool	saveToFile();

public:
	class MergeFileThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;
		
		AosDataSortReqPtr	mCaller;

	public:
		MergeFileThrd(AosDataSortReqPtr &caller)
		:
		OmnThrdShellProc("MergeFileReq"),
		mCaller(caller)
		{
		}

		virtual bool run();
		virtual bool procFinished();
	};

	// ThreadedObj Interface
/*	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
	{
		while (state == OmnThrdStatus::eActive)
		{
			mLockRaw->lock();
			AosBuffPtr buff = mMultiSorter->nextBuff();
			AosBuffPtr allbuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
			while(buff && allbuff->dataLen() <= eMaxBuffSize) // 1G
			{
				allbuff->appendBuffData(buff);	
				buff = mMultiSorter->nextBuff();
			}
			
			if (buff)
			{
				allbuff->appendBuffData(buff);	
			}
		
			if (allbuff->dataLen() > 0)
			{
				if (mMergeFile)
				{
					aos_assert_rl(mFile, mLockRaw, false);
					bool rslt = mCaller->sanitycheck(allbuff->data(), allbuff->dataLen()); 
					aos_assert_rl(rslt, mLockRaw, false);
					mFile->append(allbuff->data(), allbuff->dataLen(), true);
					mCaller->updateProcTotal(allbuff->dataLen());
				}
				else
				{
//					mCaller->mergeData(allbuff, mRundata);
//					procTotal += allbuff->dataLen();
//					OmnScreen << " @@@@@@@@ bufflen : " << allbuff->dataLen() << "  procTotal : " << procTotal << endl;
				}
			}
			
			if (mMergeFinished)
			{
				state = OmnThrdStatus::eExit;
				if (mMergeFile)
				{
					mCaller->deleteFiles(mFileIds, mRundata);
				}
				int64_t mergedtotal = mMultiSorter->getMergedTotal();
				aos_assert_rl(mergedtotal >= 0, mLockRaw, false);
				//mCaller->updateProcTotal(mergedtotal);
				mCaller->updateMergedTotal(mergedtotal);
			}
			mLockRaw->unlock();
		}
		return true;
	}
	virtual bool    signal(const int threadLogicId) {return true;}
	virtual void    heartbeat(const int tid) {}
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) {return true;}
*/
};

#endif

