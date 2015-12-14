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
// 2015-5-5 Modify by Koala Ren
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_Tester_BlobSETester_h
#define Aos_BlobSE_Tester_BlobSETester_h

#include "BlobSE/Ptrs.h"
#include "BlobSE/Tester/Ptrs.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDocHeader.h"
//#include "DfmUtil/DfmDocType.h"
#include "DfmUtil/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "Rundata/Rundata.h"
#include "JimoAPI/JimoDocEngine.h"
#include <ext/hash_map>
#include <map>
#include <vector>

class AosBlobSETester : public OmnTestPkg,
						public OmnThreadedObj
{
public:
    enum Operations
    {
        eNotCreate,
        eCreateDocs,
        eModifyDocs,
        eDeleteDocs,
        eReadDocs
    };
	enum
	{
		eDefaultTries = 100,
		eMaxBuffSize = 500000000,   //500M
		eMaxThreads = 100
	};
    typedef struct//add by koala ren
    {
        u64         docid;
        u64         timestamp;
        u64         snapshot;
        u32         repeat;
        u32         seed_id;
        Operations  oper;
    }DocEntry;

private:
    vector<vector<DocEntry> >           mDocs;
	AosRundataPtr						mRundata;
    int									mTries;
	int                                 mFinishedTries;
    int									mBatchSize;
	int									mTestCount;
	int					    			mCreateWeight;
	int									mDeleteWeight;
	int									mModifyWeight;
	int									mReadWeight;
	int									mReadInvalidWeight;
	int									mReadValidWeight;
	int									mReadDeletedWeight;
	u32									mNumOfThreads;
	u32									mNumFinished;
	u64									mDocidAllocateSize;
	i64									mTestDurationSec;
	u64									mReadTestCount;
	u64									mCreateTestCount;
	u64									mDelTestCount;
	u64									mModifyTestCount;
	u64									mReadFailedCount;
	u64									mCreateFailedCount;
	u64									mDelFailedCount;
	u64									mModifyFailedCount;
	u32									mCrtSec;
	OmnMutex*							mLockRaw;
	OmnMutexPtr							mLock;
	AosBlobSEPtr						mBlobSE;
	OmnThreadPtr        				mThreads[eMaxThreads];
    u64									mMaxDocid;				// Ketty 2015/04/05
public:
	AosBlobSETester();
	~AosBlobSETester();

	virtual bool		start();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);

private:
	bool	    runOneTest();

	bool	    oneBatchTests(
			    const int	batch_size,
			    u64			&start_docid,
			    u32			&num_docids);

	bool	        testModifyDoc();

	bool	        testCreateDoc();

	bool	        testDeleteDoc();

	bool	        testReadDoc();

    Operations      getDocState(u64 index);

    u64             generateSnapshotID();//add Koala Ren

    u64             pickTimestamp(u64 index);

    u64             pickSnapshotID(u64 index);

    u64             getDocID(u64 index);

    bool            isDocEqualOriginal(u64 index, const AosXmlTagPtr &doc);

    bool            wayOfReadDoc(AosXmlTagPtr &doc, u64 docid, const OmnString &objid);

    bool            wayOfDeleteDoc(u64 docid,const OmnString &objid);

    bool            wayOfCreateDoc(AosXmlTagPtr &doc, OmnString &docStr);

    static  bool    generateContents(u32 seed_id, u32 repeat, OmnString &docStr);//add Koala Ren
};
#endif

