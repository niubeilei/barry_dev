////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2015-11-20 Created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_TesterStmInterface_BlobSETesterController_H_
#define Aos_BlobSE_TesterStmInterface_BlobSETesterController_H_
#include "aosUtil/Types.h"
#include "BlobSE/TesterStmInterface/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Thread/Mutex.h"
#include "Util/Ptrs.h"
#include "BlobSE/Ptrs.h"
#include "Rundata/Ptrs.h"

class AosBlobSETesterController : public OmnTestPkg
{
private:
	OmnMutex*	mLock;
	u64			mOprIDLastAppended;
	u64			mOprIDLastApplied;
	u32			mNumOfThreads;
	u32			mNumFinished;

	u64			mReadTestCount;
	u64			mReadFailedCount;
	u64			mCreateTestCount;
	u64			mCreateFailedCount;
	u64			mModifyTestCount;
	u64			mModifyFailedCount;
	u64			mDelTestCount;
	u64			mDelFailedCount;

	AosRundataPtr			mRundata;
	AosBlobSEAPIPtr			mBlobSE;
	vector<OmnThreadPtr>	mThreads;

public:
	AosBlobSETesterController();
	virtual ~AosBlobSETesterController();

	u32 getNumOfThreads() const
	{
		return mNumOfThreads;
	}

	virtual bool		start();	//OmnTestPkg interface

	bool appendEntry(
			const AosBuffPtr	&pBuff,
			u64					&ullOprID);

	bool applyChanges();

	u64 getOprIdLastApplied() const;

	void incNumFinished()
	{
		__sync_add_and_fetch(&mNumFinished, 1);
	}
	void incReadTestCount()
	{
		__sync_add_and_fetch(&mReadTestCount, 1);
	}
	void incReadFailedCount()
	{
		__sync_add_and_fetch(&mReadFailedCount, 1);
	}
	void incCreateTestCount()
	{
		__sync_add_and_fetch(&mCreateTestCount, 1);
	}
	void incCreateFailedCount()
	{
		__sync_add_and_fetch(&mCreateFailedCount, 1);
	}
	void incModifyTestCount()
	{
		__sync_add_and_fetch(&mModifyTestCount, 1);
	}
	void incModifyFailedCount()
	{
		__sync_add_and_fetch(&mModifyFailedCount, 1);
	}
	void incDelTestCount()
	{
		__sync_add_and_fetch(&mDelTestCount, 1);
	}
	void incDelFailedCount()
	{
		__sync_add_and_fetch(&mDelFailedCount, 1);
	}
};

#endif /* Aos_BlobSE_TesterStmInterface_BlobSETesterController_H_ */
