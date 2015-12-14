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
// Modification History:
// 04/11/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocFileMgr_DfmFmtTester_DocInfo_h
#define Aos_DocFileMgr_DfmFmtTester_DocInfo_h 

#include "API/AosApi.h"
#include "DocFileMgr/DfmFmtTester/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#include "Thread/Mutex.h"

#include <set>
#include <vector>
using namespace std;


class AosDocInfo3 : public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eMaxSize = 10000000,    // 10M

		eSizeLevel0 = 30,
		eSizeLevel1 = 200,      // 30-200
		eSizeLevel2 = 1000,     // 200-1k
		eSizeLevel3 = 5000,     // 1k-5k
		eSizeLevel4 = 30000,    // 5k-30k
		eSizeLevel5 = 60000,    // 60k
		eSizeLevel6 = 130000,   // 130k
		eSizeLevel7 = 250000,   // 250k
		eSizeLevel8 = 500000,   // 500k
		eSizeLevel9 = 1000000,  // 1M
		eSizeLevel10 = 5000000, // 5M 
		eSizeLevel11 = eMaxSize,    // 10M
	
		eLevel1Weight = 10,
		eLevel2Weight = 10,
		eLevel3Weight = 10,
		eLevel4Weight = 10,
		eLevel5Weight = 10,
		eLevel6Weight = 10,
		eLevel7Weight = 0,
		eLevel8Weight = 0,
		eLevel9Weight = 0,
		eLevel10Weight = 0,
		eLevel11Weight = 0,
		eLevelNum = 11,
		eMaxLevel = 100,

	};

private:
	OmnMutexPtr	mLock;
	u64			mDocid;
	u32			mPattern;
	u32			mRepeat;
	
	static int 	smMinSizes[eMaxLevel];
	static int 	smMaxSizes[eMaxLevel];
	static int	smWeights[eMaxLevel];

public:
	AosDocInfo3(
		const u64 docid,
		const u32 pattern,
		const u32 repeat);
	~AosDocInfo3();

	static void staticInit();
	//static void randData(u32 &pattern, u32 &repeat);
	static AosBuffPtr randData(u32 &pattern, u32 &repeat);

	AosDocInfo3Ptr	clone();
	void reset(const u32 pattern, const u32 repeat);

	bool isSameData(const AosBuffPtr &buff);

	u64	getDocid(){ return mDocid; };
	u64	getLocalDocid(){ return mDocid / AosGetNumCubes(); };
	u32	getPattern(){ return mPattern; };
	u32	getRepeat(){ return mRepeat; };
	
	void lock(){mLock->lock();};
	void unlock(){mLock->unlock();};
	OmnMutexPtr getLock(){return mLock;};
	
	static AosDocInfo3Ptr serializeFromStatic(const AosBuffPtr &buff);
	bool 	serializeTo(const AosBuffPtr &buff);

};

#endif
