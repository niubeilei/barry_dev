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
// 2013/02/15	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapEngine_Testers_IILTester_h
#define Aos_BitmapEngine_Testers_IILTester_h

#include "BitmapEngine/Ptrs.h"
#include "BitmapEngine/BitmapInstr.h"
#include "BitmapEngine/Testers/Ptrs.h"
#include "BitmapTreeMgr/Ptrs.h"
#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosIILTester : public OmnTestPkg
{
public:
	enum
	{
		eNumStrings = 1000,
		eDftTries = 1000000,
		eBuffLength = 1000000,
		eDftBatchSize = 100000,
		eDftRecordLen = 50
	};

private:
	typedef hash_map<const u64, AosBitmapInstr, u64_hash, u32_cmp> imap_t;
	typedef hash_map<const u64, AosBitmapInstr, u64_hash, u32_cmp>::iterator imapitr_t;

	AosRandomBuffArrayPtr	mBuffArray;
	int						mRecordLen;
	AosRundataPtr			mRundata;
	int						mTries;
	OmnString				mIILName;
	AosCompareFunPtr		mCompareFunc;
	int						mBatchSize;
	int						mNumStrings;
	int						mKeyLen;
	AosBitmapExecutorPtr	mExecutor;
	AosBitmapTreePtr		mTree;
	AosQueryBitmapObjPtr	mLastBitmap;

public:
	AosIILTester();
	~AosIILTester() {}

	virtual bool		start();

private:
	bool	init();
	bool	basicTest();
	bool	config();
	bool	grandTorturer();
	bool 	addContents();
	bool 	checkResults();
	bool	verifyLeaves();
	bool	verifyLevelOneNodes();
	bool	verifyAddBits();
	bool	verifyTempBits();
	bool	verifyRebuildNodes();
	bool 	verifyContents();
	bool 	verifyLeavesAndLevelOneNodes(const vector<u64> &leaf_ids);
	bool 	verifyLeafParents(const vector<u64> &leaf_ids, AosU64BoolHash &level_one_map);
	bool 	verifyIILs(vector<u64> &leaf_ids);
	bool 	verifyOneIIL(const AosIILObjPtr &iil, vector<u64> &leaf_bitmap_ids);
	OmnString getKey(const char *seed, const u32 seqno, const u64 &docid);
	bool 	verifyTreeStructure();
	bool 	verifyLevelOneNodes( 
					AosU64BoolHash &level_one_map,
					const vector<u64> &level_one_nodes);
	bool 	verifyLevelOneNodes( 
					const vector<u64> &leaf_ids,
					const vector<u64> &level_one_nodes,
					AosU64BoolHash &level_one_map);
	int 	getLeafIdx(const vector<u64> &leaf_bitmap_ids, const u64 &leaf_id);
	bool 	getLevelOneNodes(vector<u64> &level_one_nodes);
	bool 	verifyAddBitsPath();
	bool 	verifyCreateBitmaps();
	bool 	checkAddBits(const u64 &parent_id, const u64 &bitmap_id);
	AosQueryBitmapObjPtr retrieveLeaf(
					const u64 &parent_id, 
					const u64 &leaf_id);
};
#endif

