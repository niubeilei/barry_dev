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
// 2013/02/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapEngine_Testers_BitmapExecutorTester_h
#define Aos_BitmapEngine_Testers_BitmapExecutorTester_h

#include "BitmapEngine/Ptrs.h"
#include "BitmapEngine/Testers/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/Opr.h"
#include <vector>
using namespace std;


class AosBitmapExecutorTester : public OmnTestPkg,
								public AosBmpExeCaller
{
public:

private:
	enum
	{
		eDftTries = 1000000,
		eMaxBitmapId = 100000000,
		eDftNumNodes = 100
	};

	struct Entry
	{
		u64		bitmap_id;
		int 	node_level;
		int		idx1;
		int		idx2;

		Entry(const u64 &bitmap_id, 
				const int node_level,
				const int idx1,
				const int idx2)
		:
		bitmap_id(bitmap_id),
		node_level(node_level),
		idx1(idx1),
		idx2(idx2)
		{
		}
	};

	typedef hash_map<const u64, AosBitmapObjPtr, u64_hash, u64_cmp> bmap_t;
	typedef hash_map<const u64, AosBitmapObjPtr, u64_hash, u64_cmp>::iterator bmapitr_t;

	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	AosRundataPtr 			mRundata;
	vector<Entry>			mBitmapsCreated;
	AosIILExecutorObjPtr	mExecutor;
	vector<u64>			  * mMolds;
	int						mNumMolds;
	int						mTries;
	vector<AosBitmapTestPtr> mNodes;
	vector<AosBitmapTestPtr> mPendingLeaves;
	vector<AosBitmapTestPtr> mProcessedLeaves;
	int						mNumNodes;
	bmap_t					mBitmaps;
	bool					mCallFinished;

public:
	AosBitmapExecutorTester();
	~AosBitmapExecutorTester() {}

	virtual bool		start();
	virtual void reqProcessed(const AosRundataPtr &rdata);

private:
	bool	init();
	bool	basicTest();
	bool	config();
	bool	grandTorturer();
	bool	createBitmap();
	AosBitmapTestPtr createBitmap(const int nodelevel,const int iillevel);
	bool 	insertBits();
	bool 	removeBits();
	bool 	rebuildBitmap();
	bool 	finish();
	bool 	getBitmapId();
	bool 	getBitmapIdByIILID();
	AosBitmapTestPtr pickNode(const bool flag);
	AosBitmapTestPtr pickLeaf();
	AosBitmapTestPtr createLeaf();
	int		pickMold();
	bool	verifyLeaf(const AosBitmapTestPtr &node);
	bool	check();
	bool	verifyResults();
};
#endif
