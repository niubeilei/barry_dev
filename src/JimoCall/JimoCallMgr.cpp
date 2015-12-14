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
// 2014/12/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "JimoCall/JimoCallMgr.h"

#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"

static u32 sgNumJimoCallMgrs = 10;
static AosJimoCallMgr *sgJimoCallMgrs[sgNumJimoCallMgrs];

static __thread AosJimoCallMgr * sgJimoCallMgr = 0;

AosJimoCall *
AosCreateJimoCall(
		AosRundata *rdata, 
		const int package, 
		const int func)
{
	if (!sgJimoCallMgr) 
	{
		if (!sgInited)
		{
		 	sgLock.lock();
		 	if (!sgInited)
		 	{
		 		for (u32 i=0; i<sgNumJimoCallMgrs; i++)
		 		{
		 			sgJimoCallMgrs[i] = OmnNew AosJimoCallMgr(i);
		 		}
		 	}
		 	sgInited = true;
		 	sgLock.unlock();
		}
		 
		u32 thread_id = OmnGetCurrentThreadId() % sgNumJimoCallMgrs;
		sgJimoCallMgr = sgJimoCallMgrs[thread_id];
	}

	return sgJimoCallMgr->createJimoCall(rdta, package, func);
}


AosJimoCallMgr::AosJimoCallMgr()
{
	mCrtJimoCallID = (OmnGetCurrentThreadId() << 32);
}


AosJimoCallMgr::~AosJimoCallMgr()
{
}


AosJimoCall *
AosJimoCallMgr::createJimoCall(
		AosRundata *rdata, 
		const int package, 
		const int func)
{
	// This is thread safe. 
	u64 jimocall_id = mCrtJimoCallID++;
	AosJimoCallPtr jimo_call(rdata, package, func, jimocall_id);

	
#endif
