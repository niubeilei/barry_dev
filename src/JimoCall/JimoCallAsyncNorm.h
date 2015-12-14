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
#ifndef Aos_JimoCall_JimoCallAsyncNorm_h
#define Aos_JimoCall_JimoCallAsyncNorm_h

#include "JimoCall/JimoCall.h"

class AosJimoCallAsyncNorm : public AosJimoCall
{
protected:
	enum
	{
		eDftTimerMs = 10000		// 10 seconds
	};

	u32					mTimerMs;
	AosJimoCallerPtr	mCaller;
	u64					mCallTime;
	int					mEndpointID;
	OmnMutex *			mLock;

public:
	AosJimoCallAsyncNorm(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const AosJimoCallerPtr &caller,
		AosClusterObj *cluster);		// For normal RPC

	AosJimoCallAsyncNorm(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const AosJimoCallerPtr &caller,
		AosClusterObj *cluster,
		const int endpoint_id);	// For direct normal RPC

	virtual ~AosJimoCallAsyncNorm();

	virtual bool makeCall(AosRundata *rdata);
	virtual bool callResponded(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual void checkTimer(AosRundata *rdata);
	virtual bool waitForFinish();

private:
	void logTimeoutCall(AosRundata *rdata);
};
#endif

