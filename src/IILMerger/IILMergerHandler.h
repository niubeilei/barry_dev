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
#ifndef AOS_IILMerger_IILMergerHandler_h
#define AOS_IILMerger_IILMergerHandler_h

#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/BuffArray.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosIILMergerHandler : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	u64         		mIILID; 
	u32         		mSiteid;
	AosCompareFunPtr	mComp;
	bool 				mStable;
	int 				mMaxBucket;
	int             	mFreeBucket;
public:
	AosIILMergerHandler(
			const u64 iilid,
			const u32 siteid,
			const AosCompareFunPtr &comp, 
			const bool stable,
			const int maxBucket = 100,
			const int freeBucket = 1)
	:
	mIILID(iilid),
	mSiteid(siteid),
	mComp(comp),
	mStable(stable),
	mMaxBucket(maxBucket),
	mFreeBucket(freeBucket)
	{
	}

	~AosIILMergerHandler() {}
	
	virtual bool	proc(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata) = 0;
	
	AosCompareFunPtr getComp(){return mComp;}
	bool getStable(){return mStable;}
	int getMaxBucket(){return mMaxBucket;}
	int getFreeBucket(){return mFreeBucket;}
};
#endif

