////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// Created: 2011/10/20 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SecondTransMap_SecondTransMap_h
#define AOS_SecondTransMap_SecondTransMap_h 
/*
#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "TransUtil/Ptrs.h"
#include "IDTransMap/Ptrs.h"
#include "IDTransMap/IDTransMap.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
using namespace std;

class AosSecondTransMap : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftMaxCachedSize = 100000,
		eDftIILsPerBlock = 250000,

		eMaxNumBlocks = 1000,
		eDftMaxBlocksLoaded = 10,
		eDftNumIILsPerBlock = 100,
		eFileStart = 1000,

		eBuffLen = 10000, 
		eBuffIncSize = 10000
	};

	int			mBucketSize;
	OmnFilePtr	mFile;
	OmnMutexPtr	mLock;
	u32 		mCrtPos;
	u32 		mCrtTail;
	u32			mMaxCachedSize;
	u32 *		mBlocks[eMaxNumBlocks];
	int			mCounts[eMaxNumBlocks];
	int			mNumBlocksLoaded;
	int			mMaxBlocksLoaded;
	u32			mNumIILsPerBlock;
	u32			mBlockSize;
	AosBuffPtr	mBuff;
	OmnMutexPtr	mFinishedLock;
	OmnCondVarPtr	mFinishedCondVar;
	OmnFilePtr	mFinishedFile;
	u32			mThreadFreq;
	OmnString	mDirname;
	OmnString	mBackupDir;

public:
	AosSecondTransMap();
	~AosSecondTransMap();
	
	// ThreadedObj Interface
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool checkThread(OmnString &err, const int thrdLogicId) const{return true;}
	virtual bool signal(const int threadLogicId){ return true; };

	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);

	bool addTrans(
				const u64 &iilid,
				const AosIDTransVectorPtr &transes,
				const AosRundataPtr &rdata);

private:
	bool addTrans(
				const u64 &id,
				const AosIILTransPtr &trans,
				const bool needRsp,
				const AosRundataPtr &rdata);
	bool addTransToTail(
				const u64 &iilid, 
				const AosIILTransPtr &trans,
				const AosRundataPtr &rdata);
	bool saveCachedTrans(const AosRundataPtr &rdata);
	u32 getBlockIdx(const u32 &iilid) const;
	u32 * allocateBlockMemory() const;
	void releseBlockMemory(const u32 * ptr);
	u32 getBlockOffset(const int remove_idx) const;
	int getRelativeId(const u64 &iilid) const;
	OmnFilePtr getNextLogFile();
	bool procTrans();
	bool readBlock(
				const OmnFilePtr &file, 
				const u32 block_idx, 
				char *block);
	bool procOneIIL(
				const u64 &iilid, 
				const u32 tail,
				const OmnFilePtr &file);
};	
*/
#endif

