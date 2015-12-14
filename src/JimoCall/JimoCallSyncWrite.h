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
// 2015/03/15 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_JimoCallSyncWrite_h
#define Aos_JimoCall_JimoCallSyncWrite_h

#include "JimoCall/JimoCall.h"

class AosJimoCallSyncWrite : public AosJimoCall
{
	enum
	{
		eNotRespondedYet = 0,
		ePositiveResponse = 1,
		eNegativeResponse = 2
	};

	OmnMutex *		mLock;

	u32				mNumResponses;
	u32				mNumPositiveResponses;
	bool			mCallResponded;

public:
	AosJimoCallSyncWrite(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const u64 dist_id, 
		const u32 rep_policy,
		AosClusterObj *cluster);

	AosJimoCallSyncWrite(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const OmnString &dist_str,
		const u32 rep_policy,
		AosClusterObj *cluster);

	virtual bool makeCall(AosRundata *rdata);
	virtual bool callResponded(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual void checkTimer(AosRundata *rdata);

private:
	inline bool enoughResponded() const
	{
		return mNumPositiveResponses >= AosRepPolicy::getNumSync(mReplicPolicy) ||
			   mNumResponses >= mEndpointIDs.size();
	}
	inline bool writeTimerExpired(const u64 crt_timestamp) const
	{
		if (crt_timestamp <= mStartTime) return false;
		if (crt_timestamp-mStartTime < mTimerMs) return false;
		return true;
	}
};
#endif

