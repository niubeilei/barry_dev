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
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DataScanner_DocReadRunner_h
#define AOS_DataScanner_DocReadRunner_h

#include "DataScanner/DocScanner.h"
#include "Thread/ThrdShellProc.h"
#include "SEInterfaces/DataCacherObj.h"

class AosDocReadRunner : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

	enum
	{
		eMaxThreads = 10,
		eDftNumThreads = 4
	};

private:
	vector<AosBitmapPtr>		mBitmaps;
	AosDataCacherObjPtr 			mCacher;
	AosRundataPtr					mRundata;
	OmnMutexPtr         			mLock;
	OmnCondVarPtr       			mCondVar;
	OmnSemPtr       				mSem;
	OmnThreadPtr					mThreads[eMaxThreads];
	int								mNumThreads;
	int 							mNumReqs;
	vector<bool>					mFinished;

public:
	//constructor
	AosDocReadRunner(const int &num_threads);
	
	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool proc(const vector<AosBitmapPtr> &bitmaps,
			const AosDataCacherObjPtr &cacher,
			const int &num,
			const AosRundataPtr &rdata);

private :

	bool readData(const int index, const AosRundataPtr &rdata);

	AosXmlTagPtr getDocs(
		const AosBitmapPtr &bitmap,
		AosBitmapPtr &result_bitmap,
		const AosRundataPtr &rdata);

	bool diffBitmap(
		const AosBitmapPtr &a,
		const AosBitmapPtr &b,
		const AosRundataPtr &rdata);

	//bool read(const int &index);
};
#endif

#endif
