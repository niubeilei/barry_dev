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
// 10/23/2011	Created by chen Ring
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransBucket_TransBucket_h
#define AOS_TransBucket_TransBucket_h 
/*
#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "IDTransMap/Ptrs.h"
#include "IDTransMap/IDTransMap.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Thread/Ptrs.h"


class AosTransBucket : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{

		eHeaderPosSize 		= sizeof(u32),
		eTailPosSize 		= sizeof(u32),
		eHeaderEntrySize 	= eHeaderPosSize+eTailPosSize,
		eNextPosSize 		= sizeof(u32),
		eBlockOffsetSize 	= sizeof(u32),
		eBlockCrtLenSize 	= sizeof(u32)
	};
	OmnMutexPtr			mLock;
	OmnFilePtr			mFile;
	u64 				mStartId;
	u32					(*mHeaderBuff)[2];
	AosBuffPtr 			mBlockBuff;
	u32					mBlockOffset;			
	u32					mBlockCrtLen;			
	u32					mNumTrans;			
	int					mRefCount;			

	static u32 			smHeaderSize;
	static u32 			smHeaderNumEntries;
	static u32 			smBodyBlockSize;
	static u32 			smMaxBuckets;
	static u32 			smMaxTrans;
	static u32 			smFreeBlockStart;
	static OmnMutexPtr	smLock;
public:
	AosTransBucketPtr	mPrev;
	AosTransBucketPtr	mNext;
	
public:
	AosTransBucket(
			const u64 &start_id, 
			const OmnFilePtr &file,
			const AosRundataPtr &rdata);
	~AosTransBucket();

	static
	void 	config(
			const u32 numEntries, 
			const u32 maxBuckets,
			const u32 datablockSize,
			const u32 maxTrans);
	bool addTrans(const u64 &id, 
				const AosIDTransVectorPtr &transes,
				const AosRundataPtr &rdata);
	bool procTrans(
			const u64 &id,
			const AosIDTransVectorPtr &transes,	
			const AosRundataPtr &rdata);
	bool readFromFile(
				const u64 &start_id, 
				const OmnFilePtr &file, 
				const AosRundataPtr &rdata);
	//bool saveToFile(const AosRundataPtr &rdata);
	bool procAllTrans(const u32 siteid, const AosRundataPtr &rdata);
	bool procOneTrans(bool &trans_processed,const u32 siteid, const AosRundataPtr &rdata);
	u64	getID()const {return mStartId;}
	int addCount(){return mRefCount++;}

private:
	bool readHeader();
	bool saveHeader();
	bool saveBlock();
	bool getBlock(bool needNew);
	bool sanityCheck();
	bool checkOneTrans(const u32 header, const u32 tail);
};	
i*/
#endif

