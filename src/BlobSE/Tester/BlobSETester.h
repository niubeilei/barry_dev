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
//   
//
// Modification History:
// 2015-1-8 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_Tester_BlobSETester_h
#define Aos_BlobSE_Tester_BlobSETester_h

#include "BlobSE/Ptrs.h"
#include "BlobSE/Tester/Ptrs.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDocHeader.h"
#include "UtilData/DfmDocType.h"
#include "DfmUtil/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "Rundata/Rundata.h"
#include <ext/hash_map>
#include <map>
#include <vector>

class AosBlobSETester : public OmnTestPkg, 
						public OmnThreadedObj
{
public:
	enum
	{
		eDefaultTries = 100,
		eMaxBuffSize = 500000000,   //500M
		eMaxThreads = 100
	};
	typedef std::pair<u32, u32> p_seed_repeat_t;
	typedef __gnu_cxx::hash_map<const u64, p_seed_repeat_t, std::u64_hash, std::u64_cmp> map_t;
	typedef map_t::iterator itr_t;

private:
	int									mTries;
	int									mBatchSize;
	int									mTestCount;
	int									mCreateWeight;
	int									mDeleteWeight;
	int									mModifyWeight;
	int									mReadWeight;
	int									mReadInvalidWeight;
	int									mReadValidWeight;
	int									mReadDeletedWeight;
	bool								mReturnOnFail;
	bool								mJustCheckWhatLeft;
	u32									mNumOfThreads;
	u32									mNumFinished;
	u64									muFileCreated;
	u64									mDocidAllocateSize;
	i64									mTestDurationSec;
	u32									mSiteID;
	u32									mCubeID;
	u64									mAseID;
	u64									mLastAllocatedDocID;
	u64									mReadTestCount;
	u64									mCreateTestCount;
	u64									mDelTestCount;
	u64									mModifyTestCount;
	u64									mCreateSnapshotTestCount;
	u64									mCommitSnapshotTestCount;
	u64									mRollbackSnapshotTestCount;
	u64									mSubmitSnapshotTestCount;
	u64									mModofyDocFromSnapTestCount;
	u64									mDeleteDocFromSnapTestCount;
	u64									mCreateDocToSnapTestCount;
	u64									mReadDocFromSnapTestCount;
	u64									mReadFailedCount;
	u64									mCreateFailedCount;
	u64									mDelFailedCount;
	u64									mModifyFailedCount;
	u64									mOprID;
	u32									mCrtSec;
	AosDfmDocType::E					mDocType;
	OmnMutex*							mLockRaw;
	OmnMutexPtr							mLock;
	AosDfmDocPtr						mDoc;
	AosRundataPtr						mRundata;
	AosBlobSEPtr						mBlobSE;
	AosBlobSEAPIPtr						mBlobSEAPI;
	map_t								mDocSeedMap;		//index of seeds
	std::vector<u64>					mvDoc;					//docid
	std::vector<u64>					mvDeletedDoc;			//deleted docid
	OmnThreadPtr        				mThreads[eMaxThreads];
	OmnFilePtr							mFile;
	static AosBuffPtr					ptrBuff;
	
	std::map<u32, AosLocalSnapshotPtr>	mSnaps;					// Ketty 2015/04/05
	u32									mMaxSnapshotNum;		// Ketty 2015/04/05
	u64									mMaxDocid;				// Ketty 2015/04/05
	int									mCreateSnapshotWeight;		// Ketty 2015/04/07
	int									mCommitSnapshotWeight;		// Ketty 2015/04/07	
	int									mRollbackSnapshotWeight;	// Ketty 2015/04/07
	int									mSubmitSnapshotWeight;		// Ketty 2015/04/07
	int									mCreateDocToSnapWeight;	// Ketty 2015/04/07
	int									mModifyDocFromSnapWeight;	// Ketty 2015/04/07
	int									mDeleteDocFromSnapWeight;	// Ketty 2015/04/07
	int									mReadDocFromSnapWeight;	// Ketty 2015/04/07
public:
	AosBlobSETester();
	~AosBlobSETester();

	virtual bool		start();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);

private:
	bool	runOneTest();

	bool	oneBatchTests(
			const int	batch_size,
			u64			&start_docid,
			u32			&num_docids);

	bool	testModifyDoc();

	bool	testModifyDocWithRaftInterface(u64 &ullOprID);

	bool	testCreateDoc(const u64 docid);

	bool	testCreateDocWithRaftInterface(
			const u64 docid,
			const u64 ullOprID);

	bool	testDeleteDoc();

	bool	testDeleteDocWithRaftInterface(u64 &ullOprID);

	bool	testReadDoc();

	bool	testReadDocWithRaftInterface();

	bool	testRestart();

	bool	loadSeed();

	u32		pickSeed();

	bool	allocateDocid(
			u64	&start_docid,
			u32	&num_docids);

	AosBuffPtr	createData(
			const u64	docid,
			const u32	seed_id = 0);

// Ketty Start 2015/04/05
private:
	bool 	testCreateSnapshot();

	bool 	testCommitSnapshot();
	
	bool 	testRollbackSnapshot();

	bool 	testSubmitSnapshot();

	bool 	testCreateDocToSnap(const u64 new_docid);

	bool 	testModifyDocFromSnap();

	bool 	testDeleteDocFromSnap();

	bool 	testReadDocFromSnap();

	bool	testCompactionAsRaftLeader();

	bool	compactionUnitTest();

private:
	bool 	commitLocalSnapshot(u64 &snap_id);

	bool 	rollbackLocalSnapshot(u64 &snap_id);

	bool 	submitLocalSnapshot(
				u64 &target_snap_id,
				u64 &submit_snap_id);

	AosLocalSnapshotPtr randGetLocalSnapshot();

	bool 	deleteLocalSnapshot(const u32 snap_id);

	bool 	deleteFromLocalDb(const u64 docid);

	bool 	saveToLocalDb(
				const u64 docid,
				const u32 seed_id,
				const u32 repeat);

	bool 	readFromLocalDb(
				const u64 docid,
				bool &exist,
				u32 &seed_id,
				u32 &repeat);

// Ketty End 2015/04/05


};
#endif

