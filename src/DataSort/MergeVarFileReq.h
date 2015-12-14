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

class AosMergeVarFileReq : public AosDataSortReq 
{
	OmnDefineRCObject;

	enum
	{
		eMaxBodyBuffSize = 500000000,	// 500M
		eMaxBuffSize = 100000000   		//100M
	};

	OmnMutexPtr					mLock;
	OmnMutex *					mLockRaw;
	AosDataSortObj *	        mCallerRaw;
	AosCompareFunPtr 			mComp;
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

public:
	AosMergeVarFileReq(
		AosDataSortObj * caller,
		const AosCompareFunPtr &comp,
		const AosMultiFileSorterPtr &sorter,
		const OmnLocalFilePtr &file,
		const vector<u64> &fileids,
		const vector<AosNetFileObjPtr> &virfiles,
		const bool merge_file,
		const AosRundataPtr &rdata);
	
	~AosMergeVarFileReq();
	
	bool 	procReq();
	bool	saveToFile();

	bool	procHeaderBuff(
				const AosBuffPtr &headBuff,
				AosBuffPtr &bodyBuff);

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
};

#endif

