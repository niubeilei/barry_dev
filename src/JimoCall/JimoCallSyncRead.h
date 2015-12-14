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
#ifndef Aos_JimoCall_JimoCallSyncRead_h
#define Aos_JimoCall_JimoCallSyncRead_h

#include "JimoCall/JimoCall.h"

class AosJimoCallSyncRead : public AosJimoCall
{
	OmnMutex *		mLock;

public:
	AosJimoCallSyncRead(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const u64 dist_id, 
		const u32 rep_policy,
		AosClusterObj* cluster);

	virtual bool makeCall(AosRundata *rdata);
	virtual bool callResponded(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual void checkTimer(AosRundata *rdata);

private:
	void logTimeoutCall(AosRundata *rdata);

};
#endif

